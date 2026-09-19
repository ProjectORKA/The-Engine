#include "AgentEngineCommands.hpp"

#include "AgentBridge.hpp"
#include "AgentImage.hpp"
#include "../UserInterface.hpp"
#include "../UI/UiHost.hpp"

#include <cctype>
#include <stdexcept>

namespace
{
	const AgentValue emptySchema = AgentValue::Object{{"type", "object"}};

	const AgentValue& require(const AgentValue& params, const String& name)
	{
		const AgentValue* value = params.find(name);
		if(!value) throw std::runtime_error("Missing parameter: " + name);
		return *value;
	}

	Double number(const AgentValue& params, const String& name)
	{
		const AgentValue& value = require(params, name);
		if(!value.isNumber()) throw std::runtime_error(name + " must be a number");
		return value.asNumber();
	}

	Int integer(const AgentValue& params, const String& name)
	{
		const AgentValue& value = require(params, name);
		if(!value.isInteger()) throw std::runtime_error(name + " must be an integer");
		return static_cast<Int>(value.asInteger());
	}

	Bool boolean(const AgentValue& params, const String& name)
	{
		const AgentValue& value = require(params, name);
		if(!value.isBool()) throw std::runtime_error(name + " must be a boolean");
		return value.asBool();
	}

	String string(const AgentValue& params, const String& name)
	{
		const AgentValue& value = require(params, name);
		if(!value.isString()) throw std::runtime_error(name + " must be a string");
		return value.asString();
	}

	Window& contextWindow(const AgentCommandContext& context)
	{
		if(context.window) return *context.window;
		for(Window& window : ui.windows)
			if(context.hasTargetWindow && window.id == context.targetWindowId) return window;
		throw std::runtime_error("Window no longer exists");
	}

	AgentValue windowValue(const Window& window)
	{
		const IVec2 framebuffer = window.getContentSize();
		const DVec2 mouse = window.mousePosition();
		return AgentValue::Object{
			{"id", window.id},
			{"title", window.title},
			{"framebufferWidth", framebuffer.x},
			{"framebufferHeight", framebuffer.y},
			{"visible", window.isVisible},
			{"focused", ui.focusedWindow == &window},
			{"shouldClose", window.shouldClose()},
			{"frameIndex", window.agentFrameIndex.load()},
			{"mouseX", mouse.x},
			{"mouseY", mouse.y}
		};
	}

	Int mouseButton(const String& name)
	{
		if(name == "left") return 0;
		if(name == "right") return 1;
		if(name == "middle") return 2;
		if(name == "x1") return 3;
		if(name == "x2") return 4;
		throw std::runtime_error("Unknown mouse button");
	}

	Int keyCode(String name)
	{
		if(name.size() == 1)
		{
			const unsigned char character = static_cast<unsigned char>(name[0]);
			return std::toupper(character);
		}
		for(Char& character : name) character = static_cast<Char>(
			std::toupper(static_cast<unsigned char>(character)));
		const Map<String, Int> names{
			{"SPACE", GLFW_KEY_SPACE}, {"ENTER", GLFW_KEY_ENTER}, {"ESC", GLFW_KEY_ESCAPE},
			{"ESCAPE", GLFW_KEY_ESCAPE}, {"TAB", GLFW_KEY_TAB}, {"BACKSPACE", GLFW_KEY_BACKSPACE},
			{"DELETE", GLFW_KEY_DELETE}, {"INSERT", GLFW_KEY_INSERT}, {"HOME", GLFW_KEY_HOME},
			{"END", GLFW_KEY_END}, {"PAGEUP", GLFW_KEY_PAGE_UP}, {"PAGEDOWN", GLFW_KEY_PAGE_DOWN},
			{"LEFT", GLFW_KEY_LEFT}, {"RIGHT", GLFW_KEY_RIGHT}, {"UP", GLFW_KEY_UP},
			{"DOWN", GLFW_KEY_DOWN}, {"CTRL", GLFW_KEY_LEFT_CONTROL},
			{"CONTROL", GLFW_KEY_LEFT_CONTROL}, {"SHIFT", GLFW_KEY_LEFT_SHIFT},
			{"ALT", GLFW_KEY_LEFT_ALT}, {"SUPER", GLFW_KEY_LEFT_SUPER},
			{"F1", GLFW_KEY_F1}, {"F2", GLFW_KEY_F2}, {"F3", GLFW_KEY_F3}, {"F4", GLFW_KEY_F4},
			{"F5", GLFW_KEY_F5}, {"F6", GLFW_KEY_F6}, {"F7", GLFW_KEY_F7}, {"F8", GLFW_KEY_F8},
			{"F9", GLFW_KEY_F9}, {"F10", GLFW_KEY_F10}, {"F11", GLFW_KEY_F11}, {"F12", GLFW_KEY_F12}
		};
		const auto found = names.find(name);
		if(found == names.end()) throw std::runtime_error("Unknown key name");
		return found->second;
	}

