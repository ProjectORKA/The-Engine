#include "AiChatAgent.hpp"

#include "AiToolRegistry.hpp"

AiChatAgentResult runAiChatAgent(
	const AiOpenAiClientConfig& config,
	Vector<AiChatMessage> messages,
	const String& systemPrompt,
	const Bool enableTools,
	const Int maxToolRounds)
{
	AiChatAgentResult agentResult;

	for (Int round = 0; round < maxToolRounds; ++round)
	{
		const AiChatCompletionResult completion = aiChatCompletion(config, messages, systemPrompt, enableTools);
		if (!completion.succeeded)
		{
			agentResult.error = completion.error;
			return agentResult;
		}

		if (completion.toolCalls.empty())
		{
			agentResult.succeeded = true;
			agentResult.assistantText = completion.assistantText;
			return agentResult;
		}

		AiChatMessage assistantMessage;
		assistantMessage.role = "assistant";
		assistantMessage.content = completion.assistantText;
		assistantMessage.toolCalls = completion.toolCalls;
		messages.push_back(std::move(assistantMessage));

		for (const AiToolCall& call : completion.toolCalls)
		{
			const AiToolExecutionResult toolResult = executeAiTool(call.name, call.argumentsJson);
			agentResult.toolTrace.push_back(
				call.name + "(" + call.argumentsJson + ") → "
				+ (toolResult.succeeded ? toolResult.output.substr(0, 120) : toolResult.error));

			AiChatMessage toolMessage;
			toolMessage.role = "tool";
			toolMessage.toolCallId = call.id;
			toolMessage.content = toolResult.succeeded ? toolResult.output : ("Error: " + toolResult.error);
			messages.push_back(std::move(toolMessage));
		}
	}

	agentResult.error = "Tool loop exceeded maximum rounds.";
	return agentResult;
}
