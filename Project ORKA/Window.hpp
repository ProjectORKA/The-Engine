#pragma once

#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"

struct UIElement;

struct Window {

	UShort id = 0;

	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	Bool fullScreen = true;
	
	APIWindow apiWindow = nullptr;
	Area windowedModeSize = Area(1);
	Bool borderlessFullScreen = false;
	
	Renderer renderer;

	//thread
	Thread thread;

	UIElement * content = nullptr;

	void render();

	void show();
	void hide();
	void destroy();
	void setWindowed();
	void setCallbacks();
	void centerWindow();
	void restoreWindow();
	void decorateWindow();
	void updatePosition();
	void undecorateWindow();
	void destroyAPIWindow();
	void setIcon(Path path);
	void initializeGraphicsAPI();
	void setExclusiveFullscreen();
	void setPosition(IVec2 position);
	void createAPIWindow(String title);
	void create(String title, UIElement * element);
	
	Bool shouldClose();
	Bool isFullScreen();

	Area getWindowFrameSize();
	Area getWindowContentSize();
};

void windowThread(Window& window);

//window callbacks
void whenWindowCloseRequest(APIWindow apiWindow);
void whenWindowDamagedOrRefreshed(APIWindow apiWindow);
void whenMonitorChanged(APIMonitor monitor, Int event);
void whenCharIsTyped(APIWindow apiWindow, UInt character);
void whenMouseEnterWindow(APIWindow apiWindow, Int entered);
void whenWindowChangedFocus(APIWindow apiWindow, Int focused);
void whenWindowWasMaximized(APIWindow apiWindow, Int maximized);
void whenWindowWasMinimized(APIWindow apiWindow, Int minimized);
void whenWindowResized(APIWindow apiWindow, Int width, Int height);
void whenFramebufferIsResized(APIWindow apiWindow, Int width, Int height);
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis);
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods);
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition);
void whenFileDroppedOnWindow(APIWindow apiWindow, Int count, const Char** paths);
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale);
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers);