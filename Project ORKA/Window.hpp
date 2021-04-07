#pragma once

#include "Rectangle.hpp"
#include "GraphicsAPI.hpp"
#include "Debug.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" // needs to be below "Renderer.hpp"
#include "Settings.hpp"
#include "Layer.hpp"
#include "Threading.hpp"
#include "InputSystem.hpp"

struct Window {
	//settings
	String title = "Project ORKA";
	Bool fullScreen = false;
	Bool borderlessFullScreen = false;
	Bool decorated = true;
	Bool isShown = true;

	Int windowContentWidth = DEFAULT_WINDOW_WIDTH;
	Int windowContentHeight = DEFAULT_WINDOW_HEIGHT;

	//local variables
	Bool duplicateWindow = false;
	Bool isCapturingCursor = false;
	DVec2 cursorPosition;
	Vec2 windowPosition;
	Double deltaX = 0;
	Double deltaY = 0;
	APIWindow apiWindow = nullptr;

	InputSystem inputSystem;

	Renderer renderer;
	Layer layer = Layer(renderer, createGameInstanceLayer, renderGameInstanceLayer, nullptr);

	//thread
	Thread thread;

	void create();
	void destroy();

	//windowposition
	void updatePosition();
	void center();
	void setPosition(Vec2 position);
	void getWorkableArea(Rect<int>& rect);
	
	//rendering
	void pushFrame();
	void setWindowed();
	void setFullscreen();
	void setIcon(Path path);
	void updateFramebuffers();

	//input
	void processInput();
	void captureCursor();
	void uncaptureCursor();
	void processCameraMovement();
	void changeAntiAliasing(UShort antiAliasing);
	
	//window api
	bool shouldClose();
	void createAPIWindow();
	void destroyAPIWindow();
	void initializeGraphicsAPI();
};

void windowThread(Window& window);

void whenWindowChangedFocus(APIWindow window, Int focused);
void whenWindowWasMaximized(APIWindow window, Int maximized);
void whenWindowWasMinimized(APIWindow window, Int minimized);
void whenWindowIsBeingMoved(APIWindow window, int xpos, int ypos);
void whenWindowAPIThrowsError(Int error, const char* description);
void whenMouseIsMoving(APIWindow window, Double xpos, Double ypos);
void whenFramebufferIsResized(APIWindow window, Int width, Int height);
void whenMouseIsScrolling(APIWindow window, Double xoffset, Double yoffset);
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods);
void whenButtonIsPressed(APIWindow window, Int key, Int scancode, Int action, Int mods);