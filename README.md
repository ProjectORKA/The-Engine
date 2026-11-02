# Project-ORKA
The official engine source code for Project ORKA

## What this is
This is a game engine and software playground for creating the things you want.
Use it for Visualizations, Simulations, Games, Tools and whatever you can come up with, just make sure to upload your creations so others can enjoy them too.

Want to know more? [Heres our Discord](https://discord.gg/NUH4bRzqwJ)

Or take a look at our website: [project-orka.com](https://project-orka.com)

## Features
* Clean and easy to read
* Game-independent renderer (allows for visual customization without impacting the game)
* Multiple window support
* Mesh loader
* Texture loader
* Instant fullscreen (Alt + Enter)
* Transparent framebuffer
* Separate input thread (main thread) (even lower input lag)

## Getting started
Windows + Visual Studio 2022 (Desktop development with C++). Libraries are installed through [vcpkg](https://vcpkg.io); the list is in `libraries.txt`. The [ORKA Launcher](https://www.project-orka.com/getting-started) sets this up automatically.

Build from the repository root:
```
powershell -ExecutionPolicy Bypass -File .\scripts\build-orka.ps1
```

Main libraries: [GLFW](https://www.glfw.org/) (windows), [GLEW](https://glew.sourceforge.net/) (OpenGL), [GLM](https://github.com/g-truc/glm) (math), [Assimp](https://www.assimp.org/) (meshes), [FreeType](https://freetype.org/) (fonts), [OpenAL Soft](https://openal-soft.org/) (audio), [ASIO](https://think-async.com/Asio/) (networking), [Optick](https://github.com/bombomby/optick) (profiling, included).

## License
Project ORKA is licensed under [CC BY-NC-SA 4.0](LICENSE): free to use, modify and share for non-commercial purposes, with credit, and modified versions must use the same license.
Third-party code, fonts and assets keep their own licenses, listed in [THIRD_PARTY.md](THIRD_PARTY.md).