	Int modifierMask(const AgentValue& params)
	{
		const AgentValue* modifiers = params.find("modifiers");
		if(!modifiers) return 0;
		if(!modifiers->isArray()) throw std::runtime_error("modifiers must be an array");
		Int result = 0;
		for(const AgentValue& value : modifiers->asArray())
		{
			if(!value.isString()) throw std::runtime_error("modifier names must be strings");
			String name = value.asString();
			for(Char& character : name) character = static_cast<Char>(
				std::toupper(static_cast<unsigned char>(character)));
			if(name == "SHIFT") result |= GLFW_MOD_SHIFT;
			else if(name == "CTRL" || name == "CONTROL") result |= GLFW_MOD_CONTROL;
			else if(name == "ALT") result |= GLFW_MOD_ALT;
			else if(name == "SUPER") result |= GLFW_MOD_SUPER;
			else throw std::runtime_error("Unknown modifier");
		}
		return result;
	}

	AgentValue inspectLegacy(UIElement& root)
	{
		AgentValue::Array nodes;
		Function<void(UIElement&, Int)> visit = [&](UIElement& element, const Int depth)
		{
			if(nodes.size() >= 4096) return;
			const IVec2 position = element.getPosition();
			const IVec2 size = element.getSize();
			nodes.emplace_back(AgentValue::Object{
				{"id", element.getId()}, {"parentId", element.parent ? element.parent->getId() : maxUInt},
				{"depth", depth}, {"name", element.getDebugName()},
				{"x", position.x}, {"y", position.y}, {"width", size.x}, {"height", size.y},
				{"children", static_cast<U64>(element.children.size())}
			});
			for(UIElement* child : element.children) if(child) visit(*child, depth + 1);
		};
		visit(root, 0);
		return AgentValue::Object{{"kind", "legacy"}, {"nodes", std::move(nodes)}};
	}

	Ui::UiHost* findUiHost(UIElement& element)
	{
		if(auto* host = dynamic_cast<Ui::UiHost*>(&element)) return host;
		for(UIElement* child : element.children)
			if(child) if(Ui::UiHost* host = findUiHost(*child)) return host;
		return nullptr;
	}

	AgentValue inspectUi(Window& window)
	{
		AgentValue result = inspectLegacy(window);
		Ui::UiHost* host = findUiHost(window);
		if(!host) return result;
		AgentValue::Array nodes;
		host->document().traverse([&](const Ui::UiNodeHandle handle, Ui::UiNode& node)
		{
			if(nodes.size() >= 8192) return;
			const Ui::UiSemanticData& semantics = node.state.semantics;
			nodes.emplace_back(AgentValue::Object{
				{"index", handle.index}, {"generation", handle.generation},
				{"parentIndex", node.parentHandle().index},
				{"type", node.authored.typeName}, {"id", node.authored.id}, {"text", node.authored.text},
				{"x", node.computed.position.x}, {"y", node.computed.position.y},
				{"width", node.computed.size.x}, {"height", node.computed.size.y},
				{"visible", node.state.visible}, {"enabled", node.state.enabled},
				{"focused", node.state.focused}, {"hovered", node.state.hovered},
				{"semanticLabel", semantics.label}, {"semanticValue", semantics.value}
			});
		});
		result.asObject()["modern"] = AgentValue::Object{
			{"nodeCount", static_cast<U64>(host->document().nodeCount())},
			{"nodes", std::move(nodes)}
		};
		return result;
	}

	void add(const String& method, const String& summary, const AgentThreadAffinity affinity,
		const Bool requiresWindow, AgentCommandHandler handler)
	{
		AgentCommand command;
		command.description = {method, summary, affinity, emptySchema, AgentValue::Object{}, requiresWindow};
		command.handler = std::move(handler);
		String error;
		if(!agentBridge.registerCommand(command, &error) && error != "Method is already registered")
			throw std::runtime_error("Could not register " + method + ": " + error);
	}
}

