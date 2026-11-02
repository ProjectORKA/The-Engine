# AGENTS.md - Project ORKA Agent Guide

This file gives AI coding agents the minimum context needed to work safely and effectively in this repository.

**After every completed task, re-evaluate this file** (see [Continuous Improvement](#continuous-improvement--re-evaluate-agentsmd-after-every-task)) and update it when the session exposed a gap that future agents should not repeat.

**Scope:** Root `AGENTS.md` is for **repo-wide** engine, build, and agent behavior only. **Game- and feature-specific** rules belong in that project’s `<Project>.md` (see [Per-project docs](#per-project-docs)), not here.

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
- `Project ORKA/UI/DESIGN_RULES.md` - **modern UI rules, start-here paths, theme, layout, controls, validation (read before any UI work).**
- `Project ORKA/UI/UI_REDESIGN_PLAN.md` - **target UI architecture (app shell, components, surfaces, multi-window); read before game-shell or large UI refactors.**
- `Project ORKA/Renderer.hpp` + `Project ORKA/Renderer.cpp` - render facade and systems.
- `Project ORKA/Settings.hpp` - project-wide constants and debug toggles.
- Repo root `ISSUES.md` + `FEATURES.md` - agent-maintained backlog (bugs/debt vs. future ideas); append while working (see [Issues and feature backlog](#issues-and-feature-backlog)).
- Per-game guides when touching a demo — e.g. `Project ORKA/AI/AiChat.md`, `Project ORKA/SimpleRTS.md`, `Project ORKA/Pong.md`.

## Per-project docs

Use a **dedicated markdown file named after the project** for rules that apply to one game, prototype, or vertical feature—not to the whole engine. Do **not** call these `AGENTS.md` (that name is reserved for the repo root guide).

- **Naming:** `<Project>.md` using the game’s usual code or launcher name (e.g. `AiChat.md`, `SimpleRTS.md`).
- **Location:** Next to the code — in the feature folder if one exists (`Project ORKA/AI/AiChat.md`), otherwise beside the main game sources (`Project ORKA/SimpleRTS.md` next to `SimpleRTS.cpp`).
- **Content:** Start-here paths, lifecycle quirks, preset/data locations, input/UI traps, feature-specific build/runtime notes.
- **Root `AGENTS.md`:** Link from Start Here only—**do not** copy long game-specific checklists here.
- **Subsystems** with existing guides (e.g. `Project ORKA/UI/DESIGN_RULES.md`) stay there; link from root `AGENTS.md` instead of duplicating.

When adding a game with non-obvious agent pitfalls, create or extend its `<Project>.md` in the same change (or the session that discovered the pitfall).

## Architecture Guardrails

- Keep the **GameSimulation** and **GameRenderer** separation intact.
  - Simulation logic belongs in `update()` on background simulation side.
  - Rendering and GPU operations belong in renderer/UI side.
- Prefer adding new behavior by extending existing systems instead of inventing parallel frameworks.
- Most visuals are routed through the `UIElement` tree. Keep new UI/render surfaces compatible with that model.
- Resource lookups are usually name-based through `ResourceManager`; avoid hardcoded absolute paths.

## Discover, Reuse, and Keep Call Sites Current

Agents must **inspect thoroughly before writing**. Skipping discovery or stopping at the first match produces duplicate implementations and half-migrated legacy paths.

**Before adding code:**

1. **Search the repo** for existing helpers, subsystems, and patterns (names, folders, and related concepts—not only the exact symbol the task mentions).
2. **Prefer extend or modify** an existing system over a parallel copy; if legacy and modern paths both exist (e.g. `UIElement` vs `Project ORKA/UI/`), follow the doc-linked **current** path and do not add new code on the obsolete one unless the task explicitly requires it.
3. **Read call sites and siblings** — games in `GameLoader`, demos, and feature folders often mirror engine APIs; understand how neighbors use the API you are changing.

**When changing APIs, behavior, or shared types:**

- **Update every use** in the same change when feasible: definitions, includes, `GameLoader` entries, per-game shells, presets, and tests/validation hooks that depend on the contract.
- **Engine changes often ripple into many games/projects** — after an engine edit, grep for old names/signatures and fix or migrate consumers; do not leave games on stale patterns while the engine moved on.
- **Remove or redirect duplication** you touch: one canonical implementation beats two drift-prone copies; if you consolidate, delete or clearly deprecate the dead path in the same session or log debt in `ISSUES.md` with a concrete follow-up.
- **Leave the tree consistent** — no new “temporary” duplicate helpers, outdated comments that contradict code, or half-updated feature sets.

**Thoroughness bar:** If you have not searched for alternatives and listed (mentally or in notes) the main files/games affected, you are not done planning the edit.

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

## Modern UI

When editing `Project ORKA/UI/`, `Project ORKA/Data/UI/`, UI samples, game launcher UI, or theme/painter code, follow `Project ORKA/UI/DESIGN_RULES.md` (start-here paths, agent requirements, theme, layout, controls, validation). For large app-shell refactors, also read `Project ORKA/UI/UI_REDESIGN_PLAN.md`.

Pointer hits on a `UiHost` are handled by `UiEventRouter`. Legacy `UIElement::inputEvent` still runs for `ui.capturing` and for keys the retained document does not handle.

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
5. If the game has setup quirks, preset paths, or input/UI traps agents keep missing, document them in that game’s `<Project>.md` (not root `AGENTS.md`).

## Safe Editing Rules for Agents

- Make focused edits only for the requested task—but **within that scope**, still update all affected call sites and avoid leaving parallel legacy implementations (see [Discover, Reuse, and Keep Call Sites Current](#discover-reuse-and-keep-call-sites-current)).
- Do not refactor unrelated files "while here."
- Do not introduce new third-party dependencies unless explicitly requested.
- Do not paste third-party code or assets (Shadertoy shaders, snippets, textures, fonts) without checking the license first. Shadertoy defaults to CC BY-NC-SA 3.0 and some authors forbid reuse entirely. Credit the source in the file and add it to `THIRD_PARTY.md`; the project license is in `LICENSE`.
- Do not run destructive git commands.
- If project appears dirty, avoid reverting unrelated local changes.
- Never write build output, logs, or scratch files to the repo root (one-off `cl.exe` / launch redirects included); use your temp dir or `x64/`.
- `Project ORKA/Cache/` is engine-generated (AI runtimes, configs, user saves) and gitignored: never commit it. Defaults that must ship go in `Data/` and get seeded into Cache at runtime.

## Repository critique and improvement ideas

Maintainers want agents to **think critically about the engine and repo**, not only execute the immediate task. When you have enough context from reading code, say what you think—constructively and specifically.

**Speak up when you notice:**

- Systems that feel rudimentary, outdated, or superseded by a newer path (e.g. legacy vs modern UI).
- Unnecessary complexity, duplication, or architecture that fights the stated direction in docs like `UI_REDESIGN_PLAN.md`.
- Oversized or unfocused files, weak naming, folder layout that hides ownership, or patterns that will hurt the next agent.
- Better approaches you would recommend (with tradeoffs), even if they are out of scope for the current change.

**How to deliver it:**

- **In conversation** — brief, evidence-based notes tied to paths or subsystems; useful when finishing a task or when the user asks for a read on the repo.
- **In the backlog** — actionable one-liners in `ISSUES.md` (debt, hazards, confusion) or `FEATURES.md` (refactors, structure, polish); same rules as [Issues and feature backlog](#issues-and-feature-backlog). Game- or feature-local observations can go in that project’s `<Project>.md` when a rule would help future agents there.
- **Do not expand the diff** — critique does not license drive-by refactors; log or discuss unless the user asks you to fix it now.

Honest assessment is part of good agent output here; silence on obvious structural pain is not neutrality—it wastes maintainer time.

## Issues and feature backlog

While implementing a task, capture discoveries that are **out of scope** for the current change instead of silently forgetting them or expanding the diff.

- **`ISSUES.md`** (repo root) — problems: bugs, broken behavior, regressions, tech debt, confusing or fragile code, missing tests, threading/render hazards you noticed but did not fix.
- **`FEATURES.md`** (repo root) — future improvements: enhancements, UX polish, refactors worth doing later, tooling or workflow ideas.

Rules:

- Append **short, actionable** bullets (area/file, what is wrong or desired, why it matters). Do not write essays or session logs.
- **Do not duplicate** — skim the file first; merge or extend an existing item when it is the same issue.
- **Do not block the task** — log and continue unless the user asked you to fix it now.
- Create either file with a one-line title if it does not exist yet.
- **Repo-wide agent behavior** still belongs in root `AGENTS.md`; **game/feature behavior** belongs in that project’s `<Project>.md`; one-off findings belong in `ISSUES.md` / `FEATURES.md`.

## Continuous Improvement — Re-evaluate AGENTS.md After Every Task

**Required:** At the end of every completed task or prompt, re-read this file and decide whether it needs an update so future agents avoid the same mistakes.

### When to update

Add or refine guidance when the session revealed:

- A mistake an agent made that clearer rules would have prevented (wrong file read first, wrong build command, ignored architecture split, skipped UI design rules, duplicate system instead of reuse, engine change without updating games/call sites, etc.).
- Missing or misleading context that caused rework, bad assumptions, or user correction.
- A recurring pitfall in this repo (build toolchain quirks, threading traps, naming/path conventions, lifecycle hooks agents keep missing)—**engine-wide** only.
- A gap between what agents tried and what the codebase actually expects.

### When not to update

- One-off bugs, typos, or user-specific preferences unrelated to repo-wide agent behavior (log bugs/debt in `ISSUES.md`, ideas in `FEATURES.md`).
- **Game- or feature-specific** rules (single demo, one preset flow, one game’s UI)—put them in that project’s `<Project>.md` (or subsystem doc), and add a one-line link in root Start Here if agents need to find it.
- Details that belong in a specialized doc (`Project ORKA/UI/DESIGN_RULES.md`, per-game `*.md`, subsystem READMEs, etc.) — link to those instead of copying them here.
- Overly narrow rules tied to a single file or line unless the pattern is likely to repeat **across the engine**.

### How to update

- Keep additions **short, actionable, and generalizable** — state the rule and where to look, not a full postmortem.
- Prefer extending an existing section over adding a new one unless the topic is genuinely new.
- If the fix is obvious and repo-wide, update `AGENTS.md` in the same session before finishing.
- If the lesson is unclear or might be controversial, propose the change to the user instead of silently editing.
- Do not bloat this file: remove or merge stale rules when adding new ones.

### Quick checklist (run before you sign off)

1. Did anything in this task fail, get corrected, or need extra explanation?
2. Would a future agent hit the same issue on **unrelated** work with only root `AGENTS.md`?
3. If yes, add or tighten one **repo-wide** rule (or a link to the right doc). If the lesson is game/feature-local, update that project’s `<Project>.md` instead. If no, leave root `AGENTS.md` unchanged.

## Session Cleanup (Required Before Sign-Off)

Treat cleanup as part of the task, not optional follow-up.

1. **Clear agent todos** — mark completed or cancel stale items; do not leave in-progress todos from finished work. If the task surfaced out-of-scope bugs or ideas, confirm they are recorded in `ISSUES.md` / `FEATURES.md`.
2. **Do not add markdown clutter** — no task plans, status logs, session summaries, or “DONE” checklists in new `.md` files unless the user explicitly asked for documentation.
   **Current state only:** docs, comments, backlog entries and agent memories describe the repo as it is now and where it is going. No history, changelog notes, "previously / used to / was renamed" remarks, postmortems or resolved items. Delete such text entirely once it stops being true; git history is the only record of the past.
3. **Delete temporary docs** created during the task (plans, benchmarks, migration notes) once the code is merged-quality. Keep only durable docs the repo already uses (`AGENTS.md`, `ISSUES.md`, `FEATURES.md`, `UI/DESIGN_RULES.md`, subsystem READMEs when genuinely needed).
4. **Prefer code over docs** for how things work — short comments at non-obvious call sites beat parallel documentation.
5. **Fold lasting rules into the right doc** — repo-wide → root `AGENTS.md`; game/feature → that project’s `<Project>.md` (one short rule, not a postmortem). Remove or merge stale text when adding new guidance; do not grow root `AGENTS.md` with game catalogs.
6. **Leave the tree reviewable** — no debug scaffolding, commented-out experiments, or duplicate helpers introduced “for later.”

## Validation Guidance

- Prefer lightweight checks (targeted compile/test command if explicitly requested).
- If build/test execution is unavailable, provide clear manual verification steps tied to changed code paths.
- UI headless tests (`--modern-ui-test`): see `Project ORKA/UI/DESIGN_RULES.md` (Validation).
- Agent bridge semantic UI (no pointer): `Project ORKA/Agent/README.md` — `ui.inspect`, `ui.command.invoke`, `ui.property.*`, `ui.node.*` on windows with `UiAuthoredHost`.
- For visual/layout bugs, verify at runtime instead of guessing: launch with `--agent-bridge`, wait ~10 s for the intro sequence (earlier `ui.inspect` returns zero-size nodes), find launcher id `game-<index>` (order in `GameLoader::buildDefaultEntries`) in `ui.inspect`, pass its `index` and `generation` to `ui.node.activate` as `nodeIndex` / `nodeGeneration`, then `screenshot.capture` (saved under `AgentArtifacts/` next to the exe; `agent.waitFrames` needs a `windowId`) + `ui.inspect` (computed x/y/width/height per node).

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

- minimal but complete (no missing call sites or duplicate systems left behind),
- aligned with current engine architecture,
- thread/render safe,
- easy for maintainers to review quickly,
- readable without C++ expertise — plain functions, familiar names, no unnecessary cleverness,
- and, when the work surfaced real problems or better directions, **communicated or logged** per [Repository critique and improvement ideas](#repository-critique-and-improvement-ideas) instead of leaving them unsaid.
