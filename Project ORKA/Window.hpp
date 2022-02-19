#pragma once

#include "Basics.hpp"
#include "TiledMath.hpp"
#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"
#include "ECS.hpp"

enum class WindowState {
	minimized,
	windowed,
	maximized,
	fullscreen
};

struct Window {

	UShort id = 0;

	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	WindowState windowState = WindowState::windowed;
	
	APIWindow apiWindow = nullptr;
	Area windowedModeSize = Area(1);
	
	Renderer renderer;

	ECS ecs;

	//thread
	Thread thread;

	UIElement * content = nullptr;

	//windowstate
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
	Window& insert(UIElement& element);
	void createAPIWindow(String title, Area size);
	void create(String title, Area size, Bool decorated, WindowState state);

	Bool shouldClose();
	Bool isCapturing();
	Bool isFullScreen();
	Bool isKeyPressed(Key key);

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
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition);
void whenFilesDroppedOnWindow(APIWindow apiWindow, Int count, const Char** paths);
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale);
void whenMouseIsPressed(APIWindow apiWindow, Int mouseButton, Int action, Int mods);
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers);