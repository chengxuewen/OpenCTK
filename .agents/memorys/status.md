# OpenCTK 子模块状态

> 最后更新: 2026-06-09

## 基本信息

| 项 | 值 |
|---|---|
| 当前 commit | `803add1` |
| 分支 | `master` |
| 未推送提交 | 1 (`803add1`) |
| 工作树 | 干净 |
| MSRTC 父项目 commit | `02a1078` |

## MSRTC 修改摘要

| 文件数 | 描述 |
|---|---|
| 9 | OCTK_ALT_NAMESPACE 参数化 |
| 1 | http.hpp include 修正 |
| **10** | **总计修改文件** |

## OCTK_ALT_NAMESPACE 使用

- MSRTC 不再设置 `OCTK_ALT_NAMESPACE` (改用 `msrtc_add_library` 自定义实现)
- 参数化本身无害，保持向后兼容
- 若未来有其他消费者需要，可直接利用此机制
