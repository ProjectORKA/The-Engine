#pragma once

#include "OrkaTypes.hpp"
#include "SimpleRtsWorldSettings.hpp"

struct SimpleRtsWorldPresetEntry
{
	String id;
	String label;
	Bool builtIn = false;
};

struct SimpleRtsWorldPresets
{
	[[nodiscard]] static Path builtInPresetsDirectory();
	[[nodiscard]] static Path userPresetsDirectory();

	[[nodiscard]] static Vector<SimpleRtsWorldPresetEntry> listPresets();
	[[nodiscard]] static Bool loadPreset(const String& presetId, SimpleRtsWorldSettings& outSettings, String* errorOut = nullptr);
	[[nodiscard]] static Bool saveUserPreset(const String& presetId, const SimpleRtsWorldSettings& settings, String* errorOut = nullptr);
	[[nodiscard]] static Bool deleteUserPreset(const String& presetId, String* errorOut = nullptr);

	[[nodiscard]] static String loadLastPresetId();
	static void setLastPresetId(const String& presetId);

	[[nodiscard]] static Bool loadLastSettings(SimpleRtsWorldSettings& outSettings);
};
