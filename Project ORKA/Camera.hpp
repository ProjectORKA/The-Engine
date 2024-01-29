#pragma once

#include "Renderer.hpp"

struct Camera
{
	void rotate(DVec2 rotation);
	void setLocation(Vec3 location);
	void render(Renderer& renderer) const;
	void setFieldOfView(Float fieldOfView);
	void setRotation(const DVec3& rotation);
	void setFarClipValue(Float farClipValue);
	void setNearClipValue(Float nearClipValue);
	void setLocation(Float x, Float y, Float z);
	void renderOnlyRot(Renderer& renderer) const;
	void setRotation(Double x, Double y, Double z);

	[[nodiscard]] Matrix viewMatrix() const;
	[[nodiscard]] Vec3   getLocation() const;
	[[nodiscard]] Vec3   getUpVector() const;
	[[nodiscard]] DVec3  getRotation() const;
	[[nodiscard]] Double getRotationZ() const;
	[[nodiscard]] Double getRotationX() const;
	[[nodiscard]] Vec3   getRightVector() const;
	[[nodiscard]] Float  getFieldOfView() const;
	[[nodiscard]] Float  getFarClipValue() const;
	[[nodiscard]] Vec3   getForwardVector() const;
	[[nodiscard]] Float  getNearClipValue() const;
	[[nodiscard]] Matrix viewMatrixOnlyRot() const;
	[[nodiscard]] Matrix projectionMatrix(Float aspectRatio) const;

private:
	Float fieldOfView = 80.0;
	Vec3  location    = Vec3(0);
	DVec3 rotation    = DVec3(0, 0, 0);

	Float nearClipValue = 0.1f;
	Float farClipValue  = 10000.0f;

	Vec3 upVector      = {0.0f, 0.0f, 1.0f};
	Vec3 rightVector   = {1.0f, 0.0f, 0.0f};
	Vec3 forwardVector = {0.0f, 1.0f, 0.0f};

	void update();
};
