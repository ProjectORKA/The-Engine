#include "UiAgentBridgeApi.hpp"

#include "../Authoring/UiWidgetBridge.hpp"
#include "../Binding/UiBindingRegistry.hpp"
#include "../UiHost.hpp"
#include "../Widgets/UiWidgets.hpp"
#include "../../UIElement.hpp"
#include "../../UserInterface.hpp"
#include "../../Window.hpp"

#include <stdexcept>

namespace Ui
{
	namespace
	{
		constexpr SizeT maximumInspectNodes = 8192;
		constexpr SizeT maximumPropertyEntries = 4096;

		[[nodiscard]] String semanticRoleName(const UiSemanticRole role)
		{
			switch (role)
			{
			case UiSemanticRole::Application: return "application";
			case UiSemanticRole::Button: return "button";
			case UiSemanticRole::CheckBox: return "checkbox";
			case UiSemanticRole::Dialog: return "dialog";
			case UiSemanticRole::Image: return "image";
			case UiSemanticRole::Link: return "link";
			case UiSemanticRole::List: return "list";
			case UiSemanticRole::ListItem: return "listitem";
			case UiSemanticRole::Menu: return "menu";
			case UiSemanticRole::MenuItem: return "menuitem";
			case UiSemanticRole::ProgressBar: return "progressbar";
			case UiSemanticRole::Radio: return "radio";
			case UiSemanticRole::Slider: return "slider";
			case UiSemanticRole::Text: return "text";
			case UiSemanticRole::TextBox: return "textbox";
			default: return "none";
			}
		}

		[[nodiscard]] String widgetKindName(const UiWidgetKind kind)
		{
			switch (kind)
			{
			case UiWidgetKind::Box: return "box";
			case UiWidgetKind::Text: return "text";
			case UiWidgetKind::Image: return "image";
			case UiWidgetKind::Icon: return "icon";
			case UiWidgetKind::ScrollView: return "scrollview";
			case UiWidgetKind::VirtualList: return "virtuallist";
			case UiWidgetKind::FocusScope: return "focusscope";
			case UiWidgetKind::Portal: return "portal";
			case UiWidgetKind::Overlay: return "overlay";
			case UiWidgetKind::WorldSurface: return "worldsurface";
			case UiWidgetKind::GameViewport: return "gameviewport";
			case UiWidgetKind::Button: return "button";
			case UiWidgetKind::Toggle: return "toggle";
			case UiWidgetKind::Checkbox: return "checkbox";
			case UiWidgetKind::RadioGroup: return "radiogroup";
			case UiWidgetKind::Slider: return "slider";
			case UiWidgetKind::Range: return "range";
			case UiWidgetKind::Progress: return "progress";
			case UiWidgetKind::TextField: return "textfield";
			case UiWidgetKind::NumberField: return "numberfield";
			case UiWidgetKind::TextArea: return "textarea";
			case UiWidgetKind::Select: return "select";
			case UiWidgetKind::Combo: return "combo";
			default: return "unknown";
			}
		}

		[[nodiscard]] String uiValueTypeName(const UiValueType type)
		{
			switch (type)
			{
			case UiValueType::Bool: return "bool";
			case UiValueType::Number: return "number";
			case UiValueType::String: return "string";
			case UiValueType::Array: return "array";
			case UiValueType::Object: return "object";
			default: return "null";
			}
		}

		[[nodiscard]] AgentValue uiValueToAgent(const UiValue& value)
		{
			switch (value.type)
			{
			case UiValueType::Bool: return AgentValue(value.boolean);
			case UiValueType::Number: return AgentValue(value.number);
			case UiValueType::String: return AgentValue(value.string);
			case UiValueType::Array:
			{
				AgentValue::Array items;
				items.reserve(value.array.size());
				for (const UiValue& item : value.array) items.push_back(uiValueToAgent(item));
				return AgentValue(std::move(items));
			}
			case UiValueType::Object:
			{
				AgentValue::Object fields;
				for (const auto& [key, item] : value.object) fields[key] = uiValueToAgent(item);
				return AgentValue(std::move(fields));
			}
			default: return AgentValue(nullptr);
			}
		}

