#pragma once

#include "Basics.hpp"
#include "TiledMath.hpp"
#include "Threading.hpp"
#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"
#include "ECS.hpp"
#include "Input.hpp"

enum class WindowState {
	minimized,
	windowed,
	maximized,
	fullscreen
};

struct Window {
	UShort id = 0;
	Thread thread;
	Renderer renderer;
	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	APIWindow apiWindow = nullptr;
	UIElement * content = nullptr;
	Area windowedModeSize = Area(1);
	Vector<Path> droppedFilePaths = {};
	WindowState windowState = WindowState::windowed;
	
	//mouse
	MouseState mouseState;

	//new shit
	Bool capturing = false;
	MouseMovement mousePos = MouseMovement(0);
	MouseMovement mouseDelta = MouseMovement(0);
	MouseMovement mousePosBotLeft = MouseMovement(0);

	//generic inputs
	InputID altKey = InputID(InputType::KeyBoard, ALT);
	InputEvent escape = InputEvent(InputType::KeyBoard, ESC, 1);
	InputEvent enter = InputEvent(InputType::KeyBoard, ENTER, 1);

	//windowstate
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
	Window& insert(UIElement& element);
	void createAPIWindow(String title, Area size);
	void create(String title, Area size, Bool decorated, WindowState state);

	Bool shouldClose();
	Bool isCapturing();
	Bool isFullScreen();
	Bool isKeyPressed(Int key);
	Bool pressed(InputID input);

	Area getFrameSize();
	Area getContentSize();
};

void windowThread(Window& window);

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