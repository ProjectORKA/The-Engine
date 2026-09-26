#pragma once

#include "../UIElement.hpp"

#include <atomic>
#include "Core/UiDocument.hpp"
#include "Input/UiEventQueue.hpp"
#include "Input/UiEventRouter.hpp"
#include "Input/UiPlatformTextInput.hpp"
#include "Layout/UiLayoutEngine.hpp"
#include "Layout/UiOverflowController.hpp"
#include "Render/UiBasicTheme.hpp"
#include "Theme/UiAppTheme.hpp"
#include "Render/UiPaintBuilder.hpp"
#include "Text/UiFontService.hpp"
#include "Text/UiSvgIconService.hpp"

namespace Ui
{
	class UiWidgetFactory;

	// Adapter between the existing immediate UIElement tree and the retained UI.
	// Each host owns one document and one per-window input queue.
	struct UiHost final : UIElement
	{
		explicit UiHost(SizeT eventCapacity = UiEventQueue::defaultCapacity);

		void create(Window& window) override;
		void update(Window& window) override;
		void render(Window& window) override;
		void destroy(Window& window) override;
		void inputEvent(Window& window, InputEvent& input) override;

		[[nodiscard]] Bool enqueueEvent(UiEvent event);
		[[nodiscard]] UiDocument& document() { return retainedDocument; }
		[[nodiscard]] const UiDocument& document() const { return retainedDocument; }
		[[nodiscard]] UiEventQueue& eventQueue() { return queuedEvents; }
		[[nodiscard]] UiEventRouter& eventRouter() { return routedEvents; }
		[[nodiscard]] const UiEventRouter& eventRouter() const { return routedEvents; }
		[[nodiscard]] UiPlatformTextInputBridge& textInputBridge() { return platformTextInput; }
		[[nodiscard]] UiGlfwClipboard& clipboard() { return platformClipboard; }
		[[nodiscard]] UiLayoutEngine& layoutEngine() { return retainedLayoutEngine; }
		[[nodiscard]] const UiLayoutEngine& layoutEngine() const { return retainedLayoutEngine; }
		[[nodiscard]] UiOverflowController& overflowController() { return retainedOverflowController; }
		[[nodiscard]] const UiOverflowController& overflowController() const { return retainedOverflowController; }
		[[nodiscard]] UiFontService& fontService() { return retainedFontService; }
		[[nodiscard]] const UiFontService& fontService() const { return retainedFontService; }
		[[nodiscard]] UiSvgIconService& iconService() { return retainedIconService; }
		[[nodiscard]] const UiSvgIconService& iconService() const { return retainedIconService; }
		[[nodiscard]] UiFontFaceId registerFontPath(
			const String& resourceName, const Path& resolvedPath,
			const String& family, const String& style = "Regular",
			Int faceIndex = 0);
		void registerFontFallbacks(
			const String& family, Vector<String> fallbackFamilies);
		[[nodiscard]] Bool setDefaultFontFamily(const String& family);
		// Registers the first available operating system UI font as the default
		// family. Returns false when none resolve, leaving the built-in font.
		[[nodiscard]] Bool useSystemDefaultFont();
		void useAppTheme(const UiAppTheme& theme, UiTextStyleProvider textStyleProvider = {});
		void useBasicTheme(UiTextStyleProvider textStyleProvider = {});
		void setPaintStyleTokens(UiPaintStyleTokenProvider provider);
		void setPainter(UiNodePainter painter);
		[[nodiscard]] const UiAppTheme& appTheme() const { return retainedAppTheme; }
		[[nodiscard]] const Vector<UiPaintBuildDiagnostic>& paintDiagnostics() const { return paintBuildDiagnostics; }
		[[nodiscard]] const Vector<UiRenderDiagnostic>& renderDiagnostics() const { return packetDiagnostics; }

		void setLayoutDebugEnabled(const Bool enabled) { layoutDebugMode = enabled; }
		[[nodiscard]] Bool layoutDebugEnabled() const { return layoutDebugMode; }
		void requestLayoutDebugToggle()
		{
			layoutDebugToggleRequested.store(true, std::memory_order_release);
		}
		// Optional factory hook for window-thread input drivers (number-field scrub).
		void registerWidgetFactory(UiWidgetFactory* factory) { widgetFactoryForInput = factory; }

	private:
		UiDocument retainedDocument;
		UiLayoutEngine retainedLayoutEngine;
		UiEventQueue queuedEvents;
		UiEventRouter routedEvents;
		UiOverflowController retainedOverflowController;
		UiPlatformTextInputBridge platformTextInput;
		UiGlfwClipboard platformClipboard;
		UiFontService retainedFontService;
		UiSvgIconService retainedIconService;
		UiDirtyPaintBuilder paintBuilder;
		UiNodePainter nodePainter;
		Vector<UiPaintBuildDiagnostic> paintBuildDiagnostics;
		Vector<UiRenderDiagnostic> packetDiagnostics;
		struct TextMeasurerState
		{
			UiNodeHandle handle;
			String text;
			UInt styleRevision = 0;
			U64 fontGeneration = 0;
			UiTextPaintStyle style;
		};
		Vector<TextMeasurerState> textMeasurers;
		UiAppTheme retainedAppTheme = UiAppTheme::light();
		UiTextStyleProvider retainedTextStyleProvider;
		UiPaintStyleTokenProvider paintStyleTokenProvider;
		void refreshAppThemePainter();
		DVec2 pointerPosition = DVec2(0);
		Bool pointerPositionKnown = false;
		UiWidgetFactory* widgetFactoryForInput = nullptr;
		Bool created = false;
		Bool layoutDebugMode = false;
		std::atomic<Bool> layoutDebugToggleRequested{ false };

		void updateViewport(Window& window);
		void synchronizePointer(Window& window);
		void dispatchEvents(Window& window);
		void synchronizeTextMeasurers();
	};
}
