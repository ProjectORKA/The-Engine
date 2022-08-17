# Project-ORKA
The official engine source code for Project ORKA

## What this is
The goal (for now) is to have a playable game/engine hybrid, nothing more nothing less.
Its not general purpose, and its not meant for commercial use.
For now its just a game that can be modified easily by changing the actual code around.

The code for this project aims to maximize performance and readability at the same time. We use conventions that might be untypical or restrictive, but it has to be in order to be as clean and intuitive as possible. The code is meant to be read and used by a lot of people, people who might have just started programming and dont want to read 1000 pages of documentation.

At the moment the engine is Windows only and so the easiest way to set it up, is by using Visual Studio 2019.
With Visual Studio just open the .sln file, the libraries should already be connected as they use relative paths.
The Data folder should automatically be copied to the output folder when you run, but the dlls for assimp however need to be copied over to the final .exe.

## Features
* Clean and easy to read
* Game-independent renderer (allows for visual customization without impacting the game)
* Multiple window support
* Mesh loader
* Texture loader
* Instant fullscreen (Alt + Enter)
* Transparent framebuffer
* Separate input thread (main thread) (even lower input lag)

### Requirements (already included):
* [GLFW](https://www.glfw.org/) - Windows
* [GLEW](http://glew.sourceforge.net/) - OpenGL
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - Math
* [ASSIMP](https://www.assimp.org/) - Mesh Loader
* [STB IMAGE](https://github.com/nothings/stb) - Image loading
* [OPTICK](https://github.com/bombomby/optick) - Profiling
