#pragma once

#include "Basics.hpp"

enum class ThingType
{
	undefined,
	SolarSystem,
	Star,
	Planet,
	Moon,
	Continent,
	Lake,
	Ocean,
	Island,
};

struct Thing
{
	ThingType type = ThingType::undefined;
	Map<String, String> description;
	Vector<Thing> content;
	Thing* parentThing = nullptr;
	Vector<Thing* > neighborThing;

	// generic info
	Index id{};
	Float size{};
	Vec3  position{};

	Thing();
	explicit Thing(ThingType type);

	void render();
	void generate();
	void update() const {}
};

struct SolarSystem : Thing
{
	SolarSystem();
};

void generateSolarSystem(Thing& thing);
void renderSolarSystem(Thing& thing);

struct Star : Thing
{
	Star();
};

void generateStar(Thing& thing);
void renderStar(Thing& thing);

struct Moon : Thing
{
	Moon();
};

void generateMoon(Thing& thing);
void renderMoon(Thing& thing);

struct Planet : Thing
{
	Planet();
};

void generatePlanet(Thing& thing);
void renderPlanet(Thing& thing);

struct Continent : Thing
{
	Continent();
};

void generateContinent(Thing& thing);
void renderContinent(Thing& thing);

struct Ocean : Thing
{
	Ocean();
};

void generateOcean(Thing& thing);
void renderOcean(Thing& thing);

struct Lake : Thing
{
	Lake();
};

void generateLake(Thing& thing);
void renderLake(Thing& thing);

struct Island : Thing
{
	Island();
};

void generateIsland(Thing& thing);
void renderIsland(Thing& thing);

struct Everything/* : public GameRenderer */
{
	SolarSystem solarSystem;

	void generate();
	void render();
};
