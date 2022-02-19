
#include "ECS.hpp"
#include "Renderer.hpp"

void RenderComponent::render(Renderer& renderer) {
	transform.render(renderer);
	renderer.renderMesh(meshName);
}