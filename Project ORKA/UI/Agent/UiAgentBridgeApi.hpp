#pragma once

#include "../../Agent/AgentValue.hpp"

struct Window;

namespace Ui
{
	class UiHost;

	[[nodiscard]] UiHost* findUiHost(Window& window);

	[[nodiscard]] AgentValue buildUiInspectResult(Window& window);
	[[nodiscard]] AgentValue agentUiListCommands(Window& window);
	[[nodiscard]] AgentValue agentUiInvokeCommand(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiListProperties(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiGetProperty(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiSetProperty(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiFocusNode(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiActivateNode(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiSetNodeValue(Window& window, const AgentValue& params);
	[[nodiscard]] AgentValue agentUiGetCursorState(Window& window);
}
