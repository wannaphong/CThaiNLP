/**
 * @file newmm.c
 * @brief New Maximum Matching word segmentation implementation
 */

#include "../include/newmm.h"
#include "trie.h"
#include "tcc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_GRAPH_SIZE 50
#define MAX_TOKENS 10000

/* Graph structure for BFS */
typedef struct {
    int* edges;
    int* edge_counts;
    int* capacities;
    int size;
} Graph;

/* Helper: Check if character is non-Thai */
static bool is_non_thai_char(int codepoint) {
    /* Latin letters, digits, spaces */
    if ((codepoint >= 'a' && codepoint <= 'z') ||
        (codepoint >= 'A' && codepoint <= 'Z') ||
        (codepoint >= '0' && codepoint <= '9') ||
        codepoint == ' ' || codepoint == '\t' ||
        codepoint == '\r' || codepoint == '\n') {
        return true;
    }
    /* Thai range */
    if (codepoint >= 0x0E00 && codepoint <= 0x0E7F) {
        return false;
    }
    /* Other non-Thai */
    return true;
}

/* UTF-8 helper */
static int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

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

/* Helper: Extract substring */
static char* substring(const char* text, int start, int end) {
    int len = end - start;
    char* result = (char*)malloc(len + 1);
    if (result) {
        memcpy(result, text + start, len);
        result[len] = '\0';
    }
    return result;
}

