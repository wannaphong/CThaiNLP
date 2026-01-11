/**
 * @file tcc.h
 * @brief Thai Character Cluster (TCC) tokenization
 * 
 * Implementation of tokenizer according to Thai Character Clusters (TCCs)
 * rules proposed by Theeramunkong et al. 2000.
 */

#ifndef TCC_H
#define TCC_H

#include <stdbool.h>

/**
 * @brief Get valid Thai Character Cluster breaking positions
 * 
 * @param text Input Thai text (UTF-8)
 * @param positions Output array of byte positions (caller must free)
 * @return Number of positions found
 */
int tcc_pos(const char* text, int** positions);

#endif /* TCC_H */
