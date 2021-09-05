#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Uniforms.hpp"
#include "Time.hpp"

struct SimpleCamera {
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.001f;
	Float farClipValue = 1000.0f;
	Vec3 accelerationVector = Vec3(0);

	Vec3 location = Vec3(0);
	Float rotationX = 0;
	Float rotationZ = 0;

	Vec3 forwardVector = { 0.0f, 1.0f, 0.0f };
	Vec3 rightVector = { 1.0f, 0.0f, 0.0f };
	Vec3 upVector = { 0.0f, 0.0f, 1.0f };

	virtual void update(Float delta);
	virtual void rotate(Vec2 rotation);
	virtual void render(Uniforms& uniforms, Float aspectRatio);

	Matrix viewMatrix();
	Matrix viewMatrixOnlyRot();
	Matrix projectionMatrix(Float aspectRatio);
};