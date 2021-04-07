
#include "QuadtreeSystem.hpp"

void QuadtreeNode::count()
{
	static Int nodeCount = 0;
	if (id.level == 0) {
		nodeCount = 0;
	}
	nodeCount++;
	if (subdivided) {
		c00->count();
		c01->count();
		c10->count();
		c11->count();
	}
	if (id.level == 0) {
		logDebug(String("QuadTreeNodeCount  : ").append(std::to_string(nodeCount)));
	}
}
void QuadtreeNode::update()
{
	if (!subdivided & users) {
		subdivide();
		return;
	}

	if (subdivided) {
		if (users == 0) {
			unsubdivide();
		}
		else {
			c00->update();
			c01->update();
			c10->update();
			c11->update();
		}
	}
}
void QuadtreeNode::destroy()
{
	unsubdivide();
}
void QuadtreeNode::subdivide()
{
	if (!subdivided & (id.level < MAX_CHUNK_LEVEL-1)) {
		c00 = std::make_shared<QuadtreeNode>();
		c01 = std::make_shared<QuadtreeNode>();
		c10 = std::make_shared<QuadtreeNode>();
		c11 = std::make_shared<QuadtreeNode>();

		std::thread c00Thread = std::thread(&QuadtreeNode::createChildNode, c00,std::ref(*this),0,0);
		std::thread c01Thread = std::thread(&QuadtreeNode::createChildNode, c01,std::ref(*this),0,1);
		std::thread c10Thread = std::thread(&QuadtreeNode::createChildNode, c10,std::ref(*this),1,0);
		std::thread c11Thread = std::thread(&QuadtreeNode::createChildNode, c11,std::ref(*this),1,1);

		c00Thread.join();
		c01Thread.join();
		c10Thread.join();
		c11Thread.join();

		subdivided = true;
	}
}
void QuadtreeNode::unsubdivide()
{
	if (subdivided) {
		c00->destroy();;
		c01->destroy();;
		c10->destroy();;
		c11->destroy();;
		c00.reset();
		c01.reset();
		c10.reset();
		c11.reset();
		subdivided = false;
	}
}
void QuadtreeNode::incrementUsers()
{
	users++;
}
void QuadtreeNode::decrementUsers()
{
	if (users) {
		users--;
	}
	else {
		logError("Cant have less than 0 users, error must have happened!");
	}
}
void QuadtreeNode::createRootNode()
{
	parent = nullptr;
	id.level = 0;
	id.location = ULLVec2(0);
	data.terrain.create(id);
}
void QuadtreeNode::createChildNode(QuadtreeNode& parent, Bool x, Bool y)
{
	this->parent = &parent;
	id.level = parent.id.level + 1;

	id.location.x =	parent.id.location.x | ULL(x) << 64 - id.level;
	id.location.y = parent.id.location.y | ULL(y) << 64 - id.level;

	data.terrain.create(id);
}