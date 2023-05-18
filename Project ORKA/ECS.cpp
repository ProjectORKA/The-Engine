#include "ECS.hpp"
#include "Renderer.hpp"

void RenderComponent::render(ResourceManager& resourceManager, Renderer& renderer) const {
	renderer.renderMesh(resourceManager, meshName);
}
