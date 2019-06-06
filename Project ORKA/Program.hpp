/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region ignore

#pragma once
#ifndef GAME_HPP
#define GAME_HPP

//std libs
#include <map>
#include <bitset>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <sstream>
#include <fstream>
#include <iostream>

#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure

#pragma endregion

#pragma region notes

#pragma region todo

//todo:		make whenMouseIsMoving function
//todo:		implement ECS
//todo:			+ transformation component
//todo:			- mesh component
//todo:			- rendering component
//note:		differentiate systems into write and (only) read systems
//note:		the rendering is strictly a read system

//todo:		separate rendering into own thread

#pragma endregion

#pragma region bugs

//bug:					window jumps into capture after indirectly changing focus and then changing back

//minor bug:			framebuffer flickering on resize
//clue:						main and render thread are not syncronized

//minor perf issue:		switching anti aliasing is slow and not instant
//clue:						reloading the entire window and rendering system is not instant
//possible solution:		rendering to a multisampled framebuffer could prevent having to reload the window for aa

#pragma endregion

#pragma region features
//multithreading
//entity component system
//multiple windows
//	per window render settings
//fullscreen
//	instant fullscreen
//	borderless fullscreen
//change graphics settings on run time
//render independent game instance
//
//
//
//
//
//
//
//
//
//
//
//
#pragma endregion

#pragma endregion

#pragma region preprocessor definitions

#define DEBUG					//enables debug messages

#define NUMBER_OF_AVAIVABLE_COMPONENTS 1

#pragma endregion

#pragma region data
//DATA (sorted in inverted hierarchical order)
using Entity = std::bitset<NUMBER_OF_AVAIVABLE_COMPONENTS>;
//struct Key {
//	bool holding = false;		//active every frame while button is pressed
//	bool activated = false;		//activated once every press
//	bool toggled = false;		//active every toggle
//	bool toggleStatus = false;	//do not confuse with holding! toggles with every press down
//};
struct Axis {
	double current = 0.0f;
	double previous = 0.0f;
	double delta = 0.0f;
};
struct Mesh {
	bool loaded = false;
	unsigned int indexCount = 0;
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	Mesh();
	~Mesh();
};
struct MeshHandler {
	std::map<std::string, std::shared_ptr<Mesh>> meshMap;
	MeshHandler();
	~MeshHandler();
};
struct ShaderProgram {
	bool loaded = false;
	GLuint programID;
	GLuint      modelMatrixID;
	GLuint       viewMatrixID;
	GLuint projectionMatrixID;
	ShaderProgram();
	~ShaderProgram();
};
struct GameServer;
struct InputHandler {
	//Key action;
	//Key interact;
	//Key forward;
	//Key backward;
	//Key right;
	//Key left;
	//Key up;
	//Key down;
	//Key escape;
	//Key wireframe;
	Axis yaw;
	Axis pitch;
};
struct RenderingSystem {
	ShaderProgram primitiveShader;
	MeshHandler meshHandler;
	GameServer * gameServer;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 modelMatrix;
	RenderingSystem(GameServer & gameServer);
	~RenderingSystem();
};
struct Window {
	//settings
	std::string title = "Project ORKA";
	bool fullScreen = false;
	bool borderlessFullScreen = true;
	bool decorated = true;
	unsigned int width = 1600;
	unsigned int height = 900;
	unsigned short antiAliasing = 4;

	//local variables
	bool duplicateWindow = false;
	bool capturingCursor = false;
	double curPosX, curPosY = 0;
	double deltaX, deltaY = 0;
	GLFWwindow * glfwWindow;
	InputHandler input;
	std::unique_ptr<RenderingSystem> renderingSystem;

	//thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;

	Window(GameServer & gameServer);
	~Window();
};
struct Transformation {
	glm::vec3 location = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	glm::mat4 modelMatrix();
};
struct TransformationSystem {
	std::vector<Transformation> transformations;
	std::vector<Transformation * > interfaceLayer;
};
struct ECS {
	std::vector<Entity> entities;
	TransformationSystem transformationSystem;
};
struct Chunk {
	ECS entityComponentSystem;
	Chunk();
	~Chunk();
};
struct Sky {
	glm::vec3 skyColor = glm::vec3(0.1f);
};
struct WorldSystem {
	Sky sky;
	Chunk chunk;
};
struct Time {
#define TIME_STEP 1
	bool paused = false;
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::duration<double, std::ratio<1 / TIME_STEP>> delta;
	std::chrono::duration<double, std::ratio<1 / TIME_STEP>> total;
	Time();
	~Time();
	double getDelta();
	double getTotal();
};
struct GameServer {
	Time time;
	WorldSystem worldSystem;
	GameServer();
	~GameServer();

	//thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;
};
struct WindowHandler {
	std::vector<std::shared_ptr<Window>> windows;
	WindowHandler();
	~WindowHandler();
};  
struct Program {
	GameServer gameServer;			//<-- already starts simulating the gameWorld
	WindowHandler windowHandler;	//<-- is just a container for all windows
	Program();						//<-- creates windows that render the game
	~Program();
};

#pragma endregion

#pragma region functions

//debug
void debugPrint						(const char * debugMessage);

//shader
void useShader						(ShaderProgram & program);
void loadShader						(ShaderProgram & shaderProgram, const char * vertexPath, const char * fragmentPath);

//mesh
void loadMesh						(Mesh & mesh, const char * path);
void renderMesh						(Mesh & mesh);

//render settings
void changeAntiAliasing				(Window & window, unsigned int antiAliasing);

//rendering system
void renderSky						(Sky & sky);
void renderChunk					(Chunk & chunk, RenderingSystem & renderingSystem);
void renderFrame					(RenderingSystem & renderingSystem, int width, int height);
void renderWorld					(WorldSystem & worldSystem, RenderingSystem & renderingSystem);
void renderEntities					(ECS & entityComponentSystem, RenderingSystem & renderingSystem);
void __stdcall DebugOutputCallback	(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

//input
//void resetKey						(Key & key);
//void setKey							(Key & key, bool pressed);
void captureCursor					(Window & window);
void uncaptureCursor				(Window & window);
void setAxis						(Axis & axis, double value);
void whenMouseIsMoving				(GLFWwindow* window, double xpos, double ypos);
void whenMouseIsScrolling			(GLFWwindow* window, double xoffset, double yoffset);
void whenMouseIsPressed				(GLFWwindow* window, int button, int action, int mods);
void whenButtonIsPressed			(GLFWwindow* window, int key, int scancode, int action, int mods);

//window
void centerWindow					(Window & window);
void renderWindow					(Window & window);
void reloadTheWindow				(Window & window);
void toggleFullscreen				(Window & window);
void destroyGLFWWindow				(Window & window);
void setWindowCallbacks				(Window & window);
void whenWindowChangedFocus			(GLFWwindow* window, int focused);
void setIcon						(Window & window, std::string path);
void createGLFWWindow				(Window & window, GameServer & gameServer);
void whenWindowIsResized			(GLFWwindow* window, int width, int height);
void createNewWindow				(WindowHandler & windowHandler, GameServer & gameServer);

//window handler
void checkWindowEvents					(WindowHandler & windowHandler);
void whenGLFWThrowsError			(int error, const char* description);

//time
void resetTime						(Time & time);
void updateTime						(Time & time);
//entity component system
void addTriangle					(ECS & entityComponentSystem);
void addEmptyEntity					(ECS & entityComponentSystem);

//THREADS
void GameServerThread				(GameServer & gameServer);
void WindowThread					(Window & window, GameServer * gameServer);

#pragma endregion

#endif // !GAME_HPP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////