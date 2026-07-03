# Project ORKA - Easy Wins TODO

High-impact, low-risk improvements that can noticeably improve player/dev experience quickly.

## Done In This Pass

- [x] Removed `UIButton` click/hover console spam (`Project ORKA/UIButton.cpp`).
- [x] Removed duplicate file-drop insertion (`Project ORKA/Window.cpp`).
- [x] Made simulation stop order thread-safe (`Project ORKA/Game.cpp`).
- [x] Added intro skip on first key/mouse press (`Project ORKA/ORKAIntroSequence.cpp`).
- [x] Added visual hover/active feedback for game launcher buttons (`Project ORKA/GameLoader.cpp`).
- [x] Stopped forcing debug mode in all builds and split debug/release flags (`Project ORKA/Settings.hpp`).
- [x] Made VSync configurable and enabled by default (`Project ORKA/Settings.hpp`, `Project ORKA/Window.cpp`).
- [x] Removed unconditional error/debug breaks in non-debug runs (`Project ORKA/Debug.hpp`, `Project ORKA/Settings.hpp`).
- [x] Disabled unconditional `glFinish()` in non-debug runs (`Project ORKA/Renderer.cpp`).

## Priority 1 - Next Easy Wins

- [ ] Fix shader rebuild vector growth/leak on F7 (`Project ORKA/ShaderSystem.cpp`).
  - Clear/realign shader program storage on rebuild to avoid stale entries.
- [ ] Replace string-concatenated Windows path for window icon with `Path` joins (`Project ORKA/Window.cpp`).
  - Improves portability and robustness.
- [ ] Add top-level `try/catch` around startup flow (`Project ORKA/main.cpp`).
  - Show clear error message when window/GL init fails.
- [ ] Update `Project ORKA/Features.md` startup description.
  - Keep docs aligned with intro -> game loader flow.

## Priority 2 - Medium Effort, High Impact

- [ ] Add keyboard navigation to `GameLoader` (Up/Down + Enter + Esc).
- [ ] Add optional "Skip Intro" setting persisted in a config file.
- [ ] Introduce startup modes (Fast/Full):
  - Fast = no heavy asset preload for quick game iteration.
  - Full = current behavior for full engine readiness.
- [ ] Add basic telemetry overlay toggle:
  - frame time, draw calls, active chunks, and sim update time.

## Priority 3 - Quality Of Life And Polish

- [ ] Move critical errors to file logger in addition to console (`Logger` + `Debug` integration).
- [ ] Add first-run defaults per monitor size (avoid 1920x1080 overflow on smaller screens).
- [ ] Add per-game metadata tags and filtering in GameLoader (Playground, Prototype, Game, Tool).
- [ ] Promote stable prototypes to GameLoader automatically via registration pattern.

## What ORKA Needs To Be Great

- Strong default UX:
  - fast startup, skip intro, responsive launcher, keyboard-first flow.
- Predictable runtime behavior:
  - debug-only diagnostics in debug builds, smooth release defaults.
- Clear content discoverability:
  - all major demos/tools launchable from one polished loader.
- Stable iteration loop:
  - one-key shader reload, no leaks, no accidental debugger breaks.
- Better production-like release profile:
  - fewer forced sync points, reduced console noise, safer shutdown paths.

## Subjective Current Weaknesses

- Too much "developer-debug behavior" leaks into normal usage.
- Discoverability is fragmented (many cool projects are hidden).
- Startup path still feels like a tech demo flow more than a cohesive product flow.
- Some subsystems are mature, but glue UX between them is inconsistent.