/* Simplified newmm segmentation */
static int segment_text(const char* text, Trie* trie, char*** tokens) {
    int text_len = strlen(text);
    if (text_len == 0) return 0;
    
    /* Get valid TCC positions */
    int* valid_pos;
    int num_valid = tcc_pos(text, &valid_pos);
    if (num_valid == 0) {
        free(valid_pos);
        return 0;
    }
    
    /* Allocate token array */
    *tokens = (char**)malloc(MAX_TOKENS * sizeof(char*));
    if (!*tokens) {
        free(valid_pos);
        return 0;
    }
    
    int token_count = 0;
    int pos = 0;
    
    while (pos < text_len) {
        /* Try to find longest matching word from dictionary */
        char** prefixes;
        int* lengths;
        int num_prefixes = trie_prefixes(trie, text + pos, &prefixes, &lengths);
        
        int best_len = 0;
        int best_end_pos = pos;
        
        /* Simple greedy: find longest match */
        /* But prefer shorter match if longer one leaves us with unknown Thai character */
        for (int i = 0; i < num_prefixes; i++) {
            int end_pos = pos + lengths[i];
            
            if (lengths[i] > best_len) {
                best_len = lengths[i];
                best_end_pos = end_pos;
            }
        }
        
        /* Now check if a shorter match would be better */
        /* Only if the best match leads to an unknown Thai character */
        /* and a shorter match leads to a known word */
        if (best_len > 0 && best_end_pos < text_len) {
            char** best_next_prefixes;
            int* best_next_lengths;
            int num_best_next = trie_prefixes(trie, text + best_end_pos, &best_next_prefixes, &best_next_lengths);
            
            if (num_best_next == 0) {
                /* Best match doesn't lead to a dictionary word */
                /* Check if it's a Thai character (not Latin/digit) */
                int byte_len;
                int next_cp = get_utf8_codepoint(text + best_end_pos, &byte_len);
                
                if (!is_non_thai_char(next_cp)) {
                    /* It's a Thai character that's not in dictionary */
                    /* Try shorter matches to see if they lead to dictionary words */
                    for (int i = 0; i < num_prefixes; i++) {
                        int end_pos = pos + lengths[i];
                        if (lengths[i] < best_len && end_pos < text_len) {
                            char** next_prefixes;
                            int* next_lengths;
                            int num_next = trie_prefixes(trie, text + end_pos, &next_prefixes, &next_lengths);
                            
                            if (num_next > 0) {
                                /* This shorter match leads to a dictionary word */
                                /* Prefer it */
                                best_len = lengths[i];
                                best_end_pos = end_pos;
                            }
                            
                            /* Free lookahead results */
                            for (int j = 0; j < num_next; j++) {
                                free(next_prefixes[j]);
                            }
                            free(next_prefixes);
                            free(next_lengths);
                            
                            if (num_next > 0) {
                                /* We found a better match, stop looking */
                                break;
                            }
                        }
                    }
                }
            }
            
            /* Free lookahead results */
            for (int j = 0; j < num_best_next; j++) {
                free(best_next_prefixes[j]);
            }
            free(best_next_prefixes);
            free(best_next_lengths);
        }
        
        /* Free prefix results */
        for (int i = 0; i < num_prefixes; i++) {
            free(prefixes[i]);
        }
        free(prefixes);
        free(lengths);
        
        /* If found a dictionary word, use it */
        if (best_len > 0) {
            (*tokens)[token_count++] = substring(text, pos, best_end_pos);
            pos = best_end_pos;
        } else {
            /* Handle non-dictionary word */
            /* Check if it's a non-Thai sequence */
            int byte_len;
            int cp = get_utf8_codepoint(text + pos, &byte_len);
            
            if (is_non_thai_char(cp)) {
                /* Skip all consecutive non-Thai characters of same type */
                int end = pos + byte_len;
                bool is_space = (cp == ' ' || cp == '\t');
                bool is_alpha = ((cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z'));
                bool is_digit = (cp >= '0' && cp <= '9');
                
                while (end < text_len) {
                    int next_cp = get_utf8_codepoint(text + end, &byte_len);
                    bool match = false;
                    
                    if (is_space && (next_cp == ' ' || next_cp == '\t')) match = true;
                    else if (is_alpha && ((next_cp >= 'a' && next_cp <= 'z') || (next_cp >= 'A' && next_cp <= 'Z'))) match = true;
                    else if (is_digit && ((next_cp >= '0' && next_cp <= '9') || next_cp == '.' || next_cp == ',')) match = true;
                    
                    if (!match) break;
                    end += byte_len;
                }
                
                (*tokens)[token_count++] = substring(text, pos, end);
                pos = end;
            } else {
                /* Thai character not in dictionary - advance to next TCC boundary */
                /* Find next valid TCC boundary after current position */
                int next_pos = text_len; /* Default to end of text */
                for (int i = 0; i < num_valid; i++) {
                    if (valid_pos[i] > pos) {
                        next_pos = valid_pos[i];
                        break;
                    }
                }
                
                (*tokens)[token_count++] = substring(text, pos, next_pos);
                pos = next_pos;
            }
        }
        
        if (token_count >= MAX_TOKENS - 1) break;
    }
    
    free(valid_pos);
    return token_count;
}

/* Default minimal Thai dictionary */
static const char* default_words[] = {
    "ไป", "มา", "ใน", "ที่", "และ", "หรือ", "คือ", "เป็น", "มี", "ได้",
    "จะ", "ไม่", "ของ", "กับ", "ก็", "ให้", "ถ้า", "แล้ว", "เมื่อ", "ซึ่ง",
    "นี้", "นั้น", "อยู่", "เพื่อ", "การ", "ความ", "จาก", "โดย", "อย่าง", "ถึง",
    "ว่า", "เอง", "ทุก", "แต่", "ตาม", "นัก", "ยัง", "ผล", "ผู้", "คน",
    "วัน", "ปี", "เดือน", "ครั้ง", "ตัว", "คน", "สิ่ง", "งาน", "ข้อ", "รับ",
    NULL
};

newmm_dict_t newmm_load_dict(const char* dict_path) {
    /* Create trie */
    Trie* trie = trie_create();
    if (!trie) return NULL;
    
    /* Load dictionary */
    if (dict_path) {
        if (trie_load_dict(trie, dict_path) < 0) {
            /* Failed to load, use default */
            for (int i = 0; default_words[i] != NULL; i++) {
                trie_add(trie, default_words[i]);
            }
        }
    } else {
        /* Use default dictionary */
        for (int i = 0; default_words[i] != NULL; i++) {
            trie_add(trie, default_words[i]);
        }
    }
    
    return (newmm_dict_t)trie;
}

void newmm_free_dict(newmm_dict_t dict) {
    if (dict) {
        trie_free((Trie*)dict);
    }
}

char** newmm_segment_with_dict(const char* text, newmm_dict_t dict, int* token_count) {
    if (!text || !token_count || !dict) return NULL;
    
    *token_count = 0;
    
    /* Empty text */
    if (!text[0]) return NULL;
    
    Trie* trie = (Trie*)dict;
    
    /* Segment text */
    char** tokens = NULL;
    int count = segment_text(text, trie, &tokens);
    
    *token_count = count;
    return tokens;
}

char** newmm_segment(const char* text, const char* dict_path, int* token_count) {
    if (!text || !token_count) return NULL;
    
    *token_count = 0;
    
    /* Empty text */
    if (!text[0]) return NULL;
    
    /* Create and load dictionary */
    newmm_dict_t dict = newmm_load_dict(dict_path);
    if (!dict) return NULL;
    
    /* Segment text */
    char** tokens = newmm_segment_with_dict(text, dict, token_count);
    
    /* Cleanup */
    newmm_free_dict(dict);
    
    return tokens;
}

void newmm_free_result(char** tokens, int token_count) {
    if (!tokens) return;
    
    for (int i = 0; i < token_count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}
