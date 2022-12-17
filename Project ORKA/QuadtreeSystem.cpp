
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
void QuadtreeNode::destroy() {
	unsubdivide();
}
void QuadtreeNode::unsubdivide()
{
	if (subdivided) {
		c00->destroy();
		c01->destroy();
		c10->destroy();
		c11->destroy();

		c00->removeSelfFromNeighbours();
		c01->removeSelfFromNeighbours();
		c10->removeSelfFromNeighbours();
		c11->removeSelfFromNeighbours();

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
QuadtreeNode& QuadtreeNode::nwr() {
	QuadtreeNode* cur = this;
	while (!cur->nw) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			//return self if root node because world loops in all directions
			if (cur->id.level == 0) return *cur;
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nw;
}
QuadtreeNode& QuadtreeNode::ner() {
	QuadtreeNode* cur = this;
	while (!cur->ne) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			//return self if root node because world loops in all directions
			if (cur->id.level == 0) return *cur;
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->ne;
}
QuadtreeNode& QuadtreeNode::nsr() {
	QuadtreeNode* cur = this;
	while (!cur->ns) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			//return self if root node because world loops in all directions
			if (cur->id.level == 0) return *cur;
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->ns;
}
QuadtreeNode& QuadtreeNode::nnr() {
	QuadtreeNode* cur = this;
	while (!cur->nn) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			//return self if root node because world loops in all directions
			if (cur->id.level == 0) return *cur;
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nn;
}

QuadtreeNode& QuadtreeNode::nner() {
	//get direct neighbour reference
	QuadtreeNode& a = nnr();
	QuadtreeNode& b = ner();

	//get diagonal neighnour reference
	QuadtreeNode& c = a.ner();
	QuadtreeNode& d = b.nnr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode& QuadtreeNode::nnwr() {
	//get direct neighbour reference
	QuadtreeNode& a = nnr();
	QuadtreeNode& b = nwr();

	//get diagonal neighnour reference
	QuadtreeNode& c = a.nwr();
	QuadtreeNode& d = b.nnr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode& QuadtreeNode::nser() {
	//get direct neighbour reference
	QuadtreeNode& a = nsr();
	QuadtreeNode& b = ner();

	//get diagonal neighnour reference
	QuadtreeNode& c = a.ner();
	QuadtreeNode& d = b.nsr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode& QuadtreeNode::nswr() {
	//get direct neighbour reference
	QuadtreeNode& a = nsr();
	QuadtreeNode& b = nwr();

	//get diagonal neighnour reference
	QuadtreeNode& c = a.nwr();
	QuadtreeNode& d = b.nsr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
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
void QuadtreeNode::incrementUsers()
{
	users++;
}
void QuadtreeNode::removeSelfFromNeighbours() {
	//remove reference from north neighbour
	if (nn) nn->ns = nullptr;
	//remove reference from south neighbour
	if (ns) ns->nn = nullptr;
	//remove reference from east neighbour
	if (ne) ne->nw = nullptr;
	//remove reference from west neighbour
	if (nw) nw->ne = nullptr;
}
void QuadtreeNode::createRootNode(TerrainSystem& terrainSystem)
{
	parent = nullptr;
	id.level = 0;
	id.location = ULLVec2(0);

	data.terrain = new Terrain(terrainSystem, id, nullptr, 0, 0);

}
void QuadtreeNode::subdivide(TerrainSystem& terrainSystem)
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {

		c00 = new QuadtreeNode();
		c01 = new QuadtreeNode();
		c10 = new QuadtreeNode();
		c11 = new QuadtreeNode();

		c00->create(terrainSystem, this, 0, 0);
		c01->create(terrainSystem, this, 0, 1);
		c10->create(terrainSystem, this, 1, 0);
		c11->create(terrainSystem, this, 1, 1);

		Vector<TerrainTile> tiles = generateSmoothTerrainTiles(data.terrainTile, nnr().data.terrainTile, nsr().data.terrainTile, ner().data.terrainTile, nwr().data.terrainTile);
		//north east
		c11->data.terrainTile = tiles[0];
		//north west
		c01->data.terrainTile = tiles[1];
		//south east
		c10->data.terrainTile = tiles[2];
		//south west
		c00->data.terrainTile = tiles[3];

		subdivided = true;
	}
}
QuadtreeNode& QuadtreeNode::get(TerrainSystem& terrainSystem, QuadtreeID id)
{
	if (this->id.level = id.level) return *this;
	else {
		Bool x = BitSet<64>(id.location.x)[this->id.level];
		Bool y = BitSet<64>(id.location.y)[this->id.level];

		if (!subdivided) subdivide(terrainSystem);

		if (x) {
			if (y) return c11->get(terrainSystem, id);
			else return c10->get(terrainSystem, id);
		}
		else {
			if (y) return c01->get(terrainSystem, id);
			else return c00->get(terrainSystem, id);
		}
	}
}
void QuadtreeNode::update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem)
{
	if (users) {
		if (subdivided) {
			c00->update(quadtreeSystem, terrainSystem);
			c01->update(quadtreeSystem, terrainSystem);
			c10->update(quadtreeSystem, terrainSystem);
			c11->update(quadtreeSystem, terrainSystem);
		}
		else {
			subdivide(terrainSystem);
		}
	}
	else {
		unsubdivide();
	}
}
void QuadtreeNode::create(TerrainSystem& terrainSystem, QuadtreeNode* parent, Bool x, Bool y)
{
	//set parent node
	this->parent = parent;
	//set level
	id.level = parent->id.level + 1;
	//set location
	id.location.x = parent->id.location.x | ULL(x) << 64 - id.level;
	id.location.y = parent->id.location.y | ULL(y) << 64 - id.level;
	//calculate neighbours
	if (x && y) {
		if (parent->ne && parent->ne->subdivided) ne = parent->ne->c01;
		nw = parent->c01;
		if (parent->nn && parent->nn->subdivided) nn = parent->nn->c10;
		ns = parent->c10;
	}
	if (x && !y) {
		if (parent->ne && parent->ne->subdivided) ne = parent->ne->c00;
		nw = parent->c00;
		nn = parent->c11;
		if (parent->ns && parent->ns->subdivided) ns = parent->ns->c11;
	}
	if (!x && !y) {
		ne = parent->c10;
		if (parent->nw && parent->nw->subdivided) nw = parent->nw->c10;
		nn = parent->c01;
		if (parent->ns && parent->ns->subdivided) ns = parent->ns->c01;
	}
	if (!x && y) {
		ne = parent->c11;
		if (parent->nw && parent->nw->subdivided) nw = parent->nw->c11;
		if (parent->nn && parent->nn->subdivided) nn = parent->nn->c00;
		ns = parent->c00;
	}
	//make sure neighbours have connection
	if (nn) nn->ns = this;
	if (ns) ns->nn = this;
	if (ne) ne->nw = this;
	if (nw) nw->ne = this;
	//generate terrain
	//data.terrain = new Terrain(terrainSystem, id, parent->data.terrain, x, y);
}

void QuadtreeSystem::count() {
	root->count();
}
ULLVec3 QuadtreeSystem::rayTrace(Ray ray) {
	return ULLVec3(0);
}
void QuadtreeSystem::create(TerrainSystem& terrainSystem) {
	root = new QuadtreeNode();
	root->createRootNode(terrainSystem);
}
void QuadtreeSystem::update(TerrainSystem& terrainSystem) {
	root->update(*this, terrainSystem);

	//for (int i = 0; i < 10; i++) {
	//	if (nodesToUpdate.size() == 0) {
	//		nodesToUpdate.push_back(root);
	//	}
	//	else {
	//		nodesToUpdate.front()->update(*this, terrainSystem);
	//		if (nodesToUpdate.front()->subdivided) {
	//			nodesToUpdate.push_back(nodesToUpdate.front()->c00);
	//			nodesToUpdate.push_back(nodesToUpdate.front()->c01);
	//			nodesToUpdate.push_back(nodesToUpdate.front()->c10);
	//			nodesToUpdate.push_back(nodesToUpdate.front()->c11);
	//		}
	//		nodesToUpdate.pop_front();
	//	}
	//}
}
QuadtreeNode& QuadtreeSystem::get(TerrainSystem& terrainSystem, QuadtreeID id)
{
	return root->get(terrainSystem, id);
}