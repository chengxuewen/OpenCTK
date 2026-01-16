# OpenCTK Cpp Code Style Guide

This document defines the coding style and conventions for the OpenCTK project, which are based on common practices in major C++ open source projects.

## Table of Contents

1. [Directory Structure](#directory-structure)
2. [File Naming](#file-naming)
3. [Namespace](#namespace)
4. [Class and Function Naming](#class-and-function-naming)
5. [Variable and Constant Naming](#variable-and-constant-naming)
6. [Code Formatting](#code-formatting)
7. [Header Files](#header-files)
8. [Implementation Files](#implementation-files)
9. [Comments and Documentation](#comments-and-documentation)
10. [Memory Management](#memory-management)
11. [CMake Conventions](#cmake-conventions)

## Directory Structure

OpenCTK follows a modular directory structure to ensure clear organization of code:

```
src/
├── apps/              # Applications built on top of OpenCTK
├── libs/              # Core libraries
│   ├── core/          # Core functionality
│   │   ├── include/   # Public headers
│   │   ├── source/    # Implementation files
│   │   │   ├── kernel/ # Core kernel implementation
│   │   │   ├── text/   # Text processing
│   │   │   ├── thread/ # Threading
│   │   │   └── ...     # Other modules
│   │   └── tests/     # Unit tests
│   ├── graphics/      # Graphics library
│   ├── media/         # Media processing
│   └── ...            # Other libraries
└── tools/             # Utility tools
```

### Guidelines

- **Public Headers**: Place all public headers in `include/` directory.
- **Private Headers**: Place private implementation headers in `source/` subdirectories.
- **Implementation Files**: Organize implementation files in logical subdirectories under `source/`.
- **Tests**: Create unit tests in `tests/` directory with matching source file names.

## File Naming

### Header Files

- Use lowercase with underscores: `octk_string.hpp`
- Prefix with `octk_` to avoid naming conflicts
- Private implementation headers: `octk_application_p.hpp` (with `_p` suffix)

### Source Files

- Use lowercase with underscores: `octk_string.cpp`
- Test files: `tst_string.cpp` (prefix with `tst_`)

### Example

```
include/
├── octk_string.hpp           # Public header
source/
├── text/
│   ├── octk_string.hpp       # Private implementation header
│   └── octk_string.cpp       # Implementation file
tests/
└── tst_string.cpp            # Test file
```

## Namespace

### Global Namespace

- Use `OCTK_BEGIN_NAMESPACE` and `OCTK_END_NAMESPACE` macros to define the global namespace
- All public code should be in the `octk` namespace

### Private Namespaces

- Use nested namespaces for private implementation details: `octk::detail`
- Use feature-specific namespaces where appropriate

### Example

```cpp
OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API String
{
    // Public interface
};

namespace detail
{
    // Private implementation
}

OCTK_END_NAMESPACE
```

## Class and Function Naming

### Classes and Structs

- Use CamelCase (PascalCase): `Application`, `SharedRefPtr`
- Use descriptive names that reflect the class purpose
- Private implementation classes: `ApplicationPrivate` (with `Private` suffix)

### Functions

- Member functions: `CamelCase`: `void setName(const String &name);`
- Free functions: `camelCase`: `bool isRunning();`
- Getters/setters: `getName()`, `setName()` (avoid `get_` or `set_` prefixes)
- Static functions: `static void init();`

### Methods

- Override methods with `override` keyword
- Use `const` qualifier for methods that don't modify the object

### Example

```cpp
class OCTK_CORE_API Application : public Object
{
public:
    Application(int &argc, char **argv);
    ~Application() override;
    
    bool event(Event *event) override;
    bool isRunning() const;
    void setRunning(bool running);
    
private:
    OCTK_DECLARE_PRIVATE(Application)
    OCTK_DISABLE_COPY_MOVE(Application)
};
```

## Variable and Constant Naming

### Variables

- Member variables: `mCamelCase` (prefix with `m`)
- Local variables: `camelCase`
- Avoid single-letter variables except in small scopes (e.g., loops)

### Constants

- Use `kCamelCase` for constants: `const int kMaxLength = 1024;`
- Use `static constexpr` for compile-time constants
- Enumerators: `ALL_CAPS` with underscores

### Example

```cpp
class String
{
private:
    char *mData;
    size_t mSize;
    static constexpr int kMaxBufferSize = 4096;
};

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};
```

## Code Formatting

### Indentation

- Use 4 spaces per indentation level (no tabs)
- Align function parameters vertically for readability

### Braces

- Use K&R style (opening brace on the same line)
- Always use braces for control structures, even single-line statements

### Example

```cpp
if (condition) {
    // Code here
} else {
    // Code here
}

for (int i = 0; i < count; ++i) {
    // Code here
}
```

### Line Length

- Limit lines to 120 characters
- Break long lines at logical points
- Align continuation lines with the opening parenthesis or use 4-space indent

### Example

```cpp
void longFunctionName(int parameter1,
                     int parameter2,
                     const String &parameter3,
                     bool parameter4);

result = someObject->longMethodName(parameter1,
                                   parameter2,
                                   parameter3);
```

### Spaces

- Use spaces around operators: `x = y + z` (not `x=y+z`)
- Use spaces after commas: `function(a, b, c)` (not `function(a,b,c)`)
- Use spaces after colons in range-based for loops: `for (auto &item : container)`
- No spaces inside parentheses: `function(x)` (not `function( x )`)
- No spaces before semicolons: `x++;` (not `x++ ;`)

## Header Files

### Include Guards

- Use `#pragma once` for include guards
- Alternatively, use traditional `#ifndef/#define/#endif` guards for compatibility

### Forward Declarations

- Use forward declarations to reduce dependencies
- Avoid including unnecessary headers

### Include Order

1. System headers (e.g., `<iostream>`)
2. Third-party headers (e.g., `<boost/shared_ptr.hpp>`)
3. OpenCTK headers (e.g., `<octk_string.hpp>`)

### Example

```cpp
#pragma once

#include <string>
#include <vector>

#include <octk_object.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API String
{
    // Class definition
};

OCTK_END_NAMESPACE
```

## Implementation Files

### Include Order

1. Corresponding header file
2. Other OpenCTK headers
3. Third-party headers
4. System headers

### Example

```cpp
#include "octk_string.hpp"

#include <octk_memory.hpp>

#include <cstring>
#include <memory>

OCTK_BEGIN_NAMESPACE

// Implementation

OCTK_END_NAMESPACE
```

## Comments and Documentation

### Doxygen Comments

- Use Doxygen style comments for all public APIs
- Document classes, functions, parameters, and return values

### Example

```cpp
/**
 * @brief Duplicates a string.
 *
 * This function creates a new copy of the given string.
 *
 * @param str The string to duplicate
 * @return A newly-allocated copy of @a str, or nullptr if str is nullptr
 */
static char *strdup(const char *str) { return str ? String::strndup(str, strlen(str) + 1) : nullptr; }
```

### Inline Comments

- Use `//` for inline comments
- Explain complex logic, not obvious code
- Keep comments short and to the point

### Example

```cpp
// Calculate the hash value using a modified FNV-1a algorithm
uint32_t hash = 2166136261u;
for (size_t i = 0; i < length; ++i) {
    hash ^= data[i];
    hash *= 16777619u; // FNV prime
}
```

## Memory Management

### Smart Pointers

- Use `SharedRefPtr` for reference-counted objects
- Use `UniquePtr` for exclusive ownership
- Avoid raw pointers where possible

### Example

```cpp
SharedRefPtr<Application> app = new Application(argc, argv);
UniquePtr<Buffer> buffer = std::make_unique<Buffer>(size);
```

### Memory Allocation

- Use `aligned_malloc`/`aligned_free` for aligned memory
- Always check for null after allocation
- Free memory in the same module where it was allocated

## CMake Conventions

### CMakeLists.txt Structure

1. License and copyright notice
2. Project definition and versioning
3. Options and configurations
4. Dependencies
5. Source files
6. Target definitions
7. Installation rules

### Naming

- Use `UPPER_CASE` for CMake variables: `OCTK_LIB_LINK_LIBRARIES`
- Use `CamelCase` for functions: `octk_add_library`
- Use descriptive target names: `octk_core`

### Example

```cmake
########################################################################################################################
#
# Library: OpenCTK
#
# Copyright (C) 2025~Present ChengXueWen.
#
# License: MIT License
#
########################################################################################################################

# Add lib link libraries
if(WIN32)
    list(APPEND OCTK_LIB_LINK_LIBRARIES
        shlwapi.lib     # shell,string
        ws2_32.lib      # network
        Winmm.lib)
endif()
```

## Conclusion

Following these code style guidelines ensures consistency across the OpenCTK codebase, making it more maintainable and easier for contributors to understand. These conventions are based on common practices in major C++ open source projects like Qt, Boost, and LLVM, while being adapted to the specific needs of OpenCTK.

For any questions or suggestions regarding the code style, please open an issue in the project repository.