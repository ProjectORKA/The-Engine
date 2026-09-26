#include "SimpleRtsWorldSettings.hpp"

#include "Agent/AgentValue.hpp"
#include "Math.hpp"

namespace
{
	Int readIntField(const AgentValue& object, const char* key, const Int fallback)
	{
		if (const AgentValue* field = object.find(key))
			return static_cast<Int>(field->asInteger(fallback));
		return fallback;
	}

	Float readFloatField(const AgentValue& object, const char* key, const Float fallback)
	{
		if (const AgentValue* field = object.find(key))
			return static_cast<Float>(field->asNumber(static_cast<Double>(fallback)));
		return fallback;
	}

	Bool readBoolField(const AgentValue& object, const char* key, const Bool fallback)
	{
		if (const AgentValue* field = object.find(key))
			return field->asBool(fallback);
		return fallback;
	}

	void clampSettings(SimpleRtsWorldSettings& settings)
	{
		settings.mapSize = clamp(settings.mapSize, 250.0f, 8000.0f);
		settings.terrainPointCount = clamp(settings.terrainPointCount, 500, 400000);
		settings.humans = clamp(settings.humans, 0, 50000);
		settings.rabbits = clamp(settings.rabbits, 0, 50000);
		settings.wolves = clamp(settings.wolves, 0, 50000);
		settings.trees = clamp(settings.trees, 0, 5000);
		settings.bushes = clamp(settings.bushes, 0, 50000);
		settings.berryBushes = clamp(settings.berryBushes, 0, 50000);
		settings.bodies = clamp(settings.bodies, 0, 10000);
	}
}

SimpleRtsWorldSettings SimpleRtsWorldSettings::defaults()
{
	return {};
}

SimpleRtsWorldSettings SimpleRtsWorldSettings::smallTestWorld()
{
	SimpleRtsWorldSettings settings;
	settings.mapSize = 600.0f;
	settings.terrainPointCount = 8000;
	settings.humans = 25;
	settings.rabbits = 40;
	settings.wolves = 8;
	settings.trees = 30;
	settings.bushes = 50;
	settings.berryBushes = 40;
	settings.bodies = 10;
	return settings;
}

SimpleRtsWorldSettings SimpleRtsWorldSettings::flatArenaWorld()
{
	SimpleRtsWorldSettings settings = smallTestWorld();
	settings.enableElevation = false;
	settings.enableWater = false;
	settings.mapSize = 800.0f;
	return settings;
}

SimpleRtsWorldSettings SimpleRtsWorldSettings::fullSimulationWorld()
{
	SimpleRtsWorldSettings settings;
	settings.mapSize = 3000.0f;
	settings.terrainPointCount = 250000;
	settings.humans = 10000;
	settings.rabbits = 10000;
	settings.wolves = 10000;
	settings.trees = 500;
	settings.bushes = 10000;
	settings.berryBushes = 10000;
	settings.bodies = 1000;
	return settings;
}

AgentValue SimpleRtsWorldSettings::toJson() const
{
	AgentValue::Object object;
	object["formatVersion"] = AgentValue(static_cast<I64>(formatVersion));
	object["mapSize"] = AgentValue(static_cast<Double>(mapSize));
	object["enableWater"] = AgentValue(enableWater);
	object["enableElevation"] = AgentValue(enableElevation);
	object["terrainPointCount"] = AgentValue(static_cast<I64>(terrainPointCount));
	object["humans"] = AgentValue(static_cast<I64>(humans));
	object["rabbits"] = AgentValue(static_cast<I64>(rabbits));
	object["wolves"] = AgentValue(static_cast<I64>(wolves));
	object["trees"] = AgentValue(static_cast<I64>(trees));
	object["bushes"] = AgentValue(static_cast<I64>(bushes));
	object["berryBushes"] = AgentValue(static_cast<I64>(berryBushes));
	object["bodies"] = AgentValue(static_cast<I64>(bodies));
	return AgentValue(std::move(object));
}

SimpleRtsWorldSettings SimpleRtsWorldSettings::fromJson(const AgentValue& value, String* errorOut)
{
	SimpleRtsWorldSettings settings = defaults();
	if (!value.isObject())
	{
		if (errorOut) *errorOut = "Preset root must be a JSON object.";
		return settings;
	}

	settings.mapSize = readFloatField(value, "mapSize", settings.mapSize);
	settings.enableWater = readBoolField(value, "enableWater", settings.enableWater);
	settings.enableElevation = readBoolField(value, "enableElevation", settings.enableElevation);
	settings.terrainPointCount = readIntField(value, "terrainPointCount", settings.terrainPointCount);
	settings.humans = readIntField(value, "humans", settings.humans);
	settings.rabbits = readIntField(value, "rabbits", settings.rabbits);
	settings.wolves = readIntField(value, "wolves", settings.wolves);
	settings.trees = readIntField(value, "trees", settings.trees);
	settings.bushes = readIntField(value, "bushes", settings.bushes);
	settings.berryBushes = readIntField(value, "berryBushes", settings.berryBushes);
	settings.bodies = readIntField(value, "bodies", settings.bodies);
	clampSettings(settings);
	return settings;
}
