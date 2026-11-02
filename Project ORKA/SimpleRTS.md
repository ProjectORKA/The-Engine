# Simple RTS

Agent notes for the **Simple RTS** demo/simulation. Implementation: `SimpleRTS.cpp`, `SimpleRTS.hpp`, and `SimpleRts*.cpp` under `Project ORKA/`. Repo-wide agent behavior stays in root `AGENTS.md`.

## Start here

- `SimpleRTS.cpp` / `SimpleRTS.hpp` — `SimpleRtsAppShell` (single `UiHost` root) + simulation
- `Data/UI/simple-rts.ui.html` — world setup (`@if state.setupVisible`) + `<rts-world>` game surface
- `SimpleRtsWorldSettings` + `SimpleRtsWorldPresets` — world configuration

## World presets

- Built-in presets: `Data/SimpleRts/presets/` (seeded on first run).
- User saves: cache `simple-rts/presets/`.

## Simulation and setup UI

- Simulation starts only after **Start / rebuild world**.
- **F2** toggles setup (`state.setupVisible`); when hidden, only `<rts-world>` is in the document.
- Game camera capture (LMB) routes through the app shell when setup is closed.
