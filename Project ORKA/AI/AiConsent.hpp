#pragma once

#include "../OrkaTypes.hpp"

struct AiConsentRecord
{
	Bool accepted = false;
	Int version = 0;
	String acceptedAtUtc;
};

[[nodiscard]] AiConsentRecord loadAiConsent();
[[nodiscard]] Bool saveAiConsent(const AiConsentRecord& record);
[[nodiscard]] Bool hasValidAiConsent();
[[nodiscard]] Bool acceptAiConsent();
