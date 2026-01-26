# OpenCTK 头文件重命名技能

## 1. 技能概述

该技能用于根据 OpenCTK 项目的命名规范，将 C++ 头文件重命名为符合项目风格的名称。该技能确保所有头文件遵循一致的命名模式，提高代码的可读性和可维护性。

## 2. 命名规范分析

### 2.1 基本命名规则

- **文件前缀**：所有 OpenCTK 文件必须使用 `octk_` 前缀
- **命名风格**：文件名使用蛇形命名法（snake_case）
- **扩展名**：
  - 公共头文件：`.hpp`
  - 私有头文件：`_p.hpp`
  - 模板实现文件：`.ipp` 或 `.tpp`
  - 源文件：`.cpp`

### 2.2 目录结构规则

- **公共头文件**：应放置在 `/src/libs/<module>/include/` 目录下
- **私有头文件**：应放置在 `/src/libs/<module>/include/private/` 目录下
- **模板实现文件**：应放置在 `/src/libs/<module>/include/detail/` 目录下
- **源文件**：应放置在 `/src/libs/<module>/source/` 目录下

### 2.3 命名一致性

- 头文件与对应的源文件必须具有相同的文件名（不同扩展名）
- 私有头文件必须以 `_p.hpp` 结尾，且放在 `private/` 子目录中
- 模板实现文件（`.ipp`/`.tpp`）必须放在 `detail/` 子目录中

## 3. 技能输入参数

| 参数名 | 类型 | 描述 | 必需 |
|--------|------|------|------|
| file_path | string | 要重命名的头文件路径 | 是 |
| new_name | string | 新的文件名（不带前缀和扩展名） | 是 |
| file_type | string | 文件类型：`public`、`private` 或 `template` | 是 |
| module | string | 所属模块名称（如 `media`、`core`） | 是 |

## 4. 技能实现

### 4.1 重命名逻辑

1. **验证输入参数**：检查必填参数是否存在
2. **生成新文件名**：
   - 公共头文件：`octk_<new_name>.hpp`
   - 私有头文件：`octk_<new_name>_p.hpp`
   - 模板实现文件：`octk_<new_name>.ipp` 或 `octk_<new_name>.tpp`
3. **确定目标目录**：
   - 公共头文件：`/src/libs/<module>/include/`
   - 私有头文件：`/src/libs/<module>/include/private/`
   - 模板实现文件：`/src/libs/<module>/include/detail/`
4. **执行重命名**：将文件从原路径移动到目标路径
5. **更新引用**：在项目中更新对该头文件的所有引用

### 4.2 示例

**输入**：
```json
{
  "file_path": "/src/libs/media/source/codecs/video/svc/old_header.hpp",
  "new_name": "scalable_video_controller",
  "file_type": "public",
  "module": "media"
}
```

**输出**：
```json
{
  "old_path": "/src/libs/media/source/codecs/video/svc/old_header.hpp",
  "new_path": "/src/libs/media/include/octk_scalable_video_controller.hpp",
  "status": "success",
  "updated_references": 15
}
```

## 5. 最佳实践

1. **保持一致性**：确保头文件与源文件使用相同的基本名称
2. **正确分类**：根据文件的可见性选择正确的文件类型
3. **更新所有引用**：使用工具更新项目中对重命名文件的所有引用
4. **检查依赖关系**：确保重命名不会破坏其他文件的编译
5. **遵循目录结构**：将文件放在正确的目录中，便于其他开发者查找

## 6. 错误处理

| 错误类型 | 错误信息 | 处理方式 |
|----------|----------|----------|
| 参数缺失 | 缺少必填参数 | 返回错误信息，提示用户提供完整参数 |
| 文件不存在 | 指定的文件路径不存在 | 返回错误信息，提示用户检查文件路径 |
| 无效文件类型 | 指定的文件类型无效 | 返回错误信息，提示用户使用有效的文件类型 |
| 目标文件已存在 | 目标路径已存在同名文件 | 返回错误信息，提示用户选择其他名称 |
| 权限不足 | 没有足够的权限执行重命名 | 返回错误信息，提示用户检查文件权限 |

## 7. 相关技能

- **octk-reference-header-file-generation**：生成符合规范的引用头文件
- **octk-code-format**：根据项目规范格式化代码

## 8. 版本历史

| 版本 | 日期 | 变更描述 |
|------|------|----------|
| 1.0 | 2026-01-21 | 初始版本 |