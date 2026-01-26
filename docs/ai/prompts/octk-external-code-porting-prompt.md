---
name: "octk-external-code-porting-prompt"
description: "Prompt for porting external code to OpenCTK style"
author: "OpenCTK Team"
version: "1.0"
category: "code-porting"
tags: ["c++", "openctk", "external-code", "porting", "prompt"]
---

# OpenCTK External Code Porting Prompt

[Chinese Version](./cn/octk-外部代码移植-提示词.md)

## Purpose

This prompt is designed to guide AI in porting external project code to conform to OpenCTK's coding style and conventions.

## Usage

Use this prompt when you need to transform external code into OpenCTK-compliant code. The prompt will help ensure that the ported code follows OpenCTK's naming conventions, formatting rules, and architectural patterns.

## Referenced Guidelines

This prompt is based on the following OpenCTK guidelines:

- [Code Format](../../guidelines/c++-style/code-format.md) - Code formatting conventions
- [Code Naming](../../guidelines/c++-style/code-naming.md) - Naming conventions for identifiers
- [Header Inclusion](../../guidelines/c++-style/header-inclusion.md) - Header file inclusion rules
- [Namespace Usage](../../guidelines/c++-style/namespace.md) - Namespace conventions
- [File Naming](../../guidelines/c++-style/file-naming.md) - File naming conventions

## Related Skills

Use these related skills when working with external code porting:

- [octk-reference-header-file-generation](../../ai/skills/octk-reference-header-file-generation.md) - Generate reference header files in the include directory
- [octk-header-rename](../../ai/skills/octk-header-rename.md) - Rename headers according to OpenCTK naming conventions

## Prompt Content

```
You are an expert C++ developer working on the OpenCTK project. Your task is to port external code to conform to OpenCTK's coding style and conventions. Follow these guidelines:

1. **Code Formatting**: Apply OpenCTK's .clang-format rules:
   - Allman style braces (opening brace on new line)
   - 4 spaces indentation
   - 120 character line limit
   - Right-aligned pointers/references
   - LF line endings
   - No tabs

2. **Naming Conventions**:
   - Files: Add `octk_` prefix, use snake_case, appropriate extensions (.hpp/.cpp/.ipp/.tpp/_p.hpp)
   - Namespaces: Use `octk` root namespace with appropriate subnamespaces
   - Classes/Structs/Enums: PascalCase
   - Functions/Methods: camelCase
   - Member Variables: m prefix + camelCase (mMemberVariable)
   - Local Variables: camelCase
   - Constants: UPPER_CASE_WITH_UNDERSCORES
   - Macros: UPPER_CASE_WITH_UNDERSCORES

3. **Header Files**:
   - Use `#pragma once` instead of traditional guards
   - **CRITICAL RULE: By default, ALL headers should be ported as private headers with the `_p.hpp` suffix**
   - Only create public headers (without `_p` suffix) if explicitly specified and justified
   - Follow proper include order (std, third-party, OpenCTK, project-specific)
   - Use angle brackets for std/third-party, quotes for OpenCTK/project headers

4. **Dependency Management**:
   - Replace external dependencies with OpenCTK equivalents when available
   - Create adaptation layers for missing dependencies
   - Use OpenCTK's error handling and logging mechanisms

5. **API Adaptation**:
   - Update function signatures to match OpenCTK conventions
   - Add appropriate namespace qualifiers
   - Adjust inheritance hierarchies if needed

6. **Directory Structure**:
   - Public headers: `/src/libs/<module>/include/`
   - Private headers: `/src/libs/<module>/include/private/`
   - Template implementations: `/src/libs/<module>/include/detail/`
   - Source files: `/src/libs/<module>/source/`

7. **Best Practices**:
   - Maintain original functionality
   - Add proper documentation
   - Test thoroughly
   - Follow OpenCTK's build system conventions

Please port the following code to OpenCTK style:

```cpp
// [External code to port here]
```
```