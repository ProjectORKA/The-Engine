
#pragma once

#include "Basics.hpp"
#include "PlanetSystem.hpp"
#include "PlanetCamera.hpp"

//#define SIMPLE_PLANET_RENDERER_DISTANCE 3
//
//struct SimpleQuadtreeNodeRenderData {
//	QuadtreeNode* equivalentQuadtreeNode = nullptr;
//};
//
//using RenderArea = SimpleQuadtreeNodeRenderData[SIMPLE_PLANET_RENDERER_DISTANCE][SIMPLE_PLANET_RENDERER_DISTANCE];
//
//using RenderLayers = RenderArea[64];
//
//
//struct SimplePlanetRenderer {
//
//	RenderLayers renderlayer;
//	PlanetSystem* planetSystemPtr = nullptr;
//
//
//	void create(PlanetSystem * planetSystem) {
//		planetSystemPtr = planetSystem;
//	};
//	void render(PlanetCamera & camera) {
//
//		//
//
//
//		renderlayer[0][0][0];
//	};
//};