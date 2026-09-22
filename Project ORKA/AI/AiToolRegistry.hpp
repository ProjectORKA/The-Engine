#pragma once

#include "../Agent/AgentValue.hpp"
#include "../OrkaTypes.hpp"

struct AiToolDescriptor
{
	String name;
	String description;
	Bool implemented = false;
};

struct AiToolExecutionResult
{
	Bool succeeded = false;
	String output;
	String error;
};

[[nodiscard]] Vector<AiToolDescriptor> aiRegisteredTools();
[[nodiscard]] AgentValue aiOpenAiToolDefinitions();
[[nodiscard]] AiToolExecutionResult executeAiTool(const String& name, const String& argumentsJson);
