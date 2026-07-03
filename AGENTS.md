# AGENTS.md - Project ORKA Agent Guide

This file gives AI coding agents the minimum context needed to work safely and effectively in this repository.

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
- `Project ORKA/UserInterface.hpp` + `Project ORKA/UIElement.hpp` - UI tree model.
- `Project ORKA/Renderer.hpp` + `Project ORKA/Renderer.cpp` - render facade and systems.
- `Project ORKA/Settings.hpp` - project-wide constants and debug toggles.

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
- Follow existing naming and formatting in touched files; avoid style-only churn.
- Keep comments practical and short; explain intent where logic is non-obvious.
- Preserve include ordering and local patterns used by surrounding code.

## Rendering and Shader Work

- Shaders live under `Project ORKA/Data/Shaders/`.
- If you add uniforms/attributes, update both shader and the matching CPU-side binding path.
- Avoid silent render-state leaks; use existing override/state management helpers where available.
- For debug-heavy rendering changes, prefer toggles and existing debug pathways over permanent noisy logging.

## Threading and Performance

- Simulation and window/rendering can run on different threads; avoid sharing mutable data without clear ownership/synchronization.
- Keep per-frame allocations low in hot paths.
- Prefer incremental, measurable changes in performance-sensitive systems (renderer, terrain, voxels, planet systems).

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

## Validation Guidance

- Prefer lightweight checks (targeted compile/test command if explicitly requested).
- If build/test execution is unavailable, provide clear manual verification steps tied to changed code paths.

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
- and easy for maintainers to review quickly.
