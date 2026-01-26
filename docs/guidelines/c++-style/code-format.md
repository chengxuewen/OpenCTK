---
name: "project-cpp-code-format"
description: "OpenCTK project C++ code format style. Invoke when working on OpenCTK C++ code to follow the project's code format rules."
---

# OpenCTK Project C++ Code Format Style Guide

[中文版本](./cn/代码格式.md)

## Overview

This document describes the C++ code formatting conventions used in the OpenCTK project. The project uses ClangFormat with a custom configuration to ensure consistent code formatting across the codebase.

## Core Principles

1. **Consistency**: All code should follow the same formatting rules
2. **Readability**: Formatting should enhance code readability
3. **Automatic Enforcement**: Use ClangFormat to automatically format code
4. **Version Control Friendly**: Formatting should minimize diffs in version control
5. **Modern C++ Support**: Support for modern C++ features and syntax

## ClangFormat Configuration

The project uses a custom `.clang-format` file located at the root of the repository. This file defines all the formatting rules for the project.

### Key Configuration Points

#### Basic Style
- **Base Style**: Based on LLVM style
- **Indent Width**: 4 spaces (no tabs)
- **Column Limit**: 120 characters

#### Bracing Style
- **Brace Style**: Allman style (opening braces on new lines)
- **Space Before Braces**: Control statements only
- **Indent Braces**: Yes

#### Indentation
- **Access Modifier Offset**: -4 spaces
- **Indent Case Labels**: Yes
- **Indent PP Directives**: After hash

#### Spacing
- **Space After Template Keyword**: Yes
- **Space Before Assignment Operators**: Yes
- **Space In Empty Block**: Yes
- **Pointer Alignment**: Right (e.g., `int* ptr`)

#### Line Breaks
- **Bin Pack Arguments**: No (each argument on new line)
- **Bin Pack Parameters**: No (each parameter on new line)
- **Always Break Template Declarations**: Yes

## Code Formatting Examples

### Namespaces

```cpp
OCTK_BEGIN_NAMESPACE

namespace detail
{
    // Namespace content
}

OCTK_END_NAMESPACE
```

### Classes and Structs

```cpp
class LoggerPrivate
{
public:
    LoggerPrivate(Logger* p, const char* name)
        : mPPtr(p)
        , mIdNumber(detail::loggerIdNumberCounter().fetch_add(1))
        , mName(name)
        , mNoSource(false)
    {
        // Constructor content
    }
    
    ~LoggerPrivate() { }
    
private:
    Logger* mPPtr;
    int mIdNumber;
    std::string mName;
    bool mNoSource;
};
```

### Functions

```cpp
static inline std::mutex& loggersMapMutex()
{
    static std::mutex mutex;
    return mutex;
}

bool LoggerPrivate::messageHandlerOutput(const Context& context, const char* message)
{
    const auto handlerWraper = mMessageHandlerWraper.load();
    if (handlerWraper)
    {
        handlerWraper->handler(mName, context, message);
        return mMessageHandleUniqueOwnership.load();
    }
    return false;
}
```

### Control Statements

```cpp
if (condition)
{
    // If body
}
else if (otherCondition)
{
    // Else if body
}
else
{
    // Else body
}

for (int i = 0; i < 10; ++i)
{
    // For loop body
}

while (condition)
{
    // While loop body
}

switch (value)
{
    case 1:
        // Case 1 body
        break;
    case 2:
        // Case 2 body
        break;
    default:
        // Default case body
        break;
}
```

### Template Declarations

```cpp
template <typename T>
class Vector
{
public:
    // Class content
};

template <typename IteratorType, typename ValueType>
IteratorType find(IteratorType begin, IteratorType end, const ValueType& value)
{
    // Function content
}
```

## How to Use ClangFormat

### Formatting Files

To format a single file:
```bash
clang-format -i file.cpp
```

To format all C++ files in a directory:
```bash
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### IDE Integration

Most modern IDEs support ClangFormat integration:

- **Visual Studio Code**: Install the Clang-Format extension and enable format on save
- **CLion**: Enable ClangFormat support in Settings > Editor > Code Style > C/C++
- **Visual Studio**: Install the ClangFormat extension and configure it to use the project's .clang-format file

### Git Hooks

You can set up a pre-commit hook to automatically format changed files:

```bash
#!/bin/sh

git diff --cached --name-only --diff-filter=ACM | grep -E \.(cpp|hpp)$ | xargs clang-format -i
git add -u
```

## Best Practices

1. **Format Early, Format Often**: Format your code regularly to avoid large formatting changes
2. **Follow the Existing Style**: When modifying existing code, follow the style of that file
3. **Use ClangFormat**: Always use ClangFormat to ensure consistency
4. **Don't Fight the Format**: If ClangFormat is formatting something in a way you don't like, consider adjusting your code structure rather than disabling formatting
5. **Review Format Changes**: When reviewing PRs, pay attention to formatting changes to ensure they're consistent

## Common Issues and Solutions

### Long Lines
- **Issue**: Code exceeds the 120 character column limit
- **Solution**: Break long lines at logical points (e.g., after commas, operators)

### Complex Expressions
- **Issue**: Complex expressions are hard to read even after formatting
- **Solution**: Break complex expressions into multiple lines with intermediate variables

### Template Declarations
- **Issue**: Long template declarations are hard to format
- **Solution**: Let ClangFormat handle it automatically with `AlwaysBreakTemplateDeclarations: Yes`

## Conclusion

Consistent code formatting is essential for maintaining a large codebase. By following the OpenCTK project's code formatting rules and using ClangFormat, you can ensure that your code is consistent with the rest of the project, making it easier to read, understand, and maintain.

Always refer to this document and the project's `.clang-format` file when formatting your code. When in doubt, let ClangFormat handle the formatting automatically.