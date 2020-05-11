#pragma once

#include "Camera.hpp"

struct Framebuffer {
	Vec2 size = Vec2(1600,900);

	//render info
	Vec4 backgroundColor = Vec4(0.05f, 0.05f, 0.05f, 0.75f);

	void update(Int width, Int height) {
		size.x = width;
		size.y = height;
	}
};