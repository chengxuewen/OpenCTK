---
name: "octk-external-code-porting"
description: "将外部项目代码移植到符合 OpenCTK 的编码风格和约定。通过调整命名约定、格式、头文件和依赖项，将外部代码转换为符合 OpenCTK 规范的代码。"
author: "OpenCTK Team"
version: "1.0"
category: "code-generation"
tags: ["c++", "openctk", "外部代码", "移植"]
---

# OpenCTK 外部代码移植

[English Version](../octk-external-code-porting.md)

## 1. 技能概述

此技能用于将外部项目代码移植到符合 OpenCTK 的编码风格和约定。通过调整命名约定、格式、头文件和依赖项，将外部代码转换为符合 OpenCTK 规范的代码。

## 2. 移植流程

### 2.1 分析阶段

1. **代码结构分析**：分析外部代码的目录结构、文件组织和模块依赖关系
2. **依赖分析**：识别需要替换为 OpenCTK 等效项的外部依赖
3. **风格分析**：将外部代码风格与 OpenCTK 的风格指南进行比较
4. **API 分析**：识别需要适配或替换的 API

### 2.2 转换阶段

1. **代码格式化**：将 OpenCTK 的 .clang-format 规则应用到代码上
   - 使用 OpenCTK 根目录的 `.clang-format` 文件确保一致的格式化
   - 对所有移植的文件运行 `clang-format -i <file>`
   - 确保 Allman 大括号风格、4 空格缩进和 120 列限制
   - 格式化源文件 (.cpp) 和头文件 (.hpp, .ipp, .tpp)
2. **命名约定**：更新所有标识符以遵循 OpenCTK 的命名规则
3. **文件重命名**：重命名文件以遵循 OpenCTK 的文件命名约定
   - 为所有文件添加 `octk_` 前缀
   - 使用正确的文件扩展名（源文件为 .cpp，头文件为 .hpp，私有头文件为 _p.hpp）
4. **引用头文件创建（仅 C++）**：
   - 为每个移植的头文件在适当的 include 目录中创建相应的引用头文件
   - **公共头文件**：放在模块的 `include/` 目录中
   - **私有头文件**：放在模块的 `include/private/` 目录中
   - **模板实现头文件**：放在模块的 `include/detail/` 目录中
   - 确保所有引用头文件遵循 OpenCTK 的命名约定
5. **头文件适配**：
   - 将头文件转换为 OpenCTK 的 .hpp 格式
   - **关键规则：默认情况下，所有头文件都应移植为带有 `_p.hpp` 后缀的私有头文件**
   - 只有在明确指定并证明合理的情况下，才创建公共头文件（不带 `_p` 后缀）
   - 更新所有包含语句，对 OpenCTK 头文件使用尖括号 `<>`
   - 确保私有头文件使用 `<private/octk_*.hpp>` 格式
   - 确保模板实现使用 `<detail/octk_*.ipp>` 格式
6. **依赖替换**：用 OpenCTK 等效项替换外部依赖
7. **API 适配**：修改函数签名和调用以匹配 OpenCTK 的 API 风格

### 2.3 验证阶段

1. **头文件检查**：验证所有头文件包含是否正确且存在于代码库中
   - 检查所有引用的头文件是否存在于适当的 include 目录中
   - 验证 OpenCTK 头文件使用了正确的尖括号和前缀
   - 确保私有头文件使用 `<private/octk_*.hpp>` 格式
   - 确保模板实现文件使用 `<detail/octk_*.ipp>` 格式
2. **编译检查**：确保移植后的代码能够成功编译
3. **风格检查**：验证代码是否遵循 OpenCTK 的风格指南
4. **功能检查**：确保移植后的代码保持其原始功能
5. **集成检查**：验证代码是否与 OpenCTK 代码库正确集成

## 3. 代码风格转换

### 3.1 格式规则

OpenCTK 使用以下格式规则（在 `.clang-format` 中定义）：

- **大括号风格**：Allman 风格（左大括号在新行）
- **缩进**：每个缩进级别 4 个空格
- **列限制**：120 个字符
- **指针/引用对齐**：右对齐（`Type* variable`）
- **行尾**：LF（不是 CRLF）
- **制表符使用**：永远不要使用制表符，始终使用空格

### 3.2 命名约定

#### 3.2.1 文件命名

- 所有文件必须使用 `octk_` 前缀
- 公共头文件：`octk_<name>.hpp`
- 私有头文件：`octk_<name>_p.hpp`
- 模板实现：`octk_<name>.ipp` 或 `octk_<name>.tpp`
- 源文件：`octk_<name>.cpp`
- 所有文件名使用 snake_case

