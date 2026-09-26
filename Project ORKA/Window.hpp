#pragma once

// Windows are handled by GLFW
// They run in their own thread and are managed by the global UserInterface
// Since they are part of the UI, can have content and resize it, they technically are a UIElement so they inherit from that.
// The window then recursively does the layouting calculations before rendering

// GLEW must be included before GLFW/OpenGL headers (see WindowAPI.hpp).
namespace External
{
#include <GL/glew.h>
}

#include "WindowAPI.hpp"
#include "UIElement.hpp"
#include "LifeTimeGuard.hpp"

struct Renderer;

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

struct Window;

// Transitional read-only view for code that still consumes the old member
// syntax. It resolves through Window's atomic snapshot rather than storing
// shared mutable coordinates.
struct WindowMousePositionBottomLeft
{
	const Window* window = nullptr;
	operator DVec2() const;
	operator Vec2() const;
};

struct Window final : UIElement
{
	Window();
	~Window();

	Window(const Window&)            = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&)                 = delete;
	Window& operator=(Window&&)      = delete;

private:
	UniquePointer<Renderer> rendererOwner_;

public:
	Renderer& renderer;

	Thread       thread;
	MouseState   mouseState;
	Array<Atomic<Bool>, 512> injectedKeyState{};
	Vector<Path> droppedFilePaths;
	UShort       id = 0;
	String       title;
	Atomic<U64>  agentFrameIndex = 0;

	ApiWindow   apiWindow = nullptr;
	IVec2       storedWindowedModeSize = IVec2(1);
	IVec2       windowPosition = IVec2(0);
	WindowState windowState = WindowState::Windowed;
	//mouse
	// GLFW publishes the latest logical position as one coherent, lock-free
	// snapshot. The window thread samples it once before updating and rendering.
	Atomic<U64> publishedMousePosition = 0;
	DVec2 frameMousePosition = DVec2(0);
	DVec2 frameMousePositionBotLeft = DVec2(0);
	DVec2 mouseDelta = DVec2(0);
	WindowMousePositionBottomLeft mousePosBotLeft{this};
	// Retained UI cursor scrub requests GLFW changes on the poll-events thread.
	mutable Mutex uiCursorRequestMutex;
	DVec2 uiCursorLockAnchor = DVec2(0);
	std::atomic<Bool> uiCursorCaptureRequested{ false };
	std::atomic<Bool> uiCursorReleaseRequested{ false };
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
	// Modern retained UI input seams. GLFW callbacks forward here without
	// replacing any existing legacy callback path.
	Function<void(U32)> unicodeInputCallback;
	Function<void(Int, Int, Int, Int)> keyInputCallback;
	Function<void(Bool)> focusInputCallback;
	Function<void()> compositionStartCallback;
	Function<void(const String&, SizeT, SizeT)> compositionUpdateCallback;
	Function<void(const String&)> compositionCommitCallback;
	Function<void()> compositionCancelCallback;

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
	void setTitle(const String& string);
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
	void publishMousePosition(DVec2 logicalPosition);
	// Resets frame delta after cursor mode changes or synthetic repositioning.
	void alignFrameMouseSample(DVec2 logicalPosition);
	void sampleMousePositionForFrame();
	[[nodiscard]] DVec2 mousePosition() const;
	[[nodiscard]] DVec2 mousePositionBotLeft() const;
	[[nodiscard]] const DVec2& frameMousePositionTopLeft() const { return frameMousePosition; }
	[[nodiscard]] const DVec2& frameMousePositionBottomLeft() const { return frameMousePositionBotLeft; }
	UIElement* pickElementAt(const DVec2& point);
	Bool                routeInput(InputEvent& input);
	void injectPointerMove(DVec2 logicalPosition);
	void injectMouseButton(Int button, Bool down);
	void injectScroll(Double x, Double y);
	void injectKey(Int key, Bool down, Int modifiers = 0, Bool repeat = false);
	void injectText(const String& utf8) const;
	void forwardCompositionStart() const;
	void forwardCompositionUpdate(const String& utf8, SizeT selectionStart, SizeT selectionEnd) const;
	void forwardCompositionCommit(const String& utf8) const;
	void forwardCompositionCancel() const;
	void requestUiCursorCapture(DVec2 anchor);
	void requestUiCursorRelease();
	void applyUiCursorRequests();
	[[nodiscard]] DVec2 scrubCursorAnchor() const;
};

inline WindowMousePositionBottomLeft::operator DVec2() const
{
	return window ? window->mousePositionBotLeft() : DVec2(0);
}

inline WindowMousePositionBottomLeft::operator Vec2() const
{
	return Vec2(static_cast<DVec2>(*this));
}

void windowThread(Window& window);

// window callbacks
void whenWindowCloseRequest(ApiWindow apiWindow);
void whenMouseIsMoving(ApiWindow apiWindow, Double mouseX, Double mouseY);
void whenFramebufferIsResized(ApiWindow apiWindow, Int width, Int height);
void whenMouseIsScrolling(ApiWindow apiWindow, Double xAxis, Double yAxis);
void whenFilesDroppedOnWindow(ApiWindow apiWindow, Int count, const Char** paths);
void whenMouseIsPressed(ApiWindow apiWindow, Int mouseButton, Int action, Int modifiers);
void whenButtonIsPressed(ApiWindow apiWindow, Int key, Int scanCode, Int action, Int modifiers);
void whenUnicodeCharacterIsInput(ApiWindow apiWindow, UInt codepoint);
void whenWindowFocusChanges(ApiWindow apiWindow, Int focused);
