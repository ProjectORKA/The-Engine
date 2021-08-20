#pragma once

#include "GPUMesh.hpp"
#include "SimpleCamera.hpp"

//step 1 generate base mesh

CPUMesh importRayMarchedTerrainBaseMesh() {
	CPUMesh cpuMesh;
	cpuMesh.load("rayMarchedTerrainBaseMesh");
	return cpuMesh;
}

struct RayMarchedTerrainRenderingSystem {
	CPUMesh baseMesh;
	GPUMesh renderableBaseMesh;

	void create() {
		//baseMesh = CPUMesh("rayMarchedTerrainBaseMesh");
		renderableBaseMesh.upload(baseMesh);
	};
	void update() {
		renderableBaseMesh.upload(baseMesh);
	};
	void render(SimpleCamera camera) {
		camera.renderOnlyRot();


	}
};