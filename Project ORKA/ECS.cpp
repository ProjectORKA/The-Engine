
#include "ECS.hpp"
#include "Renderer.hpp"

void Entity::render(Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(meshName);
}