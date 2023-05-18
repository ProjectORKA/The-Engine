#include "DynameshTerrain.hpp"

#include "Renderer.hpp"
#include "Camera.hpp"
#include "Transform.hpp"

void DynameshTerrain::update(Camera camera) {
	if (!loaded) {
		nodes.push_back(DynameshNode());
		nodes.back().position = Vec3(1);
		loaded = true;
	}

	Int start = lastIndex;
	Int end = start + 1000;
	end = min(end, static_cast<Int>(nodes.size()));

	for (Int i = start; i < end; i++) {
		if (nodes[i].size > 0.0001 && (distance(camera.location, nodes[i].position + Vec3(nodes[i].size / 2.0f)) < nodes
			[i].size * 16)) {
			nodes[i].size /= 2.0f;
			DynameshNode child;
			child.size = nodes[i].size;
			child.position = nodes[i].position + Vec3(0, 0, nodes[i].size);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(0, nodes[i].size, 0);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(nodes[i].size, 0, 0);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(0, nodes[i].size, nodes[i].size);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(nodes[i].size, 0, nodes[i].size);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(nodes[i].size, nodes[i].size, 0);
			nodes.push_back(child);
			child.position = nodes[i].position + Vec3(nodes[i].size, nodes[i].size, nodes[i].size);
			nodes.push_back(child);
		}
	}

	if (end == nodes.size()) { lastIndex = 0; }
	else { lastIndex = end; }
}

void DynameshNode::render(ResourceManager& resourceManager, Renderer& renderer) const {
	if (distance(Vec3(1.5), position + Vec3(size / 2)) < 0.5) {
		Transform t;

		t.rotation = Vec3(0, 0, 15.6);
		t.location = position;
		t.scale = Vec3(size);
		t.render(renderer);
		renderer.renderMesh(resourceManager, "cube");
	}
}

void DynameshTerrain::render(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.useShader(resourceManager, "debug");
	for (DynameshNode& node : nodes) { node.render(resourceManager, renderer); }
}
