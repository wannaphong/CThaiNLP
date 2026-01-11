# Makefile for CThaiNLP

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
EXAMPLES_DIR = examples
LIB_DIR = lib

# Source files
SOURCES = $(SRC_DIR)/trie.c $(SRC_DIR)/tcc.c $(SRC_DIR)/newmm.c
OBJECTS = $(BUILD_DIR)/trie.o $(BUILD_DIR)/tcc.o $(BUILD_DIR)/newmm.o

# Library
LIBRARY = $(LIB_DIR)/libcthainlp.a

# Example programs
EXAMPLE_BASIC = $(BUILD_DIR)/example_basic

# Default target
all: dirs $(LIBRARY) $(EXAMPLE_BASIC)

# Create directories
dirs:
	@mkdir -p $(BUILD_DIR) $(LIB_DIR)

# Build object files
$(BUILD_DIR)/trie.o: $(SRC_DIR)/trie.c $(SRC_DIR)/trie.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tcc.o: $(SRC_DIR)/tcc.c $(SRC_DIR)/tcc.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/newmm.o: $(SRC_DIR)/newmm.c $(SRC_DIR)/trie.h $(SRC_DIR)/tcc.h $(INCLUDE_DIR)/newmm.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build library
$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

# Build example programs
$(EXAMPLE_BASIC): $(EXAMPLES_DIR)/example_basic.c $(LIBRARY)
	$(CC) $(CFLAGS) $< -L$(LIB_DIR) -lcthainlp -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

.PHONY: all dirs clean
