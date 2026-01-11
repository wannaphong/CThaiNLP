# CThaiNLP

C implementation of Thai Natural Language Processing tools, ported from [PyThaiNLP](https://github.com/PyThaiNLP/pythainlp).

## Features

- **newmm**: Dictionary-based maximal matching word segmentation constrained by Thai Character Cluster (TCC) boundaries
- Similar API to PyThaiNLP for easy migration from Python to C
- UTF-8 support
- Efficient Trie data structure for dictionary lookup
- Handles mixed Thai/English/numeric content

## Building

### Prerequisites

- GCC or compatible C compiler
- Make

### Compilation

```bash
make
```

This will create:
- Static library: `lib/libcthainlp.a`
- Example program: `build/example_basic`

## Usage

### Basic Example

```c
#include "newmm.h"

int main() {
    const char* text = "ฉันไปโรงเรียน";
    int token_count;
    
    // Segment text (with NULL for dict_path to use default dictionary)
    char** tokens = newmm_segment(text, NULL, &token_count);
    
    // Print tokens
    for (int i = 0; i < token_count; i++) {
        printf("%s\n", tokens[i]);
    }
    
    // Free memory
    newmm_free_result(tokens, token_count);
    
    return 0;
}
```

### Compile Your Program

```bash
gcc your_program.c -I./include -L./lib -lcthainlp -o your_program
```

### Running Examples

Basic example with default dictionary:
```bash
./build/example_basic "ฉันไปโรงเรียน"
```

With custom dictionary:
```bash
./build/example_basic "ฉันไปโรงเรียน" data/thai_words.txt
```

### Running Tests

Run the test suite:
```bash
make test
```

This will compile and run all unit tests to verify the tokenizer is working correctly.

## API Reference

### Functions

#### `char** newmm_segment(const char* text, const char* dict_path, int* token_count)`

Segment Thai text into words using the newmm algorithm.

**Parameters:**
- `text`: Input text to segment (UTF-8 encoded)
- `dict_path`: Path to dictionary file (one word per line, UTF-8). Use `NULL` for default dictionary
- `token_count`: Output parameter - receives the number of tokens found

**Returns:**
- Array of strings (tokens), or `NULL` on error
- Caller must free the result using `newmm_free_result()`

**Example:**
```c
int count;
char** tokens = newmm_segment("ฉันไปโรงเรียน", "dict.txt", &count);
```

#### `void newmm_free_result(char** tokens, int token_count)`

Free memory allocated by `newmm_segment()`.

**Parameters:**
- `tokens`: Array of tokens returned by `newmm_segment()`
- `token_count`: Number of tokens in the array

**Example:**
```c
newmm_free_result(tokens, count);
```

## Dictionary Format

Dictionary files should contain one word per line in UTF-8 encoding:

```
ฉัน
ไป
โรงเรียน
วันนี้
อากาศ
ดี
มาก
```

A sample dictionary is provided in `data/thai_words.txt`.

## Comparison with PyThaiNLP

The API is designed to be similar to PyThaiNLP's `segment()` function:

**PyThaiNLP (Python):**
```python
from pythainlp.tokenize import word_tokenize

text = "ฉันไปโรงเรียน"
tokens = word_tokenize(text, engine="newmm")
print(tokens)  # ['ฉัน', 'ไป', 'โรงเรียน']
```

**CThaiNLP (C):**
```c
const char* text = "ฉันไปโรงเรียน";
int token_count;
char** tokens = newmm_segment(text, NULL, &token_count);
// tokens = ['ฉัน', 'ไป', 'โรงเรียน']
newmm_free_result(tokens, token_count);
```

## Algorithm

The newmm (New Maximum Matching) algorithm:

1. **Trie-based Dictionary Lookup**: Uses a trie data structure for efficient prefix matching
2. **Thai Character Cluster (TCC) Boundaries**: Respects Thai character cluster rules for valid word boundaries
3. **Maximal Matching**: Finds the longest dictionary word that matches at each position
4. **Fallback Handling**: Handles non-dictionary words and non-Thai characters (Latin, digits, etc.)

## Project Structure

```
CThaiNLP/
├── include/
│   └── newmm.h           # Public API header
├── src/
│   ├── newmm.c           # Main newmm implementation
│   ├── trie.c            # Trie data structure
│   ├── trie.h            # Trie header
│   ├── tcc.c             # Thai Character Cluster
│   └── tcc.h             # TCC header
├── examples/
│   └── example_basic.c   # Basic usage example
├── tests/
│   └── test_newmm.c      # Test suite
├── data/
│   └── thai_words.txt    # Sample dictionary
├── Makefile              # Build configuration
└── README.md             # This file
```

## Credits

- Original PyThaiNLP implementation: [PyThaiNLP Project](https://github.com/PyThaiNLP/pythainlp)
- newmm algorithm: Based on work by Korakot Chaovavanich
- TCC rules: Theeramunkong et al. 2000

## License

Apache License 2.0 (following PyThaiNLP's license)

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Future Enhancements

- [ ] Add more tokenization engines (attacut, deepcut, etc.)
- [ ] Improve performance with optimized data structures
- [ ] Add part-of-speech tagging
- [ ] Add named entity recognition
- [ ] Provide Python bindings (PyPI package)