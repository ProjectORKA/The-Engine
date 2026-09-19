# Project ORKA — Features

Project ORKA is an open-source C++ game engine and demo collection. It uses a **left-handed coordinate system** (+X right, +Y forward, +Z up, Blender-style). The stack includes **OpenGL/GLEW**, **GLFW**, **GLM**, **miniaudio**, and **ASIO**.

**Default entry point:** `main.cpp` launches **Sandbox** directly. A **GameLoader** menu and **ORKAIntroSequence** exist for picking games at runtime.

---

## Engine Core

### Engine (`Engine.hpp` / `Engine.cpp`)
- Bootstraps **AudioEngine**, **ResourceManager**, and optional **JobSystem** on startup
- Detects executable path from launch arguments for correct `Data/` resolution
- Clean shutdown: destroys resources and joins worker threads

### Game architecture (`Game.hpp`)
- **GameSimulation** — background-thread simulation loop (create, destroy, update; configurable tick rate)
- **GameRenderer** — `UIElement` that renders 3D into an offscreen framebuffer, then composites into the UI tree
- **connect()** links renderer and simulation sides

### Window (`Window.hpp` / `Window.cpp`)
- GLFW-backed windows as **UIElement** subclasses, each on its own thread
- Window states: minimized, windowed, maximized, fullscreen
- Decoration on/off, centering, icon, title, resize
- Mouse tracking (position, delta, bottom-left coordinates)
- Drag-and-drop file paths into the window
- Input routing: hover picking, mouse capture, element focus
- Global hotkeys: **F7** reload shaders, **F8** profiling, **F9** OpenGL logging, **Esc**, **Enter**

### User interface (`UserInterface.hpp` / `UserInterface.cpp`)
- Global `ui` singleton managing multiple windows
- GLFW init/terminate lifecycle
- **run()** main loop with optional terminate-on-exit
- Window creation with or without initial content
- Focus and capture management across windows and elements

### Resource manager (`ResourceManager.hpp` / `ResourceManager.cpp`)
- Scans `Data/` and `Cache/` for meshes, textures, and shaders
- Supported formats: `.fbx`, `.mesh`, `.png`, `.bmp`, `.exr`, `.hdr`, `.jpg`, `.vert`, `.frag`
- FBX → cached mesh pipeline with config tracking
- Path resolution for Debug/Release vs deployed layouts
- Name-based lookup for meshes, textures, vertex/fragment shaders
- Planned: file watcher for hot reload

### Job system (`JobSystem.hpp`)
- Thread pool with work queue
- `enqueue()` / `enqueueNext()` for parallel tasks
- Start, stop, and join lifecycle with optional debug logging

### Time (`Time.hpp`)
- Delta time, total elapsed time, pause/unpause
- **Timer** for scoped measurements
- **limitFramerate()** helper

### Player and camera (`Player.hpp`, `Camera.hpp`)
- **Player** — WASD/QE movement, mouse look, virtual input hooks
- **DebugPlayer** — fly camera with scroll-wheel speed scaling
- **Camera** — view/projection matrices, FOV, near/far clip, rotation

### Scene (`Scene.hpp`, `SceneSystem.hpp`)
- Hierarchical **SceneNode** with transforms and mesh indices
- **Scene2** — FBX import and mesh storage

### Platform and files
- **FileSystem.hpp** — path operations, directory scanning, file I/O, copy/move
- **Basics.hpp** — custom type aliases (`Float`, `Int`, `Bool`, etc.), GLM integration, containers
- **Platform.hpp**, **Windows.hpp**, **WindowAPI.hpp** — OS/GLFW abstraction
- **Memory.hpp**, **LifeTimeGuard.hpp**, **LockFreeQueue.hpp**, **Queue.hpp**, **JobQueue.hpp**

### Math and algorithms
- **Math.hpp**, **TiledMath.hpp**, **Transform.hpp**, **TransformationStack.hpp**
- **Random.hpp**, **Hash.hpp**, **PerlinNoise.hpp**, **SimplexNoise.hpp**
- **KDTree.hpp**, **KDTree2.hpp**, **KDTreeImplementation.hpp**
- **DelaunayTriangulation.hpp**, **Collision.hpp** (2D circle/AABB colliders)
- **Array2D.hpp**, **Heightmap.hpp** (AdvancedHeightMap with extended precision)

