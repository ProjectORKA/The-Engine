# Windows IME native adapter requirement

GLFW's character callback reports committed Unicode text, but GLFW does not expose
the in-progress Windows IME composition (`WM_IME_*`) through its public API.
`Window::forwardCompositionStart`, `forwardCompositionUpdate`,
`forwardCompositionCommit`, and `forwardCompositionCancel` are the supported
native-to-ORKA seam.

A Windows adapter must:

1. Obtain the `HWND` with `glfwGetWin32Window` from `glfw3native.h`.
2. Observe `WM_IME_STARTCOMPOSITION`, `WM_IME_COMPOSITION`, and
   `WM_IME_ENDCOMPOSITION` without replacing or permanently subclassing GLFW's
   window procedure. A supported GLFW callback/hook or an independently owned
   child text-service window is required.
3. Read `GCS_COMPSTR`, `GCS_CURSORPOS`, `GCS_RESULTSTR`, and composition
   attributes through `ImmGetCompositionStringW`, convert UTF-16 to valid UTF-8,
   and translate UTF-16 offsets to UTF-8 byte offsets before calling the seam.
4. Forward start once, updates with the composing text and selected UTF-8 range,
   commit once for `GCS_RESULTSTR`, and cancel when composition ends without a
   result or when window/widget focus is lost.
5. Keep GLFW's own `WndProc` and callback chain intact. Installing a replacement
   with `SetWindowLongPtr(GWLP_WNDPROC)` is explicitly unsupported because GLFW
   relies on its procedure for input, focus, sizing, and internal lifetime state.

The current GLFW API used by ORKA exposes `glfwGetWin32Window`, but no safe
non-replacing message hook. Therefore the engine intentionally provides the seam
and committed-character path only; it does not install native IME handling.
