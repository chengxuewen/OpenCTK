---
name: "project-cpp-naming-style"
description: "OpenCTK project C++ file naming style. Invoke when working on OpenCTK C++ code to follow the project's naming style."
---

# OpenCTK Project C++ File Naming Style

[中文版本](./cn/文件命名.md)

## Overview

This document describes the C++ file naming conventions used in the OpenCTK project. Following these conventions ensures consistency across the codebase and improves code readability and maintainability.

## Core Naming Principles

1. **Consistency**: Follow the same naming convention throughout the project
2. **Clarity**: Filenames should clearly reflect their content
3. **Portability**: Use only ASCII characters, avoid spaces and special characters
4. **Readability**: Choose names that are easy to read and understand
5. **Prefixing**: All files use the `octk_` prefix (short for OpenCTK)

## File Naming Conventions

### Naming Style

- **Style**: Snake case (lowercase with underscores)
- **Example**: `octk_platform_thread_posix.cpp`, `octk_sdp_video_format.hpp`

### File Extensions

| File Type | Extension | Example |
|-----------|-----------|---------|
| Header files | `.hpp` | `octk_strong_alias.hpp` |
| Source files | `.cpp` | `octk_platform_thread_posix.cpp` |
| Interface files | `.hpp` with `_interface` suffix | `octk_rtp_sender_interface.hpp` |
| Template implementation files | `.ipp`, `.tpp` | `octk_template_container.ipp`, `octk_template_container.tpp` |

### Special File Naming Rules

#### 1. Class Files

- One class per file (recommended)
- Filename matches class name using snake_case
- Example: For class `StrongAlias`, use `octk_strong_alias.hpp`

#### 2. Interface Files

- Add `_interface` suffix to the filename
- Example: `octk_rtp_sender_interface.hpp`

#### 3. Platform-Specific Files

- Add platform identifier at the end of the filename
- Platform identifiers: `posix`, `win`, `mac`, `linux`
- Example: `octk_platform_thread_posix.cpp`

#### 4. Test Files

- Use `tst_` prefix
- Example: `tst_strong_alias.cpp`

#### 5. Utility Files

- Use descriptive names that indicate functionality
- **Example**: `octk_string_utils.hpp`, `octk_math_helpers.cpp`

#### 6. Template Files

- Follow the same naming rules as regular class files
- Template declarations typically in `.hpp` files
- Template implementations can be in:
  - Same `.hpp` file (simpler, but may increase compilation time)
  - Separate `.ipp` or `.tpp` files (better for separating declarations and implementations)
- **Example**: `octk_template_container.hpp` (declarations), `octk_template_container.ipp` (implementations)

## Directory Structure

The OpenCTK project follows a modular directory structure:

```
src/
├── libs/
│   ├── core/
│   │   ├── source/
│   │   │   ├── thread/
│   │   │   │   └── octk_platform_thread_posix.cpp
│   │   │   └── tools/
│   │   │       └── octk_strong_alias.hpp
│   │   └── tests/
│   │       └── tst_strong_alias.cpp
│   ├── media/
│   │   ├── source/
│   │   │   ├── codecs/
│   │   │   │   └── video/
│   │   │   │       └── octk_sdp_video_format.cpp
│   │   │   ├── rtp/
│   │   │   │   └── octk_rtp_sender_interface.hpp
│   │   │   └── video/
│   │   │       └── octk_hdr_metadata.hpp
│   │   └── tests/
│   └── network/
└── apps/
    └── ...
```

## Naming Examples

| File Type | Example Filename | Description |
|-----------|------------------|-------------|
| Class file | `octk_strong_alias.hpp` | Header file for `StrongAlias` class |
| Source file | `octk_platform_thread_posix.cpp` | POSIX-specific implementation of platform thread |
| Interface file | `octk_rtp_sender_interface.hpp` | RTP sender interface definition |
| Test file | `tst_strong_alias.cpp` | Test file for `StrongAlias` class |
| Utility file | `octk_sdp_video_format.cpp` | SDP video format utility |

## Comparison with Other Naming Styles

| Style | OpenCTK Usage | Example |
|-------|---------------|---------|
| Snake case | ✅ Recommended | `octk_platform_thread_posix.cpp` |
| Camel case | ❌ Not used | `OctkPlatformThreadPosix.cpp` |
| Pascal case | ❌ Not used | `Octk_Platform_Thread_Posix.cpp` |
| Lowercase without separators | ❌ Not used | `octkplatformthreadposix.cpp` |

## Best Practices

1. **Always use `octk_` prefix**: All files must use the `octk_` prefix to avoid name collisions
2. **Be descriptive**: Choose clear and descriptive filenames that reflect the file's content
3. **Follow the directory structure**: Place files in the appropriate directory based on their functionality
4. **Use consistent extensions**: 
   - `.hpp` for headers and template declarations
   - `.cpp` for source files
   - `.ipp` or `.tpp` for template implementations
5. **Add appropriate suffixes**: Use `_interface` for interfaces, platform identifiers for platform-specific files, and `tst_` for tests
6. **Separate template declarations and implementations**: Consider using `.ipp` or `.tpp` files for template implementations to improve code organization
7. **Avoid abbreviations**: Use full words whenever possible to improve readability
8. **Keep filenames concise**: While being descriptive, avoid overly long filenames

## Common Mistakes to Avoid

1. **Missing `octk_` prefix**: All files must use the `octk_` prefix
2. **Inconsistent case**: Always use snake_case
3. **Wrong file extension**: 
   - Use `.hpp` for headers and template declarations
   - Use `.cpp` for source files
   - Use `.ipp` or `.tpp` for template implementations
4. **Generic filenames**: Avoid names like `utils.hpp` or `common.cpp` without context
5. **Mismatched filename and content**: Ensure the filename accurately reflects the file's content
6. **Platform-specific code in generic files**: Use platform identifiers for platform-specific code
7. **Mixing template declarations and implementations unnecessarily**: Consider using separate `.ipp` or `.tpp` files for complex templates

## Conclusion

Following these C++ file naming conventions ensures consistency and improves the quality of the OpenCTK codebase. Always refer to this document when creating new files or modifying existing ones.

[中文版本](./项目cpp文件命名规则.md)