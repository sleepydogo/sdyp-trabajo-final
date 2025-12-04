# Parallel Array Comparator

A high-performance parallel algorithm for comparing large integer arrays using shared memory (pthreads) and distributed memory (MPI) implementations.

## Authors

- **Tomas E. Schattmann**
- **Emiliano Mengoni**

## Overview

This project implements an efficient algorithm to compare two arrays of integers (unsorted and with duplicate elements) to determine if they are equal or different. The solution leverages parallel processing using two different paradigms:

- **Shared Memory**: Using POSIX threads (pthreads)
- **Distributed Memory**: Using MPI (Message Passing Interface) [Coming soon]

### Algorithm Description

The comparison algorithm follows these steps:

1. **Parallel Sorting**: Both arrays are divided into segments, with each thread/process sorting its assigned segment using an iterative merge sort algorithm
2. **Hierarchical Merging**: Sorted segments are progressively merged using a binary tree pattern with synchronization barriers
3. **Parallel Comparison**: Once both arrays are fully sorted, threads compare their respective segments in parallel
4. **Result Aggregation**: A shared counter determines if all segments matched

## Project Structure

```
parallel-array-comparator/
├── src/
│   ├── shared_memory/          # Pthreads implementation
│   │   ├── main.c             # Main entry point
│   │   └── parallel_comparator.c
│   ├── core/                   # Core algorithms
│   │   └── merge_sort.c       # Iterative merge sort
│   └── utils/                  # Utility functions
│       └── array_utils.c      # Array manipulation
├── include/
│   ├── shared_memory/
│   ├── core/
│   └── utils/
├── build/                      # Compiled binaries
├── benchmarks/                 # Benchmarking scripts
├── docs/                       # Additional documentation
├── tests/                      # Test files
├── Makefile
└── README.md
```

## Building the Project

### Prerequisites

- GCC compiler with pthread support
- Make
- POSIX-compliant system (Linux, macOS, Unix)

### Compilation

```bash
# Build the project
make

# Clean build artifacts
make clean

# Build and run tests
make test
```

## Usage

### Basic Usage

```bash
# Run with default settings (2^24 elements, 4 threads)
./build/parallel_comparator

# Specify array size and thread count
./build/parallel_comparator -n 16777216 -t 8

# Enable verbose output for small arrays
./build/parallel_comparator -n 1024 -t 4 -v
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-n, --size SIZE` | Array size | 16777216 (2^24) |
| `-t, --threads THREADS` | Number of threads (must be power of 2) | 4 |
| `-v, --verbose` | Enable verbose output | Disabled |
| `-h, --help` | Show help message | - |

### Example

```bash
$ ./build/parallel_comparator -n 1048576 -t 8
=== Parallel Array Comparator ===
Array size: 1048576 elements
Number of threads: 8
Max random value: 1000

Result: Arrays are DIFFERENT
Execution time: 0.234567 seconds
```

## Benchmarking

Run comprehensive benchmarks:

```bash
make benchmark
```

This will test various configurations:
- N=2^20 with 4 threads
- N=2^22 with 8 threads
- N=2^24 with 16 threads

## Performance Analysis

### Shared Memory (Pthreads)

#### Execution Times (seconds)

| Array Size | Sequential | 8 Threads | 16 Threads | 32 Threads | 64 Threads |
|------------|-----------|-----------|------------|------------|------------|
| 2^24 | 8.677 | 8.047 | 6.085 | 5.350 | 4.906 |
| 2^25 | 17.743 | 16.570 | 12.348 | 10.991 | 9.973 |
| 2^26 | 36.364 | 34.699 | 24.661 | 21.854 | 20.688 |
| 2^27 | 74.477 | 70.818 | 50.607 | 43.427 | 41.040 |

#### Speedup

| Array Size | 8 Threads | 16 Threads | 32 Threads | 64 Threads |
|------------|-----------|------------|------------|------------|
| 2^24 | 1.08x | 1.43x | 1.62x | 1.77x |
| 2^25 | 1.07x | 1.44x | 1.61x | 1.78x |
| 2^26 | 1.05x | 1.47x | 1.66x | 1.76x |
| 2^27 | 1.05x | 1.47x | 1.71x | 1.81x |

### Scalability Analysis

**Amdahl's Law**: The speedup does not scale linearly with the number of processors for a fixed problem size, confirming Amdahl's law. The sequential portion of the algorithm limits parallel efficiency.

**Gustafson's Law**: When increasing both problem size and processor count, speedup improves but doesn't reach ideal scaling, showing partial adherence to Gustafson's law.

**Strong Scalability**: ❌ Not strongly scalable - efficiency decreases as processors increase with fixed problem size.

**Weak Scalability**: ❌ Not weakly scalable - efficiency decreases even when problem size grows proportionally with processor count.

## Architecture Details

### Iterative Merge Sort

The implementation uses an iterative (bottom-up) merge sort instead of recursive approaches:

**Advantages**:
- Better cache locality
- No recursion overhead
- Easier to parallelize
- Predictable memory usage

### Synchronization Strategy

**Barriers**: Multiple barrier sets are used to synchronize threads at different merging levels:
- Even threads use one set of barriers
- Odd threads use a separate set
- Final barrier synchronizes all threads before comparison

**Semaphores**: Used to protect the shared comparison result counter from race conditions.

### Thread Work Distribution

Each thread processes a specific segment based on its ID:
```
segment_size = total_size / num_threads
segment_start = thread_id * segment_size
```

## Technical Considerations

### Memory Requirements

- Two arrays of N integers: `2 * N * sizeof(int)`
- Temporary buffers during merge: `O(N)`
- Thread structures: `O(P)` where P = number of threads

### Thread Count Limitations

- Must be a power of 2 (2, 4, 8, 16, 32, 64, ...)
- Optimal performance typically at 8-16 threads for modern CPUs
- Diminishing returns beyond 32 threads due to synchronization overhead

## Future Work

- [ ] MPI distributed memory implementation
- [ ] Hybrid MPI+OpenMP approach
- [ ] GPU acceleration with CUDA/OpenCL
- [ ] Support for non-power-of-2 thread counts
- [ ] Configurable merge strategies
- [ ] Performance profiling tools

## License

This project is developed for educational and research purposes.

## References

- Amdahl, G. M. (1967). "Validity of the single processor approach to achieving large scale computing capabilities"
- Gustafson, J. L. (1988). "Reevaluating Amdahl's Law"
- Cormen, T. H., et al. (2009). "Introduction to Algorithms" (Third Edition)

## Acknowledgments

This project was developed as an exploration of parallel and distributed computing concepts, focusing on efficient algorithm design and performance optimization.
