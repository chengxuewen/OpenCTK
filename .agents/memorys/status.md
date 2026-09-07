# OpenCTK 子模块状态

> 最后更新: 2026-09-07

## 基本信息

| 项 | 值 |
|---|---|
| 当前 commit | `78c8d7d` |
| 分支 | `master`（工作树 detached，与 master 同位） |
| 与 origin/master | 一致（无未推送提交） |
| 工作树 | 干净（仅未跟踪 `.codegraph/` 索引） |
| MSRTC 父项目 commit | `42e830b` (chengxuewen/dev) |

## ad524e9 之后提交概览（2026-06-09 → 2026-07-07）

| Commit | 类型 | 摘要 |
|---|---|---|
| `a1b886f` | fix(media) | V4L2 DMA buffer 拆除顺序 + capture 清理 fd 泄漏 |
| `57347dd` | fix(openctk) | 名称解析 guard + base names + OCTK_NAMESPACE 定义前移 |
| `33e7621` | fix(openctk) | FindWrap 子构建加 -Wno-deprecated / --no-warn-unused-cli 压告警 |
| `10f8d73` | chore | 删除未用 FindWrap（CppZMQ/FTXUI/Libnng/MessagePack/ZeroMQ） |
| `e5e936a` | fix | 模块拆分后 include 路径修正（openctk/core→media/network） |
| `6d17208` | fix | 多配置生成器保留全部 config + Windows CMake 兼容（meson python、--config flag） |
| `0e49a8d` | fix(cmake) | pkg-config 路径修复 + 系统 pkgconf 优先 |
| `84cb303` | feat(core) | yaml-cpp wrap 模块（源码构建 .a，octk::yaml） |
| `e9486a1` | fix(cmake) | 恢复 PKG_CONFIG_PATH cache + 系统路径回退 |
| `78c8d7d` | fix(cmake) | 多配置生成器检测 + Windows 构建兼容 |

## 关键能力现状

- **yaml-cpp wrap**（84cb303，最终保留）：FindWrapYamlCpp.cmake 源码构建静态 .a，`octk::yaml` 命名空间，挂在 OpenCTK::Core PUBLIC_LIBRARIES。注意 6d17208 曾因 MSVC 2019 dragonbox.h 模板错误临时移除，后续重新引入。
- **pkg-config 链**：系统 pkgconf 优先（find_program），无则构建捆绑 pkgconf-2.5.1；路径驱动完全依赖 build.sh 导出的 `PKG_CONFIG_PATH` 环境变量
- **MSRTC 不再设置 `OCTK_ALT_NAMESPACE`**（改用 msrtc_add_library 自定义实现）；参数化保留，向后兼容
- include 布局：媒体头 `openctk/media/`、网络头 `openctk/network/`（模块拆分后，勿用旧 `openctk/core/` 路径）
