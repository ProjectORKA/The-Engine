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
#include <list>

#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "GLFW/glfw3.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure

#include "Math.hpp"

#pragma endregion

#pragma region notes

#pragma region todo
//todo:		implement ECS
//note:		differentiate systems into write and (only) read systems
//note:		the rendering is strictly a read system

//todo:		mesh LOD loading
#pragma endregion

#pragma region bugs

//minor bug:			framebuffer flickering on resize
//clue:						main and render thread are not syncronized

//minor perf issue:		switching anti aliasing is slow and not instant
//clue:						reloading the entire window and rendering system is not instant
//possible solution:		rendering to a multisampled framebuffer could prevent having to reload the window for aa

#pragma endregion

#pragma region features
//engine features
//	multithreading
//	entity component system
//	independent and steady game simulation

//windows
//	multiple windows
//	instant fullscreen
//	borderless fullscreen
//	transparency

//renderer
//	change graphics settings on run time
//	render independent game instance
//	per window render settings
//  automatic instanced rendering

//editor
//  spectator camera
//	3d movement
//	speed control

#pragma endregion

#pragma region future optimizations
//combine buffers of different objects into larger chunks (combine all static objects of a chunk)
//use function callbacks to operate on only selected chunks in the world system by passing the operation function into a selection function
#pragma endregion

#pragma endregion

#pragma region preprocessor definitions

#define DEBUG

#define VIEW_DISTANCE 2.0f
#define CHUNK_LEVEL_MAX 62
#define METER_DIVIDER_INT 16777216
#define METER_DIVIDER_FLOAT 16777216.0f

#define INITIAL_CAMERA_SPEED 300
#define CAMERA_SPEED_MULTIPLIER 1.15f

#define NUMBER_OF_AVAIVABLE_COMPONENTS 2
#define TRANSFORMATION 0
#define CAMERA 1

#pragma endregion

#pragma region data
//DATA (sorted in inverted hierarchical order)

struct Time {
	bool paused = false;
	std::chrono::steady_clock::time_point lastTime;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::duration<double, std::ratio< 1 / 1 >> delta;
	std::chrono::duration<double, std::ratio< 1 / 1 >> total;
	Time();
	~Time();
	double getDelta();
	double getTotal();
};
//gameserver stuff
struct GameServer;
using ComponentIndex = unsigned int;
using ComponentStructure = std::bitset<NUMBER_OF_AVAIVABLE_COMPONENTS>;
struct Entity {
	//indices corresponding to the components defined in the archetype
	//e.g. archetype consists of components A, B, C then {0,3,2} means A[0], B[3], C[2]
	std::vector<ComponentIndex> componentIndices;
};
struct EntityTypes {
	unsigned int						numberOfEntityTypes = 0;
	std::vector<std::string>			names;
	std::vector<ComponentStructure>		structures;
	std::vector<std::vector<Entity>>	entityArrays;
	
	EntityTypes();
};
struct Transformation {
	glm::vec3 location = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};
struct TransformationSystem {
	std::vector<Transformation> transformations;
	std::vector<glm::mat4> modelMatrices;
};
struct EntityComponentSystem {
	EntityTypes entityTypes;
	TransformationSystem transformationSystem;
};
struct Chunk{
	//unsigned long long to not loose precision
	glm::u64vec3 location = glm::u64vec3(0);
	
	//max 64 levels (based on ull bits)
	unsigned short level = 0;

	//has sub nodes
	bool subdivided = false;

	//has renderable content
	bool hasContents = true;

	//pointer to parent node
	Chunk * parent = nullptr;

	//pointers to sub nodes
	Chunk * tfr = nullptr; //top front right = +X +Y +Z
	Chunk * tfl = nullptr;
	Chunk * tbr = nullptr;
	Chunk * tbl = nullptr;
	Chunk * bfr = nullptr;
	Chunk * bfl = nullptr;
	Chunk * bbr = nullptr;
	Chunk * bbl = nullptr; //bottom back left = -X -Y -Z



	//create root node
	Chunk();
	
	//create sub node
	Chunk(Chunk & chunk, bool x, bool y, bool z);
	
	//unsubdivide and destroy node
	~Chunk();
};
struct Sky {
	glm::vec3 skyColor;
	Sky();
};
struct WorldSystem {
	Sky sky;
	//glm::vec3 cameraPosition;
	EntityComponentSystem ecs;
	Chunk root;
	WorldSystem();
};
struct GameServer {
	Time gameTime;				//<-- starts the time
	//Time serverTime;
	WorldSystem worldSystem;	//<-- creates the world
	GameServer();				//<-- starts the simulation (thread)
	~GameServer();				//<-- stops the simulation (thread)

	//thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;
};

//renderer stuff
struct ChunkRenderInfo {
	Chunk * chunk;
	glm::mat4 offsetMatrix;
};
struct MeshContainer {
	std::string name = "empty";
	unsigned short primitiveMode = 0;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	bool uploadable = false;
};
struct ShaderProgram {
	
	bool loaded = false;
	
	GLuint programID;
	
	//uniforms
	GLuint mMatrixID;
	GLuint vpMatrixID;
	GLuint worldOffsetID;
	
	ShaderProgram();
	~ShaderProgram();
};
struct MeshFileInfo {
	std::string name;
	unsigned short primitiveMode;
	const char * path;
};
struct MeshSystem {
	//loader queue
	std::list<MeshFileInfo> loaderQueue;	//info to load meshes from files
	//upload queue
	std::list<MeshContainer> meshQueue;		//loaded meshes waiting for upload
	//loader thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;


