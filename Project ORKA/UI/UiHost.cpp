#include "UiHost.hpp"

#include <algorithm>

#include "../Debug.hpp"
#include "../KeyMap.hpp"
#include "../Renderer.hpp"
#include "../ResourceManager.hpp"
#include "../Window.hpp"
#include "../WindowAPI.hpp"
#include "Inspector/UiLayoutDebugOverlay.hpp"
#include "Render/UiRenderer.hpp"
#include "Widgets/UiWidgets.hpp"

namespace Ui
{
	namespace
	{
		Bool sameTextStyle(
			const UiTextPaintStyle& left, const UiTextPaintStyle& right)
		{
			return left.fontSize == right.fontSize &&
				left.lineHeight == right.lineHeight &&
				left.padding == right.padding &&
				left.wrap == right.wrap &&
				left.wrapPolicy == right.wrapPolicy &&
				left.fit.mode == right.fit.mode &&
				left.fit.minimumSize == right.fit.minimumSize &&
				left.fit.preferredSize == right.fit.preferredSize &&
				left.fit.maximumSize == right.fit.maximumSize &&
				left.fit.quantization == right.fit.quantization &&
				left.fit.maximumIterations == right.fit.maximumIterations;
		}

		struct SystemFontCandidate
		{
			const Char* file;
			const Char* family;
		};

		// Scroll offsets are expressed in pixels, so one wheel notch has to be
		// converted from its unitless platform step.
		constexpr Double scrollNotchPixels = 48;

		// Preference order for the platform UI font. The font service resolves
		// no operating system collections itself, so the host supplies a path.
		Bool hasClass(const UiNode& node, const String& value)
		{
			return std::find(node.authored.classes.begin(), node.authored.classes.end(), value) !=
				node.authored.classes.end();
		}

		constexpr SystemFontCandidate systemFontCandidates[]{
			{"segoeui.ttf", "Segoe UI"},
			{"calibri.ttf", "Calibri"},
			{"tahoma.ttf", "Tahoma"},
			{"arial.ttf", "Arial"},
			{"consola.ttf", "Consolas"},
		};
	}

	UiHost::UiHost(const SizeT eventCapacity)
		: retainedLayoutEngine(retainedDocument),
		queuedEvents(eventCapacity), routedEvents(retainedDocument),
		retainedOverflowController(retainedDocument, retainedLayoutEngine, routedEvents)
	{
		useBasicTheme();
	}

	void UiHost::create(Window& window)
	{
		if (created) return;
		created = true;
		retainedFontService.start();
		retainedIconService.setIconRoot(
			resourceManager.getOrkaDataPath() / "Icons" / "tabler");
		(void)useSystemDefaultFont();
		platformClipboard.setWindow(window.apiWindow);
		platformTextInput.setEventSink([this](UiEvent event) { return enqueueEvent(std::move(event)); });
		window.unicodeInputCallback = [this](const U32 codepoint)
			{ (void)platformTextInput.unicodeCodepoint(codepoint); };
		window.keyInputCallback = [this](const Int key, const Int scanCode, const Int action, const Int modifiers)
			{
				if (action == GLFW_PRESS && key == F12)
				{
					requestLayoutDebugToggle();
					return;
				}
				UiEvent event;
				event.type = UiEventType::Key;
				event.code = key;
				event.scanCode = scanCode;
				event.modifiers = modifiers;
				event.action = action == GLFW_REPEAT ? UiEventAction::Repeated :
					action == GLFW_PRESS ? UiEventAction::Pressed : UiEventAction::Released;
				(void)enqueueEvent(std::move(event));
			};
		window.focusInputCallback = [this](const Bool focused)
			{ (void)platformTextInput.windowFocusChanged(focused); };
		window.compositionStartCallback = [this]() { (void)platformTextInput.compositionStart(); };
		window.compositionUpdateCallback = [this](const String& text, const SizeT start, const SizeT end)
			{ (void)platformTextInput.compositionUpdate(text, start, end); };
		window.compositionCommitCallback = [this](const String& text)
			{ (void)platformTextInput.compositionCommit(text); };
		window.compositionCancelCallback = [this]() { (void)platformTextInput.compositionCancel(); };
		updateViewport(window);
	}

