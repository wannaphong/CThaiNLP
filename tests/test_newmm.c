/**
 * @file test_newmm.c
 * @brief Test program for newmm tokenizer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/newmm.h"

typedef struct {
    const char* text;
    const char* expected;
    const char* description;
} TestCase;

static int test_count = 0;
static int test_passed = 0;

void run_test(const char* text, const char* dict_path, const char* expected, const char* description) {
    test_count++;
    printf("\n[Test %d] %s\n", test_count, description);
    printf("Input: %s\n", text);
    
    int token_count;
    char** tokens = newmm_segment(text, dict_path, &token_count);
    
    /* Empty string should return NULL and token_count = 0 */
    if (!tokens && token_count == 0) {
        printf("Output: []\n");
        printf("Expected: %s\n", expected);
        if (strcmp(expected, "[]") == 0) {
            printf("✓ PASS\n");
            test_passed++;
        } else {
            printf("❌ FAIL\n");
        }
        return;
    }
    
    if (!tokens) {
        printf("❌ FAIL: Segmentation failed\n");
        return;
    }
    
    /* Build output string */
    char output[1024] = "[";
    for (int i = 0; i < token_count; i++) {
        strcat(output, "'");
        strcat(output, tokens[i]);
        strcat(output, "'");
        if (i < token_count - 1) strcat(output, ", ");
    }
    strcat(output, "]");
    
    printf("Output: %s\n", output);
    printf("Expected: %s\n", expected);
    
    /* Compare */
    if (strcmp(output, expected) == 0) {
        printf("✓ PASS\n");
        test_passed++;
    } else {
        printf("❌ FAIL\n");
    }
    
    newmm_free_result(tokens, token_count);
}

int main() {
    printf("=== CThaiNLP newmm Tokenizer Test Suite ===\n");
    
    const char* dict = "data/thai_words.txt";
    
    /* Test 1: Basic Thai sentence */
    run_test("ฉันไปโรงเรียน", dict, 
             "['ฉัน', 'ไป', 'โรงเรียน']",
             "Basic Thai sentence");
    
    /* Test 2: Thai sentence with common words */
    run_test("วันนี้อากาศดีมาก", dict,
             "['วันนี้', 'อา', 'กา', 'ศดี', 'มาก']",
             "Thai sentence with partial dictionary match");
    
    /* Test 3: English text */
    run_test("hello world", dict,
             "['hello', ' ', 'world']",
             "English text");
    
    /* Test 4: Numbers */
    run_test("123", dict,
             "['123']",
             "Numbers only");
    
    /* Test 5: Mixed content */
    run_test("ไป ABC 123", dict,
             "['ไป', ' ', 'ABC', ' ', '123']",
             "Mixed Thai, English, and numbers");
    
    /* Test 6: Empty string */
    run_test("", dict,
             "[]",
             "Empty string");
    
    /* Test 7: Single Thai word */
    run_test("ไป", dict,
             "['ไป']",
             "Single Thai word");
    
    /* Test 8: Using default dictionary */
    run_test("ฉันไปโรงเรียน", NULL,
             "['ฉั', 'น', 'ไป', 'โรง', 'เรี', 'ยน']",
             "Default dictionary (limited words)");
    
    /* Summary */
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_count - test_passed);
    
    if (test_passed == test_count) {
        printf("\n✓ All tests passed!\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed\n");
        return 1;
    }
}
