
#include "Basics.hpp"

struct TerrainTile {
	ULL x;
	ULL y;

	Short level;

	Bool iswater;
	Float height;
	
	///////////////
	Float vegetation;
	Float temperature;

	//GLOBAL SCALE
	//continents
	//islands
	//oceans

	//WATER FEATURES
	//rivers
	//deltas
	//lakes
	//waterfalls

	//LAND FEATURES
	//peninsula
	//mountains
	//hills
	//valleys
	//craters

	//SMALL LAND FEATURES
	//rocks
	//caves
	//steps
	//overhangs
	//pits
	
	//TEMPERATURE
	//snow
	//glaciers
	//deserts (basically the default)
	
	//VEGETATION
	//dirt
	//forest
	//rainforest
	//clearing

	//MAGMA LAYER DRIFT
	//fault lines
	//volcanoes

	//tundra
	
};



