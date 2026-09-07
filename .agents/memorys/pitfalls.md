# OpenCTK 已知坑点

> 在 MSRTC 上下文中使用 OpenCTK 时需要注意的问题

## CMake

### ❌ 不要在 MSRTC 中调用 octk_add_library

会写入 `OCTK_REPO_KNOWN_LIBRARIES` (CACHE INTERNAL)，污染 OpenCTK 全局状态。

### ❌ 单次 MSRTCCMakeInit include 中不要包含 OpenCTKLibraryHelpers

重 include 会触发 file-scope 代码重新评估 `_octk_namespace`。

### ⚠️ octk_install_public_wrap_headers 仅适用 OpenCTK 命名

正则模式期望 `OpenCTKWrap{Name}::Wrap{Name}` 格式，不支持 MSRTC 的 `MSRTC3rdparty::Wrap{Name}` 格式。

### ⚠️ 构建后需检查 OpenCTK 目标完整性

修改 OpenCTK cmake 文件后，验证 `cmake -LA | grep "OpenCTK::"` 仍包含 Core、Media 等。

## 3rdparty 包

### ⚠️ SDL2 使用 .7z 格式

`msrtc_fetch_3rdparty` 已包装为 `octk_fetch_3rdparty` (支持 .7z)。但 examples 默认 ON 会触发 SDL2 下载。

### ⚠️ 构建目录 3rdparty 缓存

`build/Debug/3rdparty/` 包含 iceoryx2 等预编译依赖。删除构建目录会重新编译这些耗时包。

## 子模块管理

### ⚠️ 提交前更新子模块指针

修改 `3rdparty/OpenCTK` 后，MSRTC 主项目需 `git add 3rdparty/OpenCTK` 更新指针。

### ⚠️ git status 显示子模块为 "modified content"

`3rdparty/OpenCTK` 内有未提交修改时，MSRTC 的 `git status` 会显示子模块状态。

### ⚠️ pkg-config 链完全依赖 PKG_CONFIG_PATH 环境变量 (2026-06)

0e49a8d/e9486a1 后：FindWrapWebRTC 硬编码 gio/glib 路径已注释，portal 分支逻辑已移除——
全部依赖 build.sh 导出的 `PKG_CONFIG_PATH`。手动 cmake/ninja 不经 build.sh 时找不到 GLib/WebRTC 依赖。

### ⚠️ yaml-cpp MSVC 2019 dragonbox.h 模板错误 (2026-06)

曾导致 6d17208 整体移除 yaml-cpp。Linux/pixi 正常；Windows 消费者构建失败先查此点。

