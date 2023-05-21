#pragma once

#include "Renderer.hpp"
#include "WindowAPI.hpp" //needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"
#include "LifeTimeGuard.hpp"

//#define DEBUG_Id_BUFFER

enum class WindowState : Byte
{
	Minimized,
	Windowed,
	Maximized,
	Fullscreen
};

enum class WindowDecoration : Byte
{
	Decorated = 1,
	Undecorated = 0,
};

struct Window : LifetimeGuard
{
	Renderer renderer;

	APIWindow apiWindow = nullptr;

	UShort id = 0;
	Thread thread;
	Bool isShown = true;
	IVec2 windowPosition;
	Bool decorated = true;
	Bool profiling = false;
	Bool keepRunning = true;
	UiElement* content = nullptr;
	Area windowedModeSize = Area(1);
	Vector<Path> droppedFilePaths = {};
	WindowState windowState = WindowState::Windowed;

	//mouse
	MouseState mouseState;
	Bool capturing = false;
	MouseMovement mousePos = MouseMovement(0);
	MouseMovement mouseDelta = MouseMovement(0);
	MouseMovement mousePosBotLeft = MouseMovement(0);

	//input
	InputId altKey = InputId(InputType::KeyBoard, ALT);
	InputEvent escape = InputEvent(InputType::KeyBoard, ESC, true);
	InputEvent enter = InputEvent(InputType::KeyBoard, ENTER, true);

	InputEvent startProfiling = InputEvent(InputType::KeyBoard, F8, true);
	InputEvent stopProfiling = InputEvent(InputType::KeyBoard, F8, false);

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
	void unCaptureCursor();
	void unDecorateWindow();
	void destroyApiWindow();
	void setIcon(const Path& path);
	void updateWindowState();
	void updateDecorations();
	void initializeGraphicsApi();
	void setPosition(IVec2 position);
	void resize(Int width, Int height);
	void createApiWindow(const String& title, Area size);
	void create(const String& title, Area size, Bool decorated, WindowState state, ResourceManager& resourceManager);

	Bool shouldClose() const;
	Bool isCapturing() const;
	Bool isFullScreen() const;
	Bool isKeyPressed(Int key) const;
	Bool pressed(InputId input) const;

	Area getFrameSize() const;
	Area getContentSize() const;

	Window& insert(UiElement& element);
};

void windowThread(ResourceManager& resourceManager, Window& window);

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
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scanCode, Int action, Int modifiers);