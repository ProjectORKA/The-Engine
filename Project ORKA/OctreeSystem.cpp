
#include "OctreeSystem.hpp"

//worldChunk
ULLVec3 OctreeNode::getLocation()
{
	return id.location;
}

void OctreeNode::count()
{
	static Int nodeCount = 0;
	if (id.level == 0) {
		nodeCount = 0;
	}
	nodeCount++;
	if (subdivided) {
		c000->count();
		c001->count();
		c010->count();
		c011->count();
		c100->count();
		c101->count();
		c110->count();
		c111->count();
	}

	if (id.level == 0) {
		logDebug(String("OctreeNodeCount  : ").append(std::to_string(nodeCount)));
	}
}
void OctreeNode::update() {
	
	if ((!subdivided) & users) {
		subdivide();
		return;
	}
};
void OctreeNode::destroy()
{
	if (subdivided)unsubdivide();
}

void OctreeNode::unsubdivide()
{
	if (subdivided) {
		mutex.lock();
		c000->destroy();
		c001->destroy();
		c010->destroy();
		c011->destroy();
		c100->destroy();
		c101->destroy();
		c110->destroy();
		c111->destroy();
		delete c000;
		delete c001;
		delete c010;
		delete c011;
		delete c100;
		delete c101;
		delete c110;
		delete c111;
		c000 = nullptr;
		c001 = nullptr;
		c010 = nullptr;
		c011 = nullptr;
		c100 = nullptr;
		c101 = nullptr;
		c110 = nullptr;
		c111 = nullptr;
		subdivided = false;
		quadtreeEquivalent->decrementUsers();
		mutex.unlock();
	}
}
void OctreeNode::incrementUser()
{
	users++;
}
void OctreeNode::decrementUser()
{
	if (users) users--;
	else {
		logError("User mismatch found!");
	}
}
void OctreeNode::createRootNode(QuadtreeNode & quadTreeRoot) {
	id.level = 0;
	id.location = ULLVec3(0);

	quadtreeEquivalent = &quadTreeRoot;

	ULL chunkHeight = getLocation().z;

	data.hasContent = 1;

	data.isTerrain = chunkHeight >= quadtreeEquivalent->data.terrain.lowerLimit & chunkHeight <= quadtreeEquivalent->data.terrain.upperLimit;

	isValid = true;
};
void OctreeNode::createChildNode(OctreeNode & parent, Bool x, Bool y, Bool z)
{
	this->parent = &parent;
	this->id.level = parent.id.level + 1;
	ULLVec3 tmp = parent.id.location;

	this->id.location.x = tmp.x += ULL(x) << 64 - id.level;
	this->id.location.y = tmp.y += ULL(y) << 64 - id.level;
	this->id.location.z = tmp.z += ULL(z) << 64 - id.level;

	if (x) {
		if (y) {
			quadtreeEquivalent = parent.quadtreeEquivalent->c11;
		}
		else {
			quadtreeEquivalent = parent.quadtreeEquivalent->c10;
		}
	}
	else {
		if (y) {
			quadtreeEquivalent = parent.quadtreeEquivalent->c01;
		}
		else {
			quadtreeEquivalent = parent.quadtreeEquivalent->c00;
		}
	}

	ULL chunkHeight = getLocation().z;

	data.hasContent = (chunkHeight <= quadtreeEquivalent->data.terrain.upperLimit) & (chunkHeight >= quadtreeEquivalent->data.terrain.lowerLimit);
	data.isTerrain = chunkHeight >= quadtreeEquivalent->data.terrain.lowerLimit & chunkHeight <= quadtreeEquivalent->data.terrain.upperLimit;

	//propagate "hasContent upwards"
	OctreeNode * currentParentNode = &parent;

	while ((currentParentNode != nullptr) & data.hasContent) {
		currentParentNode->data.hasContent |= data.hasContent;
		currentParentNode = currentParentNode->parent;
	}

	isValid = true;
}

void OctreeNode::subdivide()
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {
		mutex.lock();
		quadtreeEquivalent->incrementUsers();

		c000 = new OctreeNode();
		c001 = new OctreeNode();
		c010 = new OctreeNode();
		c011 = new OctreeNode();
		c100 = new OctreeNode();
		c101 = new OctreeNode();
		c110 = new OctreeNode();
		c111 = new OctreeNode();

		c000->createChildNode(*this, 0, 0, 0);
		c001->createChildNode(*this, 0, 0, 1);
		c010->createChildNode(*this, 0, 1, 0);
		c011->createChildNode(*this, 0, 1, 1);
		c100->createChildNode(*this, 1, 0, 0);
		c101->createChildNode(*this, 1, 0, 1);
		c110->createChildNode(*this, 1, 1, 0);
		c111->createChildNode(*this, 1, 1, 1);

		subdivided = true;
		mutex.unlock();
	}
}

void OctreeSystem::count() {
	root.count();
}
void OctreeSystem::create(QuadtreeSystem& quadtreeSystem) {
	root.createRootNode(quadtreeSystem.root);
}
void OctreeSystem::destroy() {
	root.destroy();
}
void OctreeSystem::update() {
	root.update();
}