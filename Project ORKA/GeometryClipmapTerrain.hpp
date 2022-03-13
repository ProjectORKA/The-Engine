
#pragma once

#include "Basics.hpp"
#include "PerlinNoise.hpp"
#include "Renderer.hpp"
#include "Array2D.hpp"

#define CLIP_MAP_LEVELS 16
#define CLIP_MAP_SIZE 64

struct ClipMap {
	Vec2 location = Vec2(0);
	Float size = 1;

	Array2D<Float> heightMap = Array2D<Float>(CLIP_MAP_SIZE);
	
	void create(Float size) {
		this->size = size;
		
		for (UInt x = 0; x < CLIP_MAP_SIZE; x++) {
			for (UInt y = 0; y < CLIP_MAP_SIZE; y++) {
				calculateSample(x, y);
			}
		}
	}

	void calculateSample(UInt x, UInt y) {
		Vec2 sample = location + ((Vec2(x, y) / Vec2(CLIP_MAP_SIZE) - Vec2(0.5)) * size);
		heightMap.get(x,y) = noise.octaveNoise0_1(sample.x, sample.y, 8);
	}
};

struct ClipMapTerrain {
	ClipMap maps[CLIP_MAP_LEVELS];

	void create() {
		for (UInt i = 0; i < CLIP_MAP_LEVELS; i++) {
			maps[i].create(pow(2,-Float(i)));
		}
	}

	//void update() {
	//	for (UInt i = 0; i < CLIP_MAP_LEVELS; i++) {
	//		maps[i].update();
	//	}
	//}
};

struct ClipMapRenderer {
	GPUMesh terrain;
	
	void update(ClipMap map) {
		CPUMesh mesh;
		mesh.meshFromHeightmap(map.heightMap, CLIP_MAP_SIZE);
		terrain.upload(mesh);
	}

	void render(Renderer & renderer) {
		terrain.render(renderer.uniforms());
	}
};

struct ClipMapTerrainRenderer {
	ClipMapRenderer mapRenderers[CLIP_MAP_LEVELS];

	void create(ClipMapTerrain& terrain) {
		for (UInt i = 0; i < CLIP_MAP_LEVELS; i++) {
			mapRenderers[i].update(terrain.maps[i]);
		}
	}

	void render( Renderer& renderer) {
		for (UInt i = 0; i < CLIP_MAP_LEVELS; i++) {
			mapRenderers[i].render(renderer);
		}
	}
};