
#include "QuadtreeSystem.hpp"

void QuadtreeSystem::count() {
	root.count();
}
void QuadtreeSystem::create() {
	root.createRootNode();
}
void QuadtreeSystem::destroy() {
	root.destroy();
}
void QuadtreeSystem::update() {
	root.update();
}