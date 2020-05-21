# Project-ORKA
The official engine source code for Project ORKA

For now Windows only, so easiest way to set it up is by using Visual Studio 2019. With Visual Studio, the libraries should already be connected as they use relative paths.
The Data folder should automatically be copied to the output folder when you build.
The dlls for assimp however need to be included for the final .exe.

## Features
* clean and easy to read
* game-independent renderer (allows for visual customization and extremely low input lag)
* multiple window support
* ".fbx" loader and fully automatic ".mesh" converter
* Texture loader
* instant fullscreen (Alt + Enter)
* transparent framebuffer
* separate input thread (main thread) (even lower input lag)

### Requirements:
* [GLFW](https://www.glfw.org/) - Windows
* [GLEW](http://glew.sourceforge.net/) - OpenGL
* [GLM](https://glm.g-truc.net/0.9.9/index.html) - Math
* [ASSIMP](https://www.assimp.org/) - Mesh Loader
