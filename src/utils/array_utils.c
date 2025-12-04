/**
 * @file array_utils.c
 * @brief Implementation of array utility functions
 *
 * @authors Tomas E. Schattmann, Emiliano Mengoni
 * @date 2024
 */

#include "utils/array_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int *create_random_array(size_t size, int max_value)
{
    int *array = (int *)malloc(size * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Failed to allocate memory for array of size %zu\n", size);
        return NULL;
    }

    fill_random_array(array, size, max_value);
    return array;
}

void fill_random_array(int *array, size_t size, int max_value)
{
    for (size_t i = 0; i < size; i++) {
        array[i] = (rand() % max_value) + 1;
    }
}

void print_array(const int *array, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int arrays_equal(const int *arr1, const int *arr2, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }
    return 1;
}
