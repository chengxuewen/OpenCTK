---
name: "project-cpp-namespace-style"
description: "OpenCTK project C++ namespace style. Invoke when working on OpenCTK C++ code to follow the project's namespace style."
---

# OpenCTK Project C++ Namespace Style Guide

[中文版本](./cn/名字空间.md)

## Overview

This document describes the C++ namespace style conventions used in the OpenCTK project. Following these conventions ensures consistency across the codebase and improves code readability and maintainability.

## Core Principles

1. **Consistency**: Follow the same namespace conventions throughout the project
2. **Clarity**: Namespace names should clearly reflect their functionality or module
3. **Avoid Pollution**: Avoid excessive use of the global namespace
4. **Hierarchy**: Use nested namespaces to create a clear hierarchy
5. **Extensibility**: Design namespace structures to support future expansion

## Global Namespace

### Using `OCTK_BEGIN_NAMESPACE` and `OCTK_END_NAMESPACE` Macros

The OpenCTK project uses custom macros to define the global namespace, which encapsulate the actual namespace name for future refactoring convenience.

**Example:**
```cpp
OCTK_BEGIN_NAMESPACE

// Public code in the octk namespace
class String
{
    // ...
};

OCTK_END_NAMESPACE
```

### Macro Definitions

These macros are typically defined in a global header file (e.g., `octk_global.hpp`), and they actually expand to:

```cpp
#define OCTK_BEGIN_NAMESPACE namespace octk \
{ \
    namespace detail \
    { \
        namespace thread \
        { 
#define OCTK_END_NAMESPACE } \
    } \
}
```

## Nested Namespaces

### Functional Module Namespaces

For different functional modules, use nested namespaces to organize code:

**Example:**
```cpp
OCTK_BEGIN_NAMESPACE

namespace string_utils
{
    std::string toUpper(const std::string &str);
    std::string toLower(const std::string &str);
}

namespace network
{
    class Socket
    {
        // ...
    };
}

OCTK_END_NAMESPACE
```

### Implementation Detail Namespaces

For internal implementation details, use the `detail` namespace for encapsulation:

**Example:**
```cpp
OCTK_BEGIN_NAMESPACE

class PublicClass
{
private:
    // Implementation details of the public class
    class PrivateImpl;
    std::unique_ptr<PrivateImpl> d_ptr;
};

namespace detail
{
    // Internal implementation details, not exposed externally
    void internalFunction()
    {
        // ...
    }
}

OCTK_END_NAMESPACE
```

## External Namespaces

### WebRTC Namespace

When integrating or depending on the WebRTC library, use the `webrtc` namespace:

**Example:**
```cpp
namespace webrtc
{
    // WebRTC-related code
    class VideoEncoder
    {
        // ...
    };
}
```

### Standard Library Namespace

When using the standard library namespace, explicitly specify the `std::` prefix and avoid using `using namespace std;`:

**Recommended:**
```cpp
std::string str = "Hello";
std::vector<int> vec;
```

**Not Recommended:**
```cpp
using namespace std;
string str = "Hello";
vector<int> vec;
```

## Anonymous Namespaces

For code that is only used within the current file, use an anonymous namespace:

**Example:**
```cpp
OCTK_BEGIN_NAMESPACE

namespace
{
    // Constants used only in this file
    const int kMaxBufferSize = 1024;
    
    // Functions used only in this file
    void helperFunction() {
        // ...
    }
}

// Public code
void publicFunction() {
    helperFunction();
}

OCTK_END_NAMESPACE
```

## Namespace Naming Conventions

1. **Use lowercase letters**: Namespace names should use lowercase letters
2. **Use underscores as separators**: For namespace names consisting of multiple words, use underscores as separators
3. **Reflect functionality**: Namespace names should clearly reflect their functionality or module
4. **Avoid abbreviations**: Use full words whenever possible, unless the abbreviation is widely accepted

**Example:**
```cpp
// Recommended
namespace network_utils {
    // ...
}

namespace media_codec {
    // ...
}

// Not Recommended
namespace netutils {
    // ...
}

namespace mc {
    // ...
}
```

## Ways to Use Namespaces

### Fully Qualified Names

For infrequently used namespace members, use fully qualified names:

**Example:**
```cpp
octk::string_utils::toUpper("hello");
```

### Namespace Aliases

For long namespaces, use namespace aliases to simplify code:

**Example:**
```cpp
namespace octk_net = octk::network;
octk_net::Socket socket;
```

### Local Using Declarations

For frequently used namespace members, use `using` declarations inside functions or classes:

**Example:**
```cpp
void processString() {
    using octk::string_utils::toUpper;
    using octk::string_utils::toLower;
    
    std::string str = "Hello";
    std::string upper = toUpper(str);
    std::string lower = toLower(str);
}
```

## Best Practices

1. **Keep namespace hierarchy simple**: Avoid excessively deep namespace nesting (recommended no more than 3 levels)
2. **Each namespace has a clear responsibility**: A namespace should contain related functionality
3. **Avoid namespace pollution**: Do not use `using namespace` in header files
4. **Use namespaces to encapsulate implementation details**: Put internal implementations in the `detail` namespace
5. **Follow consistent naming conventions**: All namespace names follow the same naming style
6. **Document namespaces**: Add brief documentation comments for each namespace

## Common Mistakes to Avoid

1. **Using `using namespace` in header files**: This pollutes all code that includes the header
2. **Excessively deep namespace nesting**: Results in verbose code and reduced readability
3. **Unclear namespace names**: Names should clearly reflect their functionality
4. **Mixing code from different namespaces**: Related functionality should be in the same namespace
5. **Not using namespaces to encapsulate internal implementations**: Internal implementations should be in the `detail` namespace

## Example Code

### Correct Namespace Usage

```cpp
// octk_string.hpp
OCTK_BEGIN_NAMESPACE

/**
 * @namespace octk::string_utils
 * @brief String utility functions
 */
namespace string_utils {
    /**
     * @brief Convert string to uppercase
     * @param str Input string
     * @return Uppercase string
     */
    std::string toUpper(const std::string &str);
    
    /**
     * @brief Convert string to lowercase
     * @param str Input string
     * @return Lowercase string
     */
    std::string toLower(const std::string &str);
}

OCTK_END_NAMESPACE
```

```cpp
// octk_string.cpp
#include <octk_string.hpp>
#include <algorithm>

OCTK_BEGIN_NAMESPACE

namespace string_utils {
    std::string toUpper(const std::string &str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    std::string toLower(const std::string &str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

OCTK_END_NAMESPACE
```

### Usage Example

```cpp
#include <octk_string.hpp>
#include <iostream>

int main() {
    std::string str = "Hello, OpenCTK!";
    
    // Using fully qualified names
    std::string upper = octk::string_utils::toUpper(str);
    std::string lower = octk::string_utils::toLower(str);
    
    std::cout << "Original: " << str << std::endl;
    std::cout << "Uppercase: " << upper << std::endl;
    std::cout << "Lowercase: " << lower << std::endl;
    
    return 0;
}
```

## Conclusion

Following these namespace style conventions ensures consistency and readability in the OpenCTK codebase. Namespaces are an important mechanism for organizing code in C++, and proper use of namespaces can avoid name conflicts, improve code modularity, and enhance maintainability.

When developing for the OpenCTK project, always refer to this document and other related style guides to ensure your code conforms to the project's namespace conventions.

[中文版本](./名字空间.md)