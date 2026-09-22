#include "AiConsent.hpp"

#include "../Agent/AgentValue.hpp"
#include "../File.hpp"
#include "AiConfig.hpp"
#include "AiPaths.hpp"

#include <chrono>
#include <filesystem>

namespace
{
	String utcNowIso()
	{
		const auto now = std::chrono::system_clock::now();
		const std::time_t t = std::chrono::system_clock::to_time_t(now);
		std::tm utc{};
		gmtime_s(&utc, &t);
		char buffer[32] = {};
		std::strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%SZ", &utc);
		return buffer;
	}
}

AiConsentRecord loadAiConsent()
{
	AiConsentRecord record;
	if (!std::filesystem::exists(AiPaths::consentFile())) return record;

	InFile file(AiPaths::consentFile());
	if (!file.isOpen) return record;

	String text;
	String line;
	while (file.readLine(line))
		text += line;

	const AgentJsonParseResult parsed = parseAgentJson(text);
	if (!parsed.succeeded() || !parsed.value.isObject()) return record;

	if (const AgentValue* accepted = parsed.value.find("accepted"))
		record.accepted = accepted->asBool(false);
	if (const AgentValue* version = parsed.value.find("version"))
		record.version = static_cast<Int>(version->asInteger(0));
	if (const AgentValue* acceptedAt = parsed.value.find("acceptedAtUtc"))
		record.acceptedAtUtc = acceptedAt->asString();

	return record;
}

Bool saveAiConsent(const AiConsentRecord& record)
{
	AiPaths::ensureLayout();
	AgentValue::Object object;
	object["accepted"] = AgentValue(record.accepted);
	object["version"] = AgentValue(static_cast<I64>(record.version));
	object["acceptedAtUtc"] = AgentValue(record.acceptedAtUtc);
	const String payload = serializeAgentJson(AgentValue(std::move(object)));

	OutFile file(AiPaths::consentFile());
	if (!file.isOpen) return false;
	file.write(payload.c_str(), payload.size());
	return true;
}

Bool hasValidAiConsent()
{
	const AiConsentRecord record = loadAiConsent();
	return record.accepted && record.version >= AiConfig::consentVersion;
}

Bool acceptAiConsent()
{
	AiConsentRecord record;
	record.accepted = true;
	record.version = AiConfig::consentVersion;
	record.acceptedAtUtc = utcNowIso();
	return saveAiConsent(record);
}
