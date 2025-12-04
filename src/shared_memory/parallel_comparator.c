/**
 * @file parallel_comparator.c
 * @brief Implementation of parallel array comparison using pthreads
 *
 * @authors Tomas E. Schattmann, Emiliano Mengoni
 * @date 2024
 */

#include "shared_memory/parallel_comparator.h"
#include "core/merge_sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *thread_sort_and_compare(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    int thread_id = args->thread_id;
    int *array1 = args->array1;
    int *array2 = args->array2;
    int num_threads = args->num_threads;
    size_t array_size = args->array_size;

    int *sorting_array;
    int segment_size = (array_size * 2) / num_threads;

    int pos_start = (thread_id / 2) * segment_size;
    int pos_end = pos_start + segment_size - 1;

    if (thread_id % 2 == 0) {
        sorting_array = array1;
    } else {
        sorting_array = array2;
    }

    merge_sort_iterative(sorting_array, pos_start, pos_end);

    if (thread_id % 2 == 0) {
        pthread_barrier_wait(&args->barriers_even[num_threads / 2]);

        for (int i = 4; i <= num_threads; i *= 2) {
            int pos_mid = pos_end;
            pos_end = pos_start + segment_size * (i / 2) - 1;

            if ((thread_id % i == 0) || (thread_id == 0)) {
                merge(sorting_array, pos_start, pos_mid, pos_end);
                pthread_barrier_wait(&args->barriers_even[num_threads / i]);
            }
        }
    } else {
        pthread_barrier_wait(&args->barriers_odd[num_threads / 2]);

        for (int i = 4; i <= num_threads; i *= 2) {
            int pos_mid = pos_end;
            pos_end = pos_start + segment_size * (i / 2) - 1;

            if (((thread_id - 1) % i == 0) || (thread_id == 1)) {
                merge(sorting_array, pos_start, pos_mid, pos_end);
                pthread_barrier_wait(&args->barriers_odd[num_threads / i]);
            }
        }
    }

    pthread_barrier_wait(args->barrier_full);

    int is_equal = 1;
    segment_size = array_size / num_threads;
    pos_start = thread_id * segment_size;
    pos_end = pos_start + segment_size - 1;

    for (int i = pos_start; i <= pos_end; i++) {
        if (array1[i] != array2[i]) {
            is_equal = 0;
            break;
        }
    }

    pthread_mutex_lock(args->mutex);
    *(args->comparison_result) += is_equal;
    pthread_mutex_unlock(args->mutex);

    pthread_exit(NULL);
}

int parallel_compare_arrays(comparator_config_t *config)
{
    pthread_t *threads = malloc(config->num_threads * sizeof(pthread_t));
    thread_args_t *thread_args = malloc(config->num_threads * sizeof(thread_args_t));
    pthread_barrier_t *barriers_even = malloc((config->num_threads + 1) * sizeof(pthread_barrier_t));
    pthread_barrier_t *barriers_odd = malloc((config->num_threads + 1) * sizeof(pthread_barrier_t));
    pthread_barrier_t barrier_full;
    pthread_mutex_t mutex;
    int comparison_result = 0;

    if (threads == NULL || thread_args == NULL || barriers_even == NULL || barriers_odd == NULL) {
        fprintf(stderr, "Failed to allocate memory for thread structures\n");
        return -1;
    }

    pthread_mutex_init(&mutex, NULL);

    for (int j = 1; j <= config->num_threads; j *= 2) {
        pthread_barrier_init(&barriers_even[j], NULL, j);
        pthread_barrier_init(&barriers_odd[j], NULL, j);
    }
    pthread_barrier_init(&barrier_full, NULL, config->num_threads);

    for (int i = 0; i < config->num_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].array1 = config->array1;
        thread_args[i].array2 = config->array2;
        thread_args[i].array_size = config->size;
        thread_args[i].num_threads = config->num_threads;
        thread_args[i].barriers_even = barriers_even;
        thread_args[i].barriers_odd = barriers_odd;
        thread_args[i].barrier_full = &barrier_full;
        thread_args[i].mutex = &mutex;
        thread_args[i].comparison_result = &comparison_result;

        if (pthread_create(&threads[i], NULL, thread_sort_and_compare, &thread_args[i]) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            return -1;
        }
    }

    for (int i = 0; i < config->num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int result = (comparison_result == config->num_threads);

    pthread_mutex_destroy(&mutex);
    for (int j = 1; j <= config->num_threads; j *= 2) {
        pthread_barrier_destroy(&barriers_even[j]);
        pthread_barrier_destroy(&barriers_odd[j]);
    }
    pthread_barrier_destroy(&barrier_full);

    free(threads);
    free(thread_args);
    free(barriers_even);
    free(barriers_odd);

    return result;
}
