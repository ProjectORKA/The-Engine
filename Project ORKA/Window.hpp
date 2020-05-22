#pragma once

#include "Rectangle.hpp"
#include "GraphicsAPI.hpp"
#include "Debug.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" // needs to be below "Renderer.hpp"

struct Window {
	//settings
	String title = "Project ORKA";
	Bool fullScreen = false;
	Bool borderlessFullScreen = false;
	Bool decorated = true;
	Bool isShown = true;

	UShort antiAliasing = 4; //[TODO]

	//local variables
	Bool duplicateWindow = false;
	Bool capturingCursor = false;
	DVec2 cursorPosition;
	Vec2 windowPosition;
	Double deltaX = 0;
	Double deltaY = 0;
	APIWindow apiWindow = nullptr;

	Renderer renderer;

	//thread
	Bool keepThreadRunning = true;
	Thread thread;

	//windowposition
	void updatePosition();
	void center();
	void setPosition(Vec2 position);
	void getWorkableArea(Rect<int>& rect);
	
	//rendering
	void pushFrame();
	void renderLoop();
	void setFullscreen();
	void setWindowed();
	void setIcon(Path path);

	//input
	void processCameraMovement();
	void captureCursor();
	void uncaptureCursor();
	void processInput();
	void changeAntiAliasing(UShort antiAliasing);
	//api
	void create();
	void createAPIWindow();
	void destroyAPIWindow();
	void destroy();
	void reload();
	void initializeGraphicsAPI();
	bool shouldClose();
	//thread
	void startThread();
	void stopThread();
};

void windowThread(Window& window);

void whenWindowChangedFocus(APIWindow window, Int focused);
void whenWindowWasMinimized(APIWindow window, Int minimized);
void whenWindowIsBeingMoved(APIWindow window, int xpos, int ypos);
void whenWindowAPIThrowsError(Int error, const char* description);
void whenMouseIsMoving(APIWindow window, Double xpos, Double ypos);
void whenFramebufferIsResized(APIWindow window, Int width, Int height);
void whenMouseIsScrolling(APIWindow window, Double xoffset, Double yoffset);
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods);
void whenButtonIsPressed(APIWindow window, Int key, Int scancode, Int action, Int mods);