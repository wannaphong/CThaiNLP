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
