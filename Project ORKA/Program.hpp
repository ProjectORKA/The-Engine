#pragma region ignore

#pragma once
#ifndef GAME_HPP
#define GAME_HPP

//std libs
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <memory>

#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#pragma endregion

//bug:					not recapturing cursor on reloadWindow
//minor bug:			framebuffer flickering on resize
//clue:						main and render thread are not syncronized

//minor perf issue:		switching anti aliasing is slow and not instant

//cleanup:				turn shader creation into own code 

#define DEBUG					//enables debug messages

//STRUCTS (sorted in inverted hierarchical order)

struct Key {
	bool holding = false;		//active every frame while button is pressed
	bool activated = false;		//activated once every press
	bool toggled = false;		//active every toggle
	bool toggleStatus = false;	//do not confuse with holding! toggles with every press down
};
struct Axis {
	double current = 0.0f;
	double previous = 0.0f;
	double delta = 0.0f;
};
struct Mesh {
	bool loaded = false;
	GLuint vertexArrayObject;
	GLuint vertexBuffer;
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
	ShaderProgram();
	~ShaderProgram();
};
struct GameServer;
struct InputHandler {
	Key action;
	Key interact;
	Key forward;
	Key backward;
	Key right;
	Key left;
	Key up;
	Key down;
	Key escape;
	Key wireframe;
	Axis yaw;
	Axis pitch;
};
struct RenderingSystem {
	ShaderProgram primitiveShader;
	MeshHandler meshHandler;
	GameServer * gameServer;
	RenderingSystem(GameServer * gameServer);
	~RenderingSystem();
};
struct Window {
	//settings
	std::string title = "Project ORKA";
	bool fullScreen = false;
	bool borderlessFullScreen = false;
	unsigned int width = 1600;
	unsigned int height = 900;
	unsigned short antiAliasing = 4;

	//local variables
	bool capturingCursor = false;
	//int winPosX, winPosY = 0;
	double curPosX, curPosY = 0;
	double deltaX, deltaY = 0;
	GLFWwindow * glfwWindow;
	InputHandler input;
	RenderingSystem * renderingSystem;

	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;

	Window(GameServer * gameServer);
	~Window();
};
struct Entity {
	std::string type = "empty";
	Entity();
	~Entity();
};
struct Triangle : public Entity {
	Triangle();
};
struct Chunk {
	std::vector<std::shared_ptr<Entity>> entities;
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


//FUNCTIONS
//debug
void debugPrint								(const char * debugMessage);
//shader
void useShader								(ShaderProgram & program);
void loadShader(ShaderProgram & shaderProgram, const char * vertexPath, const char * fragmentPath);
//mesh
void loadMesh						(Mesh & mesh);
void renderMesh						(Mesh & mesh);
//render settings
void changeAntiAliasing				(Window & window, unsigned int antiAliasing);
//rendering system
void setDebugMessageCallback		();
void renderSky						(Sky & sky);
void renderChunks					(Chunk & chunk, RenderingSystem & renderingSystem);
void renderEntity					(Entity & entity, RenderingSystem & renderingSystem);
void renderFrame					(RenderingSystem & renderingSystem, int width, int height);
void renderWorld					(WorldSystem & worldSystem, RenderingSystem & renderingSystem);
void renderEntities					(std::vector<std::shared_ptr<Entity>>& entities, RenderingSystem & renderingSystem);
void __stdcall DebugOutputCallback	(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
//input
void resetKey						(Key & key);
void setKey							(Key & key, bool pressed);
void setAxis						(Axis & axis, double value);
void whenMouseIsPressed				(GLFWwindow* window, int button, int action, int mods);
void whenMouseIsScrolling			(GLFWwindow* window, double xoffset, double yoffset);
void whenButtonIsPressed			(GLFWwindow* window, int key, int scancode, int action, int mods);
//window
void renderWindow					(Window & window);
void setWindowHints					(Window & window);
void captureCursor					(Window & window);
void uncaptureCursor				(Window & window);
void toggleFullscreen				(Window & window);
void setIcon						(Window & window, std::string path);
void createNewWindow				(WindowHandler & windowHandler, GameServer * gameServer);
void centerWindow					(Window & window);
void reloadTheWindow				(Window & window);
void destroyGLFWWindow				(Window & window);
void setWindowCallbacks				(Window & window);
void processWindowInputs			(Window & window);
void whenWindowChangedFocus			(GLFWwindow* window, int focused);
void createGLFWWindow				(Window & window, GameServer * gameServer);
void whenWindowIsResized			(GLFWwindow* window, int width, int height);
//window handler
void updateWindows				(WindowHandler & windowHandler);
void whenGLFWThrowsError		(int error, const char* description);
//time
void resetTime					(Time & time);
void updateTime					(Time & time);

//THREADS
void GameServerThread			(GameServer & gameServer);
void WindowThread				(Window & window, GameServer * gameServer);

#endif // !GAME_HPP