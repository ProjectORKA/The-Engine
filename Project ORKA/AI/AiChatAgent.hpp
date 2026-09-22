#pragma once

#include "AiOpenAiClient.hpp"

struct AiChatAgentResult
{
	Bool succeeded = false;
	String assistantText;
	String error;
	Vector<String> toolTrace;
};

[[nodiscard]] AiChatAgentResult runAiChatAgent(
	const AiOpenAiClientConfig& config,
	Vector<AiChatMessage> messages,
	const String& systemPrompt,
	Bool enableTools,
	Int maxToolRounds = 8);
