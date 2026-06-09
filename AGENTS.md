# OpenCTK (3rdparty 子模块)

## 项目概述

OpenCTK — 开源 C++ 工具包。作为 MSRTC 的 3rdparty submodule 引入。

## 在 MSRTC 中的角色

- **构建方式**: `add_subdirectory(3rdparty/OpenCTK)` 集成
- **依赖关系**: MSRTC 的 core/engine 库链接到 `OpenCTK::Core` / `OpenCTK::Media`
- **CMake 复用**: MSRTC 安全复用了 `octk_find_package`、`octk_add_subdirectory`、`octk_option` 等，但**不复用** `octk_add_library`

## 关键文件

| 路径 | 用途 |
|---|---|
| `cmake/OpenCTKLibraryHelpers.cmake` | `octk_add_library()` 定义 (~820行) |
| `cmake/OpenCTKTargetHelpers.cmake` | 目标别名、属性设置 |
| `cmake/OpenCTKGlobalStateHelpers.cmake` | `OCTK_REPO_KNOWN_LIBRARIES` 全局注册表 |
| `cmake/OpenCTKScopeFinalizerHelpers.cmake` | `cmake_language(DEFER)` 回调系统 |
| `cmake/OpenCTKPublicWrapHelpers.cmake` | 三方库头文件包装安装 |
| `src/libs/core/` | OpenCTK 核心库 (spdlog, yaml-cpp, fmt, json 等) |
| `src/libs/media/` | WebRTC/FFmpeg 媒体库 |
| `src/libs/network/` | 网络/HTTP 库 |

## MSRTC 进行的修改

### OCTK_ALT_NAMESPACE 参数化 (commit 803add1)

对 9 个 cmake 文件添加了 `OCTK_ALT_NAMESPACE` 备用变量支持。所有硬编码 `"OpenCTK"` 字符串替换为 `_octk_namespace` 文件作用域变量 (`if(DEFINED OCTK_ALT_NAMESPACE)` → `"MSRtc"` 回退 `OCTK_NAMESPACE`)。

**修改文件**: OpenCTKTargetHelpers, OpenCTKLibraryHelpers, OpenCTKFindPackageHelpers, OpenCTKExecutableHelpers, OpenCTKPkgConfigHelpers, OpenCTKPublicWalkLibsHelpers, OpenCTKPublicWrapHelpers, OpenCTKFlagHandlingHelpers, OpenCTKInternalTargets

**向后兼容**: 未设 `OCTK_ALT_NAMESPACE` 时行为与原版完全相同。

### 其他修改
- `http.hpp`: 修正本地 include 为完整 `openctk/network/` 路径

## 子模块更新规则

- 更新前确认 MSRTC cmake configure + build 通过
- 修改子模块文件后，需要在 MSRTC 主项目中 `git add 3rdparty/OpenCTK` 更新指针
- 不要在子模块的 `src/libs/` C++ 源码中随意修改 (momo/sora-cpp-sdk 第三方代码)
