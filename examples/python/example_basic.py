#!/usr/bin/env python3
"""
Basic example of using CThaiNLP Python bindings
"""

from cthainlp import word_tokenize


def main():
    # Example 1: Basic usage
    text = "ฉันไปโรงเรียน"
    print(f"Input text: {text}")
    
    tokens = word_tokenize(text)
    print(f"Tokens: {tokens}")
    print()
    
    # Example 2: More complex sentence
    text2 = "วันนี้อากาศดีมาก"
    print(f"Input text: {text2}")
    
    tokens2 = word_tokenize(text2)
    print(f"Tokens: {tokens2}")
    print()
    
    # Example 3: Mixed Thai, English, and numbers
    text3 = "ไป ABC 123"
    print(f"Input text: {text3}")
    
    tokens3 = word_tokenize(text3)
    print(f"Tokens: {tokens3}")
    print()
    
    # Example 4: With custom dictionary (optional)
    # Uncomment if you have a custom dictionary file
    # tokens_custom = word_tokenize(text, custom_dict="path/to/dict.txt")
    # print(f"Tokens with custom dict: {tokens_custom}")
    
    # Example 5: Explicitly specify engine
    tokens4 = word_tokenize(text, engine="newmm")
    print(f"Tokens with newmm engine: {tokens4}")


if __name__ == "__main__":
    main()
