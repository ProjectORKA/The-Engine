#pragma once

#include "Basics.hpp"
#include "TiledMath.hpp"
#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"


struct Window {

	enum WindowState {
		minimized,
		windowed,
		maximized,
		fullscreen
	};

	UShort id = 0;

	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	WindowState windowState = windowed;
	
	APIWindow apiWindow = nullptr;
	Area windowedModeSize = Area(1);
	
	Renderer renderer;

	//thread
	Thread thread;

	UIElement * content = nullptr;

	//windowstate
	void render();
	void destroy();
	void setWindowed();
	void setMaximized();
	void centerWindow();
	void setCallbacks();
	void setMinimized();
	void setFullscreen();
	void decorateWindow();
	void updatePosition();
	void undecorateWindow();
	void destroyAPIWindow();
	void setIcon(Path path);
	void updateWindowState();
	void updateDecorations();
	void initializeGraphicsAPI();
	void setPosition(IVec2 position);
	void resize(Int width, Int height);
	void createAPIWindow(String title, Area size);
	void create(String title, UIElement* element, Area size, Bool decorated, WindowState state);
	
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
void whenFramebufferIsResized(APIWindow apiWindow, Int width, Int height);
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis);
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods);
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition);
void whenFilesDroppedOnWindow(APIWindow apiWindow, Int count, const Char** paths);
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale);
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers);