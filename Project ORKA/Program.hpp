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
#include <filesystem>
#include <shared_mutex>

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

#include "Time.hpp"
#include "Math.hpp"
#include "PerlinNoise.hpp"

#pragma endregion

#pragma region notes

#pragma region todo
//todo:		update time only once for every game tick (gamesimulation) and every frame (renderer)
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

#define DEBUG								//if defined enables debug messages
#define CHUNK_LEVEL_MAX 62					//the highest detail level the world system can go (max 62 62 = 63 + 1 extra precision for half way point)
#define INITIAL_CAMERA_SPEED 1				//1 as fast as a human 300 as fast as light
#define CAMERA_SPEED_MULTIPLIER 1.1f		//controls the increase in speed when scrolling mouse
#define CHUNK_DISTANCE_MULTIPLIER 1.003f	//controls the increase in distance for the world system

#pragma endregion

#pragma region data
//DATA (sorted in inverted hierarchical order)

// generic data (these can have member functions as they are very small closed systems used multiple times throughout the program)
using Index = unsigned int;
enum ComponentType {
	TransformationComponentType,
	RenderComponentType,
	ModelMatrixComponentType,
	ComponentType_COUNT
};
enum ShaderType{
	VertexShaderType = GL_VERTEX_SHADER,
	TessellationControlShaderType = GL_TESS_CONTROL_SHADER,
	TessellationEvaluationShaderType = GL_TESS_EVALUATION_SHADER,
	GeometryShaderType = GL_GEOMETRY_SHADER,
	FragmentShaderType = GL_FRAGMENT_SHADER,
	ComputeShaderType = GL_COMPUTE_SHADER
};
struct Transformation {
	glm::mat4 calculateModelMatrix();

	void addRotation(glm::vec3 rotation);
	void setRotation(glm::vec3 rotation);
	void addTranslation(glm::vec3 location);
	void setTranslation(glm::vec3 location);
	void setScale(glm::vec3 scale);
	void setScale(float scale);
	glm::vec3 getLocation();
private:
	glm::vec3 location	= glm::vec3(0);
	glm::vec3 rotation	= glm::vec3(0);
	glm::vec3 scale		= glm::vec3(1);

	glm::mat4 translationMatrix = glm::mat4(1);
	glm::mat4 rotationMatrix	= glm::mat4(1);
	glm::mat4 scaleMatrix		= glm::mat4(1);
};
struct Entity {
	unsigned int indices[ComponentType_COUNT];
};
struct Chunk{
	std::vector<Entity> entities;							//<-- objects inside the chunk
	glm::u64vec3 location = glm::u64vec3(0);				//<-- location of the chunk
	unsigned short level = 0;								//<-- detail level of the chunk 0 = big 62 = small
	bool subdivided = false;								//<-- bool to check if chunk has sub nodes
	std::unique_ptr<Chunk> tfr = nullptr;					//<-- pointers to sub nodes
	std::unique_ptr<Chunk> tfl = nullptr;					//    "tfr" means "top front right" = +Z +Y +X from the middle of the chunk
	std::unique_ptr<Chunk> tbr = nullptr;
	std::unique_ptr<Chunk> tbl = nullptr;
	std::unique_ptr<Chunk> bfr = nullptr;
	std::unique_ptr<Chunk> bfl = nullptr;
	std::unique_ptr<Chunk> bbr = nullptr;
	std::unique_ptr<Chunk> bbl = nullptr;					//    bottom back left = -X -Y -Z
	
	std::mutex mutex;										//<-- locks access if in use
	std::chrono::steady_clock::time_point expirationDate;	//<-- if it runs out simulation will delete the chunk
};
struct Sky {
	glm::vec3 skyColor = glm::vec3(0.0f);
};
struct RenderComponentSystem{
	std::shared_mutex mutex;
	std::vector<const char*> names;
	std::vector<glm::mat4> modelMatrices;
};
struct ModelMatrixSystem{
	std::mutex mutex;
	std::vector<glm::mat4> modelMatrices;
};
struct TransformationSystem{
	std::shared_mutex mutex;
	std::vector<Transformation> transformations;
};
struct WorldSystem { //[TODO] let worldsystem point to batches of entities stored inside an  entity system instead of storing them inside the chunk
	Sky sky;			//<-- gives renderer a color to fill the background [TODO] add mesh support
	Chunk root;			//<-- root chunk for octree-based world system
};
struct GameSimulation {
	Time gameTime;													//<-- stores total and delta times

	WorldSystem worldSystem;										//<-- advanced container for objects
	TransformationSystem transformationSystem;						//<-- stores location rotation and scale of an object
	RenderComponentSystem renderComponentSystem;					//<-- gives renderer a key that can be associated with renderable content
	ModelMatrixSystem modelMatrixSystem;							//<-- stores model matrices to be used by the renderers

	//thread
	bool keepThreadRunning = true;									//<-- stops thread if set to false
	std::unique_ptr<std::thread> thread;							//<-- pointer to the thread
};
struct ChunkRenderInfo {
	Chunk * chunk;
	glm::vec3 chunkOffsetVector;
};
struct MeshFileInfo {
	std::string name;
	unsigned short primitiveMode;
	std::string path;
};
struct CPUMesh {
	std::string name = "empty";
	unsigned short primitiveMode = 0;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	bool readyToUploadToGPU = false;
};
struct GPUMesh {
	std::string name = "empty";
	bool readyToRender = false;
	unsigned short primitiveMode = GL_TRIANGLES;
	unsigned int indexCount = 0;
	GLuint vertexArrayObject = 0;
	GLuint vertexBuffer = 0;
	GLuint uvBuffer = 0;
	GLuint indexBuffer = 0;
	GLuint positionBuffer = 0;
};
struct MeshSystem {
	std::list<MeshFileInfo> meshLoadQueue;	//info to load meshes from files
	std::list<CPUMesh> cpuMeshes;			//loaded meshes waiting for upload
	std::vector<GPUMesh> gpuMeshes;			//uploaded meshes ready to be drawn

	std::map<Index, Index> renderComponentIndexToGPUMeshIndex;

	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;
};
struct Camera {
	// hard data
	glm::u64vec3 location = glm::u64vec3(0, 0, 1000000000);
	glm::vec3 subChunkLocation = glm::vec3(0);

	float cameraRotationX = 0.0f;
	float cameraRotationZ = 0.0f;
	float fieldOfView = 85.0;
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
struct Shader {
	GLuint shaderID;
};
struct ShaderProgram {
	bool loaded = false;
	GLuint programID = 0;

	//uniform IDs
	GLuint timeID = 0;
	GLuint chunkOffsetVectorID = 0;
	GLuint mMatrixID = 0;
	GLuint vpMatrixID = 0;
	GLuint worldOffsetID = 0;
};
struct Renderer {
	Time renderTime;

	Shader vertexShader;
	Shader fragmentShader;
	ShaderProgram primitiveChunkShaderProgram;

	MeshSystem meshSystem;
	GameSimulation* gameSimulation;

	//settings
	float minimumFrameRate = 100;
	float maximunFrameRate = 200;
	bool wireframeMode = false;
	bool chunkBorders = false;
	bool adjustRenderVariables = true;
	unsigned int worldSystemRenderDistance = 100;

	Camera camera;

	std::vector<std::vector<ChunkRenderInfo>> chunkRenderQueues;

	glm::mat4 projectionMatrix = glm::mat4(1);
	glm::mat4 viewMatrix = glm::mat4(1);
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
	double curPosX = 0;
	double curPosY = 0;
	double deltaX = 0;
	double deltaY = 0;
	GLFWwindow* glfwWindow = nullptr;

	Renderer renderer;

	//thread
	bool keepThreadRunning = true;
	std::unique_ptr<std::thread> thread;

