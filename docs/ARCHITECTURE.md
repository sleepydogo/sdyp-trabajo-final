# Architecture Documentation

## Table of Contents

1. [Overview](#overview)
2. [Module Organization](#module-organization)
3. [Algorithm Design](#algorithm-design)
4. [Synchronization Mechanisms](#synchronization-mechanisms)
5. [Performance Considerations](#performance-considerations)

## Overview

The Parallel Array Comparator implements a divide-and-conquer approach to compare two large integer arrays efficiently using parallel processing. The architecture is designed with modularity and performance in mind.

## Module Organization

### Core Modules

#### `core/merge_sort`

**Purpose**: Implements the iterative merge sort algorithm

**Key Functions**:
- `merge()`: Merges two sorted subarrays
- `merge_sort_iterative()`: Sorts an array segment using bottom-up merge sort

**Design Decisions**:
- Iterative approach chosen over recursive for better cache locality
- In-place merging with temporary buffer allocation
- No recursion overhead, making it suitable for parallel execution

#### `utils/array_utils`

**Purpose**: Provides array manipulation utilities

**Key Functions**:
- `create_random_array()`: Allocates and initializes arrays
- `fill_random_array()`: Populates arrays with random values
- `print_array()`: Debug utility for displaying arrays
- `arrays_equal()`: Sequential comparison for verification

**Design Decisions**:
- Separation of concerns: array operations isolated from sorting logic
- Configurable random value ranges
- Memory safety with null checks

#### `shared_memory/parallel_comparator`

**Purpose**: Orchestrates parallel comparison using pthreads

**Key Components**:
- `comparator_config_t`: Configuration structure
- `thread_args_t`: Thread-specific parameters
- `thread_sort_and_compare()`: Thread worker function
- `parallel_compare_arrays()`: Main coordination function

**Design Decisions**:
- Thread-per-segment model
- Hierarchical barrier synchronization
- Separate barrier sets for even/odd threads

## Algorithm Design

### Phase 1: Parallel Sorting

```
Thread Assignment:
- Even threads (0, 2, 4, ...): Sort array1 segments
- Odd threads (1, 3, 5, ...): Sort array2 segments

Each thread:
1. Calculate segment boundaries
2. Sort segment using iterative merge sort
3. Wait at barrier
```

### Phase 2: Hierarchical Merging

```
Merging Pattern (for num_threads = 8):

Level 1: 8 threads → 4 merged segments
  Threads 0,2,4,6 merge with their pairs

Level 2: 4 threads → 2 merged segments
  Threads 0,4 merge

Level 3: 2 threads → 1 sorted array
  Thread 0 merges

Barriers ensure synchronization at each level
```

### Phase 3: Parallel Comparison

```
After both arrays are sorted:

Each thread:
1. Compare its segment of array1 vs array2
2. Update shared counter if segment matches
3. Exit early if mismatch found

Final check:
  Arrays equal if counter == num_threads
```

## Synchronization Mechanisms

### Barrier Synchronization

**Purpose**: Ensure all threads complete a phase before proceeding

**Implementation**:
```c
pthread_barrier_t barriers_even[NUM_THREADS + 1];
pthread_barrier_t barriers_odd[NUM_THREADS + 1];
pthread_barrier_t barrier_full;
```

**Barrier Hierarchy**:
- `barriers_even[32]`: 32 even threads
- `barriers_even[16]`: 16 threads after first merge
- `barriers_even[8]`: 8 threads after second merge
- ...
- `barrier_full`: All threads before comparison

### Semaphore Protection

**Purpose**: Protect shared comparison result counter

```c
sem_wait(&semaphore);
comparison_result += is_equal;
sem_post(&semaphore);
```

## Performance Considerations

### Cache Optimization

1. **Iterative Merge Sort**: Better cache locality than recursive
2. **Contiguous Memory**: Arrays allocated as single blocks
3. **Thread Affinity**: OS typically assigns threads to cores efficiently

### Memory Access Patterns

```
Thread 0: [0 ----------- N/P)
Thread 1:     [N/P ------- 2N/P)
Thread 2:         [2N/P --- 3N/P)
...

Each thread accesses contiguous memory regions
Minimizes cache line conflicts between threads
```

### Bottlenecks

1. **Barrier Synchronization**: All threads must wait for slowest thread
2. **Memory Bandwidth**: Limited by RAM throughput for large arrays
3. **Merge Overhead**: Hierarchical merging requires coordination

### Scaling Characteristics

**Strong Scaling** (fixed problem size, increasing processors):
- Limited by Amdahl's Law
- Sequential portions: initialization, final comparison
- Synchronization overhead increases with thread count

**Weak Scaling** (proportional problem and processor increase):
- Better but not ideal
- Memory bandwidth becomes bottleneck
- Barrier synchronization overhead

## Thread Safety

### Shared Data Structures

| Structure | Access Pattern | Protection |
|-----------|----------------|------------|
| `array1`, `array2` | Disjoint segments | None needed (implicit) |
| `comparison_result` | Read-modify-write | Semaphore |
| Barriers | Multiple threads | Built-in pthread barriers |

### Race Condition Prevention

1. **Segment Isolation**: Each thread writes to distinct array regions
2. **Atomic Operations**: Semaphore-protected counter updates
3. **Barrier Coordination**: Prevents premature data access

## Error Handling

### Memory Allocation Failures

```c
if (array == NULL) {
    fprintf(stderr, "Allocation failed\n");
    exit(EXIT_FAILURE);
}
```

### Thread Creation Failures

```c
if (pthread_create(...) != 0) {
    fprintf(stderr, "Thread creation failed\n");
    return -1;
}
```

### Parameter Validation

- Thread count must be power of 2
- Array size must be positive
- Arguments checked before processing

## Future Architectural Improvements

1. **Dynamic Thread Pool**: Avoid creation/destruction overhead
2. **Work Stealing**: Balance load for irregular data
3. **NUMA Awareness**: Optimize for multi-socket systems
4. **Hybrid Parallelism**: Combine threads with SIMD instructions
5. **Adaptive Algorithm**: Choose strategy based on input size

## References

- POSIX Threads Programming: https://computing.llnl.gov/tutorials/pthreads/
- Synchronization Primitives: https://man7.org/linux/man-pages/man7/pthreads.7.html
- Parallel Algorithm Design: Grama et al., "Introduction to Parallel Computing"
