# OpenCTK Header Rename Skill

## 1. Skill Overview

This skill is used to rename C++ header files according to OpenCTK project naming conventions, ensuring consistent naming patterns across all header files to improve code readability and maintainability.

## 2. Naming Convention Analysis

### 2.1 Basic Naming Rules

- **File Prefix**: All OpenCTK files must use the `octk_` prefix
- **Naming Style**: Files use snake_case naming convention
- **Extensions**:
  - Public headers: `.hpp`
  - Private headers: `_p.hpp`
  - Template implementation: `.ipp` or `.tpp`
  - Source files: `.cpp`

### 2.2 Directory Structure Rules

- **Public headers**: Should be placed in `/src/libs/<module>/include/` directory
- **Private headers**: Should be placed in `/src/libs/<module>/include/private/` directory
- **Template implementation**: Should be placed in `/src/libs/<module>/include/detail/` directory
- **Source files**: Should be placed in `/src/libs/<module>/source/` directory

### 2.3 Naming Consistency

- Header files must have the same base name as their corresponding source files (different extensions)
- Private headers must end with `_p.hpp` and be placed in the `private/` subdirectory
- Template implementation files (`.ipp`/`.tpp`) must be placed in the `detail/` subdirectory

## 3. Skill Input Parameters

| Parameter Name | Type | Description | Required |
|----------------|------|-------------|----------|
| file_path | string | Path of the header file to rename | Yes |
| new_name | string | New file name (without prefix and extension) | Yes |
| file_type | string | File type: `public`, `private`, or `template` | Yes |
| module | string | Module name (e.g., `media`, `core`) | Yes |

## 4. Skill Implementation

### 4.1 Rename Logic

1. **Validate Input Parameters**: Check if all required parameters are present
2. **Generate New File Name**:
   - Public header: `octk_<new_name>.hpp`
   - Private header: `octk_<new_name>_p.hpp`
   - Template implementation: `octk_<new_name>.ipp` or `octk_<new_name>.tpp`
3. **Determine Target Directory**:
   - Public header: `/src/libs/<module>/include/`
   - Private header: `/src/libs/<module>/include/private/`
   - Template implementation: `/src/libs/<module>/include/detail/`
4. **Execute Rename**: Move the file from the original path to the target path
5. **Update References**: Update all references to this header file in the project

### 4.2 Example

**Input**:
```json
{
  "file_path": "/src/libs/media/source/codecs/video/svc/old_header.hpp",
  "new_name": "scalable_video_controller",
  "file_type": "public",
  "module": "media"
}
```

**Output**:
```json
{
  "old_path": "/src/libs/media/source/codecs/video/svc/old_header.hpp",
  "new_path": "/src/libs/media/include/octk_scalable_video_controller.hpp",
  "status": "success",
  "updated_references": 15
}
```

## 5. Best Practices

1. **Maintain Consistency**: Ensure header files have the same base name as their corresponding source files
2. **Correct Classification**: Choose the appropriate file type based on visibility
3. **Update All References**: Use tools to update all references to the renamed file in the project
4. **Check Dependencies**: Ensure renaming doesn't break compilation of other files
5. **Follow Directory Structure**: Place files in the correct directories for easy discovery by other developers

## 6. Error Handling

| Error Type | Error Message | Handling Method |
|------------|---------------|-----------------|
| Missing Parameters | Missing required parameters | Return error message, prompt user to provide complete parameters |
| File Not Found | Specified file path does not exist | Return error message, prompt user to check file path |
| Invalid File Type | Specified file type is invalid | Return error message, prompt user to use valid file type |
| Target File Exists | Target path already has a file with the same name | Return error message, prompt user to choose another name |
| Insufficient Permissions | Insufficient permissions to execute rename | Return error message, prompt user to check file permissions |

## 7. Related Skills

- **octk-reference-header-file-generation**: Generate reference header files according to conventions
- **octk-code-format**: Format code according to project specifications

## 8. Version History

| Version | Date | Change Description |
|---------|------|--------------------|
| 1.0 | 2026-01-21 | Initial version |