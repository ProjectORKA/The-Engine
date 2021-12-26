#pragma once

#include "Math.hpp"
#include "Time.hpp"
#include "Basics.hpp"
#include "Uniforms.hpp"
#include "TerrainSystem.hpp"
#include "RenderRegion.hpp"
//
//#include "SimpleCamera.hpp"
//

//
//struct PlanetCamera : public SimpleCamera{
//
//	ULLVec3 chunkLocation = ULLVec3(ULLONG_MAX/2);
//	Int speedMultiplier = INITIAL_CAMERA_SPEED;
//	Float cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, INITIAL_CAMERA_SPEED);
//
//	void update(Float delta)override;
//	void render(Uniforms& uniforms, Float aspectRatio) override;
//};