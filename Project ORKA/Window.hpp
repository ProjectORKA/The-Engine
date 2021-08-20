#pragma once

#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UserInterface.hpp"

struct Window {
	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	Bool fullScreen = true;
	Bool duplicateWindow = false;
	String title = "Project ORKA";
	APIWindow apiWindow = nullptr;
	Bool borderlessFullScreen = false;
	
	Renderer renderer;

	Area windowedModeSize = Area(1);

	//window contents
	Vector<UIElement*> contents;

	//thread
	Thread thread;
	void show();
	void hide();
	void create();
	void destroy();
	void setWindowed();
	void setCallbacks();
	void centerWindow();
	void restoreWindow();
	void decorateWindow();
	void updatePosition();
	void createAPIWindow();
	void undecorateWindow();
	void destroyAPIWindow();
	void setIcon(Path path);
	void initializeGraphicsAPI();
	void setExclusiveFullscreen();
	void setPosition(IVec2 position);
	
	Bool shouldClose();
	Bool isFullScreen();

	Area getWindowContentSize();
	Area getWindowFrameSize();
};

void windowThread(Window& window);

//callbacks
	//window
void whenWindowChangedFocus(APIWindow window, Int focused);
void whenWindowWasMaximized(APIWindow window, Int maximized);
void whenWindowWasMinimized(APIWindow window, Int minimized);
void whenFramebufferIsResized(APIWindow window, Int width, Int height);
	//mouse
void whenMouseIsMoving(APIWindow window, Double xpos, Double ypos);
void whenMouseIsScrolling(APIWindow window, Double xoffset, Double yoffset);
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods);
	//keyboard
void whenButtonIsPressed(APIWindow window, Int key, Int scancode, Int action, Int mods);