### Networking (`ASIONetworking.hpp`, `Networking.hpp`, `TripleNineMessage.hpp`)
- ASIO standalone UDP sockets
- Triple Nine client/server protocol (connect, ping, disconnect, server info)
- Heuristic networking prototype for incremental nearest-neighbor mesh building

---

## Rendering

### Renderer (`Renderer.hpp` / `Renderer.cpp`)
Central rendering facade with subsystems:

| Subsystem | Capabilities |
|-----------|--------------|
| **MeshSystem** | Named GPU meshes, instanced rendering, fullscreen quad, procedural basics |
| **ShaderSystem** | Named shader programs, hot reload/rebuild, uniform binding |
| **TextureSystem** | Named GPU textures, multi-slot binding |
| **TextRenderSystem** | Bitmap text rendering |
| **MatrixSystem** | Transform/uniform matrix stack |
| **PrimitivesRenderer** | Lines, points, circles, wireframe cubes, planes, rectangles |

**Render state:** wireframe, culling, depth test, alpha blending (configurable src/dst/equation), scissor, render regions

**Coordinate spaces:** normalized, aspect-correct, screen (top/bottom-left), UI (browser-like)

**Drawing:** meshes (single and instanced), fullscreen shaders, post-process passes, atmosphere, sky, primitives, text

**Framebuffers:** ID buffer, post-process buffer; frame counting and target FPS

### Graphics API (`GraphicsAPI.hpp`)
- OpenGL 4.x wrapper: textures, buffers, VAOs, framebuffers, shaders
- Typed enums for filters, wrapping, blend modes, formats, attachments
- **OpenGlContext** — state tracking, debug callback, object labeling, optional OpenGL trace logging

### Framebuffer (`Framebuffer.hpp`)
- Multi-attachment FBOs with resize, clear (color/depth), readback, completeness checks

### Shaders (`Data/Shaders/` — 109 GLSL files)
Post-processing and effects include:
- **Bloom** — prefilter, downsample, upsample chain (Triple Nine uses 8 bloom passes)
- **Tonemapping**, **gamma correction**, **auto-exposure**
- **Blur**, **blurred UI**
- **Erosion pipeline** — multi-buffer Shadertoy-style terrain erosion (BufferA/B, final, tonemap)
- **MXAO** ambient occlusion
- **Depth cutter**, **ID shader** (picking/debug)
- Game-specific shaders: MoonCrash, Melon, Flappy Bird, Simple RTS, Triple Nine, Orbital Oblivion, Sortr, Image Viewer, SDF, voxels, and more

### Planet and terrain (`PlanetSystem.hpp`, `PlanetRenderSystem.hpp`, `TerrainSystem.hpp`)
- **QuadtreeSystem** — adaptive terrain LOD (up to 63 levels, millimeter-scale precision)
- **OctreeSystem** — 3D spatial subdivision tied to quadtree
- **TerrainSystem** — height textures per chunk level, AdvancedHeightMap generation
- **PlanetRenderSystem** — GPU mesh per quadtree node, subdivide/unsubdivide, draw-distance culling, chunk borders, world distortion, vertex color modes

### Voxels (`Voxels.hpp`)
- Bit-packed **VoxelChunk** (configurable edge size, default 64³)
- **GPUVoxelVolume** — 3D texture upload
- Two render modes: **slice rendering** and **GPU ray marching** (`voxelSlices`, `voxelRay` shaders)
- Procedural chunk generation, camera controls

### SDF rendering (`SDFRenderer.hpp`)
- Signed-distance-field scene ray marching (`sdfScene`, `sdfTerrain`, `sdfOctreeTerrain` shaders)
- Debug fly camera

### Shadertoy / erosion (`ShadertoyRenderer.hpp`)
- Multi-pass GPU simulation: heightmap → detail → lighting → auto-exposure → tonemap
- Ping-pong framebuffers (BufferA/B), HDR buffer, star texture, dither texture
- 4096² heightmap resolution