	void UiHost::updateViewport(Window& window)
	{
		IVec2 logicalSize = window.getSize();
		if (window.apiWindow)
		{
			glfwGetWindowSize(window.apiWindow, &logicalSize.x, &logicalSize.y);
		}
		const IVec2 framebufferSize = window.getContentSize();
		const UInt previousRevision = retainedDocument.viewport.revision;
		retainedDocument.viewport.set(logicalSize, framebufferSize);
		if (retainedDocument.viewport.revision == previousRevision) return;

		UiEvent resize;
		resize.type = UiEventType::Resize;
		resize.logicalSize = retainedDocument.viewport.logicalSize;
		resize.framebufferSize = retainedDocument.viewport.framebufferSize;
		(void)enqueueEvent(resize);
		retainedDocument.markDirty(retainedDocument.root(), DirtyFlag::Measure);
	}

	void UiHost::synchronizePointer(Window& window)
	{
		// Consume the window thread's coherent frame snapshot directly. Pointer
		// moves are latest-state input, so they never need to queue or backlog.
		const DVec2 framePosition = window.frameMousePositionTopLeft();
		if (pointerPositionKnown && framePosition == pointerPosition) return;
		pointerPositionKnown = true;
		pointerPosition = framePosition;

		UiEvent move;
		move.type = UiEventType::PointerMove;
		move.position = pointerPosition;
		(void)routedEvents.dispatch(move);
	}

	void UiHost::dispatchEvents(Window& window)
	{
		for (const UiEvent& event : queuedEvents.drain())
		{
			(void)routedEvents.dispatch(event);
		}
		routedEvents.applyDeferredOperations();
	}

	void UiHost::update(Window& window)
	{
		if (!created) create(window);
		if (layoutDebugToggleRequested.exchange(false, std::memory_order_acq_rel))
		{
			layoutDebugMode = !layoutDebugMode;
			logDebug(String("UI layout debug ") + (layoutDebugMode ? "enabled" : "disabled") +
				" (F12)");
		}
		updateViewport(window);
		synchronizePointer(window);
		retainedDocument.applyDeferredMutations();
		retainedDocument.processDirty(dirty(DirtyFlag::Bindings) | dirty(DirtyFlag::Style));
		synchronizeTextMeasurers();
		if (widgetFactoryForInput) widgetFactoryForInput->setInputWindow(&window);
		dispatchEvents(window);
		if (widgetFactoryForInput) widgetFactoryForInput->driveNumberScrubFrame(window);
		window.applyUiCursorRequests();
		retainedLayoutEngine.layout();
		retainedOverflowController.synchronizeAfterLayout();
		if (retainedOverflowController.consumePendingRelayout())
		{
			retainedLayoutEngine.layout();
			retainedOverflowController.synchronizeAfterLayout();
		}
		// Listener callbacks may enqueue retained-tree mutations. Apply them only
		// after routing has finished, then bring layout current for this frame.
		retainedDocument.applyDeferredMutations();
		retainedDocument.processDirty(dirty(DirtyFlag::Bindings) | dirty(DirtyFlag::Style));
		synchronizeTextMeasurers();
		retainedLayoutEngine.layout();
		retainedOverflowController.synchronizeAfterLayout();
		if (retainedOverflowController.consumePendingRelayout())
		{
			retainedLayoutEngine.layout();
			retainedOverflowController.synchronizeAfterLayout();
		}

		if (!apiWindowCursorIsCaptured(window.apiWindow))
		{
			ApiCursorType cursor = ApiCursorType::Arrow;
			UiNodeHandle hovered = routedEvents.hoveredNode();
			while (const UiNode* node = retainedDocument.get(hovered))
			{
				const String& type = node->authored.typeName;
				if (type == "NumberField")
				{
					cursor = hasClass(*node, "is-number-editing")
						? ApiCursorType::IBeam : ApiCursorType::Hand;
					break;
				}
				if (type == "TextField" || type == "TextArea")
				{
					cursor = ApiCursorType::IBeam;
					break;
				}
				if (type == "Button" || type == "Toggle" || type == "Checkbox" ||
					type == "Select" || type == "Combo" || type == "Slider" || type == "Range" ||
					type == "MenuItem")
				{
					cursor = ApiCursorType::Hand;
					break;
				}
				hovered = node->parentHandle();
			}
			apiWindowSetStandardCursor(window.apiWindow, cursor);
		}
	}

