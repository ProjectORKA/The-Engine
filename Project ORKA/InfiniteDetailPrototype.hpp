#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Random.hpp"

enum class ThingType {
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

struct Thing;

using ThingDescription = Map<String,String>;
using ThingContent = Vector<Thing>;

struct Thing {
	ThingType type = ThingType::undefined;
	ThingDescription description;
	ThingContent content;
	Thing* parentThing = nullptr;
	Vector<Thing* > neightbourThing;

	//generic info
	Float size;
	Vec3 position;
	Index id;


	Thing();
	Thing(ThingType type);

	void generate();
	void render();
	void update() {};
};

struct SolarSystem : public Thing {
	SolarSystem();
};
void generateSolarSystem(Thing& thing);
void renderSolarSystem(Thing& thing);

struct Star : public Thing {
	Star();
};
void generateStar(Thing& thing);
void renderStar(Thing& thing);

struct Moon: public Thing {
	Moon();
};
void generateMoon(Thing& thing);
void renderMoon(Thing& thing);

struct Planet : public Thing {
	Planet();
};
void generatePlanet(Thing& thing);
void renderPlanet(Thing& thing);

struct Continent : public Thing {
	Continent();
};
void generateContinent(Thing& thing);
void renderContinent(Thing& thing);

struct Ocean : public Thing {
	Ocean();
};
void generateOcean(Thing& thing);
void renderOcean(Thing& thing);

struct Lake : public Thing {
	Lake();
};
void generateLake(Thing& thing);
void renderLake(Thing& thing);

struct Island : public Thing {
	Island();
};
void generateIsland(Thing& thing);
void renderIsland(Thing& thing);

struct Everything/* : public GameRenderer */ {
	SolarSystem solarSystem;
	
	void generate();
	void render();
};