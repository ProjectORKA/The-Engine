
#include "Scene.hpp"
#include "Renderer.hpp"

void Scene::render(Renderer& renderer) {
}

void Scene::update() {
}

void RenderableComponent::render(Renderer& renderer, Object& object) {
	object.transform.render(renderer);
	renderer.renderMesh(meshname);
}
