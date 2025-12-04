# Contributing to Parallel Array Comparator

Thank you for your interest in contributing to this project!

## Development Setup

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd parallel-array-comparator
   ```

2. **Build the project**
   ```bash
   make
   ```

3. **Run tests**
   ```bash
   make test
   ```

## Code Style Guidelines

### C Code Standards

- Follow C99 standard
- Use 4 spaces for indentation (no tabs)
- Maximum line length: 100 characters
- Use descriptive variable names
- Add comments for complex algorithms

### Example

```c
/**
 * @brief Brief description
 *
 * Detailed description of the function
 *
 * @param param1 Description
 * @return Description of return value
 */
int my_function(int param1)
{
    // Implementation
    return 0;
}
```

### File Organization

- **Headers**: Place in `include/` directory
- **Implementation**: Place in `src/` directory
- **Tests**: Place in `tests/` directory
- **Documentation**: Place in `docs/` directory

## Pull Request Process

1. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**
   - Write clean, documented code
   - Add tests if applicable
   - Update documentation

3. **Test your changes**
   ```bash
   make clean && make
   make test
   ```

4. **Commit your changes**
   ```bash
   git add .
   git commit -m "Brief description of changes"
   ```

5. **Push and create PR**
   ```bash
   git push origin feature/your-feature-name
   ```

## Commit Message Guidelines

Format:
```
<type>: <subject>

<body>

<footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding or updating tests
- `build`: Build system changes

Example:
```
feat: Add MPI distributed memory implementation

- Implement MPI-based array comparison
- Add configuration for multi-node execution
- Update documentation

Closes #123
```

## Testing

### Unit Tests

Add tests for new functionality:

```c
// tests/test_merge_sort.c
#include "core/merge_sort.h"
#include <assert.h>

void test_merge_basic() {
    int arr[] = {3, 1, 4, 1, 5, 9};
    merge_sort_iterative(arr, 0, 5);
    // Add assertions
}
```

### Performance Tests

Use the benchmark script:

```bash
cd benchmarks
./run_benchmarks.sh
```

## Documentation

### Code Documentation

- Use Doxygen-style comments
- Document all public functions
- Explain complex algorithms
- Include usage examples

### Updating Documentation

When adding features:

1. Update `README.md` if user-facing
2. Update `docs/API.md` for API changes
3. Update `docs/ARCHITECTURE.md` for design changes

## Areas for Contribution

### High Priority

- [ ] MPI distributed memory implementation
- [ ] Additional test coverage
- [ ] Performance optimizations
- [ ] Support for non-power-of-2 thread counts

### Medium Priority

- [ ] Benchmark visualization tools
- [ ] Automated testing suite
- [ ] Memory profiling
- [ ] Additional sorting algorithms

### Nice to Have

- [ ] GPU acceleration (CUDA/OpenCL)
- [ ] Hybrid MPI+OpenMP
- [ ] Web-based benchmark dashboard
- [ ] Docker containerization

## Performance Benchmarking

When optimizing:

1. Measure baseline performance
2. Make incremental changes
3. Re-benchmark after each change
4. Document improvements

```bash
# Benchmark before
make benchmark > before.txt

# Make changes
# ...

# Benchmark after
make benchmark > after.txt

# Compare
diff before.txt after.txt
```

## Debugging

### Compile with debug symbols

```bash
make CFLAGS="-g -O0 -Wall"
```

### Use Valgrind for memory leaks

```bash
valgrind --leak-check=full ./build/parallel_comparator -n 1024 -t 4
```

### Use GDB for debugging

```bash
gdb ./build/parallel_comparator
(gdb) run -n 1024 -t 4
```

## Questions or Issues?

- Check existing issues first
- Create detailed bug reports
- Include system information
- Provide reproduction steps

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
