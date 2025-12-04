/**
 * @file main.c
 * @brief Main entry point for parallel array comparator
 *
 * This program compares two large arrays using parallel processing with
 * shared memory (pthreads). It generates random arrays, sorts them in
 * parallel, and compares them for equality.
 *
 * @authors Tomas E. Schattmann, Emiliano Mengoni
 * @date 2024
 */

#include "shared_memory/parallel_comparator.h"
#include "utils/array_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DEFAULT_ARRAY_SIZE (1 << 24)
#define DEFAULT_NUM_THREADS 4
#define MAX_RANDOM_VALUE 1000

/**
 * @brief Prints usage information
 */
void print_usage(const char *program_name)
{
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\nOptions:\n");
    printf("  -n, --size SIZE        Array size (default: 2^24 = %d)\n", DEFAULT_ARRAY_SIZE);
    printf("  -t, --threads THREADS  Number of threads (default: %d)\n", DEFAULT_NUM_THREADS);
    printf("  -v, --verbose          Enable verbose output\n");
    printf("  -h, --help             Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -n 1048576 -t 8\n", program_name);
    printf("  %s --size 16777216 --threads 16\n", program_name);
}

int main(int argc, char *argv[])
{
    size_t array_size = DEFAULT_ARRAY_SIZE;
    int num_threads = DEFAULT_NUM_THREADS;
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--size") == 0) {
            if (i + 1 < argc) {
                array_size = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
            if (i + 1 < argc) {
                num_threads = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    if (num_threads <= 0 || (num_threads & (num_threads - 1)) != 0) {
        fprintf(stderr, "Error: Number of threads must be a power of 2\n");
        return 1;
    }

    printf("=== Parallel Array Comparator ===\n");
    printf("Array size: %zu elements\n", array_size);
    printf("Number of threads: %d\n", num_threads);
    printf("Max random value: %d\n\n", MAX_RANDOM_VALUE);

    srand(time(NULL));

    int *array1 = create_random_array(array_size, MAX_RANDOM_VALUE);
    int *array2 = create_random_array(array_size, MAX_RANDOM_VALUE);

    if (array1 == NULL || array2 == NULL) {
        fprintf(stderr, "Failed to allocate arrays\n");
        free(array1);
        free(array2);
        return 1;
    }

    if (verbose && array_size <= 20) {
        printf("Array 1: ");
        print_array(array1, array_size);
        printf("Array 2: ");
        print_array(array2, array_size);
        printf("\n");
    }

    comparator_config_t config = {
        .array1 = array1,
        .array2 = array2,
        .size = array_size,
        .num_threads = num_threads
    };

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int result = parallel_compare_arrays(&config);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_time = (end.tv_sec - start.tv_sec) +
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    if (verbose && array_size <= 20) {
        printf("Sorted Array 1: ");
        print_array(array1, array_size);
        printf("Sorted Array 2: ");
        print_array(array2, array_size);
        printf("\n");
    }

    printf("Result: Arrays are %s\n", result ? "EQUAL" : "DIFFERENT");
    printf("Execution time: %.6f seconds\n", elapsed_time);

    free(array1);
    free(array2);

    return 0;
}