### Software renderer (`SoftwareRendererGame.hpp`)
- CPU pixel buffer rasterized each frame, uploaded as GPU texture
- Demonstrates off-CPU rendering pipeline

### GPU simulation (`GPUSim.hpp`)
- GPU simulation via fragment shaders (`gpuSim` shader)
- Flip-flop framebuffers, 1024² world

### Atmosphere and sky
- Dedicated **atmosphere** and **sky** shaders
- `Renderer::renderAtmosphere()`, `renderSky()`
- MoonCrash atmosphere pass

### Text rendering
- **BitmapTextRenderSystem** — active bitmap font rendering
- **MSDFText** — MSDF font demo (`msdfFont` shader)
- **GlyphTextRenderSystem** — header present; not fully wired in TextRenderSystem

### Procedural meshes (`ProceduralMeshes.hpp`)
- Axis lines, planes, letter quads, wireframe cubes, triangle-strip grids

### Mesh pipeline (`CPUMesh.hpp`, `GPUMesh.hpp`)
- CPU mesh load/integrity check → GPU upload
- Position/normal/UV/color/index buffers
- Instanced rendering with transforms, directions, scales

### Other rendering systems
- **OctreeSystemRenderer.hpp** — octree visualization
- **TerrainRendering.hpp**, **SphereMeshing.hpp**
- **PointRenderer**, **RectangleRenderer**
- **RenderingOverrides.hpp** — RAII depth/wireframe/alpha-blend state overrides
- **Uniforms.hpp**, **GlobalUniformData.hpp**
- **Image.hpp**, **CPUTexture.hpp**, **GPUTexture.hpp**, **FramebufferTexture.hpp**

### Particle system (`ParticleSystem.hpp`, `PongParticleSystem.hpp`)
- Generic particle pool with lifetime, velocity, size interpolation
- Used by Pong for collision effects

---

## Audio

### AudioEngine (`AudioEngine.hpp` / `AudioEngine.cpp`)
- Facade over **miniaudio** (PIMPL hides backend)
- Load/play/stop **AudioTrack** by string ID
- Loop support

### AudioSystem (`AudioSystem.hpp`) — legacy OpenAL
- OpenAL device/context, **Sound** waveform generation (sine)
- **AudioBuffer** / **AudioSource** with play/pause/stop
- Built-in audio test

### Sound generators (`SoundGenerators.hpp`)
- Procedural sine wave generator

---

## Input and UI

### Input (`Input.hpp`, `KeyMap.hpp`)
- **InputId** — keyboard, mouse, scroll typed identifiers
- **InputEvent** — press/release with consumption flag
- Full GLFW key/mouse enum mapping

### UIElement (`UIElement.hpp` / `UIElement.cpp`)
- Flexbox-like layout: row/column, gap, padding, growX/growY
- Background/border/text colors, hover/active/focused states
- Recursive layout (sizes, positions), hit testing, mouse enter/leave
- Virtual lifecycle: create, update, render, destroy, inputEvent, renderInteractive

### UIButton (`UIButton.hpp` / `UIButton.cpp`)
- Clickable button with hover/pressed states, optional embedded content
- Virtual `doThis()` callback

### GameLoader (`GameLoader.hpp` / `GameLoader.cpp`)
- Post-intro game picker UI (column of launch buttons with name + description)
- Records selection and closes window to launch chosen game
- Fifteen registered entries (see Games and demos)

### ORKA intro sequence (`ORKAIntroSequence.hpp`)
- Animated ORKA logo intro (~5s), undecorated window
- Hands off to any follow-up `UIElement` (used by Organizer, Triple Nine)

### Performance graph (`PerformanceGraph.hpp`)
- Rolling FPS/time graph as UI overlay (Simple RTS uses it)

---

## Games and demos

### Registered in GameLoader (15)

