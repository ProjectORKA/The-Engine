#pragma once

#include "Renderer.hpp"

struct Camera {
	Vec3 location = Vec3(0);
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.1f;
	Float farClipValue = 10000.0f;

	Vec3 upVector = {0.0f, 0.0f, 1.0f};
	Vec3 rightVector = {1.0f, 0.0f, 0.0f};
	Vec3 forwardVector = {0.0f, 1.0f, 0.0f};

	void rotate(DVec2 rotation);
	void render(Renderer& renderer);
	void setRotation(const DVec3& rotation);
	void renderOnlyRot(Renderer& renderer);

	DVec3 getRotation() const;
	Matrix viewMatrix() const;
	Double getRotationZ() const;
	Double getRotationX() const;
	Matrix viewMatrixOnlyRot() const;
	Matrix projectionMatrix(Float aspectRatio) const;

private:
	DVec3 rotation = DVec3(0, 0, 0);
	void update();
};
