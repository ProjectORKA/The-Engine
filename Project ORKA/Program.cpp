
#include "Program.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



//function definitions
void resetKey						(Key & key) {
	key.holding = false;
	key.activated = false;
	key.toggled = false;
	key.toggleStatus = false;
}
void renderSky						(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void resetTime						(Time & time) {
	time.currentTime = time.lastTime = std::chrono::steady_clock::now();
	time.delta = std::chrono::seconds::zero();
	time.total = std::chrono::seconds::zero();
}
void updateTime						(Time & time) {
	time.lastTime = time.currentTime;
	time.currentTime = std::chrono::steady_clock::now();

	if (!time.paused) {
		time.delta = time.currentTime - time.lastTime;
		time.total += time.delta;
	}
	else {

		time.delta = std::chrono::seconds::zero();

	}
}
void loadMesh						(Mesh & mesh)
{
	debugPrint("|--- Mesh is being loaded!");
	glGenVertexArrays(1, &mesh.vertexArrayObject);
	glBindVertexArray(mesh.vertexArrayObject);

	std::vector<glm::vec3> vertecies;
	vertecies.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	vertecies.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vertecies.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	glGenBuffers(1, &mesh.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertecies.size() * sizeof(glm::vec3), vertecies.data(), GL_STATIC_DRAW);
	mesh.loaded = true;
	debugPrint("|--- Mesh was loaded!");
};
void renderMesh						(Mesh & mesh) {
	if (!mesh.loaded) loadMesh(mesh);
	glBindVertexArray(mesh.vertexArrayObject);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}
void renderWindow					(Window & window) {
	int vWidth, vHeight;
	glfwGetFramebufferSize(window.glfwWindow, &vWidth, &vHeight);
	renderFrame(*window.renderingSystem, vWidth, vHeight);
	glfwSwapBuffers(window.glfwWindow);
}
void centerWindow					(Window & window) {
	//figure out center of WORKABLE area
	int x, y, w, h;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &w, &h);
	int centerOfWorkableAreaX = x + (w / 2);
	int centerofWorkableAreaY = y + (h / 2);
	//figure out top left corner based on center (x,y reused)
	x = centerOfWorkableAreaX - (window.width / 2);
	y = centerofWorkableAreaY - (window.height / 2);
	glfwSetWindowPos(window.glfwWindow, x, y);
}
void captureCursor					(Window & window) {
	if (glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		glfwGetCursorPos(window.glfwWindow, &window.curPosX, &window.curPosY);
		glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window.glfwWindow, 0, 0);
		window.capturingCursor = true;
	}
}
void setWindowHints					(Window & window) {
	const GLFWvidmode * videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, window.antiAliasing);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//transparency
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GL_FALSE);
}
void uncaptureCursor				(Window & window) {
	if (glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window.glfwWindow, window.curPosX, window.curPosY);
		window.capturingCursor = false;
	}
}
void reloadTheWindow				(Window & window) {
	debugPrint("\nWindow is being reloaded!");
				//3. stop rendering thread
				window.keepThreadRunning = false;
				window.thread->join();
				//delete window.thread;

			//2. save current position
			int x, y;
			glfwGetWindowPos(window.glfwWindow, &x, &y);

		//1. destroy window
		GameServer * tmp = window.renderingSystem->gameServer;
		destroyGLFWWindow(window);

	//mirror

		//1. create window
		createGLFWWindow(window, tmp);

			//2. set current position
			glfwSetWindowPos(window.glfwWindow, x, y);

				//3. start rendering thread
				window.keepThreadRunning = true;
				window.thread = std::make_unique<std::thread>(WindowThread, std::ref(window), tmp);

}
void toggleFullscreen				(Window & window) {
	window.fullScreen = !window.fullScreen;
	if (window.fullScreen) {
		/*glfwGetWindowPos(window.glfwWindow, &window.winPosX, &window.winPosY);*/

		if (window.borderlessFullScreen) {
			glfwSetWindowAttrib(window.glfwWindow, GLFW_DECORATED, GLFW_FALSE);
			//glfwRestoreWindow(window.glfwWindow);
			//centerWindow(window);
			glfwMaximizeWindow(window.glfwWindow);
		}
		else {
			glfwMaximizeWindow(window.glfwWindow);
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window.glfwWindow, monitor, 0, 0, window.width = videoMode->width, window.height = videoMode->height, GLFW_DONT_CARE);
		}
	}
	else {
		glfwSetWindowMonitor(window.glfwWindow, nullptr, 0, 0, window.width, window.height, GLFW_DONT_CARE);
		glfwRestoreWindow(window.glfwWindow);
		//centerWindow(window);
		glfwSetWindowAttrib(window.glfwWindow, GLFW_DECORATED, GLFW_TRUE);
	}
}
void destroyGLFWWindow				(Window & window) {
	delete window.renderingSystem;
	glfwDestroyWindow(window.glfwWindow);
	uncaptureCursor(window);
}
void setWindowCallbacks				(Window & window) {
	//glfwSetWindowPosCallback(window.glfwWindow, whenWindowIsMoved);
	glfwSetFramebufferSizeCallback(window.glfwWindow, whenWindowIsResized);
	glfwSetScrollCallback(window.glfwWindow, whenMouseIsScrolling);
	glfwSetKeyCallback(window.glfwWindow, whenButtonIsPressed);
	glfwSetMouseButtonCallback(window.glfwWindow, whenMouseIsPressed);
	glfwSetWindowFocusCallback(window.glfwWindow, whenWindowChangedFocus);
}
void processWindowInputs			(Window & window) {
	setKey(window.input.forward, glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS);
	setKey(window.input.backward, glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS);
	setKey(window.input.right, glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS);
	setKey(window.input.left, glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS);
	setKey(window.input.down, glfwGetKey(window.glfwWindow, GLFW_KEY_Q) == GLFW_PRESS);
	setKey(window.input.up, glfwGetKey(window.glfwWindow, GLFW_KEY_E) == GLFW_PRESS);
	setKey(window.input.escape, glfwGetKey(window.glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window.glfwWindow));
	setKey(window.input.wireframe, glfwGetKey(window.glfwWindow, GLFW_KEY_F) == GLFW_PRESS);
	if (window.capturingCursor) {
		glfwGetCursorPos(window.glfwWindow, &window.deltaX, &window.deltaY);
		glfwSetCursorPos(window.glfwWindow, 0, 0);
		setAxis(window.input.yaw, window.deltaX);
		setAxis(window.input.pitch, window.deltaY);
	}
	else {
		glfwGetCursorPos(window.glfwWindow, &window.curPosX, &window.curPosY);
	}
}
void setKey							(Key & key, bool pressed) {
	if (key.holding != pressed) {
		key.toggled = true;
	}

	if (key.toggled && pressed) {
		key.activated = true;
	}
	else {
		key.activated = false;
	}

	if (key.activated) {
		key.toggleStatus = !key.toggleStatus;
	}

	key.holding = pressed;
}
void useShader						(ShaderProgram & program) {
	if (!program.loaded) {
		loadShader(program, "shaders/primitive.vert", "shaders/primitive.frag");
	}
	glUseProgram(program.programID);
}
void setAxis						(Axis & axis, double value) {
	axis.previous = axis.current;
	axis.current = value;
	axis.delta = axis.current - axis.previous;
}
void debugPrint						(const char * debugMessage) {
#ifdef DEBUG
	std::cout << debugMessage << std::endl;
#endif // DEBUG
}
void updateWindows					(WindowHandler & windowHandler) {
	glfwWaitEvents();
	for (unsigned int i = 0; i < windowHandler.windows.size(); i++) {
		if (windowHandler.windows[i]->input.escape.holding){//glfwWindowShouldClose(windowHandler.windows[i]->glfwWindow)) {
			windowHandler.windows.erase(windowHandler.windows.begin() + i);
		}
	}
}
void whenWindowChangedFocus			(GLFWwindow * window, int focused) {

	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (focused) {
		if (parentWindowClass->capturingCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}
void setIcon						(Window & window, std::string path) {
	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(window.glfwWindow, 1, images);
	stbi_image_free(images[0].pixels);
}
void whenGLFWThrowsError			(int error, const char * description)
{
	std::cout << "Error: " << description << std::endl;
	std::getchar();
	exit(EXIT_FAILURE);
}
void createGLFWWindow				(Window & window, GameServer * gameServer) {
	setWindowHints(window);

	try {
		window.glfwWindow = glfwCreateWindow(window.width, window.height, window.title.c_str(), NULL, NULL);
		if (!window.glfwWindow) {
			throw std::exception("Failed to initialize Window!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	glfwSetWindowUserPointer(window.glfwWindow, &window);

	setIcon(window, "icon.png");

	if (glfwRawMouseMotionSupported()) glfwSetInputMode(window.glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	window.capturingCursor ? captureCursor(window) : uncaptureCursor(window);

	window.renderingSystem = new RenderingSystem(gameServer);


	//glfwSetWindowPos(window.glfwWindow, window.winPosX, window.winPosY);

	if (window.fullScreen) {
		window.fullScreen = false;
		toggleFullscreen(window);
	}
	
	glfwShowWindow(window.glfwWindow);
	
	setWindowCallbacks(window);
}
void whenWindowIsResized			(GLFWwindow * window, int width, int height) {
	Window & parentWindowClass = *(static_cast<Window *>(glfwGetWindowUserPointer(window)));
	parentWindowClass.width = width;
	parentWindowClass.height = height;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		centerWindow(parentWindowClass);
	}
}
void changeAntiAliasing				(Window & window, unsigned int antiAliasing) {
	window.antiAliasing = antiAliasing;
	reloadTheWindow(window);
}
void renderChunks					(Chunk & chunk, RenderingSystem & renderingSystem) {
	renderEntities(chunk.entities, renderingSystem);
}
void renderEntity					(Entity & entity, RenderingSystem & renderingSystem) {
	if (entity.type == "triangle") {
		useShader(renderingSystem.primitiveShader);
		renderMesh(*renderingSystem.meshHandler.meshMap["triangle"]);
	}
}
void whenMouseIsScrolling			(GLFWwindow * window, double xoffset, double yoffset) {

	//get parent class

	//Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	//
	//static int mvspeed = 1;
	//mvspeed += (int)yoffset;
	//parentWindowClass->camera->cameraMovementSpeed = pow(1.1f, mvspeed);
}
void whenMouseIsPressed				(GLFWwindow * window, int button, int action, int mods) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			setKey(parentWindowClass->input.action, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		}
		else {
			captureCursor(*parentWindowClass);
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			uncaptureCursor(*parentWindowClass);
		}
	}
}
void createNewWindow				(WindowHandler & windowHandler, GameServer * gameServer) {
	std::shared_ptr<Window> tmp = std::make_shared<Window>(gameServer);
	windowHandler.windows.push_back(tmp);
}
void renderFrame					(RenderingSystem & renderingSystem, int width, int height) {
	//glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderWorld(renderingSystem.gameServer->worldSystem, renderingSystem);
	glUseProgram(0); // somehow gets rid of shader recompilation on nvidia cards
}
void renderWorld					(WorldSystem & worldSystem, RenderingSystem & renderingSystem) {
	renderSky(worldSystem.sky);
	renderChunks(worldSystem.chunk, renderingSystem);
}
void whenButtonIsPressed			(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		toggleFullscreen(*parentWindowClass);
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(parentWindowClass->glfwWindow, GLFW_TRUE);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		changeAntiAliasing(*parentWindowClass, 0);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		changeAntiAliasing(*parentWindowClass, 8);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		parentWindowClass->renderingSystem->gameServer->time.paused = !parentWindowClass->renderingSystem->gameServer->time.paused;
	}

}
void renderEntities					(std::vector<std::shared_ptr<Entity>>& entities, RenderingSystem & renderingSystem) {
	for (unsigned int i = 0; i < entities.size(); i++) {
		renderEntity(*(entities[i]), renderingSystem);
	}
}
void loadShader						(ShaderProgram & shaderProgram, const char * vertexPath, const char * fragmentPath) {
	debugPrint("Loading Shader");
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexPath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		throw std::runtime_error{ "Could not load vertex shader!" };
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentPath, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else {
		throw std::runtime_error{ "Could not load fragment shader!" };
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertexPath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragmentPath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	shaderProgram.programID = glCreateProgram();
	glAttachShader(shaderProgram.programID, VertexShaderID);
	glAttachShader(shaderProgram.programID, FragmentShaderID);
	glLinkProgram(shaderProgram.programID);

	// Check the program
	glGetProgramiv(shaderProgram.programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgram.programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgram.programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(shaderProgram.programID, VertexShaderID);
	glDetachShader(shaderProgram.programID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	shaderProgram.loaded = true;
}
void __stdcall DebugOutputCallback	(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		printf("OpenGL Debug Output message : ");

		if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

		if (type == GL_DEBUG_TYPE_ERROR_ARB)					printf("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");
		printf("Message : %s\n", message);
	}
}

//threads
void GameServerThread(GameServer & gameServer) {
	while (gameServer.keepThreadRunning) {
		updateTime(gameServer.time);
		gameServer.worldSystem.sky.skyColor = glm::vec3((1 + sin((float)gameServer.time.getTotal())) / 2.0f);
	}
}
void WindowThread(Window & window, GameServer * gameServer)
{
	glfwMakeContextCurrent(window.glfwWindow);

	glewExperimental = true;
	try {
		if (glewInit() != GLEW_OK) {
			throw std::exception("Failed to initialize GLEW!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugOutputCallback, 0);

	while (window.keepThreadRunning) {
		processWindowInputs(window);
		renderWindow(window);
	}
}

//constructor definitions
Time::Time() {
	resetTime(*this);
	debugPrint("|--Time was created!");
}
Time::~Time()
{
	debugPrint("|--Time was destroyed!");
}
Mesh::Mesh()
{
	debugPrint("|-----Mesh was created!");
}
Mesh::~Mesh()
{
	if (loaded) {
		//glDeleteBuffers(vertexBuffer, 0);
		//glDeleteVertexArrays(vertexArrayObject, 0);
	}
	loaded = false;
	debugPrint("|-----Mesh destroyed!");
}
Chunk::Chunk()
{
	std::shared_ptr<Entity> tmp = std::make_shared<Triangle>();
	entities.push_back(tmp);
	debugPrint("|--Chunk was created!");
}
Chunk::~Chunk()
{
	debugPrint("|--Chunk was destroyed!");
}
Entity::Entity()
{
	debugPrint("|---Entity was created!");
}
Entity::~Entity()
{
	debugPrint("|---Entity destroyed!");
}
Program::Program() {
	debugPrint("|Program was created!");

	debugPrint("\nCreating a Window!");
	createNewWindow(windowHandler, &gameServer);

	createNewWindow(windowHandler, &gameServer);
	
	debugPrint("\nStarted Window Management Loop!");
	while(windowHandler.windows.size() > 0){
		updateWindows(windowHandler);
	}
	debugPrint("\nNo more Windows! Program is being terminated!");
}
Program::~Program()
{
	debugPrint("|Program was destroyed!");
}
Triangle::Triangle() {
	type = "triangle";
	debugPrint("|--+Triangle (Entity) was created!");
}
double Time::getDelta() {
	return delta.count();
}
double Time::getTotal() {
	return total.count();
}
GameServer::GameServer() {
	thread = std::make_unique<std::thread>(GameServerThread, std::ref(*this));
	debugPrint("|-GameServer was created!");
}
GameServer::~GameServer() {
	keepThreadRunning = false;
	thread->join();
	debugPrint("|-GameServer was destroyed!");
}
MeshHandler::MeshHandler()
{
	meshMap["triangle"] = std::make_shared<Mesh>();
	debugPrint("|----MeshHandler was created!");
}
MeshHandler::~MeshHandler()
{
	debugPrint("|----MeshHandler was destroyed!");
}
WindowHandler::WindowHandler() {
	try {
		if (glfwInit() != GLFW_TRUE) {
			throw std::exception("Failed to initialize GLFW!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};
	glfwSetErrorCallback(whenGLFWThrowsError);
	debugPrint("|-WindowHandler was created!");
}
WindowHandler::~WindowHandler() {
	windows.clear();
	glfwTerminate();
	debugPrint("|-WindowHandler was destroyed!");
}
ShaderProgram::ShaderProgram()
{
	debugPrint("|----ShaderProgram was created!");
}
ShaderProgram::~ShaderProgram() {
	glDeleteProgram(programID);
	loaded = false;
	debugPrint("|----ShaderProgram was destroyed!");
}
Window::Window(GameServer * gameServer)
{
	createGLFWWindow(*this, gameServer);
	centerWindow(*this);

	thread = std::make_unique<std::thread>(WindowThread, std::ref(*this), gameServer);
	debugPrint("|--Window was created!");
}
Window::~Window() {
	keepThreadRunning = false;
	thread->join();

	destroyGLFWWindow(*this);
	debugPrint("|--Window was destroyed!");
}
RenderingSystem::RenderingSystem(GameServer * gameServer)
{
	this->gameServer = gameServer;
	debugPrint("|---RenderingSystem was created!");
}
RenderingSystem::~RenderingSystem()
{
	debugPrint("|---RenderingSystem was destroyed!");
}
