# API Documentation

## Table of Contents

1. [Core Module](#core-module)
2. [Utils Module](#utils-module)
3. [Shared Memory Module](#shared-memory-module)
4. [Data Structures](#data-structures)

---

## Core Module

### Header: `core/merge_sort.h`

#### `merge()`

Merges two adjacent sorted subarrays into a single sorted array.

**Signature**:
```c
void merge(int *array, int start, int mid, int end)
```

**Parameters**:
- `array`: Pointer to the array containing both subarrays
- `start`: Starting index of the first subarray
- `mid`: Ending index of the first subarray (mid + 1 is start of second subarray)
- `end`: Ending index of the second subarray (inclusive)

**Preconditions**:
- `array[start..mid]` must be sorted
- `array[mid+1..end]` must be sorted
- `start <= mid < end`

**Postconditions**:
- `array[start..end]` is sorted

**Time Complexity**: O(n) where n = end - start + 1

**Space Complexity**: O(n) for temporary arrays

**Example**:
```c
int arr[] = {1, 5, 9, 2, 4, 7};
merge(arr, 0, 2, 5);  // Merges [1,5,9] with [2,4,7]
// Result: arr = {1, 2, 4, 5, 7, 9}
```

---

#### `merge_sort_iterative()`

Sorts an array segment using iterative (bottom-up) merge sort.

**Signature**:
```c
void merge_sort_iterative(int *array, int start, int end)
```

**Parameters**:
- `array`: Pointer to the array to sort
- `start`: Starting index of the segment to sort
- `end`: Ending index of the segment to sort (inclusive)

**Preconditions**:
- `array` is a valid pointer
- `start <= end`
- Sufficient memory available for temporary buffer

**Postconditions**:
- `array[start..end]` is sorted in ascending order

**Time Complexity**: O(n log n) where n = end - start + 1

**Space Complexity**: O(n) for temporary buffer

**Example**:
```c
int arr[] = {64, 34, 25, 12, 22, 11, 90};
merge_sort_iterative(arr, 0, 6);
// Result: arr = {11, 12, 22, 25, 34, 64, 90}
```

---

## Utils Module

### Header: `utils/array_utils.h`

#### `create_random_array()`

Allocates and initializes an array with random integers.

**Signature**:
```c
int *create_random_array(size_t size, int max_value)
```

**Parameters**:
- `size`: Number of elements in the array
- `max_value`: Maximum random value (exclusive), minimum is 1

**Returns**:
- Pointer to allocated array on success
- `NULL` on memory allocation failure

**Postconditions**:
- Returned array contains `size` random integers in range [1, max_value)
- Caller is responsible for freeing the returned pointer

**Example**:
```c
int *arr = create_random_array(1000, 100);
if (arr != NULL) {
    // Use array...
    free(arr);
}
```

---

#### `fill_random_array()`

Fills an existing array with random integers.

**Signature**:
```c
void fill_random_array(int *array, size_t size, int max_value)
```

**Parameters**:
- `array`: Pointer to the array to fill
- `size`: Number of elements to fill
- `max_value`: Maximum random value (exclusive), minimum is 1

**Preconditions**:
- `array` must point to allocated memory of at least `size` integers
- `srand()` should be called before for varied randomness

**Postconditions**:
- Array is filled with random values in range [1, max_value)

**Example**:
```c
int arr[100];
srand(time(NULL));
fill_random_array(arr, 100, 1000);
```

---

#### `print_array()`

Prints an array to standard output.

**Signature**:
```c
void print_array(const int *array, size_t size)
```

**Parameters**:
- `array`: Pointer to the array to print
- `size`: Number of elements to print

**Output Format**: Space-separated integers followed by newline

**Example**:
```c
int arr[] = {1, 2, 3, 4, 5};
print_array(arr, 5);
// Output: 1 2 3 4 5
```

---

#### `arrays_equal()`

Compares two arrays for equality.

**Signature**:
```c
int arrays_equal(const int *arr1, const int *arr2, size_t size)
```

**Parameters**:
- `arr1`: First array
- `arr2`: Second array
- `size`: Number of elements to compare

**Returns**:
- `1` if arrays are equal (all elements match)
- `0` if arrays differ

**Time Complexity**: O(n), worst case compares all elements

**Example**:
```c
int a[] = {1, 2, 3};
int b[] = {1, 2, 3};
int c[] = {1, 2, 4};

arrays_equal(a, b, 3);  // Returns 1
arrays_equal(a, c, 3);  // Returns 0
```

---

## Shared Memory Module

### Header: `shared_memory/parallel_comparator.h`

#### `parallel_compare_arrays()`

Compares two arrays using parallel sorting and comparison.

**Signature**:
```c
int parallel_compare_arrays(comparator_config_t *config)
```

**Parameters**:
- `config`: Pointer to configuration structure containing:
  - `array1`: First array to compare
  - `array2`: Second array to compare
  - `size`: Size of both arrays
  - `num_threads`: Number of threads to use (must be power of 2)

**Returns**:
- `1` if arrays are equal after sorting
- `0` if arrays differ
- `-1` on error (memory allocation or thread creation failure)

**Preconditions**:
- Both arrays allocated with `size` elements
- `num_threads` is a power of 2
- `size` is evenly divisible by `num_threads`

**Postconditions**:
- Both arrays are sorted (side effect)
- Thread resources are cleaned up

**Time Complexity**: O(n log n / p) where p = num_threads (theoretical)

**Example**:
```c
int *arr1 = create_random_array(1024, 100);
int *arr2 = create_random_array(1024, 100);

comparator_config_t config = {
    .array1 = arr1,
    .array2 = arr2,
    .size = 1024,
    .num_threads = 4
};

int result = parallel_compare_arrays(&config);
printf("Arrays are %s\n", result ? "equal" : "different");

free(arr1);
free(arr2);
```

---

#### `thread_sort_and_compare()`

Thread worker function for parallel sorting and comparison.

**Signature**:
```c
void *thread_sort_and_compare(void *arg)
```

**Parameters**:
- `arg`: Pointer to `thread_args_t` structure

**Returns**:
- `NULL` (pthread requirement)

**Thread Lifecycle**:
1. Sort assigned segment using iterative merge sort
2. Participate in hierarchical merging with barriers
3. Compare assigned segment after full sort
4. Update shared comparison result
5. Exit

**Note**: This function is called internally by `parallel_compare_arrays()` and should not be called directly.

---

## Data Structures

### `comparator_config_t`

Configuration for parallel array comparison.

```c
typedef struct {
    int *array1;        // First array to compare
    int *array2;        // Second array to compare
    size_t size;        // Size of both arrays
    int num_threads;    // Number of threads to use
} comparator_config_t;
```

**Constraints**:
- `num_threads` must be a power of 2 (2, 4, 8, 16, ...)
- `size` should be evenly divisible by `num_threads`

---

### `thread_args_t`

Thread-specific arguments for parallel execution.

```c
typedef struct {
    int thread_id;                    // Unique thread identifier
    int *array1;                      // Pointer to first array
    int *array2;                      // Pointer to second array
    size_t array_size;                // Total size of arrays
    int num_threads;                  // Total number of threads
    pthread_barrier_t *barriers_even; // Barriers for even threads
    pthread_barrier_t *barriers_odd;  // Barriers for odd threads
    pthread_barrier_t *barrier_full;  // Final barrier for all threads
    sem_t *semaphore;                 // Semaphore for synchronization
    int *comparison_result;           // Shared result counter
} thread_args_t;
```

**Usage**: Internal structure managed by `parallel_compare_arrays()`

---

## Error Codes

Functions in this library use the following error conventions:

| Return Value | Meaning |
|-------------|---------|
| `1` | Success / True |
| `0` | Failure / False |
| `-1` | Error condition |
| `NULL` | Memory allocation failure |

---

## Thread Safety

### Thread-Safe Functions

- `thread_sort_and_compare()` - Designed for parallel execution
- `merge()` - Safe when operating on disjoint array segments
- `merge_sort_iterative()` - Safe when operating on disjoint segments

### Not Thread-Safe

- `create_random_array()` - Uses `rand()`, call with single thread or use thread-safe RNG
- `fill_random_array()` - Uses `rand()`, same caveat as above
- `print_array()` - Multiple threads writing to stdout will interleave output

---

## Memory Management

All allocated memory should be freed by the caller:

```c
// Array allocation
int *arr = create_random_array(1000, 100);
// ... use array ...
free(arr);  // Caller's responsibility
```

Internal temporary allocations (merge buffers) are automatically freed within functions.

---

## Performance Tips

1. **Array Size**: Use multiples of thread count for even work distribution
2. **Thread Count**: Power of 2 required; optimal is typically 8-16 on modern CPUs
3. **Cache Effects**: Larger arrays may experience slowdown due to cache misses
4. **Randomness**: Call `srand(time(NULL))` once at program start
5. **Verification**: Use `arrays_equal()` for testing, but it's sequential

---

## Example Programs

### Minimal Example

```c
#include "shared_memory/parallel_comparator.h"
#include "utils/array_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    int *arr1 = create_random_array(1024, 100);
    int *arr2 = create_random_array(1024, 100);

    comparator_config_t config = {arr1, arr2, 1024, 4};
    int result = parallel_compare_arrays(&config);

    printf("Result: %s\n", result ? "EQUAL" : "DIFFERENT");

    free(arr1);
    free(arr2);
    return 0;
}
```

### Benchmarking Example

```c
#include "shared_memory/parallel_comparator.h"
#include "utils/array_utils.h"
#include <stdio.h>
#include <time.h>

int main() {
    size_t size = 1 << 24;
    int threads = 16;

    int *arr1 = create_random_array(size, 1000);
    int *arr2 = create_random_array(size, 1000);

    comparator_config_t config = {arr1, arr2, size, threads};

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int result = parallel_compare_arrays(&config);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Time: %.6f seconds\n", elapsed);
    printf("Result: %s\n", result ? "EQUAL" : "DIFFERENT");

    free(arr1);
    free(arr2);
    return 0;
}
```
