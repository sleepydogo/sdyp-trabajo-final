/**
 * @file parallel_comparator.h
 * @brief Parallel array comparator using shared memory (pthreads)
 *
 * This module implements a parallel algorithm to compare two arrays using
 * multiple threads. The algorithm sorts both arrays in parallel and then
 * compares them element by element.
 *
 * @authors Tomas E. Schattmann, Emiliano Mengoni
 * @date 2024
 */

#ifndef PARALLEL_COMPARATOR_H
#define PARALLEL_COMPARATOR_H

#include <stddef.h>
#include <pthread.h>
#include "core/barrier.h"

/**
 * @brief Configuration for parallel array comparison
 */
typedef struct {
    int *array1;           /**< First array to compare */
    int *array2;           /**< Second array to compare */
    size_t size;           /**< Size of both arrays */
    int num_threads;       /**< Number of threads to use */
} comparator_config_t;

/**
 * @brief Thread-specific arguments for parallel comparison
 */
typedef struct {
    int thread_id;         /**< Unique thread identifier */
    int *array1;           /**< Pointer to first array */
    int *array2;           /**< Pointer to second array */
    size_t array_size;     /**< Total size of arrays */
    int num_threads;       /**< Total number of threads */
    pthread_barrier_t *barriers_even;     /**< Barriers for even threads */
    pthread_barrier_t *barriers_odd;      /**< Barriers for odd threads */
    pthread_barrier_t *barrier_full;      /**< Final barrier for all threads */
    pthread_mutex_t *mutex;               /**< Mutex for synchronization */
    int *comparison_result;               /**< Shared result counter */
} thread_args_t;

/**
 * @brief Compares two arrays using parallel sorting and comparison
 *
 * This function creates multiple threads to sort and compare two arrays
 * in parallel. The arrays are sorted using an iterative merge sort
 * algorithm, then compared element by element.
 *
 * @param config Configuration containing arrays and execution parameters
 * @return 1 if arrays are equal, 0 otherwise
 */
int parallel_compare_arrays(comparator_config_t *config);

/**
 * @brief Thread function for parallel sorting and comparison
 *
 * Each thread sorts its portion of both arrays, then participates in
 * merging operations, and finally compares corresponding segments.
 *
 * @param arg Pointer to thread_args_t structure
 * @return NULL
 */
void *thread_sort_and_compare(void *arg);

#endif /* PARALLEL_COMPARATOR_H */
