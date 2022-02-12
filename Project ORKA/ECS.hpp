#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Math.hpp"
#include "Transform.hpp"

struct Renderer;

struct Entity {
	Transform transform;
	Name meshName = "empty";
	void render(Renderer& renderer);
};