		[[nodiscard]] UiValue agentToUiValue(const AgentValue& value)
		{
			if (value.isNull()) return UiValue::null();
			if (value.isBool()) return UiValue::fromBool(value.asBool());
			if (value.isNumber()) return UiValue::fromNumber(value.asNumber());
			if (value.isString()) return UiValue::fromString(value.asString());
			if (value.isArray())
			{
				Vector<UiValue> items;
				items.reserve(value.asArray().size());
				for (const AgentValue& item : value.asArray()) items.push_back(agentToUiValue(item));
				return UiValue::fromArray(std::move(items));
			}
			if (value.isObject())
			{
				Map<String, UiValue> fields;
				for (const auto& [key, item] : value.asObject()) fields[key] = agentToUiValue(item);
				return UiValue::fromObject(std::move(fields));
			}
			throw std::runtime_error("Unsupported property or argument value type");
		}

		[[nodiscard]] const AgentValue& requireParam(const AgentValue& params, const String& name)
		{
			const AgentValue* value = params.find(name);
			if (!value) throw std::runtime_error("Missing parameter: " + name);
			return *value;
		}

		[[nodiscard]] String optionalId(const AgentValue& params)
		{
			const AgentValue* id = params.find("id");
			if (!id || !id->isString() || id->asString().empty())
				throw std::runtime_error("id must be a non-empty string");
			return id->asString();
		}

		[[nodiscard]] UiNodeHandle resolveNode(UiHost& host, const AgentValue& params)
		{
			if (const AgentValue* id = params.find("id"); id && id->isString() && !id->asString().empty())
			{
				if (const UiHostAgentSurface& surface = host.agentSurface(); surface.widgetBridge)
				{
					const UiNodeHandle fromWidget = surface.widgetBridge->nodeForId(id->asString());
					if (fromWidget.isValid()) return fromWidget;
				}
				UiNodeHandle found;
				host.document().traverse([&](const UiNodeHandle handle, const UiNode& node)
					{
						if (!found.isValid() && node.authored.id == id->asString()) found = handle;
					});
				if (!found.isValid()) throw std::runtime_error("Unknown ui id: " + id->asString());
				return found;
			}

			const AgentValue& indexValue = requireParam(params, "nodeIndex");
			const AgentValue& generationValue = requireParam(params, "nodeGeneration");
			if (!indexValue.isInteger() || !generationValue.isInteger())
				throw std::runtime_error("nodeIndex and nodeGeneration must be integers");
			const UiNodeHandle handle{
				static_cast<UInt>(indexValue.asInteger()),
				static_cast<UInt>(generationValue.asInteger())};
			if (!host.document().isValid(handle)) throw std::runtime_error("UI node handle is stale or invalid");
			return handle;
		}

		[[nodiscard]] AgentValue semanticsObject(const UiSemanticData& semantics)
		{
			return AgentValue::Object{
				{"role", semanticRoleName(semantics.role)},
				{"label", semantics.label},
				{"value", semantics.value},
				{"selected", semantics.selected},
				{"checked", semantics.checked},
				{"showTextCaret", semantics.showTextCaret},
				{"selectionStart", static_cast<U64>(semantics.selectionStart)},
				{"selectionEnd", static_cast<U64>(semantics.selectionEnd)},
				{"caretByte", static_cast<U64>(semantics.caretByte)}
			};
		}

		AgentValue inspectLegacy(UIElement& root)
		{
			AgentValue::Array nodes;
			Function<void(UIElement&, Int)> visit = [&](UIElement& element, const Int depth)
			{
				if (nodes.size() >= maximumInspectNodes) return;
				const IVec2 position = element.getPosition();
				const IVec2 size = element.getSize();
				nodes.emplace_back(AgentValue::Object{
					{"id", element.getId()}, {"parentId", element.parent ? element.parent->getId() : maxUInt},
					{"depth", depth}, {"name", element.getDebugName()},
					{"x", position.x}, {"y", position.y}, {"width", size.x}, {"height", size.y},
					{"children", static_cast<U64>(element.children.size())}
				});
				for (UIElement* child : element.children) if (child) visit(*child, depth + 1);
			};
			visit(root, 0);
			return AgentValue::Object{{"kind", "legacy"}, {"nodes", std::move(nodes)}};
		}
	}

