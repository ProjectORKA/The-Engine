
#pragma once

#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"

//#define DEBUG_ID_BUFFER

enum class WindowState {
	minimized,
	windowed,
	maximized,
	fullscreen
};

enum WindowDecoration {
	decorated = 1,
	undecorated = 0,
};

struct Window {

	Renderer renderer;

	APIWindow apiWindow = nullptr;


	UShort id = 0;
	Thread thread;
	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	Bool profiling = false;
	Bool keeprunning = true;
	UIElement* content = nullptr;
	Area windowedModeSize = Area(1);
	Vector<Path> droppedFilePaths = {};
	WindowState windowState = WindowState::windowed;

	//mouse
	MouseState mouseState;
	Bool capturing = false;
	MouseMovement mousePos = MouseMovement(0);
	MouseMovement mouseDelta = MouseMovement(0);
	MouseMovement mousePosBotLeft = MouseMovement(0);

	//input
	InputID altKey = InputID(InputType::KeyBoard, ALT);
	InputEvent escape = InputEvent(InputType::KeyBoard, ESC, 1);
	InputEvent enter = InputEvent(InputType::KeyBoard, ENTER, 1);

	InputEvent startProfiling = InputEvent(InputType::KeyBoard, F8, 1);
	InputEvent stopProfiling = InputEvent(InputType::KeyBoard, F8, 0);

	void destroy();
	void setWindowed();
	void setMaximized();
	void centerWindow();
	void setCallbacks();
	void setMinimized();
	void setFullscreen();
	void captureCursor();
	void decorateWindow();
	void updatePosition();
	void uncaptureCursor();
	void undecorateWindow();
	void destroyAPIWindow();
	void setIcon(Path path);
	void updateWindowState();
	void updateDecorations();
	void initializeGraphicsAPI();
	void setPosition(IVec2 position);
	void resize(Int width, Int height);
	void createAPIWindow(String title, Area size);
	void create(String title, Area size, Bool decorated, WindowState state, Engine& engine);

	Bool shouldClose();
	Bool isCapturing();
	Bool isFullScreen();
	Bool isKeyPressed(Int key);
	Bool pressed(InputID input);

	Area getFrameSize();
	Area getContentSize();

	Window& insert(UIElement& element);
};

void windowThread(Engine& engine, Window& window);

//window callbacks
void whenWindowCloseRequest(APIWindow apiWindow);
void whenWindowWasMaximized(APIWindow apiWindow, Int maximized);
void whenWindowWasMinimized(APIWindow apiWindow, Int minimized);
void whenMouseIsMoving(APIWindow apiWindow, Double mouseX, Double mouseY);
void whenFramebufferIsResized(APIWindow apiWindow, Int width, Int height);
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis);
void whenFilesDroppedOnWindow(APIWindow apiWindow, Int count, const Char** paths);
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale);
void whenMouseIsPressed(APIWindow apiWindow, Int mouseButton, Int action, Int modifiers);
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers);