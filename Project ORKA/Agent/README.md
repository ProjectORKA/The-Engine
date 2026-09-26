# Project ORKA Agent Bridge

The agent bridge is an opt-in localhost control surface for automation, inspection, input, screenshots, logs, and profiling. It is disabled unless the engine is launched with agent mode enabled.

## Launch

Launch the engine with:

```text
Project ORKA.exe --agent-bridge --agent-port 42123 --agent-token <random-secret>
```

- `--agent-bridge` enables the bridge.
- `--agent-port <port>` selects the loopback TCP port (default client port: `42123`).
- `--agent-token <token>` sets the shared bearer token required on every request.

If `--agent-token` is omitted, the engine generates a 256-bit token and prints
`ORKA_AGENT_ENDPOINT` and `ORKA_AGENT_TOKEN` to its console.

Use a long, randomly generated token. For the PowerShell client, prefer setting it in the environment so it is not repeated in shell history:

```powershell
$env:ORKA_AGENT_TOKEN = '<random-secret>'
.\scripts\orka-agent.ps1 -Pretty
```

## PowerShell client

`scripts/orka-agent.ps1` is compatible with Windows PowerShell 5.1. It connects only to `127.0.0.1`, sends one request, reads one response, and closes the connection.

Core parameters:

- `-Port` defaults to `42123`.
- `-Token` falls back to `ORKA_AGENT_TOKEN` and is required.
- `-Method` defaults to `agent.health`.
- `-Params` accepts either a PowerShell object/hashtable or a JSON string.
- `-TimeoutMs` defaults to `5000` and is sent to the bridge as well as being used for client I/O timeouts.
- `-Pretty` writes formatted JSON. Without it, the response result is emitted as a PowerShell object.

Convenience operations retain the same common parameters:

- `-ListMethods`
- `-Windows`
- `-Screenshot [-WindowId <id>] [-OutputPath <path>]`
- `-MouseMove -X <x> -Y <y> [-WindowId <id>]`
- `-Click [-X <x> -Y <y>] [-Button left|middle|right|x1|x2] [-ClickCount <n>] [-WindowId <id>]`
- `-KeyPress <key> [-Modifiers Ctrl,Shift,Alt,Super] [-WindowId <id>]`
- `-Text <text> [-WindowId <id>]`
- `-Logs [-AfterId <id>] [-Limit <n>]`
- `-ProfileStart [-ProfileName <name>]`
- `-ProfileStop [-ProfileId <id>]`
- `-ProfileRead [-ProfileId <id>] [-OutputPath <path>]`
- `-WaitFrames <count>`

Only one convenience operation can be selected per invocation. Use `-Method` and `-Params` for every method not covered by a convenience option.

## Wire protocol

Transport is raw TCP on IPv4 loopback. Every message is:

1. A 4-byte unsigned-compatible message length encoded little-endian.
2. Exactly that many bytes of UTF-8 JSON, without a BOM.

There are no delimiters or trailing newlines. Readers must handle partial socket reads for both the header and payload. The supplied client rejects empty frames and frames larger than 64 MiB.

A request has this shape:

```json
{
  "version": 1,
  "id": "caller-generated-request-id",
  "token": "shared-secret",
  "method": "agent.health",
  "params": {},
  "timeoutMs": 5000
}
```

A successful response echoes the request identity:

```json
{
  "version": 1,
  "id": "caller-generated-request-id",
  "ok": true,
  "result": {}
}
```

A failed response contains a structured error:

```json
{
  "version": 1,
  "id": "caller-generated-request-id",
  "ok": false,
  "error": {
    "code": "invalid_params",
    "message": "windowId is required",
    "data": {}
  }
}
```

Callers must treat unknown fields as forward-compatible, verify matching response IDs, and use `agent.capabilities` or `agent.listMethods` instead of assuming that every optional handler is active.

## Method surface

Agent lifecycle and discovery:

- `agent.capabilities` returns protocol limits, feature flags, coordinate conventions, and artifact capabilities.
- `agent.listMethods` returns currently registered methods and, when available, their parameter metadata.
- `agent.health` reports bridge and host health.
- `agent.invoke` invokes a registered action by name with nested action parameters.
- `agent.waitFrames` waits for a requested number of presented/update frames. Params: `frames`.

Window control:

- `window.list` lists known windows and their IDs, titles, dimensions, focus, and visibility state.
- `window.focus` focuses a window. Params: `windowId`.
- `window.resize` requests a logical client-area size. Params: `windowId`, `width`, `height`.
- `window.close` requests an orderly close. Params: `windowId`.

Input:

- `input.pointerMove` moves the pointer. Params: `x`, `y`, optional `windowId`.
- `input.mouseButton` changes a button state. Params: `button`, `down`, optional `windowId`.
- `input.click` clicks at the current or supplied position. Params: `button`, optional `x`, `y`, `clickCount`, `windowId`.
- `input.scroll` injects scrolling. Params: `deltaX`, `deltaY`, optional `windowId`.
- `input.key` injects a key press or state transition. Params include `key`, optional `down`, `repeat`, `modifiers`, `windowId`.
- `input.text` injects UTF-8 text through the text-input path. Params: `text`, optional `windowId`.

Coordinates are logical client coordinates with `(0, 0)` at the top-left, positive X to the right, and positive Y downward. The bridge converts logical coordinates to platform/framebuffer coordinates when required. Window IDs should be obtained from `window.list`; omission means the bridge-selected active/default window only where the method advertises that behavior.

Inspection and artifacts:

- `screenshot.capture` captures a window or framebuffer. Params include optional `windowId` and `path`.
- `ui.inspect` returns the current inspectable UI tree/state, including retained-node semantics, widget kinds, and registered authored commands when a `UiAuthoredHost` is active. Params: optional `windowId`.
- `ui.commands.list` lists authored UI command names and argument types. Params: `windowId`.
- `ui.command.invoke` runs an authored command without pointer input. Params: `windowId`, `name`, optional `arguments` array.
- `ui.properties.list` lists binding property paths and values. Params: `windowId`, optional `prefix`.
- `ui.property.get` reads one binding property. Params: `windowId`, `path`.
- `ui.property.set` writes one binding property and syncs authored UI. Params: `windowId`, `path`, `value`.
- `ui.node.focus` focuses a node by markup `id` or `nodeIndex`/`nodeGeneration`. Params: `windowId`, target selector.
- `ui.node.activate` sends an activate event to a node (buttons, menu items, command-bound controls). Params: `windowId`, target selector.
- `ui.node.setValue` sets a widget value by markup `id`. Params: `windowId`, `id`, `value` (`bool`, `number`, or `string`), optional `notify` (default `true`).
- `logs.read` returns buffered structured log records. Params include optional `afterId` and `limit`.
- `profile.start` starts a profiling session. Params may include `name`.
- `profile.stop` stops the active or identified session. Params may include `profileId`.
- `profile.read` returns profile metadata/data or writes it as an artifact. Params may include `profileId` and `path`.

Exact optional parameters and result fields can evolve. Treat `agent.capabilities` and `agent.listMethods` from the running build as authoritative.

## Artifact paths

Screenshots and large profiles are server-side artifacts, not binary bytes embedded in the JSON frame. When `path` is omitted, the bridge chooses a unique file beneath `AgentArtifacts` and returns the resolved path and metadata. Relative paths are resolved beneath that directory; absolute paths are accepted only when they remain beneath it. A rejected, inaccessible, or unsupported path produces a structured error.

Paths are interpreted by the engine process, not by the client process. Relative-path bases and allowed roots are reported by `agent.capabilities`; do not assume the caller's current directory. The PowerShell client passes `-OutputPath` through and does not copy or decode the artifact.

## Registration and thread affinity

The TCP listener handles framing, authentication, validation, and dispatch, but engine operations are provided by registered handlers. Window, renderer, UI, simulation, logging, and profiler subsystems register methods while their owning objects are alive and unregister them during teardown. Consequently, the available method list can change during startup, game switches, window destruction, and shutdown.

A handler that touches thread-affine state is queued to its owning window/render/simulation thread. It must not execute GPU, GLFW, UI-tree, or simulation mutations directly on the listener thread. The bridge completes the response only after the queued operation finishes or the request deadline expires. A timeout does not grant permission to run thread-affine work on the wrong thread.

## Security and limits

- The bridge must bind to loopback only. It is not a remote-control protocol and has no TLS.
- The token is a bearer secret. Compare it before dispatch, avoid logging it, and rotate it between automation sessions.
- Do not expose the port through firewall rules, port forwarding, containers, or remote tunnels unless an authenticated encrypted boundary is intentionally added.
- Prefer `ORKA_AGENT_TOKEN` over placing tokens in scripts, source control, process arguments, or recorded command history.
- Requests are bounded by frame-size, queue, rate, artifact, log-buffer, and timeout limits advertised by `agent.capabilities`.
- Calls can fail with `method_not_found` when a subsystem is not registered, `invalid_params`, `unauthorized`, `busy`, `timeout`, `not_supported`, or an implementation-specific structured error.
- Input is synthetic and may still be constrained by Windows focus, integrity-level, and foreground activation rules.
- `waitFrames` requires the relevant loop to be advancing; minimized, paused, loading, or shutting-down hosts can time out.
- Screenshot, UI inspection, logs, and profiles are snapshots and may not be mutually atomic.

## Examples

Health and discovery:

```powershell
.\scripts\orka-agent.ps1 -Pretty
.\scripts\orka-agent.ps1 -ListMethods -Pretty
.\scripts\orka-agent.ps1 -Windows
```

Generic calls with JSON or PowerShell parameters:

```powershell
.\scripts\orka-agent.ps1 -Method ui.inspect -Params '{"windowId":1}' -Pretty
.\scripts\orka-agent.ps1 -Method window.resize -Params @{
    windowId = 1
    width = 1280
    height = 720
}
```

Input and synchronization:

```powershell
.\scripts\orka-agent.ps1 -MouseMove -WindowId 1 -X 320 -Y 180
.\scripts\orka-agent.ps1 -Click -WindowId 1 -X 320 -Y 180 -Button left
.\scripts\orka-agent.ps1 -KeyPress F1 -WindowId 1
.\scripts\orka-agent.ps1 -KeyPress S -Modifiers Ctrl,Shift -WindowId 1
.\scripts\orka-agent.ps1 -Text 'Hello, ORKA!' -WindowId 1
.\scripts\orka-agent.ps1 -WaitFrames 2
```

Artifacts, logs, and profiles:

```powershell
.\scripts\orka-agent.ps1 -Screenshot -WindowId 1 -OutputPath 'agent-artifacts\frame.png' -Pretty
.\scripts\orka-agent.ps1 -Logs -AfterId 100 -Limit 500
.\scripts\orka-agent.ps1 -ProfileStart -ProfileName 'menu-open'
.\scripts\orka-agent.ps1 -ProfileStop
.\scripts\orka-agent.ps1 -ProfileRead -OutputPath 'agent-artifacts\menu-open.json' -Pretty
```
