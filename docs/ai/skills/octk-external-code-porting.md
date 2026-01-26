---
name: "octk-external-code-porting"
description: "Port external project code to conform to OpenCTK's coding style and conventions. Transform external code into OpenCTK-compliant code by adjusting naming conventions, formatting, headers, and dependencies."
author: "OpenCTK Team"
version: "1.0"
category: "code-generation"
tags: ["c++", "openctk", "external-code", "porting"]
---

# OpenCTK External Code Porting

[Chinese Version](./cn/octk-外部代码移植.md)

## 1. Skill Overview

This skill is designed to port external project code to conform to OpenCTK's coding style and conventions. It transforms external code into OpenCTK-compliant code by adjusting naming conventions, formatting, headers, and dependencies.

## 2. Porting Process

### 2.1 Analysis Phase

1. **Code Structure Analysis**: Analyze the external code's directory structure, file organization, and module dependencies
2. **Dependency Analysis**: Identify external dependencies that need to be replaced with OpenCTK equivalents
3. **Style Analysis**: Compare external code style with OpenCTK's style guidelines
4. **API Analysis**: Identify APIs that need to be adapted or replaced

### 2.2 Transformation Phase

1. **Code Formatting**: Apply OpenCTK's .clang-format rules to the code
   - Use OpenCTK's root `.clang-format` file for consistent formatting
   - Run `clang-format -i <file>` on all ported files
   - Ensure Allman brace style, 4-space indentation, and 120-column limit
   - Format both source files (.cpp) and header files (.hpp, .ipp, .tpp)
2. **Naming Conventions**: Update all identifiers to follow OpenCTK's naming rules
3. **File Renaming**: Rename files to follow OpenCTK's file naming conventions
   - Add `octk_` prefix to all files
   - Use correct file extensions (.cpp for source, .hpp for headers, _p.hpp for private headers)
4. **Reference Header Creation (C++ Only)**: 
   - For each ported header file, create the corresponding reference header in the appropriate include directory
   - **Public headers**: Place in the module's `include/` directory
   - **Private headers**: Place in the module's `include/private/` directory
   - **Template implementation headers**: Place in the module's `include/detail/` directory
   - Ensure all reference headers follow OpenCTK's naming conventions
5. **Header Adaptation**: 
   - Convert header files to OpenCTK's .hpp format
   - **CRITICAL RULE: By default, ALL headers should be ported as private headers with the `_p.hpp` suffix**
   - Only create public headers (without `_p` suffix) if explicitly specified and justified
   - Update all include statements to use angle brackets `<>` for OpenCTK headers
   - Ensure private headers use `<private/octk_*.hpp>` format
   - Ensure template implementations use `<detail/octk_*.ipp>` format
6. **Dependency Replacement**: Replace external dependencies with OpenCTK equivalents
7. **API Adaptation**: Modify function signatures and calls to match OpenCTK's API style

### 2.3 Validation Phase

1. **Header Check**: Verify all header file includes are correct and exist in the codebase
   - Check that all referenced headers exist in the appropriate include directories
   - Verify proper use of angle brackets and prefixes for OpenCTK headers
   - Ensure private headers use `<private/octk_*.hpp>` format
   - Ensure template implementations use `<detail/octk_*.ipp>` format
2. **Compilation Check**: Ensure the ported code compiles successfully
3. **Style Check**: Verify code follows OpenCTK's style guidelines
4. **Functionality Check**: Ensure the ported code maintains its original functionality
5. **Integration Check**: Verify the code integrates properly with the OpenCTK codebase

## 3. Code Style Transformation

### 3.1 Formatting Rules

OpenCTK uses the following formatting rules (defined in `.clang-format`):

- **Brace Style**: Allman style (opening braces on new lines)
- **Indentation**: 4 spaces per indentation level
- **Column Limit**: 120 characters
- **Pointer/Reference Alignment**: Right-aligned (`Type* variable`)
- **Line Endings**: LF (not CRLF)
- **Tab Usage**: Never use tabs, always use spaces

### 3.2 Naming Conventions

#### 3.2.1 File Naming

- All files must use the `octk_` prefix
- Public headers: `octk_<name>.hpp`
- Private headers: `octk_<name>_p.hpp`
- Template implementations: `octk_<name>.ipp` or `octk_<name>.tpp`
- Source files: `octk_<name>.cpp`
- Use snake_case for all filenames

#### 3.2.2 Identifier Naming

- **Namespaces**: Use `octk` as the root namespace, with subnamespaces for modules
- **Classes/Structs/Enums**: PascalCase (`ClassName`)
- **Functions/Methods**: camelCase (`functionName`)
- **Member Variables**: m prefix + camelCase (`mMemberVariable`)
- **Local Variables**: camelCase (`localVariable`)
- **Constants**: UPPER_CASE_WITH_UNDERSCORES (`CONSTANT_NAME`)
- **Macros**: UPPER_CASE_WITH_UNDERSCORES (`MACRO_NAME`)

