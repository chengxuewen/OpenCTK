# GitHub Copilot 编码规则

## C++ 核心规则

### 代码风格
- **命名空间**： 
    - 使用 `OCTK_BEGIN_NAMESPACE`和`OCTK_END_NAMESPACE` 包装所有octk_开头的文件代码
    - 如果include的头文件没有定义OCTK_BEGIN_NAMESPACE宏，则include <octk_global.hpp>
    - 细节实现 放在`namespace detail {} // namespace detail` 命名空间中
    - 命名空间代码不缩进
- **命名规范**：
    - 类名：PascalCase（如 `EventLoop`、`AbstractEventDispatcher`）
    - 成员变量：带m前缀（如 `mDispatcher`、`mLoop`）
    - 函数：camelCase（如 `initialize()`、`runOnce()`）
    - 常量：UPPER_SNAKE_CASE
- **头文件规范**：
    - 格式：`#pragma once` 置于文件首行
    - 宏定义：`#ifndef _OCTK_<FILE>_HPP` 作为备选（仅在特殊需求时）
    - 包含：source目录下文件包含`octk_`开头的头文件时需要使用绝对路径方式`<...>`
    - 前缀：如果包含`octk_`开头的头文件是以`.ipp`或`.tpp`结尾的，则需要加前缀`detail/`，例如<detail/octk_meta_type.tpp>
    - 私有：如果包含`octk_`开头的头文件是`*_p.*`格式的说明该文件是私有实现文件，则需要加前缀`private/`，例如<private/octk_platform_thread_p.hpp>
- **指针/引用**：优先使用智能指针 `std::shared_ptr<T>`、`std::unique_ptr<T>`
- **线程安全**：使用 `std::mutex` 和 `std::lock_guard` 保护共享资源

### 架构模式
- **抽象接口**：使用纯虚类定义扩展点（如 `AbstractEventDispatcher`）
- **工厂模式**：通过 `EventBackendFactory` 实现后端适配
- **RAII**：确保资源自动释放，析构函数调用 `finalize()`

### 跨平台支持
- **编译器检测**：`#if defined(_MSVC_) || defined(__GNUC__) || defined(__clang__)`
- **平台宏**：`OCTK_OS_WINDOWS`、`OCTK_OS_LINUX`、`OCTK_OS_MACOS`
- **导出符号**：`OCTK_<MODULE>_API`用于 DLL/SO 符号可见性
- **对齐**：`OCTK_ALIGNED(N)` 宏处理不同编译器的对齐语法

### 错误处理
- **错误码**：使用 `std::error_code` 和 `std::error_category`
- **日志**：统一使用 OCTK_DEBUG、OCTK_INFO、OCTK_WARN、OCTK_ERROR 宏

### API 文档
- **工具**：Doxygen 生成 C++ 文档，Sphinx 生成 Python 文档
- **格式**：C++ 使用 `//` 风格注释，Python 使用 docstring
- **输出**：`docs/api/` 目录存放生成的 HTML/PDF

### 持续集成
- **代码检查**：clang-format（C++）、black（Python）、eslint（JavaScript）
- **静态分析**：clang-analyzer、cppcheck、SonarQube
- **编译**：支持 Debug/Release/RelWithDebInfo/MinSizeRel
- **平台**：Linux (GCC/Clang)、macOS (Clang)、Windows (MSVC)

### 模块划分与依赖
- **core**：基础设施（事件循环、线程池、内存管理）
- **media**：音视频处理（编码、解码、格式转换如 ARGB→I420）
- **network**：网络传输（协议栈、TCP/UDP、WebSocket、WebRTC）
- **service**：高层服务（OSGI、微服务）
- **依赖方向**：service → network → core（严格单向依赖）

### 文件组织

### Agent限制
- **限制**：不允许修改*instructions.md文件
- **禁止**：不允许创建*instructions.md文件

