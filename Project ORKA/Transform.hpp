#pragma once

#include "Basics.hpp"

struct Renderer;

struct Transform
{
	void setX(Float x);
	void setY(Float y);
	void setZ(Float z);

	[[nodiscard]] Vec3   getSize() const;
	[[nodiscard]] Matrix getMatrix() const;
	[[nodiscard]] Vec3   getRotation() const;
	[[nodiscard]] Vec3   getLocation() const;

	void move(Vec3 translation);
	void scaleBy(const Vec3& scaleFactor);
	void render(Renderer& renderer) const;
	void rotate(const Vec3& axis, Float angle);

	void setSize(Float size);
	void setSize(const Vec3& size);
	void setLocation(const Vec3& pos);
	void setSize(Float x, Float y, Float z);
	void setRotation(const Vec3& eulerAngles);
	void setLocation(Float x, Float y, Float z);
	void setRotation(Float x, Float y, Float z);

	Transform();
	Transform(Vec3 position);
	Transform(Vec3 position, Vec3 rotation, Vec3 size);

private:
	Vec3       position = Vec3(0);
	Vec3       size     = Vec3(1);
	Quaternion rotation = Quaternion(1, 0, 0, 0);
};