### 3.3 Header File Conventions

#### 3.3.1 Header Guards

Use `#pragma once` instead of traditional header guards

#### 3.3.2 Include Order

1. OpenCTK headers (grouped by module) - Place OpenCTK-related header includes at the top of the file
2. Standard library headers
3. Third-party library headers
4. Project-specific headers

#### 3.3.3 Include Syntax
- **Standard and third-party headers**: Use angle brackets: `<header.h>`
- **OpenCTK public headers**: Use angle brackets: `<octk_header.hpp>`
- **OpenCTK private headers**: Use angle brackets with `private/` prefix: `<private/octk_header_p.hpp>`
- **OpenCTK template implementations**: Use angle brackets with `detail/` prefix: `<detail/octk_header.ipp>` or `<detail/octk_header.tpp>`

**CRITICAL RULE: ALL OpenCTK-related header includes must use angle brackets (<>), not quotes ("").**

#### 3.3.4 C++ Feature Compatibility

- **C++11 Compatibility**: Ensure code is compatible with C++11 standards
- **Replace Modern C++ Features**: Replace features introduced in C++14 and later with OpenCTK equivalents
  - `std::optional` → `Optional` from `<octk_optional.hpp>` (REMOVE `#include <optional>`)
  - `std::nullopt` → `utils::nullopt` from `<octk_optional.hpp>`
  - `std::string_view` or `absl::string_view` → `StringView` from `<octk_string_view.hpp>`
  
  **NOTE**: Inside the `OCTK_BEGIN_NAMESPACE` block, use the types without the `octk::` prefix (e.g., `ArrayView` instead of `octk::ArrayView`, `Optional` instead of `octk::optional`)
  - Logging macros replacement:
    - `RTC_LOG(LS_ERROR)` → `OCTK_ERROR()`
    - `RTC_LOG(LS_WARNING)` → `OCTK_WARNING()`
    - `RTC_LOG(LS_INFO)` → `OCTK_INFO()`
    - `RTC_LOG(LS_VERBOSE)` → `OCTK_VERBOSE()`
  - CHECK macros replacement:
    - `RTC_CHECK` → `OCTK_CHECK`
    - `RTC_DCHECK` → `OCTK_DCHECK`
    - `RTC_CHECK_EQ` → `OCTK_CHECK_EQ`
    - `RTC_DCHECK_EQ` → `OCTK_DCHECK_EQ`
    - `RTC_CHECK_NE` → `OCTK_CHECK_NE`
    - `RTC_DCHECK_NE` → `OCTK_DCHECK_NE`
    - `RTC_CHECK_LT` → `OCTK_CHECK_LT`
    - `RTC_DCHECK_LT` → `OCTK_DCHECK_LT`
    - `RTC_CHECK_LE` → `OCTK_CHECK_LE`
    - `RTC_DCHECK_LE` → `OCTK_DCHECK_LE`
    - `RTC_CHECK_GT` → `OCTK_CHECK_GT`
    - `RTC_DCHECK_GT` → `OCTK_DCHECK_GT`
    - `RTC_CHECK_GE` → `OCTK_CHECK_GE`
    - `RTC_DCHECK_GE` → `OCTK_DCHECK_GE`
  - `std::variant` → `Variant` from `<octk_variant.hpp>`
  - `std::any` → `Any` from `<octk_any.hpp>`
  - `std::unique_ptr` → `std::unique_ptr` (if available) or OpenCTK smart pointers
  - `std::shared_ptr` → `std::shared_ptr` (if available) or OpenCTK smart pointers
  - Range-based for loops → Use traditional for loops if needed
  - Lambda expressions → Use function objects or traditional functions if needed
  - Move semantics → Use OpenCTK equivalents if needed
  - `absl::InlinedVector` → `InlinedVector` from `<octk_inlined_vector.hpp>`

**CRITICAL RULE: ALWAYS use OpenCTK's own implementations for modern C++ features instead of standard library implementations.**

## 4. Dependency Replacement

### 4.1 Common Dependencies to Replace

| External Dependency | OpenCTK Equivalent |
|---------------------|---------------------|
| STL containers | OpenCTK's container utilities |
| External logging | OpenCTK's logging system |
| External threading | OpenCTK's threading utilities |
| External time utilities | OpenCTK's time utilities |
| External memory management | OpenCTK's memory management |

### 4.2 Dependency Mapping

