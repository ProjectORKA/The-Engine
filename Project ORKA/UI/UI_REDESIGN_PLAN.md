# UI redesign plan (retained UI as the app shell)

This document is the **source of truth** for why ORKA is moving from a hybrid UI stack to a single retained, authored UI model. Future agents should read this before large UI or game-shell refactors.

Related docs: `UI/DESIGN_RULES.md` (styling/widgets today), `AGENTS.md` (threading guardrails).

---

## Goal (one sentence)

**One global retained UI document owns windows, layout, text, and input; games and tools are composable components (interactive surfaces + HUD markup), not separate `UIElement` trees per app.**

---

## Current state (as of 2026)

### Two widget stacks, one tree shell

| Layer | Role today | Key types / files |
|-------|------------|-------------------|
| **Shell** | Window layout, hit-test order, legacy draw chrome | `UIElement`, `Window` (`Window.hpp`), `UserInterface.cpp` |
| **Modern retained UI** | Document, flex layout, MSDF text, controls | `UiHost`, `UiDocument`, `UiLayoutEngine`, `UiWidgetFactory`, `UiAuthoredHost` |
| **Games** | Often the **root** of the shell | `GameRenderer : UIElement`, `PongRenderer`, `SimpleRtsRenderer` + bolt-on panels |
| **Immediate text** | HUD / debug without layout | `Renderer::text()` in game `renderScene` |

Modern surfaces (launcher, controls demo, AI chat, RTS setup) are implemented as:

```text
Window → wrapper UIElement → UiHost (child) → authored .ui.html
```

Games like **Pong** use **no** `UiHost`: score is `Renderer::text()` inside `renderScene`. Games that have real time settings or an editor invert the desired model: the **game** is at the window root; realtime settings UI is a sibling panel.

### Authored UI today

- Bundles: paired `*.ui.html` + `*.ui.scss` under `Data/UI/`.
- Pipeline: `UiAuthoredHost` → `UiBundleCompiler` → `UiTemplateInstantiator` + `UiWidgetBridge`.
- Supported tags are **flat** (see `DESIGN_RULES.md`); `@for` / `@if` / bindings exist; **no** Angular-style component imports or multi-file composition yet.
- `dense-editor.ui.html` is an **aspirational** layout (`<world-view>`, grid panes) — not wired as the engine shell.

### Viewport-shaped ideas (partial, not product)

- `UiWidgetFactory::gameViewport()` / `worldSurface()` exist (`UiWidgets.hpp`) but **`renderGameViewports()` is not driven from `UiHost::render`**.
- This plan **deprecates “viewport widget” as the primary model** in favor of **interactive surface nodes** (see below). Viewport APIs may be folded into that or removed after migration.

### Input and threading (constraint for redesign)

- **Main thread:** `glfwPollEvents` → GLFW callbacks → `Window::routeInput` → legacy bubble and/or `UiHost::enqueueEvent`.
- **Window thread:** `sampleMousePositionForFrame` → `UiHost::update` (pointer sync, queue drain, layout, scrub drivers).
- See prior agent discussions: frame `mouseDelta` is **window-thread**; queued buttons/keys are **main → window**. Any unified model must preserve one coherent policy (document in this file when decided).

---

## Problems (why redesign)

1. **Inverted ownership** — Games are layout roots; app UI is attached to the game. Target: **app UI is root; game is a region inside it.**
2. **Duplicate systems** — Two layouts (`UIElement` flex vs `UiLayoutEngine`), two text paths (`Renderer::text` vs `UiTextPainter`), two input paths (immediate vs queued router).
3. **Integration tax** — Using modern layout/text requires **adding a `UiHost` child**, not calling one API from game code.
4. **Windows are special** — OS windows are tied to `Window : UIElement` + one child tree; no first-class “multiple logical windows / splits” in the document.
5. **No component composition** — Cannot ship `pong.html` and `<pong-score />` from another bundle; no multi-root “app DOM” assembly.
6. **Debuggability** — No built-in live DOM inspector in the UI itself.

---

## Target architecture

### 1. Single app document (`UiDocument`)

- Every ORKA application frame is described by **one retained document** (possibly assembled from many component templates).
- **OS windows** are nodes in that document (see §3), not the only place UI lives.
- Legacy `UIElement` becomes a **thin adapter** (or is deleted after migration): size sync, optional GL backbuffer hook, **no** parallel widget set.

### 2. Game = interactive surface (not a dedicated “viewport” type)

A running game is a **normal layout node** (conceptually a `<div>`) with:

