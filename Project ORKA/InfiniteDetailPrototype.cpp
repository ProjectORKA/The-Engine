#include "InfiniteDetailPrototype.hpp"
#include "Debug.hpp"
#include "Random.hpp"

Thing::Thing() = default;

Thing::Thing(const ThingType type)
{
	this->type = type;
}

void Thing::generate()
{
	switch(type)
	{
		case ThingType::SolarSystem:
			generateSolarSystem(*this);
			break;
		case ThingType::Star:
			generateStar(*this);
			break;
		case ThingType::Planet:
			generatePlanet(*this);
			break;
		case ThingType::Moon:
			generateMoon(*this);
			break;
		case ThingType::Continent:
			generateContinent(*this);
			break;
		case ThingType::Lake:
			generateLake(*this);
			break;
		case ThingType::Ocean:
			generateOcean(*this);
			break;
		case ThingType::Island:
			generateIsland(*this);
			break;
		default:
			logError("");
	}
}

void Thing::render()
{
	switch(type)
	{
		case ThingType::SolarSystem:
			renderSolarSystem(*this);
			break;
		case ThingType::Star:
			renderStar(*this);
			break;
		case ThingType::Planet:
			renderPlanet(*this);
			break;
		case ThingType::Moon:
			renderMoon(*this);
			break;
		case ThingType::Continent:
			renderContinent(*this);
			break;
		case ThingType::Lake:
			renderLake(*this);
			break;
		case ThingType::Ocean:
			renderOcean(*this);
			break;
		case ThingType::Island:
			renderIsland(*this);
			break;
		default:
			logError("");
	}
}

void generateSolarSystem(Thing& thing)
{
	// add sun
	thing.content.emplace_back(ThingType::Star);

	// add planets
	const UInt numberOfPlanets = randomUIntFast(10);
	for(UInt i = 0; i < numberOfPlanets; i++) thing.content.emplace_back(ThingType::Planet);

	// generate contents
	for(auto c : thing.content) c.generate();
}

void renderSolarSystem(Thing& thing) {}

void generateStar(Thing& thing) {}

void renderStar(Thing& thing) {}

void generateMoon(Thing& thing) {}

void renderMoon(Thing& thing) {}

void generatePlanet(Thing& thing) {}

void renderPlanet(Thing& thing) {}

void generateContinent(Thing& thing) {}

void renderContinent(Thing& thing) {}

void generateOcean(Thing& thing) {}

void renderOcean(Thing& thing) {}

void generateLake(Thing& thing) {}

void renderLake(Thing& thing) {}

void generateIsland(Thing& thing) {}

void renderIsland(Thing& thing) {}

Planet::Planet()
{
	type = ThingType::Planet;
}

void Everything::generate()
{
	solarSystem.generate();
}

void Everything::render()
{
	solarSystem.render();
}

Star::Star()
{
	type = ThingType::Star;
}

SolarSystem::SolarSystem()
{
	type = ThingType::SolarSystem;
}

Moon::Moon()
{
	type = ThingType::Moon;
}

Continent::Continent()
{
	type = ThingType::Continent;
}

Ocean::Ocean()
{
	type = ThingType::Ocean;
}

Lake::Lake()
{
	type = ThingType::Lake;
}

Island::Island()
{
	type = ThingType::Island;
}