#### 3.2.2 标识符命名

- **命名空间**：使用 `octk` 作为根命名空间，模块使用子命名空间
- **类/结构体/枚举**：PascalCase（`ClassName`）
- **函数/方法**：camelCase（`functionName`）
- **成员变量**：m 前缀 + camelCase（`mMemberVariable`）
- **局部变量**：camelCase（`localVariable`）
- **常量**：大写加下划线（`CONSTANT_NAME`）
- **宏**：大写加下划线（`MACRO_NAME`）

### 3.3 头文件约定

#### 3.3.1 头文件保护

使用 `#pragma once` 而不是传统的头文件保护

#### 3.3.2 包含顺序

1. OpenCTK 头文件（按模块分组） - 将 OpenCTK 相关的头文件包含放在文件顶部
2. 标准库头文件
3. 第三方库头文件
4. 项目特定头文件

#### 3.3.3 包含语法

- **标准库和第三方库头文件**：使用尖括号：`<header.h>`
- **OpenCTK 公共头文件**：使用尖括号：`<octk_header.hpp>`
- **OpenCTK 私有头文件**：使用带 `private/` 前缀的尖括号：`<private/octk_header_p.hpp>`
- **OpenCTK 模板实现**：使用带 `detail/` 前缀的尖括号：`<detail/octk_header.ipp>` 或 `<detail/octk_header.tpp>`

**关键规则：所有 OpenCTK 相关的头文件包含必须使用尖括号 (<>)，而不是引号 ("")。**

#### 3.3.4 C++ 特性兼容性

- **C++11 兼容性**：确保代码与 C++11 标准兼容
- **替换现代 C++ 特性**：将 C++14 及以后引入的特性替换为 OpenCTK 等效项
  - `std::optional` → `<octk_optional.hpp>` 中的 `Optional`（移除 `#include <optional>`）
  - `std::nullopt` → `<octk_optional.hpp>` 中的 `utils::nullopt`
  - `std::string_view` 或 `absl::string_view` → `<octk_string_view.hpp>` 中的 `StringView`
  
  **注意**：在 `OCTK_BEGIN_NAMESPACE` 块内部，直接使用类型名而不需要 `octk::` 前缀（例如，使用 `ArrayView` 而不是 `octk::ArrayView`，使用 `Optional` 而不是 `octk::optional`）
  - 日志宏替换：
    - `RTC_LOG(LS_ERROR)` → `OCTK_ERROR()`
    - `RTC_LOG(LS_WARNING)` → `OCTK_WARNING()`
    - `RTC_LOG(LS_INFO)` → `OCTK_INFO()`
    - `RTC_LOG(LS_VERBOSE)` → `OCTK_VERBOSE()`
  - CHECK 宏替换：
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
  - `std::variant` → `<octk_variant.hpp>` 中的 `Variant`
  - `std::any` → `<octk_any.hpp>` 中的 `Any`
  - `std::unique_ptr` → `std::unique_ptr`（如果可用）或 OpenCTK 智能指针
  - `std::shared_ptr` → `std::shared_ptr`（如果可用）或 OpenCTK 智能指针
  - 范围基 for 循环 → 如果需要，使用传统 for 循环
  - Lambda 表达式 → 如果需要，使用函数对象或传统函数
  - 移动语义 → 如果需要，使用 OpenCTK 等效项
  - `absl::InlinedVector` → `<octk_inlined_vector.hpp>` 中的 `InlinedVector`

**关键规则：始终使用 OpenCTK 自己的现代 C++ 特性实现，而不是标准库实现。**

## 4. 依赖替换

### 4.1 常见依赖替换

| 外部依赖 | OpenCTK 等效项 |
|----------|----------------|
| STL 容器 | OpenCTK 的容器工具 |
| 外部日志 | OpenCTK 的日志系统 |
| 外部线程 | OpenCTK 的线程工具 |
| 外部时间工具 | OpenCTK 的时间工具 |
| 外部内存管理 | OpenCTK 的内存管理 |

### 4.2 依赖映射

1. **识别不可用依赖**：列出所有在 OpenCTK 中不可用的外部依赖
2. **查找替换项**：将每个不可用依赖映射到 OpenCTK 等效项
3. **修改代码**：更新代码以使用 OpenCTK 的等效功能
4. **添加适配层**：如果没有直接等效项，创建适配层

## 5. API 适配

### 5.1 函数签名适配