void registerAgentEngineCommands()
{
	add("agent.invoke", "Invokes another registered method", AgentThreadAffinity::Io, false,
		[](const AgentCommandContext&, const AgentValue&) { return AgentValue(); });
	add("window.list", "Lists live engine windows", AgentThreadAffinity::Main, false,
		[](const AgentCommandContext&, const AgentValue&)
		{
			AgentValue::Array windows;
			for(const Window& window : ui.windows) windows.push_back(windowValue(window));
			return AgentValue::Object{{"windows", std::move(windows)}};
		});
	add("window.focus", "Focuses a window", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue&)
		{
			Window& window = contextWindow(context);
			External::glfwFocusWindow(window.apiWindow);
			ui.focusWindow(window);
			return windowValue(window);
		});
	add("window.resize", "Resizes a window framebuffer", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			Window& window = contextWindow(context);
			window.resize(integer(params, "width"), integer(params, "height"));
			return windowValue(window);
		});
	add("window.close", "Requests orderly window closure", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue&)
		{
			Window& window = contextWindow(context);
			apiSetWindowShouldClose(window.apiWindow, true);
			return AgentValue::Object{{"closing", true}};
		});
	add("input.pointerMove", "Moves the synthetic pointer", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			Window& window = contextWindow(context);
			window.injectPointerMove({number(params, "x"), number(params, "y")});
			return windowValue(window);
		});
	add("input.mouseButton", "Changes a synthetic mouse button state", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			Window& window = contextWindow(context);
			window.injectMouseButton(mouseButton(string(params, "button")), boolean(params, "down"));
			return AgentValue::Object{{"accepted", true}};
		});
	add("input.click", "Clicks a synthetic mouse button", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			Window& window = contextWindow(context);
			const AgentValue* x = params.find("x");
			const AgentValue* y = params.find("y");
			if((x == nullptr) != (y == nullptr)) throw std::runtime_error("x and y must be supplied together");
			if(x)
			{
				if(!x->isNumber() || !y->isNumber()) throw std::runtime_error("x and y must be numbers");
				window.injectPointerMove({x->asNumber(), y->asNumber()});
			}
			const Int button = mouseButton(params.find("button") && params.find("button")->isString()
				? params.find("button")->asString() : "left");
			Int count = params.find("clickCount") ? static_cast<Int>(params.find("clickCount")->asInteger(1)) : 1;
			count = (std::clamp)(count, 1, 100);
			for(Int i = 0; i < count; ++i)
			{
				window.injectMouseButton(button, true);
				window.injectMouseButton(button, false);
			}
			return AgentValue::Object{{"accepted", true}, {"clickCount", count}};
		});
	add("input.scroll", "Injects pointer wheel movement", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			contextWindow(context).injectScroll(number(params, "deltaX"), number(params, "deltaY"));
			return AgentValue::Object{{"accepted", true}};
		});
	add("input.key", "Injects a key transition or press", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			Window& window = contextWindow(context);
			const AgentValue& key = require(params, "key");
			const Int code = key.isInteger() ? static_cast<Int>(key.asInteger())
				: key.isString() ? keyCode(key.asString()) : throw std::runtime_error("key must be a name or code");
			const Int modifiers = modifierMask(params);
			if(const AgentValue* down = params.find("down"))
			{
				if(!down->isBool()) throw std::runtime_error("down must be boolean");
				window.injectKey(code, down->asBool(), modifiers,
					params.find("repeat") && params.find("repeat")->asBool());
			}
			else
			{
				window.injectKey(code, true, modifiers);
				window.injectKey(code, false, modifiers);
			}
			return AgentValue::Object{{"accepted", true}, {"keyCode", code}};
		});
	add("input.text", "Injects UTF-8 text", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			contextWindow(context).injectText(string(params, "text"));
			return AgentValue::Object{{"accepted", true}};
		});
	add("screenshot.capture", "Captures the next completed backbuffer as PNG",
		AgentThreadAffinity::Render, true,
		[](const AgentCommandContext& context, const AgentValue& params)
		{
			return captureAgentScreenshot(contextWindow(context), params);
		});
	add("ui.inspect", "Returns legacy and retained UI structure", AgentThreadAffinity::Render, true,
		[](const AgentCommandContext& context, const AgentValue&)
		{
			return inspectUi(contextWindow(context));
		});
	add("ui.layoutDebug.toggle", "Toggles retained UI layout debug overlay", AgentThreadAffinity::Main, true,
		[](const AgentCommandContext& context, const AgentValue&)
		{
			Window& window = contextWindow(context);
			Ui::UiHost* host = findUiHost(window);
			if(!host) throw std::runtime_error("Window has no UiHost");
			host->requestLayoutDebugToggle();
			return AgentValue::Object{
				{"requested", true},
				{"windowId", window.id}
			};
		});
	add("agent.waitFrames", "Waits for a number of render frames", AgentThreadAffinity::Render, true,
		[](const AgentCommandContext& context, const AgentValue&)
		{
			return AgentValue::Object{{"frameIndex", contextWindow(context).agentFrameIndex.load()}};
		});
}