	void UiHost::synchronizeTextMeasurers()
	{
		const U64 owner = retainedFontService.serviceId();
		const U64 fontGeneration = retainedFontService.metricsGeneration();
		std::erase_if(textMeasurers, [&](const TextMeasurerState& state)
			{ return !retainedDocument.isValid(state.handle); });

		retainedDocument.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				auto tracked = std::ranges::find_if(textMeasurers,
					[&](const TextMeasurerState& state)
					{ return state.handle == handle; });
				const String& type = node.authored.typeName;
				const Bool editableField = type == "TextField" || type == "NumberField" ||
					type == "TextArea";
				// Empty editable fields still need one line of intrinsic height so parents
				// don't collapse; non-editable empty text clears its measurer as before.
				if (node.authored.text.empty() && !editableField)
				{
					if (retainedLayoutEngine.hasIntrinsicMeasurer(handle) &&
						retainedLayoutEngine.intrinsicMeasurerOwner(handle) == owner)
						retainedLayoutEngine.clearIntrinsicMeasurer(handle);
					if (tracked != textMeasurers.end()) textMeasurers.erase(tracked);
					return;
				}

				if (retainedLayoutEngine.hasIntrinsicMeasurer(handle) &&
					retainedLayoutEngine.intrinsicMeasurerOwner(handle) != owner)
				{
					if (tracked != textMeasurers.end()) textMeasurers.erase(tracked);
					return;
				}

				UiTextPaintStyle style;
				if (retainedTextStyleProvider)
					retainedTextStyleProvider(node, style);
				const Bool changed = tracked == textMeasurers.end() ||
					tracked->text != node.authored.text ||
					tracked->styleRevision != node.computed.styleRevision ||
					tracked->fontGeneration != fontGeneration ||
					!sameTextStyle(tracked->style, style);
				if (!changed) return;

				retainedLayoutEngine.setOwnedIntrinsicMeasurer(handle,
					[this, handle, style](const UiNode& measuredNode,
						const UiConstraints& constraints)
					{
						if (measuredNode.authored.text.empty())
						{
							const Float fontSize = style.fontSize > 0 ? style.fontSize : 16.0f;
							const Float lineHeight = style.lineHeight > 0 ? style.lineHeight : fontSize * 1.5f;
							const Vec2 padding = glm::max(style.padding, Vec2(0)) * 2.0f;
							const Vec2 content(0.0f, lineHeight);
							const UiIntrinsicMeasure measured{
								content, constraints.constrain(content), fontSize * 0.8f };
							if (UiNode* mutableNode = retainedDocument.get(handle))
							{
								mutableNode->computed.textFontSize = fontSize;
								mutableNode->computed.textLineHeight = lineHeight;
								mutableNode->computed.textLineCount = 1;
							}
							return UiIntrinsicMeasure{
								constraints.constrain(measured.minimum + padding),
								constraints.constrain(measured.preferred + padding),
								measured.baseline < 0 ? -1 : measured.baseline + style.padding.y };
						}

						UiTextLayoutResult textLayout;
						const UiIntrinsicMeasure result =
							retainedFontService.measureIntrinsic(
								measuredNode, constraints, style, &textLayout);
						if (UiNode* mutableNode = retainedDocument.get(handle))
						{
							const Float lineHeight = textLayout.lines.empty() ?
								0.0f : textLayout.lines.front().height;
							if (mutableNode->computed.textFontSize != textLayout.fontSize ||
								mutableNode->computed.textLineHeight != lineHeight ||
								mutableNode->computed.textLineCount != textLayout.lines.size())
								mutableNode->dirtyFlags |= dirty(DirtyFlag::Paint);
							mutableNode->computed.textFontSize = textLayout.fontSize;
							mutableNode->computed.textLineHeight = lineHeight;
							mutableNode->computed.textLineCount =
								static_cast<UInt>(textLayout.lines.size());
						}
						return result;
					}, owner);
				TextMeasurerState replacement{
					handle, node.authored.text, node.computed.styleRevision,
					fontGeneration, style };
				if (tracked == textMeasurers.end())
					textMeasurers.push_back(std::move(replacement));
				else *tracked = std::move(replacement);
				node.dirtyFlags |= dirty(DirtyFlag::Paint);
			});
	}

	void UiHost::render(Window& window)
	{
		if (retainedFontService.prepareRender(window.renderer))
		{
			paintBuilder.invalidateAll();
			retainedDocument.markDirty(
				retainedDocument.root(), DirtyFlag::Paint);
		}

		// Caret blink requires focused editable fields to repaint every frame.
		retainedDocument.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				if (node.state.focused && node.state.semantics.showTextCaret)
					retainedDocument.markDirty(handle, DirtyFlag::Paint);
			});

		if (!nodePainter) return;

		UiDisplayList displayList;
		if (!paintBuilder.build(retainedDocument, displayList, nodePainter, paintBuildDiagnostics))
			return;
		if (!appendAppScrollbarOverlays(retainedAppTheme.colors, retainedDocument, displayList))
			return;

		(void)retainedIconService.uploadPendingTextures(window.renderer);

		UiRenderPacket packet;
		if (!UiDisplayListFinalizer::finalize(displayList, packet, packetDiagnostics))
			return;

		submitUi(window.renderer, packet);

		// Paint the layout debugger in a separate display list so overlay quads
		// never interact with scroll/rounded clip stacks from the main UI tree.
		if (!layoutDebugMode) return;

		const UiNodeHandle hoveredNode = routedEvents.hoveredNode();
		if (!retainedDocument.isValid(hoveredNode)) return;

		UiDisplayList overlayList;
		const Inspector::UiLayoutDebugOverlayResult painted =
			Inspector::UiLayoutDebugOverlay::paintHovered(
				retainedDocument, retainedLayoutEngine, hoveredNode, overlayList);
		if (!painted.valid || !painted.painted) return;

		UiRenderPacket overlayPacket;
		Vector<UiRenderDiagnostic> overlayDiagnostics;
		if (!UiDisplayListFinalizer::finalize(overlayList, overlayPacket, overlayDiagnostics))
			return;

		submitUi(window.renderer, overlayPacket);
	}

	void UiHost::destroy(Window& window)
	{
		window.requestUiCursorRelease();
		widgetFactoryForInput = nullptr;
		window.unicodeInputCallback = {};
		window.keyInputCallback = {};
		window.focusInputCallback = {};
		window.compositionStartCallback = {};
		window.compositionUpdateCallback = {};
		window.compositionCommitCallback = {};
		window.compositionCancelCallback = {};
		platformTextInput.clearEventSink();
		platformClipboard.setWindow(nullptr);
		queuedEvents.clear();
		routedEvents.reset();
		retainedOverflowController.reset();
		paintBuilder.invalidateAll();
		paintBuildDiagnostics.clear();
		packetDiagnostics.clear();
		retainedFontService.shutdown(window.renderer);
		retainedIconService.shutdown(window.renderer);
		created = false;
	}

	UiFontFaceId UiHost::registerFontPath(
		const String& resourceName, const Path& resolvedPath,
		const String& family, const String& style, const Int faceIndex)
	{
		return retainedFontService.registerFontPath(
			resourceName, resolvedPath, family, style, faceIndex);
	}

	void UiHost::registerFontFallbacks(
		const String& family, Vector<String> fallbackFamilies)
	{
		retainedFontService.registerFallbackFamily(
			family, std::move(fallbackFamilies));
	}

	Bool UiHost::setDefaultFontFamily(const String& family)
	{
		return retainedFontService.setDefaultFamily(family);
	}

	Bool UiHost::useSystemDefaultFont()
	{
		const Char* windowsDirectory = std::getenv("WINDIR");
		if (!windowsDirectory) return false;
		const Path fontDirectory = Path(windowsDirectory) / "Fonts";
		for (const SystemFontCandidate& candidate : systemFontCandidates)
		{
			const Path file = fontDirectory / candidate.file;
			std::error_code error;
			if (!std::filesystem::exists(file, error) || error) continue;
			if (registerFontPath(candidate.file, file, candidate.family) ==
				invalidUiFontFaceId) continue;
			if (setDefaultFontFamily(candidate.family)) return true;
		}
		return false;
	}

	void UiHost::refreshAppThemePainter()
	{
		setPainter(makeAppThemePainter(
			retainedAppTheme, &retainedDocument, retainedTextStyleProvider, {},
			retainedFontService.shapeProvider(), &retainedIconService, paintStyleTokenProvider));
	}

	void UiHost::useAppTheme(const UiAppTheme& theme, UiTextStyleProvider textStyleProvider)
	{
		retainedAppTheme = theme;
		retainedTextStyleProvider = textStyleProvider;
		if (!retainedTextStyleProvider) retainedTextStyleProvider = makeAppTextStyleProvider(theme);
		refreshAppThemePainter();
		retainedDocument.markDirty(retainedDocument.root(), DirtyFlag::Style);
	}

	void UiHost::setPaintStyleTokens(const UiPaintStyleTokenProvider provider)
	{
		paintStyleTokenProvider = provider;
		refreshAppThemePainter();
	}

	void UiHost::useBasicTheme(UiTextStyleProvider textStyleProvider)
	{
		useAppTheme(UiAppTheme::light(), std::move(textStyleProvider));
	}

	void UiHost::setPainter(UiNodePainter painter)
	{
		nodePainter = std::move(painter);
		paintBuilder.invalidateAll();
		retainedDocument.markDirty(retainedDocument.root(), DirtyFlag::Paint);
	}

	void UiHost::inputEvent(Window& window, InputEvent& input)
	{
		UiEvent event;
		event.code = input.id.id;
		event.action = input.state ? UiEventAction::Pressed : UiEventAction::Released;

		switch (input.id.type)
		{
		case InputType::Mouse:
			event.type = UiEventType::PointerButton;
			event.position = window.mousePosition();
			break;
		case InputType::KeyBoard:
			if (window.keyInputCallback)
			{
				input.consume();
				return;
			}
			event.type = UiEventType::Key;
			break;
		case InputType::Controller:
			event.type = UiEventType::Key;
			break;
		case InputType::Scroll:
		{
			event.type = UiEventType::Scroll;
			event.position = window.mousePosition();
			const Double step = input.state ? scrollNotchPixels : -scrollNotchPixels;
			// Wheel-up moves content down, which is a decreasing scroll offset.
			if (input.id.id == 0) event.delta.x = step;
			else event.delta.y = -step;
			break;
		}
		default:
			return;
		}

		if (enqueueEvent(event)) input.consume();
	}

	Bool UiHost::enqueueEvent(UiEvent event)
	{
		return queuedEvents.push(std::move(event));
	}
}
