#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Benchmark comparison between CThaiNLP and PyThaiNLP newmm tokenizers

This script compares:
- Speed (tokens per second)
- Memory usage
- Correctness (output comparison)
"""

import sys
import io
import time
import tracemalloc
from typing import List, Tuple

# Fix Windows console encoding for Thai text
if sys.platform == "win32":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

try:
    from cthainlp import word_tokenize as cthainlp_tokenize
    CTHAINLP_AVAILABLE = True
except ImportError:
    print("Warning: CThaiNLP not available")
    CTHAINLP_AVAILABLE = False

try:
    from pythainlp import word_tokenize as pythainlp_tokenize
    PYTHAINLP_AVAILABLE = True
except ImportError:
    print("Warning: PyThaiNLP not available")
    PYTHAINLP_AVAILABLE = False


# Test corpus - mix of short and long sentences
TEST_CORPUS = [
    "ฉันไปโรงเรียน",
    "วันนี้อากาศดีมาก",
    "ประเทศไทยมีวัฒนธรรมที่หลากหลายและน่าสนใจ",
    "เขาชอบกินข้าวผัดกับไก่ทอด",
    "มหาวิทยาลัยเทคโนโลยีพระจอมเกล้าธนบุรี",
    "การศึกษาเป็นสิ่งสำคัญสำหรับการพัฒนาประเทศ",
    "ฉันชอบอ่านหนังสือและฟังเพลง",
    "โควิด-19 ส่งผลกระทบต่อเศรษฐกิจโลก",
    "ปัญญาประดิษฐ์กำลังเปลี่ยนแปลงโลก",
    "ผลไม้ไทยมีหลายชนิดเช่นมะม่วงและทุเรียน"
]


def benchmark_tokenizer(tokenize_func, corpus: List[str], engine: str, iterations: int = 100) -> Tuple[float, float, int]:
    """
    Benchmark a tokenizer function
    
    Returns:
        (time_taken, memory_peak_mb, total_tokens)
    """
    # Warm up
    for text in corpus[:2]:
        tokenize_func(text, engine=engine)
    
    # Start memory tracking
    tracemalloc.start()
    
    # Benchmark
    start_time = time.time()
    total_tokens = 0
    
    for _ in range(iterations):
        for text in corpus:
            tokens = tokenize_func(text, engine=engine)
            total_tokens += len(tokens)
    
    end_time = time.time()
    
    # Get memory usage
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    time_taken = end_time - start_time
    memory_peak_mb = peak / 1024 / 1024
    
    return time_taken, memory_peak_mb, total_tokens


def compare_outputs(corpus: List[str]):
    """Compare tokenization outputs between CThaiNLP and PyThaiNLP"""
    print("\n" + "="*80)
    print("OUTPUT COMPARISON")
    print("="*80)
    
    for i, text in enumerate(corpus[:5], 1):  # Compare first 5 sentences
        print(f"\n{i}. Text: {text}")
        
        if CTHAINLP_AVAILABLE:
            tokens_c = cthainlp_tokenize(text, engine="newmm")
            print(f"   CThaiNLP:  {tokens_c}")
        
        if PYTHAINLP_AVAILABLE:
            tokens_p = pythainlp_tokenize(text, engine="newmm")
            print(f"   PyThaiNLP: {tokens_p}")
        
        if CTHAINLP_AVAILABLE and PYTHAINLP_AVAILABLE:
            if tokens_c == tokens_p:
                print("   ✓ Identical output")
            else:
                print("   ✗ Different output")
                # Show differences
                if len(tokens_c) != len(tokens_p):
                    print(f"     Token count: CThaiNLP={len(tokens_c)}, PyThaiNLP={len(tokens_p)}")


def main():
    print("="*80)
    print("CThaiNLP vs PyThaiNLP NEWMM Benchmark")
    print("="*80)
    
    if not CTHAINLP_AVAILABLE and not PYTHAINLP_AVAILABLE:
        print("Error: Neither CThaiNLP nor PyThaiNLP is available")
        return
    
    print(f"\nTest corpus: {len(TEST_CORPUS)} sentences")
    print(f"Iterations: 100")
    print(f"Total tokenizations: {len(TEST_CORPUS) * 100}")
    
    # Benchmark CThaiNLP
    if CTHAINLP_AVAILABLE:
        print("\n" + "-"*80)
        print("Benchmarking CThaiNLP...")
        print("-"*80)
        try:
            time_c, mem_c, tokens_c = benchmark_tokenizer(cthainlp_tokenize, TEST_CORPUS, "newmm")
            print(f"Time taken:       {time_c:.4f} seconds")
            print(f"Tokens processed: {tokens_c}")
            print(f"Speed:            {tokens_c/time_c:.2f} tokens/sec")
            print(f"Peak memory:      {mem_c:.2f} MB")
        except Exception as e:
            print(f"Error: {e}")
            time_c, mem_c, tokens_c = None, None, None
    else:
        time_c, mem_c, tokens_c = None, None, None
    
    # Benchmark PyThaiNLP
    if PYTHAINLP_AVAILABLE:
        print("\n" + "-"*80)
        print("Benchmarking PyThaiNLP...")
        print("-"*80)
        try:
            time_p, mem_p, tokens_p = benchmark_tokenizer(pythainlp_tokenize, TEST_CORPUS, "newmm")
            print(f"Time taken:       {time_p:.4f} seconds")
            print(f"Tokens processed: {tokens_p}")
            print(f"Speed:            {tokens_p/time_p:.2f} tokens/sec")
            print(f"Peak memory:      {mem_p:.2f} MB")
        except Exception as e:
            print(f"Error: {e}")
            time_p, mem_p, tokens_p = None, None, None
    else:
        time_p, mem_p, tokens_p = None, None, None
    
    # Compare results
    if time_c is not None and time_p is not None:
        print("\n" + "="*80)
        print("PERFORMANCE COMPARISON")
        print("="*80)
        speedup = time_p / time_c
        print(f"Speed:  CThaiNLP is {speedup:.2f}x {'faster' if speedup > 1 else 'slower'} than PyThaiNLP")
        
        mem_ratio = mem_c / mem_p if mem_p > 0 else 0
        print(f"Memory: CThaiNLP uses {mem_ratio:.2f}x {'more' if mem_ratio > 1 else 'less'} memory than PyThaiNLP")
        
        if tokens_c == tokens_p:
            print("Output: Identical token counts ✓")
        else:
            print(f"Output: Different token counts (CThaiNLP: {tokens_c}, PyThaiNLP: {tokens_p})")
    
    # Compare outputs
    if CTHAINLP_AVAILABLE and PYTHAINLP_AVAILABLE:
        compare_outputs(TEST_CORPUS)
    
    print("\n" + "="*80)
    print("Benchmark completed")
    print("="*80)


if __name__ == "__main__":
    main()
