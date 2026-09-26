#include "UiEventRouter.hpp"

namespace Ui
{
	namespace
	{
		constexpr U64 doubleClickWindowMicroseconds = 500000;
		constexpr Double dragThresholdSquared = 16.0;
		constexpr Int keyEscape = 256;
		constexpr Int keyEnter = 257;
		constexpr Int keyTab = 258;
		constexpr Int keyRight = 262;
		constexpr Int keyLeft = 263;
		constexpr Int keyDown = 264;
		constexpr Int keyUp = 265;
		constexpr Int keySpace = 32;
		constexpr Int modifierShift = 0x0001;
		constexpr Int modifierControl = 0x0002;
		constexpr Int modifierAlt = 0x0004;
		constexpr Int modifierSuper = 0x0008;
		constexpr Int activationModifierMask =
			modifierShift | modifierControl | modifierAlt | modifierSuper;

		Bool hasActivationModifiers(const Int modifiers)
		{
			return (modifiers & activationModifierMask) != 0;
		}

		String utf8(const U32 value)
		{
			String result;
			if (value <= 0x7f) result.push_back(static_cast<Char>(value));
			else if (value <= 0x7ff)
			{
				result.push_back(static_cast<Char>(0xc0 | (value >> 6)));
				result.push_back(static_cast<Char>(0x80 | (value & 0x3f)));
			}
			else if (value <= 0xffff)
			{
				result.push_back(static_cast<Char>(0xe0 | (value >> 12)));
				result.push_back(static_cast<Char>(0x80 | ((value >> 6) & 0x3f)));
				result.push_back(static_cast<Char>(0x80 | (value & 0x3f)));
			}
			else if (value <= 0x10ffff)
			{
				result.push_back(static_cast<Char>(0xf0 | (value >> 18)));
				result.push_back(static_cast<Char>(0x80 | ((value >> 12) & 0x3f)));
				result.push_back(static_cast<Char>(0x80 | ((value >> 6) & 0x3f)));
				result.push_back(static_cast<Char>(0x80 | (value & 0x3f)));
			}
			return result;
		}

		Bool contains(const Vec2 point, const Vec2 position, const Vec2 size)
		{
			return size.x > 0 && size.y > 0 && point.x >= position.x && point.y >= position.y &&
				point.x <= position.x + size.x && point.y <= position.y + size.y;
		}

		Vec2 inversePoint(const glm::mat3& transform, const DVec2 point, Bool& invertible)
		{
			const Float determinant = glm::determinant(transform);
			invertible = std::isfinite(determinant) && std::abs(determinant) > 0.000001f;
			if (!invertible) return Vec2(0);
			const glm::vec3 local = glm::inverse(transform) *
				glm::vec3(static_cast<Float>(point.x), static_cast<Float>(point.y), 1);
			invertible = std::isfinite(local.x) && std::isfinite(local.y) && std::abs(local.z) > 0.000001f;
			return invertible ? Vec2(local.x, local.y) / local.z : Vec2(0);
		}
	}

	struct UiEventRouter::Listener
	{
		UiListenerToken token;
		UiNodeHandle node;
		UiRoutedEventType type = UiRoutedEventType::PointerMove;
		UiEventCallback callback;
		String commandId;
		UiListenerOptions options;
	};

	struct UiEventRouter::Action
	{
		UiNodeHandle node;
		UiRoutedEventType type = UiRoutedEventType::Activate;
		UiEventCallback callback;
		String commandId;
	};

	struct UiEventRouter::PointerState
	{
		UInt id = 0;
		UiPointerKind kind = UiPointerKind::Mouse;
		DVec2 position = DVec2(0);
		DVec2 pressedPosition = DVec2(0);
		UiNodeHandle hovered;
		UiNodeHandle captured;
		UiNodeHandle pressed;
		UiNodeHandle dragging;
		UiNodeHandle lastClicked;
		U64 lastClickTime = 0;
		Int button = 0;
	};

	UiEventRouter::UiEventRouter(UiDocument& document) : owner(document)
	{
	}

	UiEventRouter::~UiEventRouter() = default;

	UiListenerToken UiEventRouter::addListener(const UiNodeHandle node, const UiRoutedEventType type,
		UiEventCallback callback, const UiListenerOptions options)
	{
		if (!owner.isValid(node) || !callback ||
			listeners.size() + pendingListeners.size() >= maximumListeners) return {};
		const UiListenerToken token{ nextListener++ };
		Listener listener{ token, node, type, std::move(callback), {}, options };
		if (dispatchDepth) pendingListeners.push_back(std::move(listener));
		else listeners.push_back(std::move(listener));
		return token;
	}

