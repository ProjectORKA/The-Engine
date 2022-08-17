
#include "ECS.hpp"
#include "Renderer.hpp"

void RenderComponent::render(Renderer& renderer) {
	renderer.renderMesh(meshName);
}