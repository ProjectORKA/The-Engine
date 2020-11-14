#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Time.hpp"

#define INITIAL_CAMERA_SPEED 100				//1 as fast as a human 400 as fast as light
#define CAMERA_SPEED_MULTIPLIER 1.2f		//controls the de/increase in speed by this amount when scrolling

struct Renderer;

struct Camera {
	//hard data
	Vec3 location = Vec3(0, 0, 0);
	ULLVec3 chunkLocation = ULLVec3(0, 0, 0);

	Float cameraRotationX = -PI / 2;
	Float cameraRotationZ = -PI / 2;
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.01f;
	Float farClipValue = 10000.0f;
	Float mouseSensitivity = 0.0015f;
	Int speedMultiplier = INITIAL_CAMERA_SPEED;
	Vec3 accelerationVector = Vec3(0);

	// soft data (temporary)
	Float cameraSpeed = pow(CAMERA_SPEED_MULTIPLIER, INITIAL_CAMERA_SPEED);
	Vec3 forwardVector = { 0.0f, 1.0f, 0.0f };
	Vec3 rightVector = { 1.0f, 0.0f, 0.0f };
	Vec3 upVector = { 0.0f, 0.0f, 1.0f };

	Matrix projectionMatrix(float aspectRatio);
	Matrix viewMatrix();
	Matrix viewMatrixOnlyRot();
	void update(Time & renderTime);
	void rotate(float x, float y) {
		cameraRotationX -= mouseSensitivity * y;
		cameraRotationZ += mouseSensitivity * x;

		//prevent looking upside down
		Float cap = PI / 2;

		if (cameraRotationX < -cap) {
			cameraRotationX = -cap;
		}
		if (cameraRotationX > +cap) {
			cameraRotationX = +cap;
		}

		//calculate directional vectors
		forwardVector = Vec3(
			cos(cameraRotationX) * sin(cameraRotationZ),
			cos(cameraRotationX) * cos(cameraRotationZ),
			sin(cameraRotationX)
		);

		rightVector = Vec3(
			-sin(cameraRotationZ - PI / 2),
			-cos(cameraRotationZ - PI / 2),
			0
		);

		upVector = glm::cross(rightVector, forwardVector);
	}
	void render(Renderer& renderer);
	void applySubChunkLocation();
	void processLocation(Time& renderTime);
};

struct CameraSystem {
	Vector<Camera> cameras;
	Index currentCamera = 0;
	
	void create();
	void destroy();

	void add();
	void select(Index cameraID);
	void render(Renderer & renderer);
	Camera& current();
};