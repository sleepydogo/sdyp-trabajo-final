/**
 * @file array_utils.h
 * @brief Utility functions for array manipulation and generation
 *
 * This module provides functions for creating, filling, and displaying arrays
 * used in parallel comparison algorithms.
 *
 * @authors Tomas E. Schattmann, Emiliano Mengoni
 * @date 2024
 */

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <stddef.h>

/**
 * @brief Allocates and initializes an array with random integers
 *
 * @param size Number of elements in the array
 * @param max_value Maximum random value (exclusive)
 * @return Pointer to the allocated array, or NULL on failure
 */
int *create_random_array(size_t size, int max_value);

/**
 * @brief Fills an existing array with random integers
 *
 * @param array The array to fill
 * @param size Number of elements to fill
 * @param max_value Maximum random value (exclusive)
 */
void fill_random_array(int *array, size_t size, int max_value);

/**
 * @brief Prints an array to stdout
 *
 * @param array The array to print
 * @param size Number of elements to print
 */
void print_array(const int *array, size_t size);

/**
 * @brief Compares two arrays for equality
 *
 * @param arr1 First array
 * @param arr2 Second array
 * @param size Number of elements to compare
 * @return 1 if arrays are equal, 0 otherwise
 */
int arrays_equal(const int *arr1, const int *arr2, size_t size);

#endif /* ARRAY_UTILS_H */
