#pragma once

#include "Camera.hpp"

struct Framebuffer {
	Float width = 1600;
	Float height = 900;

	//render info
	Vec4 backgroundColor = Vec4(0.05f, 0.05f, 0.05f, 0.75f);

	void update(Int width, Int height) {
		this->width = max(1,width);
		this->height = max(1,height);
	}
};