/**
 * @file trie.c
 * @brief Trie data structure implementation
 */

#include "trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 8

/* Helper function to decode UTF-8 character */
static int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1; /* Invalid UTF-8 */
}

/* Get UTF-8 codepoint from string */
static int get_utf8_codepoint(const char* str, int* byte_len) {
    unsigned char c = (unsigned char)str[0];
    int len = utf8_char_len(c);
    int codepoint = 0;
    
    if (len == 1) {
        codepoint = c;
    } else if (len == 2) {
        codepoint = ((c & 0x1F) << 6) | (str[1] & 0x3F);
    } else if (len == 3) {
        codepoint = ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else if (len == 4) {
        codepoint = ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
                    ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
    }
    
    *byte_len = len;
    return codepoint;
}

/* Create a new trie node */
static TrieNode* trie_node_create(void) {
    TrieNode* node = (TrieNode*)calloc(1, sizeof(TrieNode));
    if (!node) return NULL;
    
    node->is_end = false;
    node->children = NULL;
    node->child_chars = NULL;
    node->num_children = 0;
    node->capacity = 0;
    
    return node;
}

/* Free a trie node and all its children */
static void trie_node_free(TrieNode* node) {
    if (!node) return;
    
    for (int i = 0; i < node->num_children; i++) {
        trie_node_free(node->children[i]);
    }
    
    free(node->children);
    free(node->child_chars);
    free(node);
}

/* Find child node by codepoint */
static TrieNode* trie_node_get_child(TrieNode* node, int codepoint) {
    for (int i = 0; i < node->num_children; i++) {
        if (node->child_chars[i] == codepoint) {
            return node->children[i];
        }
    }
    return NULL;
}

/* Add child node */
static TrieNode* trie_node_add_child(TrieNode* node, int codepoint) {
    /* Check if need to expand capacity */
    if (node->num_children >= node->capacity) {
        int new_capacity = node->capacity == 0 ? INITIAL_CAPACITY : node->capacity * 2;
        
        TrieNode** new_children = (TrieNode**)realloc(node->children, 
                                                       new_capacity * sizeof(TrieNode*));
        if (!new_children) return NULL;
        
        int* new_chars = (int*)realloc(node->child_chars, new_capacity * sizeof(int));
        if (!new_chars) {
            /* Restore old pointer to avoid leak, but still fail */
            return NULL;
        }
        
        node->children = new_children;
        node->child_chars = new_chars;
        node->capacity = new_capacity;
    }
    
    /* Create new child */
    TrieNode* child = trie_node_create();
    if (!child) return NULL;
    
    node->children[node->num_children] = child;
    node->child_chars[node->num_children] = codepoint;
    node->num_children++;
    
    return child;
}

Trie* trie_create(void) {
    Trie* trie = (Trie*)malloc(sizeof(Trie));
    if (!trie) return NULL;
    
    trie->root = trie_node_create();
    if (!trie->root) {
        free(trie);
        return NULL;
    }
    
    trie->num_words = 0;
    return trie;
}

void trie_add(Trie* trie, const char* word) {
    if (!trie || !word || !word[0]) return;
    
    /* Trim leading/trailing whitespace */
    while (*word == ' ' || *word == '\t' || *word == '\r' || *word == '\n') {
        word++;
    }
    if (!*word) return;
    
    int len = strlen(word);
    while (len > 0 && (word[len-1] == ' ' || word[len-1] == '\t' || 
                       word[len-1] == '\r' || word[len-1] == '\n')) {
        len--;
    }
    if (len == 0) return;
    
    TrieNode* current = trie->root;
    const char* ptr = word;
    const char* end = word + len;
    
    while (ptr < end) {
        int byte_len;
        int codepoint = get_utf8_codepoint(ptr, &byte_len);
        
        TrieNode* child = trie_node_get_child(current, codepoint);
        if (!child) {
            child = trie_node_add_child(current, codepoint);
            if (!child) return; /* Out of memory */
        }
        
        current = child;
        ptr += byte_len;
    }
    
    if (!current->is_end) {
        current->is_end = true;
        trie->num_words++;
    }
}

int trie_load_dict(Trie* trie, const char* dict_path) {
    if (!trie || !dict_path) return -1;
    
    FILE* fp = fopen(dict_path, "r");
    if (!fp) return -1;
    
    char buffer[1024];
    int count = 0;
    
    while (fgets(buffer, sizeof(buffer), fp)) {
        /* Remove newline */
        int len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }
        if (len > 0 && buffer[len-1] == '\r') {
            buffer[len-1] = '\0';
            len--;
        }
        
        if (len > 0) {
            trie_add(trie, buffer);
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

int trie_prefixes(Trie* trie, const char* text, char*** prefixes, int** lengths) {
    if (!trie || !text || !prefixes || !lengths) return 0;
    
    int max_prefixes = 100; /* Initial allocation */
    *prefixes = (char**)malloc(max_prefixes * sizeof(char*));
    *lengths = (int*)malloc(max_prefixes * sizeof(int));
    if (!*prefixes || !*lengths) {
        free(*prefixes);
        free(*lengths);
        return 0;
    }
    
    int count = 0;
    TrieNode* current = trie->root;
    const char* ptr = text;
    int byte_pos = 0;
    
    while (*ptr) {
        int byte_len;
        int codepoint = get_utf8_codepoint(ptr, &byte_len);
        
        TrieNode* child = trie_node_get_child(current, codepoint);
        if (!child) break;
        
        byte_pos += byte_len;
        
        if (child->is_end) {
            /* Need to expand arrays? */
            if (count >= max_prefixes) {
                max_prefixes *= 2;
                char** new_prefixes = (char**)realloc(*prefixes, max_prefixes * sizeof(char*));
                int* new_lengths = (int*)realloc(*lengths, max_prefixes * sizeof(int));
                if (!new_prefixes || !new_lengths) break;
                *prefixes = new_prefixes;
                *lengths = new_lengths;
            }
            
            /* Copy prefix */
            (*prefixes)[count] = (char*)malloc(byte_pos + 1);
            if ((*prefixes)[count]) {
                memcpy((*prefixes)[count], text, byte_pos);
                (*prefixes)[count][byte_pos] = '\0';
                (*lengths)[count] = byte_pos;
                count++;
            }
        }
        
        current = child;
        ptr += byte_len;
    }
    
    return count;
}

void trie_free(Trie* trie) {
    if (!trie) return;
    
    trie_node_free(trie->root);
    free(trie);
}
