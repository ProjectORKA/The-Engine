#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Math.hpp"
#include "Transform.hpp"

#define ENTITY_MAX_COMPONENT_NUMBER 7

struct Renderer;

struct RenderComponent {
	Name meshName = "empty";
	Transform transform;
	void render(Renderer& renderer);
};

enum Components {
	renderC,
};

struct Entity {
	U16 archetypeID = 0;
	U16 componentIDs [ENTITY_MAX_COMPONENT_NUMBER];
};

struct Archetype {
	U64 components;
};

struct ECS {
	Vector<Entity> entities;
	Vector<RenderComponent> renderComponents;
};