1. **Identify Unavailable Dependencies**: List all external dependencies that are not available in OpenCTK
2. **Find Replacements**: Map each unavailable dependency to an OpenCTK equivalent
3. **Modify Code**: Update the code to use OpenCTK's equivalent functionality
4. **Add Adaptation Layers**: If no direct equivalent exists, create adaptation layers

## 5. API Adaptation

### 5.1 Function Signature Adaptation

- Update function names to follow OpenCTK's camelCase convention
- Add `octk_` prefix to global functions
- Update parameter names to follow OpenCTK's naming conventions
- Adjust return types to match OpenCTK's type system
- Add appropriate namespace qualifiers

### 5.2 Class Adaptation

- Update class names to follow PascalCase convention
- Add `octk` namespace or appropriate subnamespace
- Update member variables to use m prefix
- Update member functions to follow camelCase convention
- Adjust inheritance hierarchies if needed

### 5.3 Enum and Constant Adaptation

- Update enum names to follow PascalCase convention
- Update enum values to follow UPPER_CASE_WITH_UNDERSCORES
- Update constants to follow UPPER_CASE_WITH_UNDERSCORES
- Add appropriate namespace qualifiers

## 6. Directory Structure Adaptation

### 6.1 OpenCTK Directory Structure

```
OpenCTK/
├── src/
│   └── libs/
│       └── <module>/
│           ├── include/          # Public headers
│           │   ├── private/      # Private headers (_p.hpp)
│           │   └── detail/       # Template implementations (.ipp/.tpp)
│           └── source/           # Source files (.cpp)
└── docs/
    └── ai/
        └── skills/              # Skill files
```

### 6.2 Code Placement

- Place ported code in the appropriate module directory
- Follow the include/source directory structure
- Place private headers in the private/ subdirectory
- Place template implementations in the detail/ subdirectory

## 7. Best Practices

1. **Incremental Porting**: Port code in small, manageable chunks
2. **Maintain Original Functionality**: Ensure the ported code behaves identically to the original
3. **Follow OpenCTK Conventions**: Strictly adhere to OpenCTK's coding style and conventions
4. **Add Documentation**: Update or add documentation to match OpenCTK's documentation style
5. **Test Thoroughly**: Test the ported code to ensure it works correctly
6. **Review Dependencies**: Carefully review all dependencies to ensure they're properly replaced
7. **Maintain Compatibility**: Ensure the ported code is compatible with OpenCTK's build system

## 8. Error Handling

### 8.1 Common Porting Issues

| Issue | Solution |
|-------|----------|
| Missing dependencies | Create adaptation layers or find alternative implementations |
| API incompatibilities | Update APIs to match OpenCTK's conventions |
| Build errors | Fix syntax issues and missing includes |
| Link errors | Check library dependencies and symbol visibility |
| Runtime errors | Debug and fix logic issues introduced during porting |

### 8.2 Error Handling Strategies

1. **Use OpenCTK's Error Handling**: Replace external error handling with OpenCTK's mechanisms
2. **Add Proper Error Checking**: Ensure all functions handle errors appropriately
3. **Use OpenCTK's Exception Model**: Follow OpenCTK's exception handling guidelines
4. **Log Errors Appropriately**: Use OpenCTK's logging system to log errors

## 9. Tools and Utilities

### 9.1 Code Formatting

- Use OpenCTK's `.clang-format` file to format code
- Run `clang-format -i <file>` to apply formatting

### 9.2 Static Analysis

- Use OpenCTK's static analysis tools to check for issues
- Run `clang-tidy` to identify potential problems

### 9.3 Build System Integration

- Update CMakeLists.txt or other build files to integrate with OpenCTK's build system
- Follow OpenCTK's build system conventions

## 10. Example Workflow

1. **Analyze External Code**: Understand the code's structure, dependencies, and functionality
2. **Create Porting Plan**: Develop a plan for porting the code to OpenCTK
3. **Apply Code Formatting**: Use clang-format to apply OpenCTK's formatting rules
4. **Update Naming Conventions**: Update class, function, and variable names to follow OpenCTK's conventions
5. **File Renaming**: Add `octk_` prefix and correct file extensions to all files
6. **Reference Header Creation (C++ Only)**: Create appropriate reference headers in the include directories (public, private, detail)
7. **Replace Dependencies**: Replace external dependencies with OpenCTK equivalents
8. **Update Headers**: Convert headers to OpenCTK's format and update include statements to use angle brackets for OpenCTK headers
9. **Build and Test**: Compile the ported code and run tests to ensure functionality
10. **Review and Refine**: Review the ported code and make any necessary refinements
11. **Integrate with OpenCTK**: Add the ported code to the OpenCTK codebase

## 11. Version History

| Version | Date | Description |
|---------|------|-------------|
| 1.0 | 2026-01-21 | Initial version |