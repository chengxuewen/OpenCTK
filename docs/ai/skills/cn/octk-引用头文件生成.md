---
name: "openctk-include-header-generator"
description: "生成OpenCTK include目录引用头文件。当添加新的octk头文件时，使用此技能快速在include目录中创建引用头文件。"
author: "OpenCTK Team"
version: "1.0"
category: "code-generation"
tags: ["c++", "openctk", "header", "include", "头文件", "引用"]
---

# OpenCTK 引用头文件生成

[English Version](../octk-reference-header-file-generation.md)

此技能在OpenCTK包含目录中生成引用头文件。这些引用头文件只是简单地包含来自源目录的实际实现头文件。

## 使用方法

当您需要在OpenCTK包含目录中创建新的引用头文件时，使用此技能。该技能将生成适当的引用头文件，其中包含实际的实现头文件。

## 输入参数

| 参数 | 类型 | 描述 | 必填 | 默认值 |
|-----------|------|-------------|----------|---------------|
| `header_name` | string | 要生成的头文件名（不带扩展名）。例如：`octk_new_feature` | 是 | - |
| `module_type` | string | 实现文件所在的模块类型。常见值：`tools`、`media`、`network` 等 | 是 | - |
| `include_dir` | string | 包含目录路径。默认为核心包含目录 | 否 | `/src/libs/core/include` |
| `source_dir` | string | 源目录路径。默认为核心源目录 | 否 | `/src/libs/core/source` |

## 输出

| 字段 | 类型 | 描述 |
|-------|------|-------------|
| `success` | boolean | 头文件是否成功生成 |
| `message` | string | 成功或错误消息 |
| `file_path` | string | 生成的引用头文件的路径 |
| `content` | string | 生成的引用头文件的内容 |

## 示例

### 输入

```json
{
  "header_name": "octk_new_feature",
  "module_type": "tools"
}
```

### 输出

```json
{
  "success": true,
  "message": "引用头文件生成成功",
  "file_path": "/src/libs/core/include/octk_new_feature.hpp",
  "content": "#include \"../source/tools/octk_new_feature.hpp\"\n"
}
```

## 实现细节

### 引用头文件命名规则

1. **文件名一致性**：引用头文件名必须与实现文件名完全匹配，包括所有后缀（如私有头文件的 `_p` 后缀）
2. **私有头文件**：对于以 `_p.hpp` 结尾的实现文件（私有实现头文件），引用头文件必须：
   - 放在 `private` 子目录中
   - 与实现文件同名（包括 `_p` 后缀）
3. **模板实现文件**：对于 `.ipp` 或 `.tpp` 文件，引用头文件必须放在 `detail` 子目录中

### 生成的引用头文件格式

#### 常规头文件
```cpp
#include "../source/<module_type>/<header_name>.hpp"
```

#### 私有头文件 (`_p.hpp`)
```cpp
#include "../source/<module_type>/<header_name>_p.hpp"
```

#### 模板实现文件
```cpp
#include "../source/<module_type>/<header_name>.ipp"
```

其中：
- `<module_type>` 是提供的模块类型输入
- `<header_name>` 是提供的头文件名输入

## 目录结构

该技能假定以下目录结构：

```
OpenCTK/
├── src/
│   └── libs/
│       └── core/
│           ├── include/          # 引用头文件在此处
│           │   ├── private/      # 私有引用头文件 (_p.hpp) 在此处
│           │   └── detail/       # 模板实现引用 (.ipp/.tpp) 在此处
│           └── source/           # 实际实现文件在此处
│               ├── tools/        # 示例模块类型目录
│               └── other_module/ # 其他模块类型目录
└── docs/
    └── ai/
        └── skills/              # 此技能文件位置
            └── cn/              # 中文技能文件位置
```

## 最佳实践

1. 所有OpenCTK头文件始终使用`octk_`前缀
2. **至关重要：文件名一致性**：引用头文件名必须与实现文件名完全匹配，包括所有后缀（如私有头文件的 `_p` 后缀）。这对于维护一致且可预测的代码库至关重要。
3. **私有头文件**：对于以 `_p.hpp` 结尾的实现文件：
   - 将引用头文件放在 `private` 子目录中
   - 确保引用头文件名与实现文件名完全匹配（包括 `_p` 后缀）
4. **模板实现文件**：对于 `.ipp` 或 `.tpp` 文件，将引用头文件放在 `detail` 子目录中
5. 选择与功能匹配的描述性模块类型
6. 在生成引用文件之前确保实际实现文件存在
7. 遵循现有命名约定以保持一致性

## 错误处理

该技能将处理常见错误：

1. 缺少必需参数
2. 无效的头文件名格式
3. 目录未找到错误
4. 文件已存在错误

## 示例工作流

1. 创建新的实现头文件：`/src/libs/core/source/tools/octk_new_feature.hpp`
2. 使用此技能生成引用头文件
3. 技能创建：`/src/libs/core/include/octk_new_feature.hpp`
4. 引用文件包含实现文件

此工作流确保代码库的一致性，并使头文件管理变得容易。