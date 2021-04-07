#pragma once

#include "Math.hpp"
#include "Time.hpp"
#include "Basics.hpp"
#include "Uniforms.hpp"
#include "ViewportSystem.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_SPEED_MULTIPLIER 1.2f		//controls the de/increase in speed by this amount when scrolling

struct PlanetCamera {

	//[TODO] make it work
	Vec3 chunkOffsetPerLevel[64] = {};

	Float cameraRotationX = -PI / 2;
	Float cameraRotationZ = -PI / 2;
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.01f;
	Float farClipValue = 10000.0f;
	
	Int speedMultiplier = INITIAL_CAMERA_SPEED;
	Vec3 accelerationVector = Vec3(0);

	Float cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, INITIAL_CAMERA_SPEED);
	Vec3 forwardVector = { 0.0f, 1.0f, 0.0f };
	Vec3 rightVector = { 1.0f, 0.0f, 0.0f };
	Vec3 upVector = { 0.0f, 0.0f, 1.0f };

	void addLocationOffset(Vec3 locationOffset);

	void rotate(float x, float y);
	void update(Time & renderTime);
	void render(Uniforms& uniforms, Viewport& currentViewport);

	Matrix projectionMatrix(float aspectRatio);
	Matrix viewMatrix();
	Matrix viewMatrixOnlyRot();

	Vec3 location = Vec3(0, 0, 0);
	ULLVec3 chunkLocation = ULLVec3(LLONG_MAX, LLONG_MAX, 0);
};