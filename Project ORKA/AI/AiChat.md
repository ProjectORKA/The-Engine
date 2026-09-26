# AI Chat

Agent notes for the **AI Chat** game and engine-managed local LLM stack. Repo-wide agent behavior stays in root `AGENTS.md`.

## Start here

- `AI/AiChat.cpp` — game entry and UI wiring
- `Data/UI/ai-chat.ui.html` — authored shell
- `AI/AiConfig.hpp` — llama.cpp release pins and runtime settings
- `AI/AiGpuDetect.cpp` — GPU detection
- `AI/AiRuntimeResolve.cpp` — CUDA zips / `cudart` DLL resolution

## Runtime

- Chat uses **llama-server** managed by the engine (not a manual user install).
- If runtime download fails with **404**, update `llamaCppReleaseTag` in `AiConfig.hpp` or rely on the GitHub API fallback path in runtime resolve.

## Scope

- NPCs, tools, and voice builds on this baseline; keep shared runtime/config changes compatible with AI Chat unless the task says otherwise.
