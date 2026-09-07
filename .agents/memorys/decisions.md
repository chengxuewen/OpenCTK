# OpenCTK 集成决策

> MSRTC 视角下的 OpenCTK 使用决策

## 为什么不复用 octk_add_library？

**决策**: MSRTC 创建自定义 `msrtc_add_library()` (~130行)，不复用 `octk_add_library()` (~820行)。

**原因**:
1. `octk_add_library()` 写入全局 CACHE 变量 (`OCTK_REPO_KNOWN_LIBRARIES`)
2. 使用 `cmake_language(DEFER)` 延迟执行，跨越 `add_subdirectory` 边界不安全
3. 创建命名空间别名 (`OpenCTK::Core`) 在 MSRTC 上下文中产生冲突
4. 1200+ 行 fork 成本 > 130 行自定义

## 为什么保留 OCTK_ALT_NAMESPACE？

**决策**: 保留已完成的参数化修改（803add1），不回退。

**原因**:
1. 修改完全向后兼容（未设变量时行为不变）
2. 对 OpenCTK 自身是质量提升（消除硬编码）
3. 为未来消费者提供灵活性
4. 回退有 git 冲突风险

## FindWrap 共享决策

**决策**: MSRTC 删除 FindWrap{GTest,OpenSSL,Libsrtp}，复用 OpenCTK 的对应模块。

**原因**: OpenCTK 已有 42 个 FindWrap 模块覆盖这些依赖。MSRTC 只保留独有的 (ZLMediaKit, Iceoryx2, TrroSdk, SDL2)。

## yaml-cpp wrap 模块

**决策**: 保留 yaml-cpp wrap（84cb303 引入，OpenCTK::Core PUBLIC_LIBRARIES，`octk::yaml` 命名空间）。

**原因**:
1. 源码构建静态 .a，无系统依赖
2. 6d17208 曾因 MSVC 2019 dragonbox.h 模板错误临时移除，后续重新引入并保留
3. MSRTC 的 yaml 配置解析链依赖此模块

**风险**: MSVC 2019 下有已知构建问题（dragonbox.h），Windows 消费者报错先查此点
