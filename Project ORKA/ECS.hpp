#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Math.hpp"
#include "Transform.hpp"
#include "ResourceManager.hpp"

#define ENTITY_MAX_COMPONENT_NUMBER 7

struct Renderer;

struct Component {};

struct TransformComponent : public Component {
	Transform transform;
};

struct RenderComponent : public Component {
	Name shaderName = "default";
	Name meshName = "empty";

	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};

enum Components {
	renderC,
};

struct Entity {
	U16 archetypeID = 0;
	U16* componentIDs = nullptr;
};

struct Archetype {
	Vector<U16> components;
	Vector<Entity> entities;
};

struct ECS {
	Vector<Archetype> archetypes;

	Vector<RenderComponent> renderComponents;
	Vector<TransformComponent> transformComponents;
};
