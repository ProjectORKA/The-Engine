#pragma once

#include "../OrkaTypes.hpp"

struct AiToolCall
{
	String id;
	String name;
	String argumentsJson;
};

struct AiChatMessage
{
	String role;
	String content;
	String toolCallId;
	Vector<AiToolCall> toolCalls;
};

struct AiChatCompletionResult
{
	Bool succeeded = false;
	String assistantText;
	String error;
	Vector<AiToolCall> toolCalls;
};

struct AiOpenAiClientConfig
{
	String host = "127.0.0.1";
	U16 port = 8765;
	String model = "qwen3.5-9b";
};

[[nodiscard]] Bool aiServerResponds(const AiOpenAiClientConfig& config);
[[nodiscard]] AiChatCompletionResult aiChatCompletion(
	const AiOpenAiClientConfig& config,
	const Vector<AiChatMessage>& messages,
	const String& systemPrompt = {},
	Bool includeTools = false);
