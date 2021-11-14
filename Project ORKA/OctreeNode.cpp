
#include "OctreeNode.hpp"
#include "Debug.hpp"

//worldChunk
OctreeNode::~OctreeNode()
{
	unsubdivide();
	quadtreeEquivalent->decrementUsers();
}
OctreeNode::OctreeNode(QuadtreeNode& quadTreeRoot) {
	id.level = 0;
	id.location = ULLVec3(0);

	quadtreeEquivalent = &quadTreeRoot;

	ULL chunkHeight = getLocation().z;

	//data.hasContent = 1;

	//data.isTerrain = (chunkHeight >= quadtreeEquivalent->data.terrain->lowerLimit) && (chunkHeight <= quadtreeEquivalent->data.terrain->upperLimit);

	quadtreeEquivalent->incrementUsers();
	isValid = true;
};
OctreeNode::OctreeNode(OctreeNode& parent, Bool x, Bool y, Bool z)
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

	//data.hasContent = (chunkHeight <= quadtreeEquivalent->data.terrain->upperLimit) && (chunkHeight >= quadtreeEquivalent->data.terrain->lowerLimit);
	//data.isTerrain = (chunkHeight >= quadtreeEquivalent->data.terrain->lowerLimit) && (chunkHeight <= quadtreeEquivalent->data.terrain->upperLimit);

	//if (data.isTerrain) {
	//	for (Int i = 0; i < 1000; i++) {
	//		Vec3 sample = Vec3(randomFloat(),randomFloat(),randomFloat());
	//		ULLVec3 wpSample = DVec3(sample) * DVec3(id.size()) + DVec3(id.location);
	//		if(terrainGenerationFunction(wpSample.x, wpSample.y) > wpSample.z ) data.TerrainPoints.push_back(sample);
	//	}

	//	logDebug(data.TerrainPoints.size());
	//}

	//propagate "hasContent upwards"
	//OctreeNode* currentParentNode = &parent;
	//while ((currentParentNode != nullptr) && data.hasContent) {
	//	currentParentNode->data.hasContent |= data.hasContent;
	//	currentParentNode = currentParentNode->parent;
	//}

	quadtreeEquivalent->incrementUsers();
	isValid = true;
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
		logDebug(String("OctreeNodeCount: ").append(std::to_string(nodeCount)));
	}
}
void OctreeNode::update() {
	if (users) {
		if (subdivided) {
			c000->update();
			c001->update();
			c010->update();
			c011->update();
			c100->update();
			c101->update();
			c110->update();
			c111->update();
		}
		else {
			subdivide();
		}
	}
	else {
		unsubdivide();
	}
};
void OctreeNode::subdivide()
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {
		mutex.lock();
		quadtreeEquivalent->incrementUsers();

		c000 = new OctreeNode(*this, 0, 0, 0);
		c001 = new OctreeNode(*this, 0, 0, 1);
		c010 = new OctreeNode(*this, 0, 1, 0);
		c011 = new OctreeNode(*this, 0, 1, 1);
		c100 = new OctreeNode(*this, 1, 0, 0);
		c101 = new OctreeNode(*this, 1, 0, 1);
		c110 = new OctreeNode(*this, 1, 1, 0);
		c111 = new OctreeNode(*this, 1, 1, 1);

		subdivided = true;
		mutex.unlock();
	}
}
void OctreeNode::unsubdivide()
{
	if (subdivided) {
		mutex.lock();
		//c000->destroy();
		//c001->destroy();
		//c010->destroy();
		//c011->destroy();
		//c100->destroy();
		//c101->destroy();
		//c110->destroy();
		//c111->destroy();
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

ULLVec3 OctreeNode::getLocation()
{
	return id.location;
}