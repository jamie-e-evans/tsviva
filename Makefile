
# Compiler and flags
CC = cc
CPPFLAGS = -Iinclude
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

# Directories
BUILD_DIR = build

# Executable
TARGET = $(BUILD_DIR)/tsviva

# Sources
CLI_SRC = cli/main.c
LIB_SRC = src/tsv.c
SOURCES = $(CLI_SRC) $(LIB_SRC)

# Default target
.PHONY: all
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES) include/tsv.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@

# Create build directory if necessary
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run tests
.PHONY: test
test: $(TARGET)
	./tests/test_file_io.sh

# Remove generated files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
