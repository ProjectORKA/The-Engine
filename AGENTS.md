# AGENTS.md - Project ORKA Agent Guide

This file gives AI coding agents the minimum context needed to work safely and effectively in this repository.

**After every completed task, re-evaluate this file** (see [Continuous Improvement](#continuous-improvement--re-evaluate-agentsmd-after-every-task)) and update it when the session exposed a gap that future agents should not repeat.

## Project Identity

- Project ORKA is a C++ game engine + playground with many demos/tools.
- Rendering stack: OpenGL + GLFW + GLEW + GLM.
- Important convention: **left-handed coordinates** (`+X right`, `+Y forward`, `+Z up`).
- Entry flow lives in `Project ORKA/main.cpp`.

## Start Here

When an edit request is broad, inspect these files first:

- `Project ORKA/main.cpp` - startup flow and app launch behavior.
- `Project ORKA/Engine.hpp` + `Project ORKA/Engine.cpp` - engine lifetime and subsystems.
- `Project ORKA/Game.hpp` - simulation/renderer split contract.
- `Project ORKA/UserInterface.hpp` + `Project ORKA/UIElement.hpp` - legacy UI tree model.
- `Project ORKA/UI/DESIGN_RULES.md` - **modern retained UI design rules (read before UI work).**
- `Project ORKA/UI/Theme/UiAppTheme.hpp` - **master app theme (colors, typography, layout, light/dark).**
- `Project ORKA/UI/Theme/UiAppFlex.hpp` - **flex spacing classes (p5–p80, g5–g80, col/row).**
- `Project ORKA/UI/UiHost.hpp` + `Project ORKA/UI/Widgets/UiWidgets.hpp` - modern UI host and controls.
- `Project ORKA/UI/Authoring/UiAuthoredHost.hpp` - load `.ui.html` + `.ui.scss` bundles onto a host.
- `Project ORKA/Data/UI/app-master.ui.scss` + `controls-demo.ui.html` - global authored styles + demo markup.
- `Project ORKA/Renderer.hpp` + `Project ORKA/Renderer.cpp` - render facade and systems.
- `Project ORKA/Settings.hpp` - project-wide constants and debug toggles.
- `Project ORKA/AI/` + **AI Chat** game (`AI/AiChat.cpp`, `Data/UI/ai-chat.ui.html`) — engine-managed local LLM via llama-server; pins in `AI/AiConfig.hpp`. GPU: `AiGpuDetect.cpp` + CUDA zips/cudart DLLs via `AiRuntimeResolve.cpp`. If runtime download fails with 404, update `llamaCppReleaseTag` or rely on GitHub API fallback.

## Architecture Guardrails

- Keep the **GameSimulation** and **GameRenderer** separation intact.
  - Simulation logic belongs in `update()` on background simulation side.
  - Rendering and GPU operations belong in renderer/UI side.
- Prefer adding new behavior by extending existing systems instead of inventing parallel frameworks.
- Most visuals are routed through the `UIElement` tree. Keep new UI/render surfaces compatible with that model.
- Resource lookups are usually name-based through `ResourceManager`; avoid hardcoded absolute paths.

## Coding Conventions (Observed in Repo)

- Use ORKA base aliases and project types (`Float`, `Int`, `Bool`, etc.) when used by neighboring code.
- Keep headers lightweight and place heavier implementation logic in `.cpp` files.
- Prefer `OrkaTypes.hpp` over `Basics.hpp` when a header only needs aliases/`Vec*`/`String`; include `Renderer.hpp` in `.cpp` files that use `window.renderer` (`Window.hpp` forward-declares `Renderer`).
- Follow existing naming and formatting in touched files; avoid style-only churn.
- Keep comments practical and short; explain intent where logic is non-obvious.
- Preserve include ordering and local patterns used by surrounding code.

### Readable, familiar code

Write code that a developer from JavaScript, Python, or C# can follow without decoding C++ idioms first.

- **Keep it simple and linear** — straight-line logic, early returns, and small named helpers beat dense one-liners or clever abstractions.
- **Avoid convoluted C++ when a plain alternative exists** — skip deep namespace nesting, heavy template metaprogramming, and obscure language tricks unless the surrounding file already relies on them.
- **Prefer cross-language conventions** — use familiar names and call shapes over std-library ceremony. Example: `print()`, `println()`, `logDebug()`, `logWarning()`, and `logError()` from `Debug.hpp` instead of `std::cout <<` / `std::cerr`.
- **Keep scope obvious** — qualify types where it helps readability; do not blanket `using namespace` in headers.
- **Match nearby code** — apply this to new and touched code; do not rewrite unrelated files to match.

## Modern UI Design Rules

When editing files under `Project ORKA/UI/`, UI samples, game launcher UI, or theme/painter code, follow `Project ORKA/UI/DESIGN_RULES.md`.

**Single source of truth:** `UiAppTheme.hpp` (colors/typography) + `UiAppFlex.hpp` (flex spacing). Install with `host.useAppTheme(theme)` and semantic classes from `UiAppClass` / `UiAppFlexClass`.

Hard requirements agents must not skip:

- **Flex + class spacing only** — flex containers default to **column**; add `row` only for horizontal stacks. Use `p5`–`p80` and `g5`–`g80` on widget classes, and `align-top-left` … `align-bot-right` for semantic child alignment (see `UiAppFlex.hpp`). No arbitrary padding, gap, or margin in surface code (legacy panels use `UiAppFlex::p10` etc.).
- **Use `UiAppTheme` tokens** — no ad-hoc color literals or font sizes. Legacy `UIElement` panels use `UiLegacyTheme`.
- **Theme paints, widgets semantic** — visuals only in `UiAppTheme.cpp`; surfaces use `UiAppClass` style classes.
- **Light + dark mode** — both modes must look correct; add a theme toggle on new app shells when practical.
- **Visible focus** on all focusable controls.
- **Use `UiWidgetFactory` controls** (toggle, checkbox, slider, select, textField) instead of mimicking them with boxes.
- **Attach layout engine + `syncControlLayouts()`** when using composite controls (toggles, checkboxes, selects).
- **Showcase new reusable tokens/components** in `Data/UI/controls-demo.ui.html` (markup) + global SCSS under `Data/UI/`.
- **Prefer authored UI** for demo/layout surfaces; keep C++ to property/command wiring via `UiAuthoredHost`.
- **CSS overflow on `div`** (`overflow`, `overflow-x`, `overflow-y`: visible/hidden/scroll/auto) is handled automatically by `UiHost` + `UiOverflowController` — clipping via scissor, wheel + scrollbar input, no manual `updateScrollMetrics()` / `applyScrollShift()`. Reserve `<scroll-view>` for virtual lists or scroll-anchor bindings.
- **No unused markup ids** — add `id="…"` only when C++ (`ensureNodeLayout`, `findById`, …), bindings (`[id]="…"` for token/theme lookup), or tests need it. Prefer semantic classes for styling; do not duplicate class names as ids.
- **No dead markup classes or UI clutter** — every class must be handled by SCSS, `UiAppTheme`, or C++ wiring. Do not add decorative aliases, one-off layout workaround classes, or duplicate helpers already on the same node; prefer fixing layout (e.g. content-aware flex mins, `min-h-0` on scroll children) or existing flex/theme tokens over new names. Nested surfaces use `panel` / `section` theme classes (highlight stacking in `UiAppTheme.cpp`); do not hand-pick nested background colors.
- **Wrap rows use `b50`–`b350`** — basis classes on items + `row wrap` on the container (balanced line breaks for grow rows); color swatches use `.color-token-grid`. Prefer short flex utilities (`fg1`, `wrap`, `mw0`, `mh0`). See `UiAppFlex.hpp` and `.token-item b100` in the controls demo.

Reference implementation: `Data/UI/controls-demo.ui.html` + `Project ORKA/GameLoader.cpp`.

## Rendering and Shader Work

- Shaders live under `Project ORKA/Data/Shaders/`.
- If you add uniforms/attributes, update both shader and the matching CPU-side binding path.
- Avoid silent render-state leaks; use existing override/state management helpers where available.
- For debug-heavy rendering changes, prefer toggles and existing debug pathways over permanent noisy logging.

## Threading and Performance

- Simulation and window/rendering can run on different threads; avoid sharing mutable data without clear ownership/synchronization.
- Keep per-frame allocations low in hot paths.
- Prefer incremental, measurable changes in performance-sensitive systems (renderer, terrain, voxels, planet systems).
- Debug|x64 and Release|x64 use PCH (`OrkaPch.hpp`). Post-build asset sync is `scripts/sync-orka-output.ps1` (incremental copy, wired in the vcxproj).
- If compile times are still a daily problem after header discipline, the next structural step is a **static library split** (`ORKA.Core`, `ORKA.Render`, `ORKA.UI`, optional `ORKA.Games`) — do not start this unless the user asks or measurement shows it is needed.

## Adding a New Game or Prototype

1. Add implementation files in `Project ORKA/` following existing game/prototype structure.
2. Hook startup/selection:
   - Add to `GameLoader` when it should be user-selectable.
   - Keep descriptions concise and clear.
3. Ensure lifecycle correctness:
   - `create()` initializes resources.
   - `update()` runs logic.
   - `renderScene()` draws.
   - `destroy()` releases resources cleanly.
4. Reuse shared systems (`Renderer`, `ResourceManager`, `Input`, `UIElement`) before adding new global state.

## Safe Editing Rules for Agents

- Make focused edits only for the requested task.
- Do not refactor unrelated files "while here."
- Do not introduce new third-party dependencies unless explicitly requested.
- Do not run destructive git commands.
- If project appears dirty, avoid reverting unrelated local changes.

## Continuous Improvement — Re-evaluate AGENTS.md After Every Task

**Required:** At the end of every completed task or prompt, re-read this file and decide whether it needs an update so future agents avoid the same mistakes.

### When to update

Add or refine guidance when the session revealed:

- A mistake an agent made that clearer rules would have prevented (wrong file read first, wrong build command, ignored architecture split, skipped UI design rules, etc.).
- Missing or misleading context that caused rework, bad assumptions, or user correction.
- A recurring pitfall in this repo (build toolchain quirks, threading traps, naming/path conventions, lifecycle hooks agents keep missing).
- A gap between what agents tried and what the codebase actually expects.

### When not to update

- One-off bugs, typos, or user-specific preferences unrelated to repo-wide agent behavior.
- Details that belong in a specialized doc (`Project ORKA/UI/DESIGN_RULES.md`, subsystem READMEs, etc.) — link to those instead of copying them here.
- Overly narrow rules tied to a single file or line unless the pattern is likely to repeat.

### How to update

- Keep additions **short, actionable, and generalizable** — state the rule and where to look, not a full postmortem.
- Prefer extending an existing section over adding a new one unless the topic is genuinely new.
- If the fix is obvious and repo-wide, update `AGENTS.md` in the same session before finishing.
- If the lesson is unclear or might be controversial, propose the change to the user instead of silently editing.
- Do not bloat this file: remove or merge stale rules when adding new ones.

### Quick checklist (run before you sign off)

1. Did anything in this task fail, get corrected, or need extra explanation?
2. Would a future agent hit the same issue with only the current `AGENTS.md`?
3. If yes, add or tighten one rule (or link to the right doc). If no, leave the file unchanged.

## Session Cleanup (Required Before Sign-Off)

Treat cleanup as part of the task, not optional follow-up.

1. **Clear agent todos** — mark completed or cancel stale items; do not leave in-progress todos from finished work.
2. **Do not add markdown clutter** — no task plans, status logs, session summaries, or “DONE” checklists in new `.md` files unless the user explicitly asked for documentation.
3. **Delete temporary docs** created during the task (plans, benchmarks, migration notes) once the code is merged-quality. Keep only durable docs the repo already uses (`AGENTS.md`, `UI/DESIGN_RULES.md`, subsystem READMEs when genuinely needed).
4. **Prefer code over docs** for how things work — short comments at non-obvious call sites beat parallel documentation.
5. **Fold lasting rules into `AGENTS.md`** when repo-wide (one short rule, not a postmortem). Remove or merge stale `AGENTS.md` text when adding new guidance.
6. **Leave the tree reviewable** — no debug scaffolding, commented-out experiments, or duplicate helpers introduced “for later.”

## Validation Guidance

- Prefer lightweight checks (targeted compile/test command if explicitly requested).
- If build/test execution is unavailable, provide clear manual verification steps tied to changed code paths.
- `--modern-ui-test` must exit on its own. A headless `Project ORKA.exe` still running (often at high CPU) after that command usually means an infinite parser/layout loop — start with `malformed-deterministic-corpus` and `UiMarkupParser::parseText` progress guarantees.

## Build Verification (Agent Checklist)

Use this when the user asks for "make sure it compiles" or "run a build yourself."

1. From repo root, run:
   - `powershell -ExecutionPolicy Bypass -File .\scripts\build-orka.ps1`
2. Default build target is `Debug|x64` for `Project ORKA.sln`.
3. Optional flags:
   - `-Configuration Release`
   - `-Platform x64` or `-Platform Win32`
   - `-Clean` (runs `Clean;Build`)
4. If the build fails with missing C++ targets (`Microsoft.Cpp.*.props/.targets`), use a proper Visual Studio MSBuild path (not legacy .NET Framework MSBuild), then rerun the script.
5. In your final response, always report:
   - exact command run,
   - whether build succeeded/failed,
   - first relevant compiler/toolchain error if failed.

### Notes for Reliable Agent Runs

- Preferred entrypoint is `scripts/build-orka.ps1` because it auto-detects a modern Visual Studio MSBuild path.
- Avoid using `C:\Windows\Microsoft.NET\Framework64\...\MSBuild.exe` for this solution unless explicitly required for diagnostics; it may fail to locate VC++ build props/targets.
- Keep validation scoped: build only the solution/config requested by the user, and do not run unrelated tooling.

## Good Agent Output

A good change in this repo is:

- minimal but complete,
- aligned with engine architecture,
- thread/render safe,
- easy for maintainers to review quickly,
- and readable without C++ expertise — plain functions, familiar names, no unnecessary cleverness.
