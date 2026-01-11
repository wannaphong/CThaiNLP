/**
 * @file cthainlp_wrapper.c
 * @brief Python C extension wrapper for CThaiNLP
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "newmm.h"

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
    
    /* Call C function */
    char** tokens = newmm_segment(text, dict_path, &token_count);
    
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
 * Module initialization function
 */
PyMODINIT_FUNC PyInit__cthainlp(void) {
    return PyModule_Create(&cthainlp_module);
}
