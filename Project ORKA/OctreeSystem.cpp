#include "OcTreeSystem.hpp"
#include "QuadTreeSystem.hpp"

void OcTreeSystem::count() const { root->count(); }
void OcTreeSystem::update() const { root->update(); }

void OcTreeSystem::destroy() {
	delete root;
	root = nullptr;
}

void OcTreeSystem::create(const QuadTreeSystem& quadTreeSystem) { root = new OcTreeNode(*quadTreeSystem.root); }
