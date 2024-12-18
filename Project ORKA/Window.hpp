#pragma once

#include "Renderer.hpp"
#include "WindowAPI.hpp" // needs to be below the rendering stuff, e.g. "Renderer.hpp"
#include "UIElement.hpp"
#include "UIContainer.hpp"
#include "LifeTimeGuard.hpp"

using WindowHandle = ULL;

enum class WindowState : Byte
{
	Minimized,
	Windowed,
	Maximized,
	Fullscreen,
};

enum class WindowDecoration : Byte
{
	Decorated   = 1,
	Undecorated = 0,
};

struct Window final : LifetimeGuard
{
	Thread       thread;
	Renderer     renderer;
	MouseState   mouseState;
	Vector<Path> droppedFilePaths;
	UShort       id          = 0;
	Bool         isVisible   = true;
	Bool         decorated   = true;
	Bool         keepRunning = true;
	Bool         capturing   = false;
	Bool         profiling   = false;
	UIContainer  content;
	ApiWindow    apiWindow        = nullptr;
	Area         windowedModeSize = Area(1);
	IVec2        windowPosition   = IVec2(0);
	WindowState  windowState      = WindowState::Windowed;
	//mouse
	DVec2 mousePos        = DVec2(0);
	DVec2 mouseDelta      = DVec2(0);
	DVec2 mousePosBotLeft = DVec2(0);
	//input
	InputId    altKey         = InputId(InputType::KeyBoard, Alt);
	InputEvent reloadShaders  = InputEvent(InputType::KeyBoard, F7, true);
	InputEvent startProfiling = InputEvent(InputType::KeyBoard, F8, true);
	InputEvent stopProfiling  = InputEvent(InputType::KeyBoard, F8, false);
	InputEvent escape         = InputEvent(InputType::KeyBoard, Esc, true);
	InputEvent enter          = InputEvent(InputType::KeyBoard, Enter, true);

	void destroy();
	void setWindowed();
	void setMaximized();
	void centerWindow();
	void setMinimized();
	void setFullscreen();
	void captureCursor();
	void decorateWindow();
	void releaseCursor();
	void unDecorateWindow();
	void destroyApiWindow();
	void updateWindowState();
	void updateDecorations();
	void setCallbacks() const;
	void updatePosition() const;
	void add(UIElement& element);
	void setVisible(Bool visible);
	void setPosition(IVec2 position);
	void initializeGraphicsApi() const;
	void setIcon(const Path& path) const;
	void resize(Int width, Int height) const;
	void setTitle(const String& string) const;
	void createApiWindow(const String& title, Area size);
	void create(const String& title, Area size, Bool decorated, Bool visible, WindowState state);

	[[nodiscard]] Bool hasContent() const;
	[[nodiscard]] Bool shouldClose() const;
	[[nodiscard]] Bool isCapturing() const;
	[[nodiscard]] Bool isFullScreen() const;
	[[nodiscard]] Area getFrameSize() const;
	[[nodiscard]] Area getContentSize() const;
	[[nodiscard]] Bool isKeyPressed(Int key) const;
	[[nodiscard]] Bool pressed(InputId input) const;
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
