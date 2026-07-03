#pragma once

// Windows are handled by GLFW
// They run in their own thread and are managed by the global UserInterface
// Since they are part of the UI, can have content and resize it, they technically are a UIElement so they inherit from that.
// The window then recursively does the layouting calculations before rendering

#include "Renderer.hpp"
#include "WindowAPI.hpp" // needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"
#include "LifeTimeGuard.hpp"

enum class WindowState : Byte
{
	Minimized,
	Windowed,
	Maximized,
	Fullscreen,
};

enum class WindowDecoration : Byte
{
	Decorated = 1,
	Undecorated = 0,
};

struct Window final : UIElement
{
	Thread       thread;
	Renderer     renderer;
	MouseState   mouseState;
	Vector<Path> droppedFilePaths;
	UShort       id = 0;

	ApiWindow   apiWindow = nullptr;
	IVec2       storedWindowedModeSize = IVec2(1);
	IVec2       windowPosition = IVec2(0);
	WindowState windowState = WindowState::Windowed;
	//mouse
	DVec2 mousePos = DVec2(0);
	DVec2 mouseDelta = DVec2(0);
	DVec2 mousePosBotLeft = DVec2(0);
	//input
	UIElement* hoveredElement = nullptr;
	UIElement* mouseCaptureOwner = nullptr;
	InputId    altKey = InputId(InputType::KeyBoard, Alt);
	InputEvent reloadShaders = InputEvent(InputType::KeyBoard, F7, true);
	InputEvent startProfiling = InputEvent(InputType::KeyBoard, F8, true);
	InputEvent stopProfiling = InputEvent(InputType::KeyBoard, F8, false);
	InputEvent openGLLogging = InputEvent(InputType::KeyBoard, F9, true);
	InputEvent escape = InputEvent(InputType::KeyBoard, Esc, true);
	InputEvent enter = InputEvent(InputType::KeyBoard, Enter, true);

	Bool isVisible = true;
	Bool decorated = true;
	Bool profiling = false;
	Bool keepRunning = true;
	Bool apiWindowValid = false;
	Bool openGLLoggingEnabled = false;
	Color backgroundColor = Color(0.0, 0.0, 0.0, 1.0);

	void destroy();
	void setWindowed();
	void setMaximized();
	void centerWindow();
	void setMinimized();
	void setFullscreen();
	void decorateWindow();
	void unDecorateWindow();
	void updateWindowState();
	void updateDecorations();
	void setCallbacks() const;
	void updatePosition() const;
	void setVisible(Bool visible);
	void setPosition(IVec2 position);
	void initializeGraphicsApi() const;
	void setIcon(const Path& path) const;
	void resize(Int width, Int height) const;
	void setTitle(const String& string) const;
	void createApiWindow(const String& title, IVec2 size, ApiWindow sharedContextWindow = nullptr);
	Bool create(const String& title, IVec2 size, Bool decorated, Bool visible, WindowState state, ApiWindow sharedContextWindow = nullptr);

	[[nodiscard]] Bool  hasContent() const;
	[[nodiscard]] Bool  shouldClose() const;
	[[nodiscard]] Bool  isCapturing() const;
	[[nodiscard]] Bool  isFullScreen() const;
	[[nodiscard]] IVec2 getFrameSize() const;
	[[nodiscard]] IVec2 getContentSize() const;
	[[nodiscard]] Color getBackgroundColor() const;
	[[nodiscard]] Bool  isKeyPressed(Int key) const;
	[[nodiscard]] Bool  pressed(InputId input) const;
	UIElement* pickElementAt(const DVec2& point);
	Bool                routeInput(InputEvent& input);
};

void windowThread(Window& window);

// window callbacks
void whenWindowCloseRequest(ApiWindow apiWindow);
void whenMouseIsMoving(ApiWindow apiWindow, Double mouseX, Double mouseY);
void whenFramebufferIsResized(ApiWindow apiWindow, Int width, Int height);
void whenMouseIsScrolling(ApiWindow apiWindow, Double xAxis, Double yAxis);
void whenFilesDroppedOnWindow(ApiWindow apiWindow, Int count, const Char** paths);
void whenMouseIsPressed(ApiWindow apiWindow, Int mouseButton, Int action, Int modifiers);
void whenButtonIsPressed(ApiWindow apiWindow, Int key, Int scanCode, Int action, Int modifiers);