| Concern | Behavior |
|---------|----------|
| **Visual** | Background / content from a **surface provider** (FBO, swapchain region, or procedural draw callback) — “special background,” not a separate engine concept in markup. |
| **Input** | Focusable, pointer capture, keyboard focus — **same event router** as buttons and text fields when the surface is “active.” |
| **Children** | Allowed — e.g. `<ingame-hud>` overlay inside `<game>`. |
| **Lifecycle** | C++ **component controller** bound by tag (e.g. `<pong-game>`) owns sim/renderer; UI binds to `state.*` properties. |

Markup shape (illustrative):

```html
<ui>
  <window>
    <editor class="row">
      <sidepanel class="fb300">…</sidepanel>
      <pong class="row fg1 p20">
        <pong-score class="fb200" player="1">
        <pong-game-view class="fg1">
        <pong-score class="fb200" player="2">
      </game>
    </editor>
  </window>
</ui>
```

**Standalone vs editor:** Same component templates; root layout differs (full-screen `<game>` only vs `<editor>` chrome). No second integration path.

### 3. Window nodes are not special

**Logical window** = document subtree rooted at `<window>` (or equivalent), mapped to:

- **One OS window** (today’s `Window`), or
- **A tile in a parent OS window** (flex sibling → automatic split), or
- **Future:** second OS window from the same document (multi-monitor / editor + game).

Examples:

```html
<ui>
  <window title="Pong P1">
    <pong player="1" />
  </window>
  <window title="Pong P2">
    <pong player="2" />
  </window>
</ui>
```

```html
<ui>
  <window title="Split demo" class="row">
      <pong player="1" class="fg1" />
      <pong player="2" class="fg1" />
  </window>
</ui>
```

**Engine work:** window manager reads document → creates/updates N OS windows OR one window with flex splits; input routing uses document focus path, not only `pickElementAt` on legacy tree.

### 4. Component system (Angular-like composition)

**Requirements:**

- **Component definition** — e.g. `pong.ui.html` + optional `pong.ui.scss` + C++ registration (controller, properties, commands).
- **Custom tags** — `<pong-score player="1" />`, `<pong-game-view />` expand to internal blueprint subtree.
- **Composition** — Root `editor.ui.html` imports/includes components; compiler produces **one merged blueprint graph** (or linked bundle with stable node IDs).
- **Inputs** — Attributes map to component `@Input`-style properties (`player="1"` → `state.pong.p1.*` or scoped registry).
- **Track / reconcile** — Reuse existing `UiTemplateInstantiator` reconcile model; extend compiler for imports and component boundaries.

**Agent note:** Do not bolt on a second template language; extend `UiMarkupParser` / `UiBundleCompiler` and document the grammar in `DESIGN_RULES.md` when implemented.

### 5. DOM viewer component (debug)

Built-in component, e.g. `<dom-viewer />` or `<ui-inspector />`:

- Renders a **textual tree** of the live retained DOM (tags, ids, classes, key bindings, focus/hover flags).
- **Must exclude its own subtree** from the dump (prevent recursion / infinite mirror).
- Usable in a **dedicated debug window** or panel:

```html
<ui>
  …
  <window title="UI Inspector">
    <dom-viewer/>
  </window>
</ui>
```

Implementation sketch: walk `UiDocument` from root (or bound subtree); format to multiline text or monospace `TextArea`; optional sync highlight on click (later phase).

---

## Relationship to existing code

| Keep / extend | Retire / shrink |
|---------------|-----------------|
| `UiDocument`, `UiLayoutEngine`, `UiEventRouter`, `UiWidgetFactory`, themes | `UIButton`, `UISlider`, hand-built `UIElement` chrome for new work |
| `UiAuthoredHost` → rename/evolve to **AppHost** | `GameRenderer : UIElement` as default game entry (replace with surface component) |
| `UiPropertyRegistry`, commands, bindings | Per-game `addElement(settingsPanel)` siblings |
| `dense-editor.ui.html` as layout reference | `UiGameViewport` as primary abstraction (merge into surface) |
| `GameSimulation` / `GameRenderer` **logic** | Scores via `Renderer::text` for product HUD |

---

## Phased migration

Phases are ordered; **do not skip phase 0** (inventory + one vertical slice).

### Phase 0 — Document and vertical slice (proof)

- [ ] Wire **one** interactive surface tag (e.g. `<pong-game>`) in a **single** `UiHost` root filling the OS window.
- [ ] Move Pong score to authored labels bound to `state.pong.score.*`.
- [ ] Prove input: surface receives pointer when focused; HUD buttons still work.
- [ ] Update this plan with “done” dates and file pointers.

**Exit criteria:** Pong playable with **no** `PongRenderer : UIElement` as window root; one `.ui.html` root.

### Phase 1 — App shell owns the window

