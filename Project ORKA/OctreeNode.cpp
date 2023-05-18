#include "OcTreeNode.hpp"
#include "Debug.hpp"

//worldChunk
OcTreeNode::~OcTreeNode() {
	unSubdivide();
	quadTreeEquivalent->decrementUsers();
}

OcTreeNode::OcTreeNode(QuadtreeNode& quadTreeRoot) {
	id.level = 0;
	id.location = ULLVec3(0);

	quadTreeEquivalent = &quadTreeRoot;

	ULL chunkHeight = getLocation().z;

	//data.hasContent = 1;

	//data.isTerrain = (chunkHeight >= quadTreeEquivalent->data.terrain->lowerLimit) && (chunkHeight <= quadTreeEquivalent->data.terrain->upperLimit);

	quadTreeEquivalent->incrementUsers();
	isValid = true;
};

OcTreeNode::OcTreeNode(OcTreeNode& parent, const Bool x, const Bool y, const Bool z) {
	this->parent = &parent;
	this->id.level = parent.id.level + 1;
	ULLVec3 tmp = parent.id.location;

	this->id.location.x = tmp.x += static_cast<ULL>(x) << 64 - id.level;
	this->id.location.y = tmp.y += static_cast<ULL>(y) << 64 - id.level;
	this->id.location.z = tmp.z += static_cast<ULL>(z) << 64 - id.level;

	if (x) {
		if (y) { quadTreeEquivalent = parent.quadTreeEquivalent->c11; }
		else { quadTreeEquivalent = parent.quadTreeEquivalent->c10; }
	}
	else {
		if (y) { quadTreeEquivalent = parent.quadTreeEquivalent->c01; }
		else { quadTreeEquivalent = parent.quadTreeEquivalent->c00; }
	}

	ULL chunkHeight = getLocation().z;

	//data.hasContent = (chunkHeight <= quadTreeEquivalent->data.terrain->upperLimit) && (chunkHeight >= quadTreeEquivalent->data.terrain->lowerLimit);
	//data.isTerrain = (chunkHeight >= quadTreeEquivalent->data.terrain->lowerLimit) && (chunkHeight <= quadTreeEquivalent->data.terrain->upperLimit);

	//if (data.isTerrain) {
	//	for (Int i = 0; i < 1000; i++) {
	//		Vec3 sample = Vec3(randomFloat(),randomFloat(),randomFloat());
	//		ULLVec3 wpSample = DVec3(sample) * DVec3(id.size()) + DVec3(id.location);
	//		if(terrainGenerationFunction(wpSample.x, wpSample.y) > wpSample.z ) data.TerrainPoints.push_back(sample);
	//	}

	//	logDebug(data.TerrainPoints.size());
	//}

	//propagate "hasContent upwards"
	//OcTreeNode* currentParentNode = &parent;
	//while ((currentParentNode != nullptr) && data.hasContent) {
	//	currentParentNode->data.hasContent |= data.hasContent;
	//	currentParentNode = currentParentNode->parent;
	//}

	quadTreeEquivalent->incrementUsers();
	isValid = true;
}

void OcTreeNode::count() const {
	static Int nodeCount = 0;
	if (id.level == 0) { nodeCount = 0; }
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

	if (id.level == 0) { logDebug(String("OctreeNodeCount: ").append(toString(nodeCount))); }
}

void OcTreeNode::update() {
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
		else { subdivide(); }
	}
	else { unSubdivide(); }
};

void OcTreeNode::subdivide() {
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {
		mutex.lock();
		quadTreeEquivalent->incrementUsers();

		c000 = new OcTreeNode(*this, false, false, false);
		c001 = new OcTreeNode(*this, false, false, true);
		c010 = new OcTreeNode(*this, false, true, false);
		c011 = new OcTreeNode(*this, false, true, true);
		c100 = new OcTreeNode(*this, true, false, false);
		c101 = new OcTreeNode(*this, true, false, true);
		c110 = new OcTreeNode(*this, true, true, false);
		c111 = new OcTreeNode(*this, true, true, true);

		subdivided = true;
		mutex.unlock();
	}
}

void OcTreeNode::unSubdivide() {
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
		quadTreeEquivalent->decrementUsers();
		mutex.unlock();
	}
}

void OcTreeNode::incrementUser() { users++; }

void OcTreeNode::decrementUser() {
	if (users) users--;
	else { logError("User mismatch found!"); }
}

ULLVec3 OcTreeNode::getLocation() const { return id.location; }
