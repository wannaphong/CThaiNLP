/**
 * @file trie.h
 * @brief Trie data structure for efficient dictionary lookup
 * 
 * Internal header for trie implementation.
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

typedef struct TrieNode {
    bool is_end;
    struct TrieNode** children;
    int* child_chars;  /* UTF-8 code points of children */
    int num_children;
    int capacity;
} TrieNode;

typedef struct Trie {
    TrieNode* root;
    int num_words;
} Trie;

/**
 * @brief Create a new empty trie
 */
Trie* trie_create(void);

/**
 * @brief Add a word to the trie
 */
void trie_add(Trie* trie, const char* word);

/**
 * @brief Load words from a dictionary file
 */
int trie_load_dict(Trie* trie, const char* dict_path);

/**
 * @brief Get all possible word prefixes from text
 * 
 * @param trie The trie structure
 * @param text Input text (UTF-8)
 * @param prefixes Output array of prefix strings (caller must free)
 * @param lengths Output array of prefix byte lengths
 * @return Number of prefixes found
 */
int trie_prefixes(Trie* trie, const char* text, char*** prefixes, int** lengths);

/**
 * @brief Free trie memory
 */
void trie_free(Trie* trie);

#endif /* TRIE_H */
