#include "TerrainTiles.hpp"

#include "Random.hpp"
#include "Math.hpp"
#include "Settings.hpp"

Vector<TerrainTile> generateTerrainTiles(TerrainTile input) {
	Vector<TerrainTile> tiles;
	tiles.resize(4);

	Double ne = randomFloat();
	Double nw = randomFloat();
	Double se = randomFloat();
	Double sw = randomFloat();

	Double average = (ne + nw + se + sw) / 4.0;

	ne -= average;
	nw -= average;
	se -= average;
	sw -= average;

	Double magnitude = max(max(abs(ne), abs(nw)), max(abs(se), abs(sw)));

	ne /= magnitude;
	nw /= magnitude;
	se /= magnitude;
	sw /= magnitude;

	ne *= MAX_TERRAIN_TILE_HEIGHT;
	nw *= MAX_TERRAIN_TILE_HEIGHT;
	se *= MAX_TERRAIN_TILE_HEIGHT;
	sw *= MAX_TERRAIN_TILE_HEIGHT;

	tiles[0].height = input.height + ne;
	tiles[1].height = input.height + nw;
	tiles[2].height = input.height + se;
	tiles[3].height = input.height + sw;

	tiles[0].level = input.level + 1;
	tiles[1].level = input.level + 1;
	tiles[2].level = input.level + 1;
	tiles[3].level = input.level + 1;

	return tiles;
}
Vector<TerrainTile> generateSmoothTerrainTiles(TerrainTile input, TerrainTile north, TerrainTile south, TerrainTile east, TerrainTile west) {
	Vector<TerrainTile> tiles;
	tiles.resize(4);

	Double ne = randomFloat();
	Double nw = randomFloat();
	Double se = randomFloat();
	Double sw = randomFloat();

	Double average = (ne + nw + se + sw) / 4.0;

	ne -= average;
	nw -= average;
	se -= average;
	sw -= average;

	Double magnitude = max(max(abs(ne), abs(nw)), max(abs(se), abs(sw)));

	ne /= magnitude;
	nw /= magnitude;
	se /= magnitude;
	sw /= magnitude;

	ne *= MAX_TERRAIN_TILE_HEIGHT;
	nw *= MAX_TERRAIN_TILE_HEIGHT;
	se *= MAX_TERRAIN_TILE_HEIGHT;
	sw *= MAX_TERRAIN_TILE_HEIGHT;

	ne /= pow(TERRAIN_ROUGHNESS, input.level);
	nw /= pow(TERRAIN_ROUGHNESS, input.level);
	se /= pow(TERRAIN_ROUGHNESS, input.level);
	sw /= pow(TERRAIN_ROUGHNESS, input.level);

	ne = dlerp(input.height, (north.height + east.height) / 2.0, 0.5) + ne;
	nw = dlerp(input.height, (north.height + west.height) / 2.0, 0.5) + nw;
	se = dlerp(input.height, (south.height + east.height) / 2.0, 0.5) + se;
	sw = dlerp(input.height, (south.height + west.height) / 2.0, 0.5) + sw;

	tiles[0].height = ne;
	tiles[1].height = nw;
	tiles[2].height = se;
	tiles[3].height = sw;

	tiles[0].level = input.level + 1;
	tiles[1].level = input.level + 1;
	tiles[2].level = input.level + 1;
	tiles[3].level = input.level + 1;

	return tiles;
}
