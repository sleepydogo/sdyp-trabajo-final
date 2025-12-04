# Changelog

All notable changes to this project will be documented in this file.

## [2.0.0] - 2024-12-04 - Professional Refactoring

### Major Refactoring

This release represents a complete professional refactoring of the parallel array comparator project, transforming it from a monolithic implementation into a clean, modular, production-ready codebase.

### Added

#### Project Structure
- Created modular directory structure with `src/`, `include/`, `docs/`, `benchmarks/`, `tests/`
- Separated code into logical modules: `core/`, `utils/`, `shared_memory/`
- Added `archive/` directory to preserve original implementation files
- Created professional `.gitignore` for build artifacts and IDE files

#### Core Modules
- **`core/merge_sort`**: Extracted iterative merge sort into reusable module
  - `merge()`: Merges two sorted subarrays
  - `merge_sort_iterative()`: Bottom-up merge sort implementation
- **`core/barrier`**: Cross-platform pthread barrier implementation
  - Custom implementation for macOS compatibility
  - Uses mutex and condition variables
- **`utils/array_utils`**: Array manipulation utilities
  - `create_random_array()`: Dynamic array allocation
  - `fill_random_array()`: Random data generation
  - `print_array()`: Debug output
  - `arrays_equal()`: Comparison utility

#### Shared Memory Module
- **`shared_memory/parallel_comparator`**: Thread coordination and comparison
  - `parallel_compare_arrays()`: Main API for parallel comparison
  - `thread_sort_and_compare()`: Worker thread function
  - Hierarchical barrier synchronization
  - Thread-safe result aggregation using mutex
- **`shared_memory/main`**: Command-line application
  - Argument parsing (`-n`, `-t`, `-v`, `-h` flags)
  - Usage help and examples
  - Performance timing
  - Configurable execution

#### Documentation
- **`README.md`**: Comprehensive user guide
  - Project overview and algorithm description
  - Build instructions and usage examples
  - Performance analysis with benchmark tables
  - Architecture details and technical considerations
- **`docs/API.md`**: Complete API reference
  - Function signatures and parameters
  - Pre/postconditions and complexity analysis
  - Usage examples for all public functions
  - Error codes and thread safety notes
- **`docs/ARCHITECTURE.md`**: Design documentation
  - Module organization and design decisions
  - Algorithm phases and synchronization mechanisms
  - Performance considerations and bottlenecks
  - Future architectural improvements
- **`docs/CONTRIBUTING.md`**: Developer guide
  - Development setup and code style
  - Pull request process and commit guidelines
  - Testing and debugging procedures
  - Areas for contribution
- **`docs/PROJECT_SUMMARY.md`**: Executive overview
  - Technical highlights and key features
  - Performance characteristics and metrics
  - Educational value and future enhancements
- **`archive/README.md`**: Legacy code documentation
  - Migration notes from old to new structure
  - Historical reference and comparison

#### Build System
- **`Makefile`**: Professional build configuration
  - Multiple targets: `all`, `clean`, `test`, `benchmark`, `help`
  - Automatic dependency tracking
  - Optimized compilation flags (`-O3`, `-Wall`, `-Wextra`)
  - Colored output and informative messages
  - Parallel compilation support

#### Benchmarking
- **`benchmarks/run_benchmarks.sh`**: Automated benchmark script
  - Configurable array sizes and thread counts
  - System information capture
  - CSV output generation
  - Performance comparison tools

#### Licensing
- **`LICENSE`**: MIT License for open distribution
- Proper copyright attribution to authors

### Changed

#### Code Quality Improvements
- Replaced monolithic `mergeIterativoV3.c` with modular architecture
- Improved naming conventions (descriptive function and variable names)
- Added comprehensive Doxygen-style documentation comments
- Enhanced error handling and validation
- Removed code duplication through modularization

#### Platform Compatibility
- Replaced `pthread_barrier_t` with custom implementation for macOS
- Changed from deprecated `sem_init()` to `pthread_mutex_t`
- Ensured C99 standard compliance
- Added cross-platform build support

#### Performance
- Maintained O(n log n) time complexity
- Optimized compilation with `-O3` flag
- Improved cache locality through iterative merge sort
- Better memory management with proper cleanup

### Fixed
- macOS compilation errors with pthread barriers
- Deprecated semaphore API warnings
- Memory allocation error handling
- Thread synchronization edge cases
- Build dependency issues

### Removed
- Monolithic implementation files (moved to `archive/`)
- Hardcoded configuration values (now command-line configurable)
- Duplicate merge sort implementations
- Temporary and debug files
- Compilation artifacts from root directory

## [1.0.0] - 2024 - Original Implementation

### Initial Release
- Basic parallel array comparison using pthreads
- Iterative merge sort algorithm
- Hierarchical merging with barriers
- Performance benchmarking on cluster
- Initial documentation and analysis

---

## Migration Guide: v1.0.0 → v2.0.0

### Building the Project

**Old:**
```bash
gcc -pthread mergeIterativoV3.c -o mergeIterativoV3
./mergeIterativoV3
```

**New:**
```bash
make
./build/parallel_comparator -n 16777216 -t 16
```

### Using the API

**Old:** Single monolithic file
```c
// All code in one file
int main() {
    // Hardcoded values
    #define ARRAY_SIZE (1 << 24)
    #define NUM_THREADS 4
}
```

**New:** Modular API
```c
#include "shared_memory/parallel_comparator.h"
#include "utils/array_utils.h"

int main() {
    comparator_config_t config = {
        .array1 = arr1,
        .array2 = arr2,
        .size = 1048576,
        .num_threads = 8
    };
    int result = parallel_compare_arrays(&config);
}
```

### Documentation

**Old:** Comments in code only

**New:** Comprehensive multi-file documentation
- User guide: `README.md`
- API reference: `docs/API.md`
- Design docs: `docs/ARCHITECTURE.md`
- Developer guide: `docs/CONTRIBUTING.md`

---

## Future Releases (Planned)

### [3.0.0] - MPI Implementation
- Distributed memory parallelism using MPI
- Multi-node execution support
- Hybrid MPI+pthreads approach

### [2.1.0] - Enhanced Testing
- Comprehensive unit test suite
- Integration tests
- Memory leak detection
- Performance regression tests

### [2.0.1] - Bug Fixes
- Minor improvements and bug fixes
- Documentation updates
- Performance optimizations

---

**Authors**: Tomas E. Schattmann, Emiliano Mengoni
**Repository**: https://github.com/yourusername/parallel-array-comparator
**License**: MIT
