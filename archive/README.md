# Archive

This directory contains the original implementation files from the development phase of the project. These files have been preserved for reference and historical purposes.

## Contents

### Original Implementations

- **`mergeIterativoV3.c`**: The main working implementation that served as the basis for the refactored code. This file contained the complete algorithm but lacked modular organization.

- **`mergeSort-pthreads.c`**: An earlier experimental version exploring pthread-based parallel merge sort.

- **`chuku.c`**: Reference implementation from external sources, used for understanding merge sort patterns.

- **`pseudo.c`**: Pseudocode and planning notes for the hierarchical merging algorithm.

### Compiled Binaries (Archived)

- **`merge`**: Compiled binary from early development
- **`mergeSort-pthreads`**: Early pthread implementation binary
- **`mergeIterativoV3`**: Main working version binary
- **`mergeIterativoV3.o`**: Object file

### Other Files

- **`asdfasdfa`**: Temporary/scratch file

## Migration Notes

The code from these files has been professionally refactored into the new modular architecture:

| Old File | New Location(s) |
|----------|----------------|
| `mergeIterativoV3.c` | Split into multiple modules:<br>- `src/core/merge_sort.c`<br>- `src/shared_memory/parallel_comparator.c`<br>- `src/utils/array_utils.c`<br>- `src/shared_memory/main.c` |
| `mergeSort-pthreads.c` | Concepts integrated into `parallel_comparator.c` |
| `chuku.c` | Reference for merge implementation |
| `pseudo.c` | Documented in `docs/ARCHITECTURE.md` |

## Key Improvements in Refactored Version

1. **Modular Design**: Code separated into logical modules (core, utils, shared_memory)
2. **Clean Headers**: Public APIs documented in header files
3. **Cross-Platform**: Added macOS compatibility (barrier implementation)
4. **Better Naming**: Descriptive function and variable names
5. **Documentation**: Comprehensive inline and external documentation
6. **Build System**: Professional Makefile with proper dependencies
7. **Error Handling**: Robust memory and thread error checking
8. **Portability**: Replaced deprecated semaphore with mutex

## Why Keep These Files?

- **Historical Reference**: Shows the evolution of the implementation
- **Learning**: Demonstrates the refactoring process
- **Comparison**: Can compare performance with original versions
- **Backup**: Preserved in case specific logic needs to be referenced

## Using Archived Files

These files are no longer maintained and may not compile with current build tools. To use them:

```bash
cd archive

# Compile original version (may require adjustments)
gcc -pthread -o mergeIterativoV3_rebuild mergeIterativoV3.c

# Note: May need to adjust for platform-specific issues
```

## Notes

- These files are provided as-is without warranty
- They may contain deprecated APIs or platform-specific code
- Use the main project implementation for production use
- Refer to `../docs/` for current documentation

---

**Preserved**: 2024
**Status**: Archived - No longer actively maintained
**Use**: Reference only - Use main project for current implementation
