# ISSUES

- **UI cursor / capture** (`UserInterface.cpp`, `Window.cpp`): legacy `ui.capture()` uses `GLFW_CURSOR_DISABLED`; retained slider drag uses `GLFW_CURSOR_HIDDEN` via `requestUiCursorCapture`. Game + retained UI on one window still need a single owner if both want capture at once.
- **Build warnings** (Debug x64, ~2,084): mostly C4244/C4245/C4267 conversions and C4100 unused params, which hide real ones. ~100 C4651/C4653 mean some files compile with options that differ from `OrkaPch.hpp`. `ULLUtil.cpp:19` and `QuadtreeNode.cpp:184` shift a 64-bit value by `64 - n`, which is UB when `n == 0`.
