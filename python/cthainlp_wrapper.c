/**
 * @file cthainlp_wrapper.c
 * @brief Python C extension wrapper for CThaiNLP
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include <stdlib.h>
#include "newmm.h"

/* Module-level dictionary cache */
static struct {
    newmm_dict_t dict;
    char* dict_path;
} dict_cache = {NULL, NULL};

/**
 * Load or retrieve cached dictionary
 */
static newmm_dict_t get_or_load_dict(const char* dict_path) {
    /* Check if we need to reload the dictionary */
    int need_reload = 0;
    
    if (dict_cache.dict == NULL) {
        /* No cached dict */
        need_reload = 1;
    } else if (dict_path == NULL && dict_cache.dict_path != NULL) {
        /* Switching from custom to default */
        need_reload = 1;
    } else if (dict_path != NULL && dict_cache.dict_path == NULL) {
        /* Switching from default to custom */
        need_reload = 1;
    } else if (dict_path != NULL && dict_cache.dict_path != NULL) {
        /* Both custom, check if path changed */
        if (strcmp(dict_path, dict_cache.dict_path) != 0) {
            need_reload = 1;
        }
    }
    
    if (need_reload) {
        /* Free old dictionary */
        if (dict_cache.dict) {
            newmm_free_dict(dict_cache.dict);
            dict_cache.dict = NULL;
        }
        if (dict_cache.dict_path) {
            free(dict_cache.dict_path);
            dict_cache.dict_path = NULL;
        }
        
        /* Load new dictionary */
        dict_cache.dict = newmm_load_dict(dict_path);
        if (dict_cache.dict && dict_path) {
            dict_cache.dict_path = strdup(dict_path);
            if (!dict_cache.dict_path) {
                /* strdup failed, clean up and return NULL */
                newmm_free_dict(dict_cache.dict);
                dict_cache.dict = NULL;
                return NULL;
            }
        }
    }
    
    return dict_cache.dict;
}

/**
 * Python wrapper for newmm_segment function
 */
static PyObject* py_newmm_segment(PyObject* Py_UNUSED(self), PyObject* args, PyObject* kwargs) {
    const char* text;
    const char* dict_path = NULL;
    int token_count;
    
    /* Parse arguments */
    static char* kwlist[] = {"text", "dict_path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|z", kwlist, &text, &dict_path)) {
        return NULL;
    }
    
    /* Get or load dictionary */
    newmm_dict_t dict = get_or_load_dict(dict_path);
    if (!dict) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to load dictionary");
        return NULL;
    }
    
    /* Call C function with cached dictionary */
    char** tokens = newmm_segment_with_dict(text, dict, &token_count);
    
    if (!tokens) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to segment text");
        return NULL;
    }
    
    /* Convert result to Python list */
    PyObject* result = PyList_New(token_count);
    if (!result) {
        newmm_free_result(tokens, token_count);
        return NULL;
    }
    
    for (int i = 0; i < token_count; i++) {
        PyObject* token_str = PyUnicode_FromString(tokens[i]);
        if (!token_str) {
            Py_DECREF(result);
            newmm_free_result(tokens, token_count);
            return NULL;
        }
        PyList_SET_ITEM(result, i, token_str);
    }
    
    /* Free C memory */
    newmm_free_result(tokens, token_count);
    
    return result;
}

/**
 * Clear cached dictionary
 */
static PyObject* py_clear_cache(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(args)) {
    if (dict_cache.dict) {
        newmm_free_dict(dict_cache.dict);
        dict_cache.dict = NULL;
    }
    if (dict_cache.dict_path) {
        free(dict_cache.dict_path);
        dict_cache.dict_path = NULL;
    }
    Py_RETURN_NONE;
}

/**
 * Module method definitions
 */
static PyMethodDef CThaiNLPMethods[] = {
    {
        "segment",
        (PyCFunction)py_newmm_segment,
        METH_VARARGS | METH_KEYWORDS,
        "Segment Thai text into words using newmm algorithm.\n\n"
        "Args:\n"
        "    text (str): Input Thai text to segment (UTF-8 encoded)\n"
        "    dict_path (str, optional): Path to dictionary file. If None, uses default.\n\n"
        "Returns:\n"
        "    list: List of string tokens\n\n"
        "Example:\n"
        "    >>> from cthainlp import _cthainlp\n"
        "    >>> tokens = _cthainlp.segment('ฉันไปโรงเรียน')\n"
        "    >>> print(tokens)\n"
        "    ['ฉัน', 'ไป', 'โรงเรียน']\n"
    },
    {
        "clear_cache",
        py_clear_cache,
        METH_NOARGS,
        "Clear the cached dictionary.\n\n"
        "This forces the next tokenization to reload the dictionary.\n"
    },
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/**
 * Module definition
 */
static struct PyModuleDef cthainlp_module = {
    PyModuleDef_HEAD_INIT,
    "_cthainlp",
    "CThaiNLP - Thai Natural Language Processing C extension module",
    -1,
    CThaiNLPMethods,
    NULL,  /* m_slots */
    NULL,  /* m_traverse */
    NULL,  /* m_clear */
    NULL   /* m_free */
};

/**
 * Module cleanup function
 */
static void module_free(void* Py_UNUSED(self)) {
    /* Clean up cached dictionary on module unload */
    if (dict_cache.dict) {
        newmm_free_dict(dict_cache.dict);
        dict_cache.dict = NULL;
    }
    if (dict_cache.dict_path) {
        free(dict_cache.dict_path);
        dict_cache.dict_path = NULL;
    }
}

/**
 * Module initialization function
 */
PyMODINIT_FUNC PyInit__cthainlp(void) {
    /* Update module definition with cleanup function */
    cthainlp_module.m_free = module_free;
    return PyModule_Create(&cthainlp_module);
}
