# Modern UI Design Rules (Agent Guide)

These rules apply to the retained UI stack under `Project ORKA/UI/` — `UiHost`, `UiWidgetFactory`, themes/painters, and UI demos.

Read this file before adding or changing widgets, themes, layouts, or UI samples.

## Start here (key paths)

When an edit touches modern UI, inspect these first:

- `Project ORKA/UI/UI_REDESIGN_PLAN.md` — target architecture for app shell, surfaces, and large refactors
- `Project ORKA/UI/Theme/UiAppTheme.hpp` + `UI/Theme/UiAppFlex.hpp` — master theme (colors, typography, light/dark) and flex spacing (`p5`–`p80`, `g5`–`g80`, `col`/`row`)
- `Project ORKA/UI/UiHost.hpp` + `UI/Widgets/UiWidgets.hpp` — modern host and controls
- `Project ORKA/UI/Authoring/UiAuthoredHost.hpp` — load `.ui.html` + `.ui.scss` bundles onto a host
- `Project ORKA/UserInterface.hpp` + `Project ORKA/UIElement.hpp` — legacy UI tree (when touching legacy panels)
- `Project ORKA/Data/UI/app-master.ui.scss` + `controls-demo.ui.html` — global authored styles and demo markup
- Reference wiring: `Project ORKA/GameLoader.cpp`; C++ sample host: `UI/Samples/UiControlsDemo.cpp`

## Agent requirements (must not skip)

**Single source of truth:** `UiAppTheme.hpp` (colors/typography) + `UiAppFlex.hpp` (flex spacing). Install with `host.useAppTheme(theme)` and semantic classes from `UiAppClass` / `UiAppFlexClass`.

