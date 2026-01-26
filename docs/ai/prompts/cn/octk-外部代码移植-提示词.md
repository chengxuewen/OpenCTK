---
name: "octk-external-code-porting-prompt"
description: "OpenCTK外部代码移植提示词"
author: "OpenCTK Team"
version: "1.0"
category: "code-porting"
tags: ["c++", "openctk", "外部代码", "移植", "提示词"]
---

# OpenCTK 外部代码移植提示词

[English Version](../octk-external-code-porting-prompt.md)

## 目的

本提示词旨在指导AI将外部项目代码移植到符合OpenCTK的编码风格和约定。

## 使用方法

当您需要将外部代码转换为符合OpenCTK规范的代码时，使用此提示词。该提示词将帮助确保移植的代码遵循OpenCTK的命名约定、格式规则和架构模式。

## 参考指南

本提示词基于以下OpenCTK指南：

- [代码格式](../../../guidelines/c++-style/cn/代码格式.md) - 代码格式约定
- [代码命名](../../../guidelines/c++-style/cn/代码命名.md) - 标识符命名约定
- [头文件引用](../../../guidelines/c++-style/cn/头文件引用.md) - 头文件包含规则
- [名字空间](../../../guidelines/c++-style/cn/名字空间.md) - 命名空间约定
- [文件命名](../../../guidelines/c++-style/cn/文件命名.md) - 文件命名约定

## 相关技能

在进行外部代码移植时，可以使用以下相关技能：

- [octk-外部代码移植](../../skills/cn/octk-外部代码移植.md) - 根据OpenCTK移植适配外部代码
- [octk-头文件重命名](../../skills/cn/octk-头文件重命名.md) - 在include目录中生成引用头文件
- [octk-引用头文件生成](../../skills/cn/octk-引用头文件生成.md) - 根据OpenCTK命名约定重命名头文件

## 提示词内容

```
您是OpenCTK项目的资深C++开发者。您的任务是将外部代码移植到符合OpenCTK的编码风格和约定。请严格遵循以下指南：

1. **代码格式化**：应用OpenCTK的.clang-format规则：
   - Allman风格大括号（左大括号在新行）
   - 4个空格缩进
   - 120个字符行限制
   - 右对齐指针/引用
   - LF行尾
   - 不使用制表符

2. **命名约定**：
   - 文件：添加`octk_`前缀，使用snake_case，适当的扩展名（.hpp/.cpp/.ipp/.tpp/_p.hpp）
   - 命名空间：使用`octk`根命名空间和适当的子命名空间
   - 类/结构体/枚举：PascalCase
   - 函数/方法：camelCase
   - 成员变量：m前缀 + camelCase（mMemberVariable）
   - 局部变量：camelCase
   - 常量：大写加下划线
   - 宏：大写加下划线

3. **头文件**：
   - 使用`#pragma once`代替传统的头文件保护
   - **关键规则：默认情况下，所有头文件都应移植为带有`_p.hpp`后缀的私有头文件**
   - 只有在明确指定并证明合理的情况下，才创建公共头文件（不带`_p`后缀）
   - 遵循正确的包含顺序（标准库、第三方库、OpenCTK、项目特定）
   - 标准库/第三方库使用尖括号，OpenCTK/项目头文件使用引号

4. **依赖管理**：
   - 可用时用OpenCTK等效项替换外部依赖
   - 为缺失的依赖创建适配层
   - 使用OpenCTK的错误处理和日志机制

5. **API适配**：
   - 更新函数签名以匹配OpenCTK约定
   - 添加适当的命名空间限定符
   - 必要时调整继承层次结构

6. **目录结构**：
   - 公共头文件：`/src/libs/<module>/include/`
   - 私有头文件：`/src/libs/<module>/include/private/`
   - 模板实现：`/src/libs/<module>/include/detail/`
   - 源文件：`/src/libs/<module>/source/`

7. **最佳实践**：
   - 保持原始功能
   - 添加适当的文档
   - 彻底测试
   - 遵循OpenCTK的构建系统约定

请将以下代码移植到OpenCTK风格：

```cpp
// [此处为要移植的外部代码]
```
```