| Game | Description | Key features |
|------|-------------|--------------|
| **UI Controls Demo** | Modern UI widget showcase | Toggles, checkboxes, sliders, text fields on the retained UI stack |
| **Sandbox** | Engine feature playground | DebugPlayer fly cam, Suzanne mesh, wireframe toggle (F), FPS overlay |
| **Pong** | Classic Pong | 2-player/AI paddles, multi-ball, particle collisions, UI-integrated |
| **Voxels** | Ray-marched voxel world | Bit-packed chunks, slice/ray modes, GPU 3D texture, lighting |
| **MoonCrash** | Planet-scale terrain | Quadtree/octree LOD, atmosphere, threaded sim, chunk debug toggles |
| **Project Melon** | Stylized melon world | Procedural paths/scenery/rocks, MelonPlayer, MelonUberShader |
| **Melon Audio Lab** | Procedural motion audio | Hum, wind, and heartbeat layers driven by player movement |
| **Sound Analysis Lab** | Audio inspection tool | Waveform zoom/scrub, FFT spectrum, spectrogram |
| **Simple RTS** | Lightweight RTS sim | Humans, wildlife, terrain, resource systems, performance graph |
| **SDF Pointcloud** | Screenspace SDF demo | Pointcloud SDF with temporal reprojection and compute hole-fill |
| **Shadertoy (Erosion)** | GPU erosion effect | Multi-pass buffers, auto-exposure, tonemap, star field |
| **Software Renderer** | CPU rasterizer demo | Pixel buffer → GPU texture upload |
| **Function Finder** | Curve-fitting playground | Searches function parameters against sampled data |
| **Triple Nine** | Arena shooter prototype | Bloom pipeline (8 passes), sky/AO/map shaders, optional intro, UDP networking scaffold |
| **Cornell Box Ray Tracer** | GPU path tracer | GGX metals, glass, area-light NEE, progressive accumulation |

### Other runnable games (not in GameLoader)

| Game | Description |
|------|-------------|
| **Organizer** | 3D org-chart visualization — companies/users as meshes, connection lines, 3D name labels, intro sequence |
| **Flappy Bird** | Side-scroller with 100 AI birds, pipe columns, pause/slow-mo, dedicated shader |
| **Snake** | Classic snake with dumb AI option, normalized-space rendering |
| **Dungeons and Discord** | 3D scene editor — entity placement, multi-select, wireframe, scene import, dice rolls |
| **MindMapper** | Force-directed graph sim (100 nodes), threaded physics, interactive rendering |
| **Physics Playground** | 128-particle Verlet sim with gravity and distance-based connections |
| **Orbital Oblivion** | Space RTS prototype — teams, 200 ships, 5 planets, flocking AI, bloom, pause |
| **SDF Renderer** | SDF ray-march scene explorer |
| **GPU Sim** | Fragment-shader cellular/GPU simulation |
| **MSDF Text** | MSDF font rendering demo |
| **Hello Triangle** | Minimal raw OpenGL triangle (VAO/VBO/IBO) |
| **AI Playground** | Empty 3D sandbox scaffold for AI experiments |
| **Prototyping Sandbox** | Swappable prototype plugins behind a fly camera (see Prototypes) |
| **Keva** | Turn-based RPG design stub — characters, items, slots, inventory (sim only, console options) |
| **Music Visualizer** | Standalone GLFW HSV shader toy (not integrated with engine UI) |
| **Tree System** | Procedural branch/leaf tree generation demo |

### Standalone tools (separate `run(argc, argv)` entry points)

| Tool | Description |
|------|-------------|
| **Image Viewer** | Image browser — zoom/pan, priority-based CPU/GPU preload, database persistence, drag-drop, delete |
| **Sortr** | Radial folder ring UI for image sorting — shape toggle, preload/unload by priority |

---

## Prototypes and experiments

### Prototyping sandbox (`PrototypingSandbox.hpp`)
- Sandbox wrapper for swappable **Prototype** plugins behind a fly camera
- **Currently active:** SineWaveTerrainRegressor
- **Available in codebase (commented out):** KDTree, AxisNeighbor, Triangulation, Labyrinth, ConvexHull, ImageApproximation, HeuristicNetworking, WeirdSphere, PersistentInfo, ClosestConnection, SquareNeighbor

### Prototype interface (`Prototype.hpp`)
- Virtual: `create`, `render`, `destroy`, `action`, `inputEvent`

### Individual prototypes

