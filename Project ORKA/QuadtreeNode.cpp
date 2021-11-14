
#include "QuadtreeNode.hpp"
#include "Debug.hpp"

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
void QuadtreeNode::create()
{
	parent = nullptr;
	id.level = 0;
	id.location = ULLVec2(0);

	data.terrain = new Terrain(id);
}
void QuadtreeNode::update()
{
	if (users) {
		if (subdivided) {
			c00->update();
			c01->update();
			c10->update();
			c11->update();
		}
		else {
			subdivide();
		}
	}
	else {
		unsubdivide();
	}
}
void QuadtreeNode::destroy() {
	unsubdivide();
}
void QuadtreeNode::subdivide()
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {

		c00 = new QuadtreeNode();
		c01 = new QuadtreeNode();
		c10 = new QuadtreeNode();
		c11 = new QuadtreeNode();

		c00->create(this, 0, 0);
		c01->create(this, 0, 1);
		c10->create(this, 1, 0);
		c11->create(this, 1, 1);

		subdivided = true;
	}
}
void QuadtreeNode::unsubdivide()
{
	if (subdivided) {
		//c00->destroy();
		//c01->destroy();
		//c10->destroy();
		//c11->destroy();

		delete c00;
		delete c01;
		delete c10;
		delete c11;

		c00 = nullptr;
		c01 = nullptr;
		c10 = nullptr;
		c11 = nullptr;

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
QuadtreeNode& QuadtreeNode::get(QuadtreeID id)
{
	if (this->id.level = id.level) return *this;
	else {
		Bool x = BitSet<64>(id.location.x)[this->id.level];
		Bool y = BitSet<64>(id.location.y)[this->id.level];
		
		if (!subdivided) subdivide();

		if (x) {
			if (y) return c11->get(id);
			else return c10->get(id);
		} else {
			if (y) return c01->get(id);
			else return c00->get(id);
		}
	}
}
void QuadtreeNode::create(QuadtreeNode* parent, Bool x, Bool y)
{
	this->parent = parent;
	id.level = parent->id.level + 1;

	id.location.x = parent->id.location.x | ULL(x) << 64 - id.level;
	id.location.y = parent->id.location.y | ULL(y) << 64 - id.level;

	data.terrain = new Terrain(id);

	//trees
	if (id.level == 4) {
		for (int i = 0; i < 1000; i++) {
			data.trees.emplace_back();
		}
	}
}