	UiListenerToken UiEventRouter::addCommandListener(const UiNodeHandle node, const UiRoutedEventType type,
		String commandId, const UiListenerOptions options)
	{
		if (!owner.isValid(node) || commandId.empty() ||
			listeners.size() + pendingListeners.size() >= maximumListeners) return {};
		const UiListenerToken token{ nextListener++ };
		Listener listener{ token, node, type, {}, std::move(commandId), options };
		if (dispatchDepth) pendingListeners.push_back(std::move(listener));
		else listeners.push_back(std::move(listener));
		return token;
	}

	Bool UiEventRouter::removeListener(const UiListenerToken token)
	{
		if (!token.isValid()) return false;
		if (dispatchDepth)
		{
			if (deferred.size() >= maximumDeferredOperations) return false;
			deferred.push_back({ DeferredType::RemoveListener, token, {} });
			return true;
		}
		const auto found = std::find_if(listeners.begin(), listeners.end(),
			[token](const Listener& listener) { return listener.token == token; });
		if (found != listeners.end())
		{
			listeners.erase(found);
			return true;
		}
		const auto pending = std::find_if(pendingListeners.begin(), pendingListeners.end(),
			[token](const Listener& listener) { return listener.token == token; });
		if (pending == pendingListeners.end()) return false;
		pendingListeners.erase(pending);
		return true;
	}

	void UiEventRouter::setCommandDispatcher(UiCommandDispatcher dispatcher)
	{
		commandDispatcher = std::move(dispatcher);
	}

	void UiEventRouter::setDefaultAction(const UiNodeHandle node, const UiRoutedEventType type,
		UiEventCallback action)
	{
		if (!owner.isValid(node)) return;
		defaults.erase(std::remove_if(defaults.begin(), defaults.end(),
			[node, type](const Action& value) { return value.node == node && value.type == type; }), defaults.end());
		if (action) defaults.push_back({ node, type, std::move(action), {} });
	}

	void UiEventRouter::setDefaultCommand(const UiNodeHandle node, const UiRoutedEventType type, String commandId)
	{
		if (!owner.isValid(node)) return;
		defaults.erase(std::remove_if(defaults.begin(), defaults.end(),
			[node, type](const Action& value) { return value.node == node && value.type == type; }), defaults.end());
		if (!commandId.empty()) defaults.push_back({ node, type, {}, std::move(commandId) });
	}

	void UiEventRouter::setCancelAction(const UiNodeHandle scope, UiEventCallback action)
	{
		if (!owner.isValid(scope)) return;
		cancels.erase(std::remove_if(cancels.begin(), cancels.end(),
			[scope](const Action& value) { return value.node == scope; }), cancels.end());
		if (action) cancels.push_back({ scope, UiRoutedEventType::Cancel, std::move(action), {} });
	}

	void UiEventRouter::setCancelCommand(const UiNodeHandle scope, String commandId)
	{
		if (!owner.isValid(scope)) return;
		cancels.erase(std::remove_if(cancels.begin(), cancels.end(),
			[scope](const Action& value) { return value.node == scope; }), cancels.end());
		if (!commandId.empty()) cancels.push_back({ scope, UiRoutedEventType::Cancel, {}, std::move(commandId) });
	}

	void UiEventRouter::setInteraction(const UiNodeHandle node, const UiInteractionData value)
	{
		if (owner.isValid(node)) interactions.insert_or_assign(node, value);
	}

	UiInteractionData UiEventRouter::interaction(const UiNodeHandle node) const
	{
		const auto found = interactions.find(node);
		return found == interactions.end() ? UiInteractionData{} : found->second;
	}