	namespace
	{
		UiHost* findUiHostInTree(UIElement& element)
		{
			if (auto* host = dynamic_cast<UiHost*>(&element)) return host;
			for (UIElement* child : element.children)
				if (child) if (UiHost* host = findUiHostInTree(*child)) return host;
			return nullptr;
		}
	}

	UiHost* findUiHost(Window& window)
	{
		return findUiHostInTree(window);
	}

	AgentValue buildUiInspectResult(Window& window)
	{
		AgentValue result = inspectLegacy(window);
		UiHost* host = findUiHost(window);
		if (!host) return result;

		const UiHostAgentSurface& surface = host->agentSurface();
		AgentValue::Array nodes;
		AgentValue::Array commands;
		if (surface.commands)
		{
			const UiCommandRegistrySnapshot snapshot = surface.commands->snapshot();
			for (const auto& [name, descriptor] : snapshot.commands)
			{
				AgentValue::Array argumentTypes;
				for (const UiValueType type : descriptor.signature.arguments)
					argumentTypes.push_back(uiValueTypeName(type));
				commands.emplace_back(AgentValue::Object{
					{"name", name},
					{"arguments", std::move(argumentTypes)}
				});
			}
		}

		host->document().traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				if (nodes.size() >= maximumInspectNodes) return;
				const UiSemanticData& semantics = node.state.semantics;
				AgentValue::Object entry{
					{"index", handle.index}, {"generation", handle.generation},
					{"parentIndex", node.parentHandle().index},
					{"type", node.authored.typeName}, {"id", node.authored.id}, {"text", node.authored.text},
					{"x", node.computed.position.x}, {"y", node.computed.position.y},
					{"width", node.computed.size.x}, {"height", node.computed.size.y},
					{"visible", node.state.visible}, {"enabled", node.state.enabled},
					{"focused", node.state.focused}, {"hovered", node.state.hovered},
					{"active", node.state.active},
					{"semanticLabel", semantics.label},
					{"semanticValue", semantics.value},
					{"semantics", semanticsObject(semantics)}
				};
				if (surface.widgetBridge && surface.widgets && !node.authored.id.empty())
				{
					const UiWidgetHandle widget = surface.widgetBridge->findById(node.authored.id);
					if (widget.isValid() && surface.widgets->isValid(widget))
					{
						entry["widgetKind"] = widgetKindName(surface.widgets->kind(widget));
						entry["widgetIndex"] = widget.index;
						entry["widgetGeneration"] = widget.generation;
					}
				}
				nodes.emplace_back(std::move(entry));
			});

		result.asObject()["modern"] = AgentValue::Object{
			{"nodeCount", static_cast<U64>(host->document().nodeCount())},
			{"focusedIndex", host->eventRouter().focusedNode().index},
			{"focusedGeneration", host->eventRouter().focusedNode().generation},
			{"commands", std::move(commands)},
			{"commandCount", static_cast<U64>(commands.size())},
			{"nodes", std::move(nodes)}
		};
		return result;
	}

	AgentValue agentUiListCommands(Window& window)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiCommandRegistry* commands = host->agentSurface().commands;
		if (!commands) return AgentValue::Object{{"commands", AgentValue::Array{}}, {"count", 0U}};
		const UiCommandRegistrySnapshot snapshot = commands->snapshot();
		AgentValue::Array listed;
		for (const auto& [name, descriptor] : snapshot.commands)
		{
			AgentValue::Array argumentTypes;
			for (const UiValueType type : descriptor.signature.arguments)
				argumentTypes.push_back(uiValueTypeName(type));
			listed.emplace_back(AgentValue::Object{
				{"name", name},
				{"arguments", std::move(argumentTypes)}
			});
		}
		return AgentValue::Object{
			{"commands", std::move(listed)},
			{"count", static_cast<U64>(listed.size())},
			{"version", snapshot.version}
		};
	}

	AgentValue agentUiInvokeCommand(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiHostAgentSurface& surface = host->agentSurface();
		if (!surface.commands) throw std::runtime_error("Window has no authored UI command registry");
		const String name = requireParam(params, "name").asString();
		Vector<UiValue> arguments;
		if (const AgentValue* args = params.find("arguments"))
		{
			if (!args->isArray()) throw std::runtime_error("arguments must be an array");
			arguments.reserve(args->asArray().size());
			for (const AgentValue& item : args->asArray()) arguments.push_back(agentToUiValue(item));
		}
		Bool accepted = false;
		host->applyAgentMutation([&]
			{
				accepted = surface.commands->invoke(name, arguments);
			});
		if (!accepted) throw std::runtime_error("Command was not found or arguments did not match");
		return AgentValue::Object{{"accepted", true}, {"name", name}};
	}

	AgentValue agentUiListProperties(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiPropertyRegistry* properties = host->agentSurface().properties;
		if (!properties) return AgentValue::Object{{"properties", AgentValue::Array{}}, {"count", 0U}};
		String prefix;
		if (const AgentValue* value = params.find("prefix"); value && value->isString()) prefix = value->asString();
		const UiPropertyRegistrySnapshot snapshot = properties->snapshot();
		AgentValue::Array listed;
		for (const auto& [path, descriptor] : snapshot.properties)
		{
			if (listed.size() >= maximumPropertyEntries) break;
			if (!prefix.empty() && path.rfind(prefix, 0) != 0) continue;
			listed.emplace_back(AgentValue::Object{
				{"path", path},
				{"type", uiValueTypeName(descriptor.value.type)},
				{"value", uiValueToAgent(descriptor.value)},
				{"version", descriptor.version}
			});
		}
		return AgentValue::Object{
			{"properties", std::move(listed)},
			{"count", static_cast<U64>(listed.size())},
			{"version", snapshot.version}
		};
	}

	AgentValue agentUiGetProperty(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiPropertyRegistry* properties = host->agentSurface().properties;
		if (!properties) throw std::runtime_error("Window has no authored UI property registry");
		const String path = requireParam(params, "path").asString();
		UiValue value;
		U64 version = 0;
		if (!properties->read(path, value, &version)) throw std::runtime_error("Unknown property path: " + path);
		return AgentValue::Object{
			{"path", path},
			{"type", uiValueTypeName(value.type)},
			{"value", uiValueToAgent(value)},
			{"version", version}
		};
	}

	AgentValue agentUiSetProperty(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiHostAgentSurface& surface = host->agentSurface();
		if (!surface.properties) throw std::runtime_error("Window has no authored UI property registry");
		const String path = requireParam(params, "path").asString();
		const AgentValue& valueParam = requireParam(params, "value");
		const UiValue value = agentToUiValue(valueParam);
		Bool accepted = false;
		host->applyAgentMutation([&]
			{
				accepted = surface.properties->set(path, value);
				if (accepted && surface.syncAuthoredBindings) surface.syncAuthoredBindings();
			});
		if (!accepted) throw std::runtime_error("Property path could not be set");
		return AgentValue::Object{{"accepted", true}, {"path", path}, {"value", uiValueToAgent(value)}};
	}

	AgentValue agentUiFocusNode(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiNodeHandle node = resolveNode(*host, params);
		Bool accepted = false;
		host->applyAgentMutation([&]
			{
				accepted = host->eventRouter().focus(node);
			});
		return AgentValue::Object{
			{"accepted", accepted},
			{"id", params.find("id") ? params.find("id")->asString() : String{}},
			{"nodeIndex", node.index},
			{"nodeGeneration", node.generation}
		};
	}

	AgentValue agentUiActivateNode(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiNodeHandle node = resolveNode(*host, params);
		Bool accepted = false;
		host->applyAgentMutation([&]
			{
				UiRoutedEvent activate{
					.type = UiRoutedEventType::Activate,
					.target = node,
					.currentTarget = node
				};
				accepted = host->eventRouter().dispatchRouted(activate);
			});
		return AgentValue::Object{
			{"accepted", accepted},
			{"id", params.find("id") ? params.find("id")->asString() : String{}},
			{"nodeIndex", node.index},
			{"nodeGeneration", node.generation}
		};
	}

	AgentValue agentUiSetNodeValue(Window& window, const AgentValue& params)
	{
		UiHost* host = findUiHost(window);
		if (!host) throw std::runtime_error("Window has no UiHost");
		const UiHostAgentSurface& surface = host->agentSurface();
		if (!surface.widgetBridge || !surface.widgets)
			throw std::runtime_error("Window has no interactive widget bridge");
		const String id = optionalId(params);
		const UiWidgetHandle widget = surface.widgetBridge->findById(id);
		if (!widget.isValid() || !surface.widgets->isValid(widget))
			throw std::runtime_error("Unknown widget id: " + id);
		const AgentValue& valueParam = requireParam(params, "value");
		const Bool notify = params.find("notify") ? params.find("notify")->asBool(true) : true;
		Bool accepted = false;
		host->applyAgentMutation([&]
			{
				const UiWidgetKind kind = surface.widgets->kind(widget);
				if (valueParam.isBool())
				{
					if (kind == UiWidgetKind::Toggle || kind == UiWidgetKind::Checkbox)
						accepted = surface.widgets->setBoolValue(widget, valueParam.asBool(), notify);
					else throw std::runtime_error("Widget does not accept boolean values");
				}
				else if (valueParam.isNumber())
				{
					if (kind == UiWidgetKind::Slider || kind == UiWidgetKind::NumberField)
						accepted = surface.widgets->setNumberValue(widget, valueParam.asNumber(), notify);
					else if (kind == UiWidgetKind::Progress)
						accepted = surface.widgets->setProgress(widget, valueParam.asNumber());
					else if (kind == UiWidgetKind::Range)
						throw std::runtime_error("Range widgets require ui.node.setRange");
					else throw std::runtime_error("Widget does not accept numeric values");
				}
				else if (valueParam.isString())
				{
					if (surface.widgets->setTextValue(widget, valueParam.asString(), notify))
						accepted = true;
					else if (surface.widgets->setSelectedValue(widget, valueParam.asString(), notify))
						accepted = true;
					else throw std::runtime_error("Widget does not accept string values");
				}
				else throw std::runtime_error("value must be bool, number, or string");
				if (surface.syncAuthoredBindings) surface.syncAuthoredBindings();
			});
		if (!accepted) throw std::runtime_error("Widget value could not be applied");
		return AgentValue::Object{{"accepted", true}, {"id", id}};
	}

	namespace
	{
		[[nodiscard]] String standardCursorName(const ApiCursorType cursor)
		{
			switch (cursor)
			{
			case ApiCursorType::Hand: return "hand";
			case ApiCursorType::IBeam: return "ibeam";
			case ApiCursorType::Arrow:
			default: return "arrow";
			}
		}
	}

	AgentValue agentUiGetCursorState(Window& window)
	{
		const DVec2 mouse = window.mousePosition();
		const Bool gameCapture = ui.capturing && ui.focusedWindow == &window;
		AgentValue::Object result{
			{"glfwCursorHidden", window.systemCursorHiddenByGlfw()},
			{"uiCursorCaptureRequested", window.uiCursorCapturePending()},
			{"gameCapture", gameCapture},
			{"queuedStandardCursor", standardCursorName(window.queuedStandardCursor())},
			{"mouseX", mouse.x},
			{"mouseY", mouse.y},
			{"frameIndex", window.agentFrameIndex.load()}
		};

		UiHost* host = findUiHost(window);
		if (!host) return result;

		const UiEventRouter& router = host->eventRouter();
		const UiNodeHandle hovered = router.hoveredNode();
		const UiNodeHandle captured = router.capturedPointerNode();
		result["hoveredIndex"] = hovered.index;
		result["hoveredGeneration"] = hovered.generation;
		result["capturedIndex"] = captured.index;
		result["capturedGeneration"] = captured.generation;

		const UiHostAgentSurface& surface = host->agentSurface();
		if (surface.widgets)
			result["numberScrubActive"] = surface.widgets->isNumberScrubActive();

		if (const UiNode* hoveredNode = host->document().get(hovered))
			result["hoveredType"] = hoveredNode->authored.typeName;
		if (const UiNode* capturedNode = host->document().get(captured))
			result["capturedType"] = capturedNode->authored.typeName;

		return result;
	}
}
