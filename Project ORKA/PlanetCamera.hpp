#pragma once

#include "Math.hpp"
#include "Time.hpp"
#include "Basics.hpp"
#include "Uniforms.hpp"
#include "TerrainSystem.hpp"
#include "RenderRegion.hpp"

#include "SimpleCamera.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_SPEED_MULTIPLIER 1.2f			//controls the de/increase in speed by this amount when scrolling
#define CAMERA_TERRAIN_LIMIT false

struct PlanetCamera : public SimpleCamera{

	ULLVec3 chunkLocation = ULLVec3(ULLONG_MAX/2);
	Int speedMultiplier = INITIAL_CAMERA_SPEED;
	Float cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, INITIAL_CAMERA_SPEED);

	void update(Float delta)override;
	void use(Uniforms& uniforms, Float aspectRatio);
};