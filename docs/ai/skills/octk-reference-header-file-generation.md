---
name: "openctk-include-header-generator"
description: "Generate OpenCTK include directory reference header files. This skill is used to quickly create reference header files in the include directory when adding new octk header files."
author: "OpenCTK Team"
version: "1.0"
category: "code-generation"
tags: ["c++", "openctk", "header", "include"]
---

# OpenCTK Reference Header File Generation

[Chinese Version](./cn/octk-引用头文件生成.md)

This skill generates reference header files in the OpenCTK include directory. These reference header files simply include the actual implementation header files from the source directory.

## Usage

Use this skill when you need to create a new reference header file in the OpenCTK include directory. The skill will generate the appropriate reference header file that includes the actual implementation header.

## Input Parameters

| Parameter | Type | Description | Required | Default Value |
|-----------|------|-------------|----------|---------------|
| `header_name` | string | The name of the header file to generate (without extension). For example: `octk_new_feature` | Yes | - |
| `module_type` | string | The module type where the implementation file is located. Common values: `tools`, `media`, `network`, etc. | Yes | - |
| `include_dir` | string | The include directory path. Defaults to the core include directory. | No | `/src/libs/core/include` |
| `source_dir` | string | The source directory path. Defaults to the core source directory. | No | `/src/libs/core/source` |

## Output

| Field | Type | Description |
|-------|------|-------------|
| `success` | boolean | Whether the header file was successfully generated. |
| `message` | string | Success or error message. |
| `file_path` | string | The path to the generated reference header file. |
| `content` | string | The content of the generated reference header file. |

## Example

### Input

```json
{
  "header_name": "octk_new_feature",
  "module_type": "tools"
}
```

### Output

```json
{
  "success": true,
  "message": "Reference header file generated successfully",
  "file_path": "/src/libs/core/include/octk_new_feature.hpp",
  "content": "#include \"../source/tools/octk_new_feature.hpp\"\n"
}
```

## Implementation Details

### Reference Header File Naming Rules

1. **Filename Consistency**: The reference header filename must exactly match the implementation filename, including all suffixes like `_p` for private headers
2. **Private Headers**: For implementation files ending with `_p.hpp` (private implementation headers), the reference header must:
   - Be placed in the `private` subdirectory
   - Have the exact same filename as the implementation file (including the `_p` suffix)
3. **Template Implementation Files**: For `.ipp` or `.tpp` files, the reference header must be placed in the `detail` subdirectory

### Generated Reference Header Format

#### Regular Headers
```cpp
#include "../source/<module_type>/<header_name>.hpp"
```

#### Private Headers (`_p.hpp`)
```cpp
#include "../source/<module_type>/<header_name>_p.hpp"
```

#### Template Implementation Files
```cpp
#include "../source/<module_type>/<header_name>.ipp"
```

Where:
- `<module_type>` is the module type provided as input
- `<header_name>` is the header name provided as input

## Directory Structure

The skill assumes the following directory structure:

```
OpenCTK/
├── src/
│   └── libs/
│       └── core/
│           ├── include/          # Reference header files go here
│           │   ├── private/      # Private reference headers (_p.hpp) go here
│           │   └── detail/       # Template implementation references (.ipp/.tpp) go here
│           └── source/           # Actual implementation files go here
│               ├── tools/        # Example module type directory
│               └── other_module/ # Other module type directories
└── docs/
    └── ai/
        └── skills/              # This skill file location
```

## Best Practices

1. Always use the `octk_` prefix for all OpenCTK header files
2. **CRITICAL: Filename Consistency**: The reference header filename must exactly match the implementation filename, including all suffixes like `_p` for private headers. This is essential for maintaining a consistent and predictable codebase.
3. **Private Headers**: For implementation files ending with `_p.hpp`:
   - Place the reference header in the `private` subdirectory
   - Ensure the reference header filename exactly matches the implementation filename (including the `_p` suffix)
4. **Template Implementation Files**: For `.ipp` or `.tpp` files, place the reference header in the `detail` subdirectory
5. Choose a descriptive module type that matches the functionality
6. Ensure the actual implementation file exists before generating the reference file
7. Follow the existing naming conventions for consistency

## Error Handling

The skill will handle common errors:

1. Missing required parameters
2. Invalid header name format
3. Directory not found errors
4. File already exists errors

## Example Workflow

1. Create a new implementation header file: `/src/libs/core/source/tools/octk_new_feature.hpp`
2. Use this skill to generate the reference header file
3. The skill creates: `/src/libs/core/include/octk_new_feature.hpp`
4. The reference file includes the implementation file

This workflow ensures consistency across the codebase and makes it easy to manage header files.