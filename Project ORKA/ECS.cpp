
#include "ECS.hpp"
#include "Renderer.hpp"

void RenderComponent::render(Engine& engine, Renderer& renderer) {
	renderer.renderMesh(engine, meshName);
}