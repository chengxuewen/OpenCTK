---
name: "project-cpp-code-naming"
description: "OpenCTK project C++ code naming style. Invoke when working on OpenCTK C++ code to follow the project's code naming rules."
---

# OpenCTK Project C++ Code Naming Style Guide

[中文版本](./cn/代码命名.md)

## Overview

This document describes the C++ code naming conventions used in the OpenCTK project. Following these conventions ensures consistency across the codebase and improves code readability and maintainability.

## Core Principles

1. **Consistency**: Follow the same naming convention throughout the project
2. **Clarity**: Names should clearly reflect their purpose and functionality
3. **Readability**: Choose names that are easy to read and understand
4. **Brevity**: Keep names concise but descriptive
5. **Avoid ambiguity**: Names should not be easily confused with other entities

## Naming Conventions by Entity Type

### 1. Namespace Names

- **Style**: snake_case (lowercase with underscores)
- **When to use**: For organizing related functionality
- **Example**:
  ```cpp
  namespace string_utils
  {
      // ...
  }
  
  namespace media_codec
  {
      // ...
  }
  ```

### 2. Class and Struct Names

- **Style**: PascalCase (capitalize first letter of each word)
- **Singular**: Use singular form
- **Descriptive**: Clearly indicate the entity's purpose
- **Example**:
  ```cpp
  struct ActiveSpatialLayers
  {
      // ...
  };
  
  class VideoEncoder
  {
      // ...
  };
  ```

### 3. Function and Method Names

- **Style**: snake_case (lowercase with underscores)
- **Verbs**: Use verbs to indicate actions
- **Clear purpose**: Name should clearly describe what the function does
- **Example**:
  ```cpp
  std::vector<std::string> split(const std::string& str, char delimiter);
  
  std::string to_upper(const std::string& str);
  
  bool try_parse_int(const std::string& str, int& out_value);
  ```

### 4. Variable Names

- **Style**: snake_case (lowercase with underscores)
- **Descriptive**: Clearly indicate the variable's purpose
- **Avoid single letters**: Except for simple loop variables (i, j, k)
- **Example**:
  ```cpp
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  ```

### 5. Function Parameters

- **Style**: snake_case (lowercase with underscores)
- **Descriptive**: Clearly indicate the parameter's purpose
- **Output parameters**: Use `out_` prefix
- **Example**:
  ```cpp
  bool try_parse_int(const std::string& str, int& out_value);
  
  std::string replace_all(const std::string& str, const std::string& from, const std::string& to);
  ```

### 6. Constants

- **Style**: kCamelCase (k prefix followed by CamelCase)
- **Descriptive**: Clearly indicate the constant's purpose
- **Example**:
  ```cpp
  constexpr float kTemporalLayeringRateScalingFactor = 0.55f;
  
  static constexpr char hex_digits[] = "0123456789ABCDEF";
  ```

### 7. Enums and Enum Values

- **Enum name**: PascalCase
- **Enum values**: kCamelCase (preferred) or ALL_CAPS
- **Example**:
  ```cpp
  enum class VideoCodecType
  {
      kH264,
      kVP8,
      kVP9,
      kAV1
  };
  
  // Alternative style
  enum ColorFormat
  {
      COLOR_FORMAT_RGB,
      COLOR_FORMAT_YUV420,
      COLOR_FORMAT_YUV422,
      COLOR_FORMAT_YUV444
  };
  ```

### 8. Macros

- **Style**: ALL_CAPS_WITH_UNDERSCORES
- **Avoid unnecessary macros**: Prefer constants or constexpr functions
- **Example**:
  ```cpp
  #define OCTK_MAX_BUFFER_SIZE 1024
  
  #define OCTK_ASSERT(x) assert(x)
  ```

### 9. Template Parameters

- **Simple parameters**: Single uppercase letter
- **Complex parameters**: Descriptive PascalCase
- **Example**:
  ```cpp
  template <typename T>  // Simple template parameter
  class Vector
  {
      // ...
  };
  
  template <typename IteratorType, typename ValueType>  // Descriptive template parameters
  IteratorType find(IteratorType begin, IteratorType end, const ValueType& value)
  {
      // ...
  }
  ```

## Naming Conventions by Scope

### 1. Global Variables

- **Avoid when possible**: Prefer local variables or singleton patterns
- **If used**: Use `g_` prefix
- **Example**:
  ```cpp
  int g_global_counter = 0;
  ```

### 2. Member Variables

- **Style**: `m` prefix followed by camelCase (preferred in OpenCTK)
- **When to use**: For all non-static member variables
- **Example**:
  ```cpp
  // Preferred style in OpenCTK
  class VideoEncoder
  {
  private:
      int mFrameCount;
      std::string mCodecName;
  };
  
  // Alternative styles (deprecated)
  class AudioDecoder
  {
  private:
      int m_frame_count; // m_ prefix with snake_case (deprecated)
      int sampleRate;    // No prefix with camelCase (deprecated)
      int sample_rate;   // No prefix with snake_case (deprecated)
  };
  ```

### 3. Static Member Variables

- **Style**: kCamelCase (same as constants)
- **Example**:
  ```cpp
  class Log
  {
  private:
      static LogLevel kDefaultLogLevel;
  };
  ```

## Best Practices

1. **Be consistent**: Follow the same naming convention throughout the codebase
2. **Use descriptive names**: Avoid abbreviations unless they are widely accepted
3. **Keep names concise**: While being descriptive, avoid overly long names
4. **Use meaningful verbs for functions**: Choose verbs that clearly indicate the function's action
5. **Avoid Hungarian notation**: Modern C++ doesn't require type prefixes
6. **Use prefixes for special cases only**: Like `out_` for output parameters or `g_` for global variables
7. **Follow the project's existing style**: When in doubt, look at existing code
8. **Document unclear names**: If a name isn't self-explanatory, add a comment

## Common Mistakes to Avoid

1. **Inconsistent naming**: Mixing different naming styles in the same file
2. **Too short names**: Names that don't clearly indicate purpose
3. **Cryptic abbreviations**: Abbreviations that aren't widely understood
4. **Using reserved words**: Avoid using C++ keywords as names
5. **Hungarian notation**: Using type prefixes like `iCount` or `strName`
6. **Misleading names**: Names that don't accurately describe what the entity does
7. **Overly long names**: Names that are unnecessarily verbose

## Examples of Good and Bad Naming

### Good
```cpp
// Clear function name with descriptive parameters
bool try_parse_double(const std::string& str, double& out_value);

// Descriptive variable name
std::string formatted_string = to_fixed_string(value, 2);

// Clear constant name
constexpr float kMaxFrameRate = 60.0f;
```

### Bad
```cpp
// Unclear function name
bool parse(const std::string& s, double& v);

// Too short variable name
std::string fs = tf(value, 2);

// Cryptic constant name
constexpr float m = 60.0f;
```

## Conclusion

Following these code naming conventions ensures consistency, readability, and maintainability across the OpenCTK codebase. Consistent naming makes the code easier to understand, debug, and modify, which is essential for a large-scale project.

Always refer to this document when writing new code or modifying existing code in the OpenCTK project. When in doubt, follow the existing style in the file or module you're working on.

[中文版本](./cn/代码命名.md)