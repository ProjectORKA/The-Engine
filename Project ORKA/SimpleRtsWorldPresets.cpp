#include "SimpleRtsWorldPresets.hpp"

#include "Agent/AgentValue.hpp"
#include "File.hpp"
#include "ResourceManager.hpp"

#include <algorithm>
#include <filesystem>

namespace
{
	String sanitizePresetId(const String& raw)
	{
		String cleaned;
		cleaned.reserve(raw.size());
		for (const char c : raw)
		{
			const Bool ok = (c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')
				|| c == '-' || c == '_';
			if (ok) cleaned.push_back(c);
		}
		if (cleaned.empty()) cleaned = "preset";
		return cleaned;
	}

	String presetLabelFromId(const String& id)
	{
		if (id == "default") return "Default (engine)";
		if (id == "small-test") return "Small test world";
		if (id == "flat-arena") return "Flat arena";
		if (id == "full-simulation") return "Full simulation";
		String label = id;
		for (char& c : label)
			if (c == '-' || c == '_') c = ' ';
		return label;
	}

	Bool readTextFile(const Path& path, String& outText)
	{
		outText.clear();
		if (!std::filesystem::exists(path)) return false;
		InFile file(path);
		if (!file.isOpen) return false;
		String line;
		while (file.readLine(line))
			outText += line;
		return true;
	}

	Bool writeTextFile(const Path& path, const String& text)
	{
		std::filesystem::create_directories(path.parent_path());
		OutFile file(path);
		if (!file.isOpen) return false;
		file.write(text.c_str(), text.size());
		return true;
	}

	Path presetFilePath(const Path& directory, const String& presetId)
	{
		return directory / (sanitizePresetId(presetId) + ".json");
	}

	void ensureBuiltInPresetFiles()
	{
		const Path directory = SimpleRtsWorldPresets::builtInPresetsDirectory();
		std::filesystem::create_directories(directory);

		struct BuiltInSeed
		{
			const char* id;
			SimpleRtsWorldSettings (*factory)();
		};

		const BuiltInSeed seeds[] = {
			{"default", SimpleRtsWorldSettings::defaults},
			{"small-test", SimpleRtsWorldSettings::smallTestWorld},
			{"flat-arena", SimpleRtsWorldSettings::flatArenaWorld},
			{"full-simulation", SimpleRtsWorldSettings::fullSimulationWorld},
		};

		for (const BuiltInSeed& seed : seeds)
		{
			const Path filePath = presetFilePath(directory, seed.id);
			if (std::filesystem::exists(filePath)) continue;
			const SimpleRtsWorldSettings settings = seed.factory();
			const String payload = serializeAgentJson(settings.toJson());
			(void)writeTextFile(filePath, payload);
		}
	}

	Bool parseSettingsFile(const Path& path, SimpleRtsWorldSettings& outSettings, String* errorOut)
	{
		String text;
		if (!readTextFile(path, text))
		{
			if (errorOut) *errorOut = "Could not read preset file.";
			return false;
		}
		const AgentJsonParseResult parsed = parseAgentJson(text);
		if (!parsed.succeeded())
		{
			if (errorOut) *errorOut = parsed.error.empty() ? "Invalid preset JSON." : parsed.error;
			return false;
		}
		outSettings = SimpleRtsWorldSettings::fromJson(parsed.value, errorOut);
		return true;
	}
}

Path SimpleRtsWorldPresets::builtInPresetsDirectory()
{
	return resourceManager.getOrkaDataPath() / "SimpleRts" / "presets";
}

Path SimpleRtsWorldPresets::userPresetsDirectory()
{
	return resourceManager.getOrkaCachePath() / "simple-rts" / "presets";
}

Vector<SimpleRtsWorldPresetEntry> SimpleRtsWorldPresets::listPresets()
{
	ensureBuiltInPresetFiles();
	std::map<String, SimpleRtsWorldPresetEntry> merged;

	const auto ingestDirectory = [&merged](const Path& directory, const Bool builtIn)
		{
			if (!std::filesystem::exists(directory)) return;
			for (const auto& entry : std::filesystem::directory_iterator(directory))
			{
				if (!entry.is_regular_file()) continue;
				if (entry.path().extension() != ".json") continue;
				const String id = entry.path().stem().string();
				SimpleRtsWorldPresetEntry preset;
				preset.id = id;
				preset.label = presetLabelFromId(id);
				preset.builtIn = builtIn;
				merged[id] = std::move(preset);
			}
		};

	ingestDirectory(builtInPresetsDirectory(), true);
	ingestDirectory(userPresetsDirectory(), false);

	Vector<SimpleRtsWorldPresetEntry> presets;
	presets.reserve(merged.size());
	for (const auto& pair : merged)
		presets.push_back(pair.second);

	std::sort(presets.begin(), presets.end(),
		[](const SimpleRtsWorldPresetEntry& a, const SimpleRtsWorldPresetEntry& b)
		{
			if (a.builtIn != b.builtIn) return a.builtIn > b.builtIn;
			return a.label < b.label;
		});
	return presets;
}

Bool SimpleRtsWorldPresets::loadPreset(const String& presetId, SimpleRtsWorldSettings& outSettings, String* errorOut)
{
	ensureBuiltInPresetFiles();
	const String id = sanitizePresetId(presetId);

	const Path userPath = presetFilePath(userPresetsDirectory(), id);
	if (std::filesystem::exists(userPath))
		return parseSettingsFile(userPath, outSettings, errorOut);

	const Path builtInPath = presetFilePath(builtInPresetsDirectory(), id);
	if (std::filesystem::exists(builtInPath))
		return parseSettingsFile(builtInPath, outSettings, errorOut);

	if (errorOut) *errorOut = "Preset not found: " + id;
	return false;
}

Bool SimpleRtsWorldPresets::saveUserPreset(const String& presetId, const SimpleRtsWorldSettings& settings, String* errorOut)
{
	const String id = sanitizePresetId(presetId);
	if (id.empty())
	{
		if (errorOut) *errorOut = "Preset name is required.";
		return false;
	}

	const String payload = serializeAgentJson(settings.toJson());
	if (!writeTextFile(presetFilePath(userPresetsDirectory(), id), payload))
	{
		if (errorOut) *errorOut = "Could not write preset file.";
		return false;
	}
	setLastPresetId(id);
	return true;
}

Bool SimpleRtsWorldPresets::deleteUserPreset(const String& presetId, String* errorOut)
{
	const String id = sanitizePresetId(presetId);
	const Path path = presetFilePath(userPresetsDirectory(), id);
	if (!std::filesystem::exists(path))
	{
		if (errorOut) *errorOut = "Only saved user presets can be deleted.";
		return false;
	}
	std::error_code ec;
	if (!std::filesystem::remove(path, ec))
	{
		if (errorOut) *errorOut = "Could not delete preset file.";
		return false;
	}
	return true;
}

String SimpleRtsWorldPresets::loadLastPresetId()
{
	const Path tagFile = userPresetsDirectory().parent_path() / "last-preset.txt";
	String text;
	if (!readTextFile(tagFile, text) || text.empty()) return "default";
	return sanitizePresetId(text);
}

void SimpleRtsWorldPresets::setLastPresetId(const String& presetId)
{
	const Path tagFile = userPresetsDirectory().parent_path() / "last-preset.txt";
	std::filesystem::create_directories(tagFile.parent_path());
	(void)writeTextFile(tagFile, sanitizePresetId(presetId));
}

Bool SimpleRtsWorldPresets::loadLastSettings(SimpleRtsWorldSettings& outSettings)
{
	const String lastId = loadLastPresetId();
	String error;
	if (loadPreset(lastId, outSettings, &error)) return true;
	return loadPreset("default", outSettings, &error);
}
