#include "AiOpenAiClient.hpp"

#include "../Agent/AgentValue.hpp"
#include "AiHttpClient.hpp"
#include "AiToolRegistry.hpp"

namespace
{
	AgentValue messageToJson(const AiChatMessage& message)
	{
		AgentValue::Object object{
			{"role", AgentValue(message.role)},
			{"content", AgentValue(message.content)}
		};

		if (!message.toolCallId.empty())
			object["tool_call_id"] = AgentValue(message.toolCallId);

		if (!message.toolCalls.empty())
		{
			AgentValue::Array toolCalls;
			for (const AiToolCall& call : message.toolCalls)
			{
				toolCalls.push_back(AgentValue::Object{
					{"id", AgentValue(call.id)},
					{"type", AgentValue("function")},
					{"function", AgentValue(AgentValue::Object{
						{"name", AgentValue(call.name)},
						{"arguments", AgentValue(call.argumentsJson)}
						})}
					});
			}
			object["tool_calls"] = AgentValue(std::move(toolCalls));
		}

		return AgentValue(std::move(object));
	}

	String buildChatPayload(
		const AiOpenAiClientConfig& config,
		const Vector<AiChatMessage>& messages,
		const String& systemPrompt,
		const Bool includeTools)
	{
		AgentValue::Array payloadMessages;
		if (!systemPrompt.empty())
		{
			payloadMessages.push_back(AgentValue::Object{
				{"role", AgentValue("system")},
				{"content", AgentValue(systemPrompt)}
				});
		}
		for (const AiChatMessage& message : messages)
			payloadMessages.push_back(messageToJson(message));

		AgentValue::Object root{
			{"model", AgentValue(config.model)},
			{"stream", AgentValue(false)},
			{"messages", AgentValue(std::move(payloadMessages))}
		};

		if (includeTools)
		{
			root["tools"] = aiOpenAiToolDefinitions();
			root["tool_choice"] = AgentValue("auto");
		}

		return serializeAgentJson(AgentValue(std::move(root)));
	}

	Vector<AiToolCall> parseToolCalls(const AgentValue& message)
	{
		Vector<AiToolCall> calls;
		const AgentValue* toolCalls = message.find("tool_calls");
		if (!toolCalls || !toolCalls->isArray()) return calls;

		for (const AgentValue& entry : toolCalls->asArray())
		{
			const AgentValue* id = entry.find("id");
			const AgentValue* function = entry.find("function");
			if (!id || !function || !id->isString() || !function->isObject()) continue;
			const AgentValue* name = function->find("name");
			const AgentValue* arguments = function->find("arguments");
			if (!name || !arguments || !name->isString()) continue;

			AiToolCall call;
			call.id = id->asString();
			call.name = name->asString();
			call.argumentsJson = arguments->isString() ? arguments->asString() : serializeAgentJson(*arguments);
			calls.push_back(std::move(call));
		}
		return calls;
	}
}

Bool aiServerResponds(const AiOpenAiClientConfig& config)
{
	const AiHttpResponse response = aiHttpRequest(
		config.host,
		config.port,
		"GET",
		"/health",
		{},
		{});
	if (response.statusCode == 200) return true;

	const AiHttpResponse models = aiHttpRequest(
		config.host,
		config.port,
		"GET",
		"/v1/models",
		{},
		{});
	return models.statusCode == 200;
}

AiChatCompletionResult aiChatCompletion(
	const AiOpenAiClientConfig& config,
	const Vector<AiChatMessage>& messages,
	const String& systemPrompt,
	const Bool includeTools)
{
	AiChatCompletionResult result;
	const String payload = buildChatPayload(config, messages, systemPrompt, includeTools);
	const AiHttpResponse response = aiHttpRequest(
		config.host,
		config.port,
		"POST",
		"/v1/chat/completions",
		payload,
		{{"Content-Type", "application/json"}});

	if (response.statusCode != 200)
	{
		result.error = response.error.empty()
			? "Chat request failed (HTTP " + toString(response.statusCode) + ")"
			: response.error;
		if (!response.body.empty()) result.error += ": " + response.body.substr(0, 256);
		return result;
	}

	const AgentJsonParseResult parsed = parseAgentJson(response.body);
	if (!parsed.succeeded())
	{
		result.error = "Invalid chat response JSON: " + parsed.error;
		return result;
	}

	const AgentValue* choices = parsed.value.find("choices");
	if (!choices || !choices->isArray() || choices->asArray().empty())
	{
		result.error = "Chat response missing choices.";
		return result;
	}

	const AgentValue& first = choices->asArray().front();
	const AgentValue* message = first.find("message");
	if (!message)
	{
		result.error = "Chat response missing message.";
		return result;
	}

	result.toolCalls = parseToolCalls(*message);
	if (!result.toolCalls.empty())
	{
		result.succeeded = true;
		if (const AgentValue* content = message->find("content"))
			if (content->isString()) result.assistantText = content->asString();
		return result;
	}

	const AgentValue* content = message->find("content");
	if (!content || !content->isString())
	{
		result.error = "Chat response missing assistant content.";
		return result;
	}

	result.succeeded = true;
	result.assistantText = content->asString();
	return result;
}