	Window() {}
	~Window() {}
	Window(const Window& other) : Window() {}
	Window(Window&& other) noexcept {}
	Window& operator=(const Window& other) { return *this = Window(other); }
	Window& operator=(Window&& other) noexcept { return *this; }
};
struct Program {
	GameSimulation gameSimulation;	//<-- already starts simulating the gameWorld
	std::list<Window> windows;
};
#pragma endregion

#pragma region functions

//program
void runProgram							();
void startGLFW							();
void stopGLFW							(Program& program);
void startGameSimulation				(GameSimulation & gameSimulation);
void stopGameSimulation					(GameSimulation & gameSimulation);
//math
float randomFloat						(float low, float high);
//debug
void debugPrint							(const char * debugMessage);
void renderGizmo						(ChunkRenderInfo& info, Renderer& renderer);
//shader
void useShaderProgram					(ShaderProgram & program);
void unloadShaderProgram				(ShaderProgram & shaderProgram);
void loadShaderProgram					(ShaderProgram & shaderProgram, Shader & vertexShader, Shader & fragmentShader);
void loadShader						(Shader& shader, GLuint shaderType, const char* shaderPath);
void unloadShader						(Shader& shader);
//mesh system
void unbindMesh							();
void unloadMesh							(GPUMesh & gpuMesh);
void loadAllMeshes						(MeshSystem & meshSystem);
void startMeshLoaderThread				(MeshSystem & meshSystem);
void stopMeshLoaderThread				(MeshSystem & meshSystem);
void uploadNextMeshFromQueue			(MeshSystem & meshSystem);
void generateCPUMeshFromLoaderQueue		(MeshSystem & meshSystem);
void bindMesh							(MeshSystem & meshSystem, Index meshIndex);
void addMeshToUploadQueue				(MeshSystem & meshSystem, CPUMesh & mesh);
void addMeshFileToLoaderQueue			(MeshSystem & meshSystem, std::filesystem::path path);
void getMeshIndexFromName				(MeshSystem & meshSystem, const char* name, Index & meshIndex);
void getMeshIndexFromRenderComponent	(Renderer & renderer, Index& inputRenderComponentID, Index& outputGPUMeshIndex);
//render settings
void changeAntiAliasing					(Window & window, unsigned short antiAliasing);
//renderer
void renderWindow						(Window & window);
void renderChunkQueue					(Renderer & renderer);
void dynamicallyAdjustValue				(unsigned int & value, Renderer& renderer);
void renderMesh							(MeshSystem & meshSystem, Index meshIndex);
void renderFrame						(Renderer & renderer, int width, int height);
void renderEntities						(ChunkRenderInfo & info, Renderer& renderer);
void renderChunk						(ChunkRenderInfo & info, Renderer & renderer);
void renderChunkBoundingBox				(ChunkRenderInfo & info, Renderer & renderer);
void initializeRenderer					(Renderer & renderer, GameSimulation& gameSimulation);
void renderInstancedMesh				(MeshSystem & meshSystem, Index meshIndex, unsigned int numberOfInstances);
void __stdcall DebugOutputCallback		(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
//input
void processKeyboardInput				(Window & window);
void captureCursor						(Window & window);
void uncaptureCursor					(Window & window);
void whenMouseIsMoving					(GLFWwindow* window, double xpos, double ypos);
void whenMouseIsScrolling				(GLFWwindow* window, double xoffset, double yoffset);
void whenMouseIsPressed					(GLFWwindow* window, int button, int action, int mods);
void whenButtonIsPressed				(GLFWwindow* window, int key, int scancode, int action, int mods);
//window
void destroyWindow						(Window & window);
void centerWindow						(Window & window);
void reloadTheWindow					(Window & window);
void toggleFullscreen					(Window & window);
void destroyGLFWWindow					(Window & window);
void setWindowCallbacks					(Window & window);
void checkWindowEvents					(Program & program);
void whenWindowChangedFocus				(GLFWwindow* window, int focused);
void setIcon							(Window & window, std::string path);
void whenGLFWThrowsError				(int error, const char* description);
void whenWindowIsResized				(GLFWwindow* window, int width, int height);
void createWindow						(Window & window, GameSimulation& gameSimulation);
void createGLFWWindow					(Window & window, GameSimulation & gameSimulation);
void createNewWindow					(std::list<Window>& windows, GameSimulation& gameSimulation);
//transformation system
void processTransformationSystem		(GameSimulation & gameSimulation);
//model matrix system
void processModelMatrixSystem			(GameSimulation & gameSimulation);
//world system
void renderSky							(Sky & sky);
void chunkIsInUse						(Chunk & chunk);
void processChunkQueue					(Chunk & chunk, Renderer & renderer);
void unsubdivideChunk					(Chunk & chunk, GameSimulation & gameSimulation);
void subdivideChunk						(Chunk & chunk, GameSimulation & gameSimulation);
void processWorldSystem					(Chunk & chunk, GameSimulation & gameSimulation);
void generateEntities					(Chunk & chunk, GameSimulation & gameSimulation);
void renderWorld						(WorldSystem & worldSystem, Renderer & renderer);
void createChildChunk					(Chunk & child, Chunk & parent, GameSimulation& gameSimulation, bool x, bool y, bool z);
//add components
void addTransformation					(Entity & entity, GameSimulation & gameSimulation);
void addModelMatrix						(Entity & entity, ModelMatrixSystem & modelMatrixSystem);
void addRenderingComponent				(Entity & entity, GameSimulation & gameSimulation, const char* name);
void addTransformation					(Entity & entity, GameSimulation & gameSimulation, Transformation & transformation);
void addModelMatrix						(Entity & entity, ModelMatrixSystem & modelMatrixSystem, Transformation& transformation);
//camera
void pocessCamera						(Camera & camera, Time & time);
void rotateCamera						(Camera & camera, float x, float y);
//threads
void GameSimulationThread				(GameSimulation & gameSimulation);
void RenderThread						(Window & window);
void MeshLoaderThread					(MeshSystem & meshSystem);

#pragma endregion


#endif // !GAME_HPP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////