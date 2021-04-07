#pragma once

#include "Basics.hpp"
#include "Math.hpp"

struct SimpleCamera {
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.01f;
	Float farClipValue = 10000.0f;
	Float mouseSensitivity = 0.0015f;
	Vec3 accelerationVector = Vec3(0);

	Vec3 forwardVector = { 0.0f, 1.0f, 0.0f };
	Vec3 rightVector = { 1.0f, 0.0f, 0.0f };
	Vec3 upVector = { 0.0f, 0.0f, 1.0f };

	void setLocation(Vec3 location);
	void rotate(Float rotationX, Float rotationZ);
	Matrix viewMatrix();
	Matrix projectionMatrix(float aspectRatio);
private:
	Vec3 location = Vec3(0);
	Float rotationX = 0;
	Float rotationZ = 0;
};