	//loaded meshes
	unsigned int meshCount = 0;;
	std::vector<std::string> names;
	std::vector<bool> loaded;
	std::vector<unsigned short> primitiveMode;
	std::vector<unsigned int> indexCount;
	std::vector<GLuint> vertexArrayObject;
	std::vector<GLuint> vertexBuffer;
	std::vector<GLuint> uvBuffer;
	std::vector<GLuint> indexBuffer;
	std::vector<GLuint> positionBuffer;

	MeshSystem();
	~MeshSystem();
};
struct Camera {
	// hard data
	glm::u64vec3 location = glm::u64vec3(0, 0, LLONG_MAX);
	glm::vec3 subChunkLocation = glm::vec3(0);

	float cameraRotationX = 0.0f;
	float cameraRotationZ = 0.0f;

	float mouseSensitivity = 0.002f;
	int speedMultiplier = INITIAL_CAMERA_SPEED;
	glm::vec3 accelerationVector = glm::vec3(0);
	
	// soft data
	float cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, INITIAL_CAMERA_SPEED);
	glm::vec3 forwardVector = { 0.0f, 1.0f, 0.0f };
	glm::vec3 rightVector	= { 1.0f, 0.0f, 0.0f };
	glm::vec3 upVector		= { 0.0f, 0.0f, 1.0f };

	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
};
struct Renderer {

	Time renderTime;

	ShaderProgram primitiveShader; //[TODO] turn into shader system
	ShaderProgram primitiveShaderInstanced;
	ShaderProgram primitiveChunk;

	MeshSystem meshSystem;
	GameServer * gameServer;

	bool wireframeMode = false;
	bool chunkBorders = false;

	Camera * camera;

	std::vector<std::vector<ChunkRenderInfo>> chunkRenderQueues;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	Renderer() = delete;
	Renderer(GameServer & gameServer, Camera & camera);
	~Renderer();
};
struct Window {
	//settings
	std::string title = "Project ORKA";
	bool fullScreen = false;
	bool borderlessFullScreen = false;
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
	std::unique_ptr<Renderer> renderer;

	Camera camera;

	//thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;

	Window(GameServer & gameServer);
	~Window();
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

//chunk
void subdivideChunk					(Chunk & chunk);
void unsubdivideChunk				(Chunk & chunk);


void renderChunk					(ChunkRenderInfo & info, Renderer & renderer);
void renderChunkBoundingBox			(ChunkRenderInfo & info, Renderer & renderer);

//world system
void renderChunkQueue				(Renderer & renderer);
void addChunkToRenderQueue			(Chunk & chunk, Renderer & renderer);
void processChunkQueue				(Chunk & chunk, Renderer & renderer);
void updateWorld					(WorldSystem & worldSystem, Time & time);
void renderWorld					(WorldSystem & worldSystem, Renderer & renderingSystem);

//math
float randomFloat					(float low, float high);

//debug
void debugPrint						(const char * debugMessage);

//shader
void useShader						(ShaderProgram & program);
void loadShader						(ShaderProgram & shaderProgram, const char * vertexPath, const char * fragmentPath);

//mesh system
void unbindMesh						();
void loadAllMeshes					(MeshSystem & meshSystem);
void createMeshFromFile				(MeshSystem & meshSystem);
void uploadNextMeshFromQueue		(MeshSystem & meshSystem);
void bindMesh						(MeshSystem & meshSystem, int index);
void renderMesh						(MeshSystem & meshSystem, int meshIndex);
void unloadMesh						(MeshSystem & meshSystem, int meshIndex);
void addMeshToUploadQueue			(MeshSystem & meshSystem, MeshContainer & mesh);
void renderInstancedMesh			(MeshSystem & meshSystem, int meshIndex, unsigned int numberOfInstances);
void getMeshIndicesFromName			(MeshSystem & meshSystem, std::string meshName, std::vector<int> & meshIndices);

//mesh loader
void addMeshFileToLoaderQueue					(MeshSystem & meshSystem, unsigned short primitiveMode, const char * path, std::string name);

//render settings
void changeAntiAliasing				(Window & window, unsigned int antiAliasing);

//rendering system
void renderSky						(Sky & sky);
void renderFrame					(Renderer & renderingSystem, int width, int height);
void renderEntities					(EntityComponentSystem & entityComponentSystem, Renderer & renderingSystem);
void __stdcall DebugOutputCallback	(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

//input
void processKeyboardInput			(Window & window);
void captureCursor					(Window & window);
void uncaptureCursor				(Window & window);
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
void checkWindowEvents				(WindowHandler & windowHandler);
void whenGLFWThrowsError			(int error, const char* description);

//time
void resetTime						(Time & time);
void updateTime						(Time & time);

//entity component system
void spawnEntity					(EntityComponentSystem & ecs, std::string name);

//transformation component
void calculateModelMatrix			(glm::mat4 & matrix, Transformation & transformation);
void processTransformations			(TransformationSystem & transformationSystem, Time & time);
void addTransformationComponent		(TransformationSystem & transformationSystem, Entity & entity);

//camera component
void pocessCamera					(Camera & camera, Time & time);
void rotateCamera					(Camera & camera, float x, float y);

//threads
void GameServerThread				(GameServer & gameServer);
void RenderThread					(Window & window);
void MeshLoaderThread				(MeshSystem & meshSystem);

#pragma endregion

#endif // !GAME_HPP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////