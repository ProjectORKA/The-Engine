
#include "QuadtreeSystem.hpp"

void QuadtreeSystem::create() {
	root = new QuadtreeNode();
	root->create();
}

void QuadtreeSystem::destroy() {
	delete root;
	root = nullptr;
}

void QuadtreeSystem::count() {
	root->count();
}
void QuadtreeSystem::update() {
	root->update();
}

ULLVec3 QuadtreeSystem::rayTrace(Ray ray) {
	return ULLVec3(0); // root.rayTrace(ray);
}

QuadtreeNode& QuadtreeSystem::get(QuadtreeID id)
{
	return root->get(id);
}
