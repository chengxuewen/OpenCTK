---
name: "project-cpp-header-inclusion"
description: "OpenCTK project C++ header inclusion style. Invoke when working on OpenCTK C++ code to follow the project's header inclusion rules."
---

# OpenCTK Project C++ Header Inclusion Style Guide

[中文版本](./cn/头文件引用.md)

## Overview

This document describes the C++ header inclusion conventions used in the OpenCTK project. Following these conventions ensures consistency across the codebase and improves code readability and maintainability.

## Core Principles

1. **Consistency**: Follow the same inclusion order throughout the project
2. **Clarity**: Make it clear where each header comes from
3. **Avoid pollution**: Minimize unnecessary includes
4. **Dependency management**: Include only what you use
5. **Build performance**: Optimize include order to reduce compilation time

## Header Inclusion Syntax

### 1. Local Project Headers (Same Module)

- **Syntax**: Use double quotes (`""`)
- **When to use**: For headers in the same module or component
- **Example**:
  ```cpp
  #include "octk_string_utils.hpp"
  ```

### 2. Project Global Headers

- **Syntax**: Use angle brackets (`<>`)
- **When to use**: For project-wide headers that are in the include path
- **Example**:
  ```cpp
  #include <octk_global.hpp>
  #include <octk_checks.hpp>
  ```

### 3. Private Implementation Headers

- **Syntax**: Use angle brackets with `private/` prefix
- **When to use**: For internal implementation details not exposed externally
- **Naming convention**: Private headers use `_p.hpp` suffix
- **Example**:
  ```cpp
  #include <private/octk_svc_rate_allocator_p.hpp>
  #include <private/octk_scalability_mode_p.hpp>
  ```

### 4. Standard Library Headers

- **Syntax**: Use angle brackets (`<>`)
- **When to use**: For C++ standard library headers
- **Example**:
  ```cpp
  #include <sstream>
  #include <algorithm>
  #include <vector>
  ```

### 5. Third-Party Library Headers

- **Syntax**: Use double quotes (`""`)
- **When to use**: For external library headers (e.g., Abseil, WebRTC)
- **Example**:
  ```cpp
  #include "absl/strings/string_view.h"
  #include "api/transport/rtp/dependency_descriptor.h"
  ```

### 6. System Headers

- **Syntax**: Use angle brackets (`<>`)
- **When to use**: For operating system or compiler-specific headers
- **Example**:
  ```cpp
  #include <wels/codec_app_def.h>
  #include <wels/codec_api.h>
  ```

## Include Order

Headers should be included in the following order to minimize dependencies and improve build performance:

1. **Same module local headers** (with quotes)
2. **Private implementation headers** (with `<private/...>`)
3. **Project global headers** (with angle brackets)
4. **Third-party library headers** (with quotes)
5. **Standard library headers** (with angle brackets)
6. **System headers** (with angle brackets)

### Example of Correct Include Order

```cpp
// 1. Same module local header
#include "octk_string_utils.hpp"

// 2. Private implementation headers
#include <private/octk_some_feature_p.hpp>

// 3. Project global headers
#include <octk_global.hpp>
#include <octk_checks.hpp>

// 4. Third-party library headers
#include "absl/strings/string_view.h"

// 5. Standard library headers
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

// 6. System headers
#include <wels/codec_app_def.h>
```

## Grouping Includes

- **Group includes by category**: Separate different categories of headers with blank lines
- **Sort includes within each group**: Sort alphabetically within each category
- **Avoid multiple includes**: Don't include the same header more than once

### Example of Grouped and Sorted Includes

```cpp
// Same module
#include "octk_video_encoder_openh264.hpp"

// Private headers
#include <private/octk_h264_bitstream_parser_p.hpp>
#include <private/octk_simulcast_utility_p.hpp>
#include <private/octk_video_encoder_openh264_p.hpp>

// Project headers
#include <octk_metrics.hpp>
#include <octk_simulcast_rate_allocator.hpp>
#include <octk_video_type.hpp>

// Standard library
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
```

## Forward Declarations

- **Use forward declarations when possible**: To reduce compilation dependencies
- **Declare in headers**: Place forward declarations at the beginning of header files
- **Use proper namespace**: Declare in the correct namespace

### Example of Forward Declarations

```cpp
// Forward declarations
namespace webrtc
{
class VideoFrame;
}

namespace octk
{
class VideoEncoder;
}

// Includes follow forward declarations
#include <octk_global.hpp>
// ...
```

## Header Guards

- **Use #pragma once**: Preferred for simplicity and speed
- **Alternative**: Use traditional include guards for maximum compatibility
- **Naming convention**: `_PROJECT_NAME_FILENAME_HPP` (all uppercase with underscores)

### Examples

**#pragma once (preferred)**: 
```cpp
#pragma once

// Header content
```

**Traditional include guards**: 
```cpp
#ifndef _OCTK_STRING_UTILS_HPP
#define _OCTK_STRING_UTILS_HPP

// Header content

#endif // _OCTK_STRING_UTILS_HPP
```

## Best Practices

1. **Include only what you use**: Don't include headers you don't directly use
2. **Avoid circular includes**: Refactor code to eliminate circular dependencies
3. **Use forward declarations**: To reduce compilation time and dependencies
4. **Minimize header size**: Keep headers focused and small
5. **Don't use using namespace in headers**: To avoid polluting the global namespace
6. **Document dependencies**: Comment includes that have specific requirements
7. **Follow the project's include order**: Consistency is key

## Common Mistakes to Avoid

1. **Including headers in the wrong order**: This can cause compilation errors or increase build time
2. **Using incorrect syntax for header types**: Using quotes for standard library headers or angle brackets for local headers
3. **Not sorting includes**: Makes it harder to find and maintain includes
4. **Over-including headers**: Increases compilation time and creates unnecessary dependencies
5. **Circular includes**: Can cause compilation errors and make code hard to maintain
6. **Using relative paths in includes**: Avoid `../header.hpp` style includes
7. **Not using forward declarations when possible**: Increases compilation time

## Conclusion

Following these header inclusion guidelines ensures consistent, maintainable, and efficient C++ code in the OpenCTK project. Proper header management reduces compilation time, minimizes dependencies, and makes the codebase more maintainable.

Always consider the impact of your include decisions on build performance and code maintainability. When in doubt, follow the existing patterns in the codebase.

[中文版本](./cn/头文件引用.md)