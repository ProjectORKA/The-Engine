
#include "QuadtreeSystem.hpp"

void QuadtreeSystem::count() {
	root.count();
}
void QuadtreeSystem::update() {
	root.update();
}

ULLVec3 QuadtreeSystem::rayTrace(Ray ray) {
	return ULLVec3(0); // root.rayTrace(ray);
}
