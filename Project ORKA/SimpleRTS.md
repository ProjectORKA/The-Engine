# Simple RTS

Agent notes for the **Simple RTS** demo/simulation. Implementation: `SimpleRTS.cpp`, `SimpleRTS.hpp`, and `SimpleRts*.cpp` under `Project ORKA/`. Repo-wide agent behavior stays in root `AGENTS.md`.

## Start here

- `SimpleRTS.cpp` / `SimpleRTS.hpp` — game lifecycle
- `SimpleRtsWorldSettings` + `SimpleRtsWorldPresets` — world configuration
- `SimpleRtsSettingsPanel` — in-game settings UI
- `Data/UI/simple-rts-world.ui.html` — world setup overlay

## World presets

- Built-in presets: `Data/SimpleRts/presets/` (seeded on first run).
- User saves: cache `simple-rts/presets/`.

## Simulation and setup UI

- Simulation starts only after **Start / rebuild world**.
- **F2** reopens the setup overlay.
- When the setup overlay is **hidden**, it must **not** participate in legacy hit-testing (`isPointInside`) or focus must return to the game renderer. Otherwise its `UiHost` consumes LMB, scroll, and blocks mouse capture.