| Prototype | Purpose |
|-----------|---------|
| **SineWaveTerrainRegressor** | Genetic algorithm fits multi-frequency sine terrain to sample heightmap |
| **HeuristicNetworkingPrototype** | Incremental k-nearest neighbor connection graph |
| **ImageApproximationPrototype** | Image/terrain texture approximation experiment |
| **LabyrinthGeneratorPrototype** | 75×75 maze generation, pathfinding, cost calculation |
| **KDTreePrototype / KDTreePrototype2** | KD-tree construction, closest-point queries, visualization |
| **AxisNeighborPrototype / AxisNeighborPrototype2** | Cardinal-direction neighbor linking in 3D point clouds |
| **SquareNeighborPrototype** | Grid-based neighbor assignment |
| **TriangulationPrototype** | Distance-sorted connection triangulation |
| **ClosestConnectionPrototype** | Incremental closest-point line graph |
| **PersistentInfoPrototype** | Persistent neighbor graph with step-based growth |
| **ParallelizedConvexHullPrototype** | 10K-point convex hull (parallel), hull visualization |
| **WeirdSphereAlgorithm** | Quad-tree sphere point injection/subdivision |
| **InfiniteDetailPrototype** | Hierarchical solar-system → planet → continent → ocean generator stub |
| **Molecules2D** | 2D particle force simulation with grid-based forces |

### Melon world subsystems
- Procedural path/scenery/rock grid generation around player
- Custom **MelonUberShader** with shadow/smoke (TODOs for collision, collectables, dash)

### Simple RTS entity systems
- **HumanSystem**, **WolfSystem**, **RabbitSystem** — AI agents
- **TreeSystem**, **BushSystem**, **BerryBushSystem**, **TreeStumpSystem** — flora
- **TerrainSystem**, **BodySystem**, **QuadtreeSystem** — world/physics spatial indexing

---

## Tools and utilities

### Logging and debug
- **Logger.hpp** — file-based logging with timestamps (`Logs/` directory)
- **Debug.hpp** — colored console output (fmt), logDebug/Warning/Error, debugbreak, pause, messageBox
- **Settings.hpp** — compile-time debug flags (OpenGL tracing, audio debug, job system debug, etc.)
- **OptickProfiling.hpp** — Optick profiler integration (F8 toggle in windows)

### Configuration (`Settings.hpp`)
- Default 1920×1080, 144 FPS target
- Planet renderer constants (chunk levels, draw distance, terrain texture size)
- Networking port configuration

### Build
- **Compiler.hpp** — build/compile utilities
- **Project ORKA.vcxproj** — Visual Studio project

### Data and assets
- **Data/Shaders/** — 109 GLSL shaders for demos and engine features
- **Data/** — meshes, textures, fonts (loaded via ResourceManager)
- **Cache/Meshes/** — FBX-derived cached meshes

### Misc utilities
- **NameGenerator.hpp**, **Plot.hpp**, **MathDNA.hpp**, **LevelledGraph.hpp**
- **BranchStructure.hpp** — tree branch data for procedural trees
- **FileTypes.hpp**, **File.hpp** — file type detection
- **Fonts.hpp**, **ImageViewerDatabase.hpp**, **ImageViewerResource.hpp**, **SortrResource.hpp**
- **Override.hpp** — generic override pattern
- **ULLUtil.hpp**

---

## Architecture summary

```
main.cpp → Engine (Audio, Resources, Jobs)
         → Game / UI entry
              → UserInterface
                   → Window (per-thread)
                        → Renderer (Mesh, Shader, Texture, Text, Primitives)
                        → UIElement tree
                             → GameRenderer (offscreen FBO → composite)
                                  ↔ GameSimulation (background thread)
```

**Notable patterns:**
- Everything visual is a **UIElement** tree (including 3D via `GameRenderer`)
- Simulations run on **background threads**; renderers connect via `connect()`
- Resources are **name-based** (meshes, shaders, textures looked up by string)
- **Left-handed** Blender-style coordinates throughout

---

## External links

- GitHub: https://github.com/ProjectORKA/Project-ORKA
- Website: https://project-orka.com
- Discord: https://discord.gg/NUH4bRzqwJ

**License note (from `main.cpp`):** Non-commercial use; modify and share freely.
