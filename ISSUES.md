# ISSUES

- **UI cursor / capture** (`UserInterface.cpp`, `Window.cpp`): legacy `ui.capture()` uses `GLFW_CURSOR_DISABLED`; retained slider drag uses `GLFW_CURSOR_HIDDEN` via `requestUiCursorCapture`. Game + retained UI on one window still need a single owner if both want capture at once.
