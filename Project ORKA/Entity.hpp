#pragma once

#include "Time.hpp"
#include "Name.hpp"
#include "Basics.hpp"
#include "Math.hpp"

struct Renderer;

struct SpaceShip {
	UShort team = 1;
	Location location = Vec3(0);
	Vec3 velocity = Vec3(0,0,0);
	Rotation rotation;

	void update(Time& time);
	void render(Renderer& renderer);
};