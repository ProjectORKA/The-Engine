# UI architecture and roadmap (retained UI as the app shell)

Target model for ORKA's UI and the work left to reach it. Read before large UI or game-shell refactors. Styling, widgets and validation: `UI/DESIGN_RULES.md`. Threading guardrails: `AGENTS.md`.

---

## Goal

**One retained UI document owns windows, layout, text and input; games and tools are composable components (interactive surfaces + HUD markup), not separate `UIElement` trees per app.**

---

## Architecture

### App shell

- One shell `UIElement` per OS window hosts a `UiHost` + `AppHost` (`using AppHost = UiAuthoredHost`). The authored document is the app; the shell is only a size/backbuffer adapter.
- Reference shells: launcher (`GameLoader.cpp`, handed over from `ORKAIntroSequence.cpp`), Pong (`Pong.cpp`, `Data/UI/pong.ui.html`), Simple RTS (`SimpleRTS.cpp`, `simple-rts.ui.html` with `state.setupVisible`), Melon Audio Lab, Sound Analysis Lab, AI Chat.

### Authored bundles

- Paired `*.ui.html` + `*.ui.scss` under `Data/UI/`, compiled by `UiAuthoredHost` → `UiBundleCompiler` → `UiTemplateInstantiator` + `UiWidgetBridge`.
- Components: `@import "…" as tag;` expands through `UiComponentExpander` / `UiComponentRegistry` (e.g. `pong-score.ui.html`, `pong-game-view.ui.html` composed into `menu.ui.html`).

### Interactive surfaces (game = layout node)

A running game is a normal layout node with:

| Concern | Behavior |
|---------|----------|
| **Visual** | Drawn by a surface provider (FBO or draw callback) into the node's computed layout rect. |
| **Input** | Same event router as controls; focus on pointer down, pointer capture like any control. |
| **Children** | Allowed, e.g. HUD markup inside the surface node. |
| **Lifecycle** | A C++ controller registered by tag (`UiMarkupSurfaceRegistry`) owns sim/renderer; markup binds to `state.*`. |

Types: `UI/Surfaces/UiInteractiveSurface.hpp`, `UiWidgetKind::InteractiveSurface`, `UiWidgetFactory::interactiveSurface()`. `UiHost::render` calls `renderInteractiveSurfaces()`.

### Logical windows

- `<window title="…">` subtrees map to OS windows (`LogicalWindow`, `mountLogicalWindow`; demo: `UiLogicalWindowsDemo`, `logical-windows-demo.ui.html`).
- Split screen is plain flex: two surface siblings in one window (`pong-split.ui.html`).

```html
<ui>
  <window title="Split demo" class="row">
    <pong player="1" class="fg1" />
    <pong player="2" class="fg1" />
  </window>
</ui>
```

### DOM viewer

`<dom-viewer />` renders a self-excluding text tree of the live document (`UI/Inspector/UiDomViewerText`); clicking a line highlights the node (`UiHost::domViewerHighlight`). Used in the Controls Demo.

### Input path

- **Main thread:** `glfwPollEvents` → GLFW callbacks → `Window::routeInput`. Pointer hits on a `UiHost` go only to that host; the host consumes an event when the document handles it. Unhandled keys continue to `UIElement::inputEvent`.
- **Window thread:** `sampleMousePositionForFrame` → `UiHost::update` (pointer sync, queue drain, layout, scrub drivers). Frame `mouseDelta` is window-thread; queued buttons/keys go main → window.

---

## Remaining work

1. **Migrate the remaining games.** Most demos still use `GameRenderer : UIElement` as the window root (29 headers). Move each to an interactive surface component with authored HUD; new games must not add `GameRenderer` roots.
2. **Shrink `UIElement` / `Window`** to a thin adapter (size sync, optional GL backbuffer). Remove `UiLegacyTheme` once no legacy panel uses it.
3. **One text path for app UI.** Product HUD text moves from `Renderer::text()` to authored labels; `Renderer::text()` stays for debug overlays.
4. **Retire `ui.capturing`.** 3D camera games (`Mooncrash`, `MelonPlayer`, `DungeonsAndDiscord`, …) still use it; route their capture through surface pointer capture.
5. **Unify the input/threading policy** (one owner for cursor capture and keyboard routing per window, see `ISSUES.md`) and record it in `DESIGN_RULES.md`.

---

## Markup conventions

- **Root tag:** `<ui>` or `<app>`, one per bundle or merged app graph.
- **Window:** `<window title="…" id="…">`; optional for single-window apps.
- **Surface / game:** project tags (`<pong-game>`, `<rts-world>`) registered by the game module in `UiMarkupSurfaceRegistry`.
- **Layout:** `UiAppFlex` classes only; split screen is flex, not an engine mode.
- **State:** `state.*` bindings per component scope; cross-window state in a shared registry.

---

## Input and focus rules

1. Hit-test the retained document first (surfaces and their HUD children included).
2. Clicking a surface focuses it; keys route to the surface controller while focused, unless focus is in a child text field.
3. Surfaces use the same pointer capture/release as controls.
4. Discrete events may be queued; continuous drag/scrub uses window-thread frame samples.
5. Each OS window views the same or a linked document; focus is per logical window.

---

## Open questions

1. **One document vs one per OS window**: a single global `UiDocument` with window subtrees (preferred for the DOM viewer) or one document per window with a sync registry.
2. **Simulation hookup**: registry from surface id → `GameSimulation` (keeps the split from `Game.hpp`).
3. **Launcher**: stay a special-case flow or become a `<launcher>` component in a permanent shell.
4. **Headless**: `--modern-ui-test` must keep exiting cleanly; DOM viewer optional in test builds.

---

## Non-goals

- Embedding a full Chromium/HTML5 engine.
- CSS grid/flex beyond what `UiLayoutEngine` + the SCSS adapter support.
- Networked UI or remote DOM.

---

## Success criteria

- New games ship component markup + controller only.
- Editor and standalone differ only by root layout markup.
- Two instances of a game in one window are two surface siblings in flex.
- The DOM viewer can inspect the full UI from a second logical window.
- One layout engine and one text renderer for app UI.

---

## Agent checklist (before large UI refactors)

1. Read this file + `DESIGN_RULES.md`.
2. Extend `UiAuthoredHost`, the bundle compiler or the surface registry instead of adding `UIElement` widgets.
3. Keep the sim/render split: surfaces draw on the window thread, sims tick on the sim thread.
4. Extend `UiMarkupParser` / `UiBundleCompiler` instead of adding a second template language; document new grammar in `DESIGN_RULES.md`.
5. When remaining work lands, delete its item here and update the Architecture section.
