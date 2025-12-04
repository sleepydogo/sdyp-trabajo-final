# Makefile for Parallel Array Comparator
# Authors: Tomas E. Schattmann, Emiliano Mengoni
# Date: 2024

CC = gcc
CFLAGS = -Wall -Wextra -O3 -pthread -Iinclude
LDFLAGS = -pthread

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

CORE_SOURCES = $(SRC_DIR)/core/merge_sort.c \
               $(SRC_DIR)/core/barrier.c
UTILS_SOURCES = $(SRC_DIR)/utils/array_utils.c
SHARED_MEM_SOURCES = $(SRC_DIR)/shared_memory/parallel_comparator.c \
                     $(SRC_DIR)/shared_memory/main.c

SOURCES = $(CORE_SOURCES) $(UTILS_SOURCES) $(SHARED_MEM_SOURCES)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

TARGET = $(BUILD_DIR)/parallel_comparator

.PHONY: all clean test benchmark help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Build successful: $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

test: $(TARGET)
	@echo "Running quick test..."
	@$(TARGET) -n 1024 -t 4 -v

benchmark: $(TARGET)
	@echo "=== Running Benchmarks ==="
	@echo "\n--- Benchmark 1: N=2^20, Threads=4 ---"
	@$(TARGET) -n 1048576 -t 4
	@echo "\n--- Benchmark 2: N=2^22, Threads=8 ---"
	@$(TARGET) -n 4194304 -t 8
	@echo "\n--- Benchmark 3: N=2^24, Threads=16 ---"
	@$(TARGET) -n 16777216 -t 16

help:
	@echo "Parallel Array Comparator - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  clean      - Remove build artifacts"
	@echo "  test       - Run a quick test"
	@echo "  benchmark  - Run performance benchmarks"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make"
	@echo "  make clean && make"
	@echo "  make test"
	@echo "  make benchmark"

# Dependency tracking
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT $(BUILD_DIR)/$*.o $< > $@