- [ ] Launcher / intro transition: post-intro shell is **only** retained UI (already close — `GameLoader` + `UiHost`).
- [ ] Standard pattern: `Window` creates **one** `AppHost`; no game type registered as direct child of `Window` except host adapter.
- [ ] `Simple RTS`: invert to shell + `<rts-world surface="…">` + setup overlay as markup, not `SimpleRtsSettingsPanel` sibling tree.

### Phase 2 — Surface nodes (game-as-div)

- [ ] Define `UiSurface` / `UiInteractiveSurface` semantic role: layout rect + draw callback + focus/capture policy.
- [ ] Map markup tag → C++ factory registration (component plugin table).
- [ ] Deprecate standalone `GameRenderer` framebuffer sizing from `UIElement` layout; size comes from **computed layout rect**.

### Phase 3 — Component compiler

- [ ] `@import` / `<component src="pong.ui.html" />` or equivalent in bundle compiler.
- [ ] Custom element registry: tag name → blueprint template + controller factory.
- [ ] Sample: `pong.ui.html` with `<pong-score />`, `<pong-game-view />`, composed into `menu.ui.html`.

### Phase 4 — Logical windows

- [ ] `<window>` node → OS window lifecycle (create, title, resize, close).
- [ ] Multi-window document (two OS windows, one process).
- [ ] Split-single-window: two surfaces in one OS window via flex (no extra OS windows).

### Phase 5 — DOM viewer + inspector UX

- [ ] `<dom-viewer />` component (self-excluding tree dump).
- [ ] Optional: pick node in viewer → highlight in main UI (later).

### Phase 6 — Legacy removal

- [ ] Remove unused `UIElement` widget controls after demos migrated.
- [ ] Single input path into `UiEventRouter` (legacy `routeInput` delegates to host or hit-test document only).
- [ ] Delete or stub `UiGameViewport` if fully replaced by surface nodes.

---

## Markup conventions (target)

- **Root tag:** `<ui>` or `<app>` — one per bundle or merged app graph.
- **Window:** `<window title="…" id="…">` — logical window; not required for single-window apps (implicit default window).
- **Surface / game:** project-specific tags (`<pong>`, `<rts-world>`, `<world-view>`) registered by game module.
- **Layout:** existing flex classes (`UiAppFlex`) — splitscreen is **only** flex, not a special engine mode.
- **State:** `state.*` bindings per component scope; document cross-window state in a shared registry when needed.

---

## Input and focus (design rules for implementation)

1. **Hit-test retained document first** (including surfaces and children HUD).
2. **Surface focus** — clicking a game surface focuses it; keys route to surface controller when focused (unless focus in child text field).
3. **Pointer capture** — surfaces use same capture/release as controls; no parallel `ui.capturing` for UI (keep `ui.capturing` only for legacy 3D camera until removed).
4. **Threading** — discrete events may enqueue; continuous drag/scrub uses window-thread frame samples (see number-field scrub pattern in `AGENTS.md`).
5. **Multi-window** — each OS window owns a view into the same or linked document; focus is per logical window.

*(Update this section when the unified policy is implemented.)*

---

## Non-goals (for now)

- Embedding a full Chromium/HTML5 engine.
- CSS grid/flex beyond what `UiLayoutEngine` + SCSS adapter already support.
- Networked UI or remote DOM.
- Backward compatibility with every old demo in one release — migrate high-value demos in phases.

---

## Open questions

1. **One document vs document per OS window** — single global `UiDocument` with window subtrees (preferred for DOM viewer) vs one document per window with sync registry.
2. **Simulation thread** — surfaces expose “tick” hooks; registry maps surface id → `GameSimulation` (unchanged principle from `Game.hpp`).
3. **Intro / GameLoader** — remain special-case process or become `<launcher>` component in permanent shell.
4. **Agent bridge / headless** — `--modern-ui-test` must still exit; DOM viewer optional in test builds.

---

## Success criteria (project-level)

- New games ship **component markup + controller** only; no new `GameRenderer : UIElement` roots.
- **Editor and standalone** differ only by root layout markup.
- Two instances of the same game in one window = **two surface siblings in flex**, no custom splitscreen code.
- **DOM viewer** can inspect full UI from a second logical window without recursion.
- **One** layout engine and **one** text renderer for app UI.

---

## Agent checklist (before large UI refactors)

1. Read this file + `DESIGN_RULES.md`.
2. Prefer extending `UiAuthoredHost` / compiler / surface registry over new `UIElement` widgets.
3. Do not add permanent `GameViewport`-only paths if surface node covers the use case.
4. Keep sim/render split; surfaces draw on window thread, sim ticks on sim thread.
5. When a phase completes, update **Phase checklist** above and add a one-line pointer in `AGENTS.md` if the workflow change is repo-wide.