- 更新函数名称以遵循 OpenCTK 的 camelCase 约定
- 为全局函数添加 `octk_` 前缀
- 更新参数名称以遵循 OpenCTK 的命名约定
- 调整返回类型以匹配 OpenCTK 的类型系统
- 添加适当的命名空间限定符

### 5.2 类适配

- 更新类名以遵循 PascalCase 约定
- 添加 `octk` 命名空间或适当的子命名空间
- 更新成员变量以使用 m 前缀
- 更新成员函数以遵循 camelCase 约定
- 必要时调整继承层次结构

### 5.3 枚举和常量适配

- 更新枚举名称以遵循 PascalCase 约定
- 更新枚举值以遵循大写加下划线
- 更新常量以遵循大写加下划线
- 添加适当的命名空间限定符

## 6. 目录结构适配

### 6.1 OpenCTK 目录结构

```
OpenCTK/
├── src/
│   └── libs/
│       └── <module>/
│           ├── include/          # 公共头文件
│           │   ├── private/      # 私有头文件 (_p.hpp)
│           │   └── detail/       # 模板实现 (.ipp/.tpp)
│           └── source/           # 源文件 (.cpp)
└── docs/
    └── ai/
        └── skills/              # 技能文件
```

### 6.2 代码放置

- 将移植的代码放在适当的模块目录中
- 遵循 include/source 目录结构
- 将私有头文件放在 private/ 子目录中
- 将模板实现放在 detail/ 子目录中

## 7. 最佳实践

1. **增量移植**：以小的、可管理的块移植代码
2. **保持原始功能**：确保移植后的代码与原始代码行为相同
3. **遵循 OpenCTK 约定**：严格遵守 OpenCTK 的编码风格和约定
4. **添加文档**：更新或添加文档以匹配 OpenCTK 的文档风格
5. **彻底测试**：测试移植后的代码以确保其正常工作
6. **审查依赖**：仔细审查所有依赖，确保它们被正确替换
7. **保持兼容性**：确保移植后的代码与 OpenCTK 的构建系统兼容

## 8. 错误处理

### 8.1 常见移植问题

| 问题 | 解决方案 |
|------|----------|
| 缺少依赖 | 创建适配层或寻找替代实现 |
| API 不兼容 | 更新 API 以匹配 OpenCTK 的约定 |
| 构建错误 | 修复语法问题和缺少的包含 |
| 链接错误 | 检查库依赖和符号可见性 |
| 运行时错误 | 调试并修复移植过程中引入的逻辑问题 |

### 8.2 错误处理策略

1. **使用 OpenCTK 的错误处理**：用 OpenCTK 的机制替换外部错误处理
2. **添加适当的错误检查**：确保所有函数适当处理错误
3. **使用 OpenCTK 的异常模型**：遵循 OpenCTK 的异常处理指南
4. **适当记录错误**：使用 OpenCTK 的日志系统记录错误

## 9. 工具和实用程序

### 9.1 代码格式化

- 使用 OpenCTK 的 `.clang-format` 文件格式化代码
- 运行 `clang-format -i <file>` 应用格式化

### 9.2 静态分析

- 使用 OpenCTK 的静态分析工具检查问题
- 运行 `clang-tidy` 识别潜在问题

### 9.3 构建系统集成

- 更新 CMakeLists.txt 或其他构建文件以与 OpenCTK 的构建系统集成
- 遵循 OpenCTK 的构建系统约定

## 10. 示例工作流程

1. **分析外部代码**：了解代码的结构、依赖关系和功能
2. **创建移植计划**：制定将代码移植到 OpenCTK 的计划
3. **应用代码格式化**：使用 clang-format 应用 OpenCTK 的格式化规则
4. **更新命名约定**：更新类、函数和变量名称以遵循 OpenCTK 的约定
5. **文件重命名**：为所有文件添加 `octk_` 前缀和正确的文件扩展名
6. **引用头文件创建（仅 C++）**：在 include 目录（public、private、detail）中创建适当的引用头文件
7. **替换依赖**：用 OpenCTK 等效项替换外部依赖
8. **更新头文件**：将头文件转换为 OpenCTK 的格式，并更新包含语句以对 OpenCTK 头文件使用尖括号
9. **构建和测试**：编译移植后的代码并运行测试以确保功能
10. **审查和完善**：审查移植后的代码并进行必要的改进
11. **与 OpenCTK 集成**：将移植后的代码添加到 OpenCTK 代码库

## 11. 版本历史

| 版本 | 日期 | 描述 |
|------|------|------|
| 1.0 | 2026-01-21 | 初始版本 |