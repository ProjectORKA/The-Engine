#pragma once

#include "Basics.hpp"
#include "Interactive.hpp"
#include "Transform.hpp"

struct Renderer;

struct Object {
	Transform transform;
};

struct RenderableComponent {
	Index objectID;
	Name meshname;
	void render(Renderer& renderer, Object& object);
};

struct Scene {
	Vector<Object> objects;
	Vector<Renderable> renderables;

	void update();
	void render(Renderer& renderer);
};