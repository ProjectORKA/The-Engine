# Pong

Agent notes for the Pong demo. Implementation: `Pong.cpp`, `Pong.hpp`, `Data/UI/pong*.ui.html`.

## Input

- **Escape** opens and closes a pause overlay with Resume and Exit. It does not close the window. Exit on the overlay does (the process ends; there is no return to the launcher).
- Opening the overlay releases cursor capture so the buttons can be clicked. Playfield clicks do not recapture while the overlay is up.
- **Alt+Enter** toggles fullscreen (main Enter and numpad Enter).
- Both run in the `Window::keyInputCallback` wrapper installed after `UiHost::create` (GLFW thread, where monitor changes are allowed). `PongSurface::inputEvent` must consume those presses. `routeInput` only walks parents, so the nested `UiHost` is skipped unless it is focused, and cursor capture skips it entirely. If the press is left unconsumed, `Window` toggles fullscreen a second time and the game stays windowed. The pause flag is an atomic written on that GLFW thread.
- **Left click** on the playfield captures the cursor (`ui.capture`). **Right click** releases it. Both are handled in `PongSurface::inputEvent` (same pattern as SimpleRTS). Clicks on chrome (e.g. Reset) do not capture.

## Play

- You are the bottom paddle. The AI is the top paddle.
- Move with the mouse, A/D, or the left and right arrow keys. Space or left click serves.
- Mouse X is mapped through `aspectCorrectNormalizedSpace` (`pongPointerToWorldX`). A raw 0..1 pointer X does not line up with the court on a wide window.
- While the cursor is captured, paddle mouse control stays active even if the virtual pointer leaves the playfield bounds (clamped to the surface).

## Look

- The shell uses `pongMonochromeTheme()` (pure black surfaces, white text, gray borders), not `UiAppTheme::light()` or `dark()`.
- The playfield clears to pure black. Paddles and the ball stay white. Court walls and impact particles are gray (`pongGray`). The ball keeps a short fading trail of its recent positions.