1. **Flex + class spacing only** — flex containers default to **column**; add `row` only for horizontal stacks. Use `p5`–`p80` and `g5`–`g80` on widget classes, and `align-top-left` … `align-bot-right` for semantic child alignment (see `UiAppFlex.hpp`). No arbitrary padding, gap, or margin in surface code (legacy panels use `UiAppFlex::p10` etc.).
2. **Use `UiAppTheme` tokens** — no ad-hoc color literals or font sizes. Legacy `UIElement` panels use `UiLegacyTheme`.
3. **Theme paints, widgets semantic** — visuals only in `UiAppTheme.cpp`; surfaces use `UiAppClass` style classes.
4. **Light + dark mode** — both modes must look correct; add a theme toggle on new app shells when practical.
5. **Visible focus** on all focusable controls.
6. **Use `UiWidgetFactory` controls** (toggle, checkbox, slider, `numberField` / `<number-input>`, select, textField) instead of mimicking them with boxes — see [Number fields](#number-fields-numberfield--number-input).
7. **Attach layout engine + `syncControlLayouts()`** when using composite controls (toggles, checkboxes, selects).
8. **Showcase new reusable tokens/components** in `Data/UI/controls-demo.ui.html` (markup) + global SCSS under `Data/UI/`.
9. **Prefer authored UI** for demo/layout surfaces; keep C++ to property/command wiring via `UiAuthoredHost`.
10. **CSS overflow on `div`** (`overflow`, `overflow-x`, `overflow-y`: visible/hidden/scroll/auto) is handled automatically by `UiHost` + `UiOverflowController` — clipping via scissor, wheel + scrollbar input, no manual `updateScrollMetrics()` / `applyScrollShift()`. Reserve `<scroll-view>` for virtual lists or scroll-anchor bindings. As in CSS, `visible` on one axis clips when the other axis scrolls, and `align-items: stretch` only fills axes whose width/height is auto — give a child an explicit size to opt out of stretch.
11. **No unused markup ids** — add `id="…"` only when C++ (`ensureNodeLayout`, `findById`, …), bindings (`[id]="…"` for token/theme lookup), or tests need it. Prefer semantic classes for styling; do not duplicate class names as ids.
12. **No dead markup classes or UI clutter** — every class must be handled by SCSS, `UiAppTheme`, or C++ wiring. Do not add decorative aliases, one-off layout workaround classes, or duplicate helpers already on the same node; prefer fixing layout (e.g. content-aware flex mins, `min-h-0` on scroll children) or existing flex/theme tokens over new names. Nested surfaces use `panel` / `section` theme classes (highlight stacking in `UiAppTheme.cpp`); do not hand-pick nested background colors.
13. **Wrap rows use `b50`–`b350`** — basis classes on items + `row wrap` on the container (balanced line breaks for grow rows); color swatches use `.color-token-grid`. Prefer short flex utilities (`fg1`, `wrap`, `mw0`, `mh0`). See `UiAppFlex.hpp` and `.token-item b100` in the controls demo.

Reference implementation: `Data/UI/controls-demo.ui.html` + `Project ORKA/GameLoader.cpp`.

## Master Theme (Required)

**All modern UI surfaces must use the unified app theme** defined in `UI/Theme/UiAppTheme.hpp`.

- **Do not** add ad-hoc color literals, spacing values, or typography sizes in demo/game UI code.
- **Do not** create per-surface palettes (the old `UiControlsDemoPalette` pattern is removed).
- Install the theme with `host.useAppTheme(theme)`. Use `UiAppFlexClass` for layout/spacing; use remaining `UiAppClass` paint helpers (e.g. `field-label`, `typography-sample`, `panel`) only where the theme paints typography or chrome.
- Legacy `UIElement` panels should use `UiLegacyTheme` helpers so colors stay aligned with the master theme.

Light and dark mode are both supported via `UiAppTheme::light()` / `UiAppTheme::dark()` and `theme.setMode(...)`.

The **UI Controls Demo** is the living style guide — every token, component, and convention must be showcased there.

## Authored UI (HTML + SCSS)

Modern surfaces can be defined declaratively under `Project ORKA/Data/UI/`:

| File | Role |
|------|------|
| `app-master.ui.scss` | Global import root (tokens + flex + shared components) |
| `tokens.ui.scss` | Global SCSS variables (`$font-body`, `$font-title`) — spacing + the two font sizes only |
| `flex.ui.scss` | Reusable layout classes (`col`, `row`, `p10`, `g20`, …) |
| `components.ui.scss` | Shared layout helpers (`.section-header`, `.labeled-row`, `.token-grid`, …) |
| `controls-demo.ui.html` | **UI Controls Demo markup** — edit this to change the demo layout |
| `controls-demo.ui.scss` | Demo-specific styles (imports `app-master.ui.scss`) |

Page-level `.ui.scss` files should start with `@import "app-master.ui.scss";` so spacing tokens and flex classes stay consistent.

Runtime pipeline: `UiAuthoredHost` loads markup + styles → `UiTemplateInstantiator` + `UiWidgetBridge` (interactive tags) → `UiStyleApplicationAdapter`. C++ only registers commands, publishes property bindings (`state.*`, `tokens.*`), and installs `UiAppTheme`.

Supported markup tags: `div`, `label`, `button`, `toggle`, `checkbox`, `slider`, `number-input`, `select`, `input` (text or `type="number"`), `textarea`, `progress`, `scroll-view`, `image`. Bindings: `[value]`, `[text]`, `[options]`, `@for (...) { ... }`, `@if (...) { ... }`, `(activate)="Command.Name"`.

```html
@for (token of tokens.colors; track token.id) {
  <div class="token-item b100 g5">
    <div class="token-swatch theme-swatch" [id]="token.id"/>
    <label class="palette-label" [text]="token.label"/>
  </div>
}

@if (userUi.visible) {
  <div class="content">...</div>
}
```

**Ids are for wiring, not styling.** Only add `id="…"` when runtime code or a binding needs it; use classes (`UiAppClass`, page SCSS) for layout and theme. Do not add ids that mirror an existing class.

**Every class must do work.** A class is valid only when it is handled by compiled SCSS (layout/size/overflow), `UiAppTheme` (typography, surfaces, swatches, icon tint), or C++ wiring. Do not add decorative or documentary class names (`section`, `icon-default`, duplicate spacing helpers). Prefer a bare flex/gap wrapper (`class="g10"`) over a no-op semantic alias. Drop redundant duplicates on the same node (e.g. `wrap` when `.token-grid` already wraps, `flex-grow` when `.slider-row` already grows, `row`/`g10` on a single-child row).

## Architecture

- **Widgets emit semantics; themes paint visuals.** Widget code sets type names, style classes, roles, and state. Colors, borders, slider tracks, switch thumbs, focus rings, and dropdown chrome belong in `UI/Theme/UiAppTheme.cpp`, not in widget/demo code.
- **Prefer `UiWidgetFactory` controls** (`button`, `toggle`, `checkbox`, `slider`, `numberField`, `select`, `textField`, `textArea`, `scrollView`) over hand-built `Box`/`Text` trees that mimic controls.
- **Attach the layout engine to the widget factory** when a surface uses toggles, checkboxes, selects, or other composite controls:
  ```cpp
  widgets = std::make_unique<UiWidgetFactory>(host.document(), host.eventRouter());
  widgets->attachLayoutEngine(host.layoutEngine());
  ```
  Call `widgets->syncControlLayouts()` after the host/root size is known (typically each frame in `update()` until layout stabilizes).
- **Do not bypass `UiHost` input routing** for retained UI surfaces. Route pointer/keyboard/text through the host event queue and router.

## Layout and Spacing

**Flex-first layout.** Flex containers default to **column** (`UiLayoutStyle`). Add the `row` class for horizontal stacks only — do not add `col` unless you need to override a parent row. **Padding and gap are the only spacing drivers** — do not use arbitrary margins (except internal scroll-offset hacks in `UiAppHelpers`).

Master spacing: `UI/Theme/UiAppFlex.hpp`. Valid class tokens only:

| Padding class | px | Gap class | px | Basis class | px |
|---------------|----|-----------|-----|-------------|-----|
| `p5`          | 5  | `g5`      | 5  | `b50`       | 50  |
| `p10`         | 10 | `g10`     | 10 | `b100`      | 100 |
| `p20`         | 20 | `g20`     | 20 | `b150`      | 150 |
| `p40`         | 40 | `g40`     | 40 | `b200`      | 200 |
| `p80`         | 80 | `g80`     | 80 | `b250`      | 250 |
|               |    |           |    | `b300`      | 300 |
|               |    |           |    | `b350`      | 350 |

**Utility classes:** `fg1` (`flex-grow: 1`), `wrap` (balanced flex line breaks), `mw0` / `mh0` (zero min width/height for shrink). Legacy names `flex-grow`, `min-w-0`, and `min-h-0` still parse. Authored `flex-wrap: balance` still parses as an alias.

**Basis classes (`b50`–`b350`)** set `flex-basis` on items inside a wrapping row (`row wrap`). **`wrap`** precomputes line breaks to minimize flex-grow stretch — especially avoiding a single orphaned item stretched across a full row. Token rows (`.color-token-grid`) use `.token-item` (`flex-grow: 1`, `flex-shrink: 0`, `min-width: 0`) so items on each line share leftover space evenly.

**Align classes** position flex children with semantic top/left/bot/right names instead of separate `justify-content` + `align-items` rules. They map to the current flex direction (`column` default, or `row` when present):

| Class | Column container | Row container |
|-------|------------------|---------------|
| `align-top-left` | top + left | left + top |
| `align-top-center` | top + center | center + top |
| `align-top-right` | top + right | right + top |
| `align-left` | center + left | left + center |
| `align-center` | center + center | center + center |
| `align-right` | center + right | right + center |
| `align-bot-left` | bottom + left | left + bottom |
| `align-bot-center` | bottom + center | center + bottom |
| `align-bot-right` | bottom + right | right + bottom |

Add classes to widget configs; `UiLayoutEngine::setStyle` applies them automatically:

```cpp
shellConfig.classes = {UiAppClass::shell, UiAppFlexClass::g10};
rowConfig.classes = {UiAppFlexClass::row, UiAppFlexClass::p10, UiAppFlexClass::g10};
```

Legacy `UIElement` panels use the same numeric scale via `UiAppFlex::p10`, `UiAppFlex::g10`, etc.

Corner radius is `0` app-wide (square chrome).

### Control row geometry

- **Body text:** `UiAppTypography::bodySize` (16 px) for labels, readouts, controls, badges, and descriptions. **Titles:** `UiAppTypography::titleSize` (28 px) only for page/section titles.
- **Row height:** derive from line height + padding (`UiAppLayout::controlRowHeight` = body line + `p10` × 2). Do not hardcode section, field, or status panel heights in SCSS.
- **Icons:** match the adjacent text size (`$font-body` or `$font-title` in SCSS; `UiAppTypography::bodySize` / `titleSize` in C++).
- **Labels in labeled rows must not steal hit tests.** Use `setHitTestInvisibleRecursive()` from `UiAppHelpers.hpp`.

### Scroll areas

- Prefer CSS overflow on a normal `div`: `overflow-y: auto` / `overflow: hidden` in authored SCSS. `UiHost` wires wheel input, clipping (scissor), and scrollbars automatically. Flex children with overflow shrink to their viewport; content inside keeps its intrinsic height and drives `scrollExtent`.
- Flex items default to **content-aware minimum sizes** (headers and labels stay readable). Scroll flex children need **`min-height: 0`** (SCSS) or **`mh0`** so they can shrink inside a column; use **`mw0`** / **`min-width: 0`** for horizontal shrink in rows.
- Keep status chrome or primary actions outside the scroller when possible.
- Use `<scroll-view>` only when you need widget-specific APIs (virtual lists, scroll anchors). Legacy manual binding via `updateScrollMetrics()` + `applyScrollShift()` remains available for those paths.

## Controls

### Toggles vs checkboxes

- **`toggle`** — switch (pill track + sliding thumb). Use for on/off settings.
- **`checkbox`** — square box with check mark. Use for selection/consent.

### Sliders

- Sliders must show **track, filled range, and thumb** in the theme painter.
- Store range metadata: `semantics.label` as `"min:max"`, `semantics.value` as current value string.

### Dropdowns (`select` / `combo`)

- Use `select` for pick-one menus; wire `onChange` and reflect the value in app state.
- Dropdown menus must be laid out when open (`syncControlLayouts`).
- Open menus reparent to the document root to avoid scroll clipping.

### Number fields (`numberField` / `<number-input>`)

- **Default display mode is Blender-style:** centered value, optional min/max progress fill, hover step chevrons, drag-to-scrub, click to switch to left-aligned typing.
- **Scrub on the window thread:** `UiWidgetFactory::driveNumberScrubFrame` from `UiHost::update` using `Window::mouseDelta`; optional cursor hide via `Window::requestUiCursorCapture` / `applyUiCursorRequests` on the poll-events thread. Do not route scrub through `PointerMove`/`Drag` or `UiHost` cursor warp.
- **Markup:** `min`/`max` (bound), `integer`, `unit`, `progress="false"`.
- **Typed commit clamps only** (min/max); `step` is for arrows/scrub finalize, not free typing (avoids `1`→`0` when step is 25).
- **Empty editable fields** must keep a one-line **intrinsic text measure** in `UiHost::synchronizeTextMeasurers` — do not fake height with control `min-height` (that makes buttons/inputs too tall).

### Focus and keyboard

- **Every focusable control must have a visible focused state** (`node.state.focused`).
- Assign sequential `tabIndex` values across a form.

## Visual Design

- **Minimal grayscale palette** — no whimsical gradients or accent colors in app chrome.
- Light mode is the default; dark mode uses the same token names with inverted surfaces.
- **Section surfaces stack highlights** — do not hand-pick nested background colors. Use `panel` / `section` theme classes; `UiAppTheme` composites one transparent overlay per layer on the parent fill (white highlight in dark mode, black darkening in light mode). Controls, list rows, and badges add further layers on their parent surface for hover/active.
- **Borders are paint-only and inset** — drawn inside the element bounds via `UiDisplayList::addInsetBorder`; they never expand layout or bleed into neighboring flex items.
- **Focus ring:** outward offset (~2 px), semi-transparent accent from theme.
- **Hover/active:** controls shift border/fill using theme hover/active surfaces.

## Samples and Demos

- **Authored reference:** `Data/UI/controls-demo.ui.html` + global SCSS under `Data/UI/`; launcher wiring in `Project ORKA/GameLoader.cpp`.
- **C++ sample host:** `UI/Samples/UiControlsDemo.cpp`.
- Reference theme: `UI/Theme/UiAppTheme.hpp` + `UI/Theme/UiAppTheme.cpp`.
- A UI demo must exercise: theme tokens (colors, spacing, typography, layout), toggles, checkboxes, sliders, dropdowns, text fields, light/dark toggle, scroll, focus rings, and live status output.

## Validation

- **`--modern-ui-test` must exit on its own.** A headless `Project ORKA.exe` still running (often at high CPU) after that command usually means an infinite parser/layout loop — start with `malformed-deterministic-corpus` and `UiMarkupParser::parseText` progress guarantees.

## Checklist Before Submitting UI Changes

1. Uses `UiAppTheme` — no hardcoded colors or spacing in surface code.
2. Padding ≥ corner radius on every rounded element.
3. Focus visible on all focusable controls.
4. Sliders and switches render meaningful chrome (not empty boxes).
5. Labels in rows do not block hit testing.
6. Widget factory has layout engine attached when using selects/toggles/checkboxes.
7. Tab order assigned; callbacks update visible state/readouts.
8. New tokens/components added to UI Controls Demo if they are reusable.
9. Build passes: `powershell -ExecutionPolicy Bypass -File .\scripts\build-orka.ps1`