	void UiEventRouter::clearNode(const UiNodeHandle node)
	{
		if (dispatchDepth)
		{
			if (deferred.size() < maximumDeferredOperations)
				deferred.push_back({ DeferredType::ClearNode, {}, node });
			return;
		}
		interactions.erase(node);
		listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
			[node](const Listener& value) { return value.node == node; }), listeners.end());
		pendingListeners.erase(std::remove_if(pendingListeners.begin(), pendingListeners.end(),
			[node](const Listener& value) { return value.node == node; }), pendingListeners.end());
		defaults.erase(std::remove_if(defaults.begin(), defaults.end(),
			[node](const Action& value) { return value.node == node; }), defaults.end());
		cancels.erase(std::remove_if(cancels.begin(), cancels.end(),
			[node](const Action& value) { return value.node == node; }), cancels.end());
		for (PointerState& state : pointers)
		{
			if (state.captured == node) state.captured = {};
			if (state.pressed == node) state.pressed = {};
			if (state.dragging == node) state.dragging = {};
			if (state.hovered == node) state.hovered = {};
		}
		if (focused == node) clearFocus();
	}

	Vector<UiNodeHandle> UiEventRouter::pathTo(UiNodeHandle node) const
	{
		Vector<UiNodeHandle> result;
		while (owner.isValid(node))
		{
			result.push_back(node);
			const UiNode* item = owner.get(node);
			node = item ? item->parentHandle() : UiNodeHandle{};
		}
		std::reverse(result.begin(), result.end());
		return result;
	}

	Bool UiEventRouter::isDescendantOf(UiNodeHandle node, const UiNodeHandle ancestor) const
	{
		while (owner.isValid(node))
		{
			if (node == ancestor) return true;
			node = owner.get(node)->parentHandle();
		}
		return false;
	}

	UiNodeHandle UiEventRouter::activeModalScope() const
	{
		for (auto iterator = modalScopes.rbegin(); iterator != modalScopes.rend(); ++iterator)
			if (owner.isValid(*iterator)) return *iterator;
		return {};
	}

	Bool UiEventRouter::isInteractive(UiNodeHandle node) const
	{
		if (!owner.isValid(node)) return false;
		const UiNodeHandle modal = activeModalScope();
		if (modal.isValid() && !isDescendantOf(node, modal)) return false;
		while (owner.isValid(node))
		{
			const UiNode& item = *owner.get(node);
			if (!item.state.visible || !item.state.enabled) return false;
			node = item.parentHandle();
		}
		return true;
	}

	UiNodeHandle UiEventRouter::hoveredNode(const UInt pointerId) const
	{
		for (const PointerState& state : pointers)
		{
			if (state.id != pointerId) continue;
			return owner.isValid(state.hovered) ? state.hovered : UiNodeHandle{};
		}
		return {};
	}

	UiNodeHandle UiEventRouter::hitTest(const DVec2 viewportPosition) const
	{
		Vector<UiNodeHandle> paintOrder;
		const_cast<UiDocument&>(owner).traverse(
			[&paintOrder](const UiNodeHandle handle, UiNode&) { paintOrder.push_back(handle); });

		for (auto candidate = paintOrder.rbegin(); candidate != paintOrder.rend(); ++candidate)
		{
			if (!isInteractive(*candidate) || !interaction(*candidate).hitTestVisible) continue;
			Bool accepted = true;
			UiNodeHandle current = *candidate;
			while (owner.isValid(current))
			{
				const UiNode* nodeItem = owner.get(current);
				if (!nodeItem)
				{
					accepted = false;
					break;
				}
				const UiNode& node = *nodeItem;
				Bool invertible = false;
				const Vec2 local = inversePoint(node.computed.hitTestTransform, viewportPosition, invertible);
				if (!invertible || (current == *candidate &&
					!contains(local, node.computed.position, node.computed.size)) ||
					(node.computed.clipSize.x > 0 && node.computed.clipSize.y > 0 &&
						!contains(local, node.computed.clipPosition, node.computed.clipSize)))
				{
					accepted = false;
					break;
				}
				current = node.parentHandle();
			}
			if (accepted) return *candidate;
		}
		return {};
	}

	UiEventRouter::PointerState* UiEventRouter::pointer(const UInt pointerId, const Bool create)
	{
		const auto found = std::find_if(pointers.begin(), pointers.end(),
			[pointerId](const PointerState& value) { return value.id == pointerId; });
		if (found != pointers.end()) return &*found;
		if (!create || pointers.size() >= maximumPointerStates) return nullptr;
		pointers.push_back({});
		pointers.back().id = pointerId;
		return &pointers.back();
	}

	void UiEventRouter::invokeAt(UiRoutedEvent& event, const UiNodeHandle node, const UiEventPhase phase)
	{
		event.currentTarget = node;
		event.phase = phase;
		event.immediatePropagationStopped = false;
		const SizeT count = listeners.size();
		for (SizeT index = 0; index < count && index < listeners.size(); ++index)
		{
			const Listener& source = listeners[index];
			if (source.node != node || source.type != event.type ||
				(source.options.capture != (phase == UiEventPhase::Capture))) continue;
			const UiEventCallback callback = source.callback;
			const String command = source.commandId;
			const UiListenerToken token = source.token;
			const Bool once = source.options.once;
			event.commandId = command;
			if (callback) callback(event);
			else if (!command.empty() && commandDispatcher) commandDispatcher(command, event);
			if (once) (void)removeListener(token);
			if (event.immediatePropagationStopped) break;
		}
	}

	Bool UiEventRouter::route(UiRoutedEvent& event)
	{
		if (!owner.isValid(event.target)) return false;
		++dispatchDepth;
		const Vector<UiNodeHandle> path = pathTo(event.target);
		for (SizeT index = 0; index + 1 < path.size() && !event.propagationStopped; ++index)
			if (owner.isValid(path[index])) invokeAt(event, path[index], UiEventPhase::Capture);
		if (!event.propagationStopped && owner.isValid(event.target))
		{
			invokeAt(event, event.target, UiEventPhase::Capture);
			if (!event.immediatePropagationStopped) invokeAt(event, event.target, UiEventPhase::Target);
		}
		if (!event.propagationStopped)
		{
			for (SizeT index = path.size(); index > 1 && !event.propagationStopped; --index)
				if (owner.isValid(path[index - 2])) invokeAt(event, path[index - 2], UiEventPhase::Bubble);
		}
		if (!event.defaultPrevented) runDefault(event);
		if (event.capturePointer) (void)capturePointer(event.pointerId, event.target);
		if (event.releasePointer) releasePointer(event.pointerId, event.target);
		if (event.captureSystemCursor) pendingSystemCursorCapture = true;
		if (event.releaseSystemCursor) pendingSystemCursorRelease = true;
		if (event.focusTarget) (void)focus(event.target);
		--dispatchDepth;
		if (!dispatchDepth) applyDeferredOperations();
		return event.handled || event.defaultPrevented;
	}

	void UiEventRouter::runDefault(UiRoutedEvent& event)
	{
		const Vector<UiNodeHandle> path = pathTo(event.target);
		for (auto node = path.rbegin(); node != path.rend(); ++node)
		{
			const auto found = std::find_if(defaults.begin(), defaults.end(),
				[&](const Action& action) { return action.node == *node && action.type == event.type; });
			if (found == defaults.end()) continue;
			const UiEventCallback callback = found->callback;
			const String command = found->commandId;
			event.currentTarget = *node;
			event.commandId = command;
			if (callback) callback(event);
			else if (!command.empty() && commandDispatcher) commandDispatcher(command, event);
			break;
		}
	}

	void UiEventRouter::runCancel(UiRoutedEvent& event)
	{
		UiNodeHandle scope = activeModalScope();
		if (!scope.isValid())
		{
			const Vector<UiNodeHandle> path = pathTo(focused);
			for (auto node = path.rbegin(); node != path.rend(); ++node)
				if (interaction(*node).focusScope) { scope = *node; break; }
		}
		if (!scope.isValid()) scope = owner.root();
		const auto found = std::find_if(cancels.begin(), cancels.end(),
			[scope](const Action& action) { return action.node == scope; });
		if (found == cancels.end()) return;
		event.currentTarget = scope;
		const UiEventCallback callback = found->callback;
		const String command = found->commandId;
		event.commandId = command;
		if (callback) callback(event);
		else if (!command.empty() && commandDispatcher) commandDispatcher(command, event);
	}

	U64 UiEventRouter::eventTime(const UiEvent& event) const
	{
		if (event.timestampMicroseconds) return event.timestampMicroseconds;
		return static_cast<U64>(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
	}

	void UiEventRouter::updateHover(PointerState& state, const UiNodeHandle target, const UiEvent& source)
	{
		if (state.hovered == target) return;
		const UiNodeHandle old = state.hovered;
		if (owner.isValid(old))
		{
			if (UiNode* node = owner.get(old))
			{
				node->state.hovered = false;
				owner.markDirty(old, DirtyFlag::Paint);
			}
			UiRoutedEvent leave{ .type = UiRoutedEventType::PointerLeave, .target = old,
				.relatedTarget = target, .position = source.position,
				.timestampMicroseconds = eventTime(source), .pointerId = state.id, .pointerKind = state.kind };
			(void)route(leave);
		}
		state.hovered = target;
		if (owner.isValid(target))
		{
			if (UiNode* node = owner.get(target))
			{
				node->state.hovered = true;
				owner.markDirty(target, DirtyFlag::Paint);
			}
			UiRoutedEvent enter{ .type = UiRoutedEventType::PointerEnter, .target = target,
				.relatedTarget = old, .position = source.position,
				.timestampMicroseconds = eventTime(source), .pointerId = state.id, .pointerKind = state.kind };
			(void)route(enter);
		}
	}

	Bool UiEventRouter::dispatch(const UiEvent& source)
	{
		cleanInvalidState();
		const U64 timestamp = eventTime(source);
		if (source.type == UiEventType::Focus && !source.focused)
		{
			cancelAllPointers();
			clearFocus();
			return false;
		}
		if (source.type == UiEventType::Resize || source.type == UiEventType::Focus) return false;

		if (source.type == UiEventType::PointerMove || source.type == UiEventType::PointerButton ||
			source.type == UiEventType::PointerCancel || source.type == UiEventType::Scroll)
		{
			PointerState* state = pointer(source.pointerId, true);
			if (!state) return false;
			state->kind = source.pointerKind;
			const DVec2 previous = state->position;
			state->position = source.position;
			const UiNodeHandle physicalTarget = hitTest(source.position);
			updateHover(*state, physicalTarget, source);

			if (source.type == UiEventType::PointerCancel)
			{
				cancelPointer(source.pointerId);
				return true;
			}
			if (source.type == UiEventType::Scroll)
			{
				const UiNodeHandle target = owner.isValid(state->captured) ? state->captured : physicalTarget;
				UiRoutedEvent wheel{ .type = UiRoutedEventType::Wheel, .target = target,
					.position = source.position, .delta = source.delta, .timestampMicroseconds = timestamp,
					.pointerId = source.pointerId, .pointerKind = source.pointerKind,
					.modifiers = source.modifiers };
				return route(wheel);
			}
			if (source.type == UiEventType::PointerMove)
			{
				const UiNodeHandle target = owner.isValid(state->captured) ? state->captured : physicalTarget;
				UiRoutedEvent move{ .type = UiRoutedEventType::PointerMove, .target = target,
					.position = source.position,
					.delta = glm::dot(source.delta, source.delta) == 0.0 ?
						source.position - previous : source.delta,
					.timestampMicroseconds = timestamp, .pointerId = source.pointerId,
					.pointerKind = source.pointerKind, .button = state->button, .modifiers = source.modifiers };
				Bool handled = route(move);
				if (owner.isValid(state->pressed))
				{
					const DVec2 distance = state->position - state->pressedPosition;
					if (!owner.isValid(state->dragging) && glm::dot(distance, distance) >= dragThresholdSquared)
					{
						state->dragging = state->pressed;
						UiRoutedEvent start = move;
						start.type = UiRoutedEventType::DragStart;
						start.target = state->dragging;
						handled = route(start) || handled;
					}
					if (owner.isValid(state->dragging))
					{
						UiRoutedEvent drag = move;
						drag.type = UiRoutedEventType::Drag;
						drag.target = state->dragging;
						handled = route(drag) || handled;
					}
				}
				return handled;
			}

			const Bool pressed = source.action == UiEventAction::Pressed;
			if (pressed)
			{
				state->pressed = physicalTarget;
				state->pressedPosition = source.position;
				state->button = source.code;
				if (UiNode* node = owner.get(state->pressed))
				{
					node->state.active = true;
					owner.markDirty(state->pressed, DirtyFlag::Paint);
				}
				UiRoutedEvent down{ .type = UiRoutedEventType::PointerDown, .target = state->pressed,
					.position = source.position, .timestampMicroseconds = timestamp,
					.pointerId = source.pointerId, .pointerKind = source.pointerKind,
					.button = source.code, .modifiers = source.modifiers };
				const Bool handled = route(down);
				if (interaction(state->pressed).focusable) (void)focus(state->pressed);
				else if (owner.isValid(focused)) clearFocus();
				return handled;
			}

			const UiNodeHandle upTarget = owner.isValid(state->captured) ? state->captured : physicalTarget;
			UiRoutedEvent up{ .type = UiRoutedEventType::PointerUp, .target = upTarget,
				.position = source.position, .timestampMicroseconds = timestamp,
				.pointerId = source.pointerId, .pointerKind = source.pointerKind,
				.button = source.code, .modifiers = source.modifiers };
			Bool handled = route(up);
			if (owner.isValid(state->dragging))
			{
				UiRoutedEvent end = up;
				end.type = UiRoutedEventType::DragEnd;
				end.target = state->dragging;
				handled = route(end) || handled;
			}
			else if (owner.isValid(state->pressed) && state->pressed == physicalTarget)
			{
				UiRoutedEvent activate = up;
				activate.type = UiRoutedEventType::Activate;
				activate.target = state->pressed;
				handled = route(activate) || handled;
				if (state->lastClicked == state->pressed &&
					timestamp >= state->lastClickTime &&
					timestamp - state->lastClickTime <= doubleClickWindowMicroseconds)
				{
					UiRoutedEvent twice = activate;
					twice.type = UiRoutedEventType::DoubleClick;
					handled = route(twice) || handled;
					state->lastClickTime = 0;
					state->lastClicked = {};
				}
				else
				{
					state->lastClicked = state->pressed;
					state->lastClickTime = timestamp;
				}
			}
			if (UiNode* node = owner.get(state->pressed))
			{
				node->state.active = false;
				owner.markDirty(state->pressed, DirtyFlag::Paint);
			}
			state->pressed = {};
			state->dragging = {};
			releasePointer(source.pointerId);
			return handled;
		}

		if (source.type == UiEventType::Text)
		{
			UiRoutedEvent text{ .type = UiRoutedEventType::TextInput, .target = focused,
				.timestampMicroseconds = timestamp, .modifiers = source.modifiers, .codepoint = source.codepoint,
				.text = source.text.empty() ? utf8(source.codepoint) : source.text };
			return route(text);
		}

		if (source.type == UiEventType::CompositionStart || source.type == UiEventType::CompositionUpdate ||
			source.type == UiEventType::CompositionCommit || source.type == UiEventType::CompositionCancel)
		{
			const UiRoutedEventType routedType =
				source.type == UiEventType::CompositionStart ? UiRoutedEventType::CompositionStart :
				source.type == UiEventType::CompositionUpdate ? UiRoutedEventType::CompositionUpdate :
				source.type == UiEventType::CompositionCommit ? UiRoutedEventType::CompositionCommit :
				UiRoutedEventType::CompositionCancel;
			UiRoutedEvent compositionEvent{ .type = routedType, .target = focused,
				.timestampMicroseconds = timestamp, .modifiers = source.modifiers, .text = source.text,
				.compositionStart = source.compositionStart, .compositionEnd = source.compositionEnd };
			return route(compositionEvent);
		}

		if (source.type == UiEventType::Key)
		{
			const Bool pressed = source.action == UiEventAction::Pressed || source.action == UiEventAction::Repeated;
			UiRoutedEvent key{ .type = pressed ? UiRoutedEventType::KeyDown : UiRoutedEventType::KeyUp,
				.target = owner.isValid(focused) ? focused : owner.root(), .timestampMicroseconds = timestamp,
				.key = source.code, .modifiers = source.modifiers };
			Bool handled = route(key);
			if (!pressed || key.defaultPrevented) return handled;
			if (source.code == keyTab) return moveFocusTab((source.modifiers & modifierShift) != 0) || handled;
			if (source.code == keyLeft) return moveFocusSpatial(UiNavigationDirection::Left) || handled;
			if (source.code == keyRight) return moveFocusSpatial(UiNavigationDirection::Right) || handled;
			if (source.code == keyUp) return moveFocusSpatial(UiNavigationDirection::Up) || handled;
			if (source.code == keyDown) return moveFocusSpatial(UiNavigationDirection::Down) || handled;
			if ((source.code == keyEnter || source.code == keySpace) && owner.isValid(focused) &&
				!hasActivationModifiers(source.modifiers))
			{
				UiRoutedEvent activate{ .type = UiRoutedEventType::Activate, .target = focused,
					.timestampMicroseconds = timestamp, .key = source.code, .modifiers = source.modifiers };
				return route(activate) || handled;
			}
			if (source.code == keyEscape)
			{
				UiRoutedEvent cancel{ .type = UiRoutedEventType::Cancel,
					.target = owner.isValid(focused) ? focused : owner.root(), .timestampMicroseconds = timestamp,
					.key = source.code, .modifiers = source.modifiers };
				handled = route(cancel) || handled;
				if (!cancel.defaultPrevented) runCancel(cancel);
			}
			return handled;
		}
		return false;
	}

	void UiEventRouter::setWorldSurfaceMapper(UiWorldSurfaceMapper mapper)
	{
		worldMapper = std::move(mapper);
	}

	Bool UiEventRouter::dispatchWorldEvent(UiEvent event, const UiWorldRay& ray)
	{
		if (!worldMapper || !worldMapper(ray, event.position)) return false;
		event.pointerKind = UiPointerKind::World;
		return dispatch(event);
	}

	Bool UiEventRouter::capturePointer(const UInt pointerId, const UiNodeHandle node)
	{
		if (!isInteractive(node)) return false;
		PointerState* state = pointer(pointerId, true);
		if (!state) return false;
		state->captured = node;
		return true;
	}

	void UiEventRouter::releasePointer(const UInt pointerId, const UiNodeHandle node)
	{
		for (PointerState& state : pointers)
			if ((pointerId == state.id || (pointerId == 0 && node.isValid())) &&
				(!node.isValid() || state.captured == node)) state.captured = {};
	}

	void UiEventRouter::cancelPointer(const UInt pointerId)
	{
		PointerState* state = pointer(pointerId, false);
		if (!state) return;
		const UiNodeHandle target = owner.isValid(state->captured) ? state->captured : state->pressed;
		if (owner.isValid(target))
		{
			UiRoutedEvent cancel{ .type = UiRoutedEventType::PointerCancel, .target = target,
				.position = state->position, .pointerId = state->id, .pointerKind = state->kind,
				.button = state->button };
			(void)route(cancel);
		}
		if (UiNode* node = owner.get(state->pressed))
		{
			node->state.active = false;
			owner.markDirty(state->pressed, DirtyFlag::Paint);
		}
		state->captured = {};
		state->pressed = {};
		state->dragging = {};
	}

	void UiEventRouter::cancelAllPointers()
	{
		Vector<UInt> ids;
		for (const PointerState& state : pointers) ids.push_back(state.id);
		for (const UInt id : ids) cancelPointer(id);
		pointers.clear();
	}

	Bool UiEventRouter::focus(const UiNodeHandle node)
	{
		if (node == focused) return true;
		if (!isInteractive(node) || !interaction(node).focusable) return false;
		const UiNodeHandle old = focused;
		if (UiNode* item = owner.get(old))
		{
			item->state.focused = false;
			owner.markDirty(old, DirtyFlag::Style);
		}
		focused = node;
		if (owner.isValid(old))
		{
			UiRoutedEvent out{ .type = UiRoutedEventType::FocusOut, .target = old, .relatedTarget = node };
			(void)route(out);
		}
		if (UiNode* item = owner.get(node))
		{
			item->state.focused = true;
			owner.markDirty(node, DirtyFlag::Style);
		}
		UiRoutedEvent in{ .type = UiRoutedEventType::FocusIn, .target = node, .relatedTarget = old };
		(void)route(in);
		return true;
	}

	void UiEventRouter::clearFocus()
	{
		const UiNodeHandle old = focused;
		focused = {};
		if (UiNode* item = owner.get(old))
		{
			item->state.focused = false;
			owner.markDirty(old, DirtyFlag::Style);
		}
		if (owner.isValid(old))
		{
			UiRoutedEvent out{ .type = UiRoutedEventType::FocusOut, .target = old };
			(void)route(out);
		}
	}

	Bool UiEventRouter::moveFocusTab(const Bool backwards)
	{
		struct Candidate { UiNodeHandle node; Int tabIndex; SizeT order; };
		Vector<Candidate> candidates;
		SizeT order = 0;
		owner.traverse([&](const UiNodeHandle node, UiNode&)
			{
				const UiInteractionData data = interaction(node);
				if (data.focusable && data.tabIndex >= 0 && isInteractive(node))
					candidates.push_back({ node, data.tabIndex, order });
				++order;
			});
		std::stable_sort(candidates.begin(), candidates.end(), [](const Candidate& left, const Candidate& right)
			{ return left.tabIndex != right.tabIndex ? left.tabIndex < right.tabIndex : left.order < right.order; });
		if (candidates.empty()) return false;
		auto current = std::find_if(candidates.begin(), candidates.end(),
			[this](const Candidate& candidate) { return candidate.node == focused; });
		SizeT index = current == candidates.end() ? (backwards ? 0 : candidates.size() - 1) :
			static_cast<SizeT>(current - candidates.begin());
		index = backwards ? (index + candidates.size() - 1) % candidates.size() : (index + 1) % candidates.size();
		return focus(candidates[index].node);
	}

	Bool UiEventRouter::moveFocusSpatial(const UiNavigationDirection direction)
	{
		if (!owner.isValid(focused)) return moveFocusTab(false);
		const UiInteractionData currentData = interaction(focused);
		const UiNodeHandle explicitNode = currentData.explicitNeighbors[static_cast<SizeT>(direction)];
		if (isInteractive(explicitNode) && interaction(explicitNode).focusable) return focus(explicitNode);

		const UiNode& current = *owner.get(focused);
		const Vec2 currentCenter = current.computed.position + current.computed.size * 0.5f;
		const Vec2 axis = direction == UiNavigationDirection::Left ? Vec2(-1, 0) :
			direction == UiNavigationDirection::Right ? Vec2(1, 0) :
			direction == UiNavigationDirection::Up ? Vec2(0, -1) : Vec2(0, 1);
		const Bool horizontal = axis.x != 0;
		UiNodeHandle best;
		Float bestScore = std::numeric_limits<Float>::infinity();
		owner.traverse([&](const UiNodeHandle node, UiNode& item)
			{
				if (node == focused || !interaction(node).focusable || !isInteractive(node)) return;
				const Vec2 center = item.computed.position + item.computed.size * 0.5f;
				const Vec2 offset = center - currentCenter;
				const Float forward = glm::dot(offset, axis);
				if (forward <= 0) return;
				const Float currentCrossStart = horizontal ? current.computed.position.y : current.computed.position.x;
				const Float currentCrossEnd = currentCrossStart +
					(horizontal ? current.computed.size.y : current.computed.size.x);
				const Float itemCrossStart = horizontal ? item.computed.position.y : item.computed.position.x;
				const Float itemCrossEnd = itemCrossStart +
					(horizontal ? item.computed.size.y : item.computed.size.x);
				const Float overlap = std::max(0.0f, std::min(currentCrossEnd, itemCrossEnd) -
					std::max(currentCrossStart, itemCrossStart));
				const Float crossDistance = std::abs(horizontal ? offset.y : offset.x);
				const Float score = forward + crossDistance * 2.0f - overlap * 0.5f;
				if (score < bestScore) { bestScore = score; best = node; }
			});
		return best.isValid() && focus(best);
	}

	void UiEventRouter::pushModalScope(const UiNodeHandle scope)
	{
		if (!owner.isValid(scope)) return;
		modalFocusRestoration.push_back(focused);
		modalScopes.push_back(scope);
		UiInteractionData data = interaction(scope);
		data.focusScope = true;
		data.modalScope = true;
		setInteraction(scope, data);
		if (!isDescendantOf(focused, scope))
		{
			clearFocus();
			(void)moveFocusTab(false);
		}
	}

	void UiEventRouter::popModalScope(const UiNodeHandle scope)
	{
		for (SizeT index = modalScopes.size(); index > 0; --index)
		{
			if (modalScopes[index - 1] != scope) continue;
			const UiNodeHandle restore = index - 1 < modalFocusRestoration.size() ?
				modalFocusRestoration[index - 1] : UiNodeHandle{};
			modalScopes.erase(modalScopes.begin() + static_cast<std::ptrdiff_t>(index - 1));
			if (index - 1 < modalFocusRestoration.size())
				modalFocusRestoration.erase(modalFocusRestoration.begin() + static_cast<std::ptrdiff_t>(index - 1));
			if (!focus(restore)) (void)moveFocusTab(false);
			return;
		}
	}

	void UiEventRouter::cleanInvalidState()
	{
		for (auto iterator = interactions.begin(); iterator != interactions.end();)
			if (!owner.isValid(iterator->first)) iterator = interactions.erase(iterator); else ++iterator;
		listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
			[this](const Listener& value) { return !owner.isValid(value.node); }), listeners.end());
		pendingListeners.erase(std::remove_if(pendingListeners.begin(), pendingListeners.end(),
			[this](const Listener& value) { return !owner.isValid(value.node); }), pendingListeners.end());
		defaults.erase(std::remove_if(defaults.begin(), defaults.end(),
			[this](const Action& value) { return !owner.isValid(value.node); }), defaults.end());
		cancels.erase(std::remove_if(cancels.begin(), cancels.end(),
			[this](const Action& value) { return !owner.isValid(value.node); }), cancels.end());
		if (!owner.isValid(focused)) focused = {};
		for (PointerState& state : pointers)
		{
			if (!owner.isValid(state.captured)) state.captured = {};
			if (!owner.isValid(state.pressed)) state.pressed = {};
			if (!owner.isValid(state.dragging)) state.dragging = {};
			if (!owner.isValid(state.hovered)) state.hovered = {};
		}
	}

	void UiEventRouter::applyDeferredOperations()
	{
		if (dispatchDepth) return;
		Vector<DeferredOperation> pending;
		pending.swap(deferred);
		for (const DeferredOperation& operation : pending)
		{
			if (operation.type == DeferredType::RemoveListener) (void)removeListener(operation.listener);
			else clearNode(operation.node);
		}
		if (!pendingListeners.empty())
		{
			listeners.insert(listeners.end(),
				std::make_move_iterator(pendingListeners.begin()),
				std::make_move_iterator(pendingListeners.end()));
			pendingListeners.clear();
		}
		cleanInvalidState();
	}

	void UiEventRouter::consumeSystemCursorRequests(Bool& capture, Bool& release)
	{
		capture = pendingSystemCursorCapture;
		release = pendingSystemCursorRelease;
		pendingSystemCursorCapture = false;
		pendingSystemCursorRelease = false;
	}

	void UiEventRouter::reset()
	{
		cancelAllPointers();
		clearFocus();
		pendingSystemCursorCapture = false;
		pendingSystemCursorRelease = false;
		listeners.clear();
		pendingListeners.clear();
		defaults.clear();
		cancels.clear();
		deferred.clear();
		modalScopes.clear();
		modalFocusRestoration.clear();
		interactions.clear();
		commandDispatcher = {};
		worldMapper = {};
	}
}
