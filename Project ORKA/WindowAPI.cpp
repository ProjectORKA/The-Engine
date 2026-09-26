#include "WindowAPI.hpp"
#include "Debug.hpp"
#include "Settings.hpp"
#include <unordered_map>

void apiWindowRestore(const ApiWindow apiWindow)
{
	glfwRestoreWindow(apiWindow);
}

void apiMaximizeWindow(const ApiWindow apiWindow)
{
	glfwMaximizeWindow(apiWindow);
}

void apiWindowDecorate(const ApiWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, true);
}

void apiMinimizeWindow(const ApiWindow apiWindow)
{
	glfwIconifyWindow(apiWindow);
}

void apiWindowUndecorate(const ApiWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, false);
}

void apiWindowSwapBuffers(const ApiWindow apiWindow)
{
	glfwSwapBuffers(apiWindow);
}

Bool apiWindowIsCapturing(const ApiWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Bool apiWindowIsDecorated(const ApiWindow apiWindow)
{
	return glfwGetWindowAttrib(apiWindow, GLFW_DECORATED);
}

Bool apiWindowIsFullscreen(const ApiWindow apiWindow)
{
	return glfwGetWindowMonitor(apiWindow) != nullptr;
}

void apiWindowEnableCursor(const ApiWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void apiWindowHideCursor(const ApiWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void apiWindowDisableCursor(const ApiWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Bool apiGetWindowShouldClose(const ApiWindow apiWindow)
{
	return glfwWindowShouldClose(apiWindow);
}

Bool apiWindowCursorIsCaptured(const ApiWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Bool apiWindowCursorIsHidden(const ApiWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN;
}

Int apiWindowGetFramebufferWidth(const ApiWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return width;
}

DVec2 apiWindowGetCursorPosition(const ApiWindow apiWindow)
{
	DVec2 cursorPosition;
	glfwGetCursorPos(apiWindow, &cursorPosition.x, &cursorPosition.y);
	return cursorPosition;
}

IVec2 apiWindowGetFramebufferSize(const ApiWindow apiWindow)
{
	IVec2 size;
	glfwGetFramebufferSize(apiWindow, &size.x, &size.y);
	return size;
}

IVec2 apiWindowGetWindowFrameSize(const ApiWindow apiWindow)
{
	IVec2 area;
	glfwGetWindowSize(apiWindow, &area.x, &area.y);
	return area;
}

Int apiWindowGetFramebufferHeight(const ApiWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return height;
}

TiledRectangle apiWindowGetWorkArea(ApiWindow apiWindow)
{
	TiledRectangle rect;
	External::glfwGetMonitorWorkarea(External::glfwGetPrimaryMonitor(), &rect.position.x, &rect.position.y, &rect.size.x, &rect.size.y);
	return rect;
}

void apiWindowSetExclusiveFullscreen(const ApiWindow apiWindow)
{
	glfwSetWindowMonitor(apiWindow, External::glfwGetPrimaryMonitor(), 0, 0, External::glfwGetVideoMode(External::glfwGetPrimaryMonitor())->width, External::glfwGetVideoMode(External::glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
}

void whenWindowApiThrowsError(Int error, const Char* description)
{
	logError(description);
}

Bool apiWindowKeyIsPressed(const ApiWindow apiWindow, const Int key)
{
	return glfwGetKey(apiWindow, key) > 0;
}

void apiWindowSetVisibility(const ApiWindow apiWindow, const Bool visible)
{
	if(visible) glfwShowWindow(apiWindow);
	else glfwHideWindow(apiWindow);
}

void apiSetWindowShouldClose(const ApiWindow apiWindow, const Bool shouldClose)
{
	glfwSetWindowShouldClose(apiWindow, shouldClose);
}

void apiWindowResize(const ApiWindow apiWindow, const Int width, const Int height)
{
	glfwSetWindowSize(apiWindow, width, height);
}

ApiWindow apiCreateWindow(const IVec2 size, const Char* title, const ApiWindow shared)
{
	return glfwCreateWindow(size.x, size.y, title, nullptr, shared);
}

void apiWindowSetCursorPosition(const ApiWindow apiWindow, const DVec2 position)
{
	glfwSetCursorPos(apiWindow, position.x, position.y);
}

static String apiCursorTypeLabel(const ApiCursorType cursor)
{
	switch (cursor)
	{
	case ApiCursorType::Hand: return "Hand";
	case ApiCursorType::IBeam: return "IBeam";
	case ApiCursorType::Arrow:
	default: return "Arrow";
	}
}

namespace
{
	struct StandardCursorCache
	{
		External::GLFWcursor* arrow = nullptr;
		External::GLFWcursor* hand = nullptr;
		External::GLFWcursor* ibeam = nullptr;

		void ensureCreated()
		{
			if (!arrow) arrow = External::glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
			if (!hand) hand = External::glfwCreateStandardCursor(GLFW_HAND_CURSOR);
			if (!ibeam) ibeam = External::glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		}

		void release()
		{
			if (arrow) { External::glfwDestroyCursor(arrow); arrow = nullptr; }
			if (hand) { External::glfwDestroyCursor(hand); hand = nullptr; }
			if (ibeam) { External::glfwDestroyCursor(ibeam); ibeam = nullptr; }
		}

		[[nodiscard]] External::GLFWcursor* pick(const ApiCursorType cursor) const
		{
			switch (cursor)
			{
			case ApiCursorType::Hand: return hand;
			case ApiCursorType::IBeam: return ibeam;
			case ApiCursorType::Arrow:
			default: return arrow;
			}
		}
	};

	StandardCursorCache& standardCursorCache()
	{
		static StandardCursorCache cache;
		return cache;
	}

	std::unordered_map<ApiWindow, ApiCursorType>& lastStandardCursorByWindow()
	{
		static std::unordered_map<ApiWindow, ApiCursorType> lastByWindow;
		return lastByWindow;
	}
}

void apiReleaseStandardCursors()
{
	standardCursorCache().release();
	lastStandardCursorByWindow().clear();
}

void apiWindowSetStandardCursor(const ApiWindow apiWindow, const ApiCursorType cursor)
{
	if (!apiWindow || apiWindowCursorIsCaptured(apiWindow)) return;

	StandardCursorCache& cache = standardCursorCache();
	cache.ensureCreated();

	if (debugCursorShapeChangesIsEnabled)
	{
		std::unordered_map<ApiWindow, ApiCursorType>& lastByWindow = lastStandardCursorByWindow();
		const ApiCursorType previous = lastByWindow[apiWindow];
		if (previous != cursor)
		{
			logDebug("Cursor shape: " + apiCursorTypeLabel(cursor));
			lastByWindow[apiWindow] = cursor;
		}
	}

	External::glfwSetCursor(apiWindow, cache.pick(cursor));
}

void apiWindowSetWindowedMode(const ApiWindow apiWindow, const TiledRectangle monitorArea)
{
	glfwSetWindowMonitor(apiWindow, nullptr, monitorArea.position.x, monitorArea.position.y, monitorArea.size.x, monitorArea.size.y, GLFW_DONT_CARE);
}
