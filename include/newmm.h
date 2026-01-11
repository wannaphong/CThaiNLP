/**
 * @file newmm.h
 * @brief Thai word segmentation using New Maximum Matching algorithm
 * 
 * Dictionary-based maximal matching word segmentation, constrained by
 * Thai Character Cluster (TCC) boundaries with improved rules.
 * 
 * This is a C port of PyThaiNLP's newmm tokenizer.
 * GitHub: https://github.com/PyThaiNLP/pythainlp
 * 
 * @author CThaiNLP
 * @date 2026
 */

#ifndef NEWMM_H
#define NEWMM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle for dictionary */
typedef void* newmm_dict_t;

/**
 * @brief Load a dictionary for reuse
 * 
 * @param dict_path Path to dictionary file (one word per line, UTF-8 encoded)
 *                  If NULL, uses a default minimal dictionary
 * @return Dictionary handle to be used with newmm_segment_with_dict()
 *         Returns NULL on error
 */
newmm_dict_t newmm_load_dict(const char* dict_path);

/**
 * @brief Free a loaded dictionary
 * 
 * @param dict Dictionary handle returned by newmm_load_dict()
 */
void newmm_free_dict(newmm_dict_t dict);

/**
 * @brief Segment Thai text using a pre-loaded dictionary
 * 
 * @param text Input Thai text to be segmented (UTF-8 encoded)
 * @param dict Pre-loaded dictionary handle from newmm_load_dict()
 * @param token_count Output parameter for number of tokens found
 * @return Array of strings (tokens), caller must free using newmm_free_result()
 *         Returns NULL on error
 */
char** newmm_segment_with_dict(const char* text, newmm_dict_t dict, int* token_count);

/**
 * @brief Segment Thai text into words using newmm algorithm
 * 
 * @param text Input Thai text to be segmented (UTF-8 encoded)
 * @param dict_path Path to dictionary file (one word per line, UTF-8 encoded)
 *                  If NULL, uses a default minimal dictionary
 * @param token_count Output parameter for number of tokens found
 * @return Array of strings (tokens), caller must free using newmm_free_result()
 *         Returns NULL on error
 */
char** newmm_segment(const char* text, const char* dict_path, int* token_count);

/**
 * @brief Free memory allocated by newmm_segment
 * 
 * @param tokens Array of tokens returned by newmm_segment()
 * @param token_count Number of tokens in the array
 */
void newmm_free_result(char** tokens, int token_count);

#ifdef __cplusplus
}
#endif

#endif /* NEWMM_H */
