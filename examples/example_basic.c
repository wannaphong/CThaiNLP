/**
 * @file example_basic.c
 * @brief Basic example of using CThaiNLP newmm tokenizer
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/newmm.h"

int main(int argc, char* argv[]) {
    const char* text;
    const char* dict_path = NULL;
    
    /* Get text from command line or use default */
    if (argc > 1) {
        text = argv[1];
    } else {
        text = "ฉันไปโรงเรียน";
    }
    
    /* Optional: dictionary path */
    if (argc > 2) {
        dict_path = argv[2];
    }
    
    printf("Input text: %s\n", text);
    printf("Segmenting...\n");
    
    /* Segment text */
    int token_count;
    char** tokens = newmm_segment(text, dict_path, &token_count);
    
    if (!tokens) {
        fprintf(stderr, "Error: Failed to segment text\n");
        return 1;
    }
    
    /* Print results */
    printf("Found %d tokens:\n", token_count);
    for (int i = 0; i < token_count; i++) {
        printf("  [%d] %s\n", i, tokens[i]);
    }
    
    /* Output in list format (like PyThaiNLP) */
    printf("\nOutput: [");
    for (int i = 0; i < token_count; i++) {
        printf("'%s'", tokens[i]);
        if (i < token_count - 1) printf(", ");
    }
    printf("]\n");
    
    /* Cleanup */
    newmm_free_result(tokens, token_count);
    
    return 0;
}
