
#include "ECS.hpp"
#include "Renderer.hpp"

void RenderComponent::render(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.renderMesh(resourceManager, meshName);
}