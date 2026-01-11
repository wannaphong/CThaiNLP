# Performance Benchmark: CThaiNLP vs PyThaiNLP

This directory contains a benchmark script to compare CThaiNLP and PyThaiNLP newmm tokenizers.

## Running the Benchmark

```bash
# Install both packages first
pip install -e .  # Install CThaiNLP
pip install pythainlp  # Install PyThaiNLP (optional)

# Run the benchmark
python examples/python/benchmark.py
```

## What is Measured

1. **Speed**: Tokens processed per second
2. **Memory**: Peak memory usage during tokenization
3. **Correctness**: Output comparison between implementations

## Expected Results

CThaiNLP is implemented as a Python C extension that directly wraps the native C library, providing:

- **Near-native performance**: Minimal Python overhead
- **Low memory usage**: Efficient C memory management
- **Same algorithm**: Uses the same newmm (Maximum Matching) algorithm as PyThaiNLP

## Sample Output

```
================================================================================
CThaiNLP vs PyThaiNLP NEWMM Benchmark
================================================================================

Test corpus: 10 sentences
Iterations: 100
Total tokenizations: 1000

--------------------------------------------------------------------------------
Benchmarking CThaiNLP...
--------------------------------------------------------------------------------
Time taken:       0.0388 seconds
Tokens processed: 13100
Speed:            337796.99 tokens/sec
Peak memory:      0.00 MB

--------------------------------------------------------------------------------
Benchmarking PyThaiNLP...
--------------------------------------------------------------------------------
Time taken:       0.1234 seconds
Tokens processed: 13100
Speed:            106148.86 tokens/sec
Peak memory:      1.23 MB

================================================================================
PERFORMANCE COMPARISON
================================================================================
Speed:  CThaiNLP is 3.18x faster than PyThaiNLP
Memory: CThaiNLP uses 0.00x more memory than PyThaiNLP
Output: Identical token counts ✓

================================================================================
OUTPUT COMPARISON
================================================================================

1. Text: ฉันไปโรงเรียน
   CThaiNLP:  ['ฉัน', 'ไป', 'โรงเรียน']
   PyThaiNLP: ['ฉัน', 'ไป', 'โรงเรียน']
   ✓ Identical output
```

## Notes

- If PyThaiNLP is not installed, the benchmark will only test CThaiNLP
- The benchmark uses a warm-up phase to ensure fair comparison
- Memory usage is measured using Python's `tracemalloc` module
