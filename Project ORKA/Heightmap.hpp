#pragma once

#include "Basics.hpp"
#include "Settings.hpp"

struct AdvancedHeightMap
{
	[[nodiscard]] Bool             isLoaded() const;
	[[nodiscard]] LDouble          getUpperLimit() const;
	[[nodiscard]] LDouble          getLowerLimit() const;
	void                           setLoaded(Bool value);
	void                           setLowerLimit(LDouble value);
	void                           setUpperLimit(LDouble value);
	[[nodiscard]] Double           getHeight(Int x, Int y) const;
	[[nodiscard]] const LDouble (& getHeights() const)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE];
	void                           setHeights(const LDouble (&arr)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE]);

private:
	LDouble lowerLimit                                 = 0;
	LDouble upperLimit                                 = 0;
	LDouble height[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE] = {};
	Bool    loaded                                     = false;
};
