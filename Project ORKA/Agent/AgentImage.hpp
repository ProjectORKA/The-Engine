#pragma once

#include "AgentValue.hpp"

struct Window;

[[nodiscard]] AgentValue captureAgentScreenshot(Window& window, const AgentValue& params);
