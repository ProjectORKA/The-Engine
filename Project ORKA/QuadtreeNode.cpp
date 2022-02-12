
#include "QuadtreeNode.hpp"
#include "Debug.hpp"
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
QuadtreeNode& QuadtreeNode::nlr() {
	QuadtreeNode* cur = this;
	while (!cur->nl) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nl;
}
QuadtreeNode& QuadtreeNode::nrr() {
	QuadtreeNode* cur = this;
	while (!cur->nr) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nr;
}
QuadtreeNode& QuadtreeNode::nbr() {
	QuadtreeNode* cur = this;
	while (!cur->nb) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nb;
}
QuadtreeNode& QuadtreeNode::nfr() {
	QuadtreeNode* cur = this;
	while (!cur->nf) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nf;
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
void QuadtreeNode::removeSelfFromNeighbours() {
	if (nf) nf->nb = nullptr;
	if (nb) nb->nf = nullptr;
	if (nr) nr->nl = nullptr;
	if (nl) nl->nr = nullptr;
}
void QuadtreeNode::create(TerrainSystem & terrainSystem)
{
	parent = nullptr;
	id.level = 0;
	id.location = ULLVec2(0);

	data.terrain = new Terrain(terrainSystem,id,nullptr,0,0);
}
void QuadtreeNode::subdivide(TerrainSystem& terrainSystem)
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {

		c00 = new QuadtreeNode();
		c01 = new QuadtreeNode();
		c10 = new QuadtreeNode();
		c11 = new QuadtreeNode();

		c00->create(terrainSystem,this, 0, 0);
		c01->create(terrainSystem,this, 0, 1);
		c10->create(terrainSystem,this, 1, 0);
		c11->create(terrainSystem,this, 1, 1);

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
			if (y) return c11->get(terrainSystem,id);
			else return c10->get(terrainSystem,id);
		} else {
			if (y) return c01->get(terrainSystem, id);
			else return c00->get(terrainSystem, id);
		}
	}
}
void QuadtreeNode::update(QuadtreeSystem & quadtreeSystem, TerrainSystem& terrainSystem)
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
void QuadtreeNode::create(TerrainSystem& terrainSystem,QuadtreeNode* parent, Bool x, Bool y)
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
		if (parent->nr && parent->nr->subdivided) nr = parent->nr->c01;
		nl = parent->c01;
		if (parent->nf && parent->nf->subdivided) nf = parent->nf->c10;
		nb = parent->c10;
	}
	if (x && !y) {
		if (parent->nr && parent->nr->subdivided) nr = parent->nr->c00;
		nl = parent->c00;
		nf = parent->c11;
		if (parent->nb && parent->nb->subdivided) nb = parent->nb->c11;
	}
	if (!x && !y) {
		nr = parent->c10;
		if (parent->nl && parent->nl->subdivided) nl = parent->nl->c10;
		nf = parent->c01;
		if (parent->nb && parent->nb->subdivided) nb = parent->nb->c01;
	}
	if (!x && y) {
		nr = parent->c11;
		if (parent->nl && parent->nl->subdivided) nl = parent->nl->c11;
		if (parent->nf && parent->nf->subdivided) nf = parent->nf->c00;
		nb = parent->c00;
	}
	//make sure neighbours have connection
	if (nf) nf->nb = this;
	if (nb) nb->nf = this;
	if (nr) nr->nl = this;
	if (nl) nl->nr = this;
	//generate terrain
	data.terrain = new Terrain(terrainSystem,id, parent->data.terrain, x, y);
}