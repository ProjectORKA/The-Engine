
#include "QuadtreeSystem2.hpp"

void QuadtreeNode2::count()
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
		logDebug(String("QuadtreeNode2Count  : ").append(std::to_string(nodeCount)));
	}
}
void QuadtreeNode2::destroy() {
	unsubdivide();
}
void QuadtreeNode2::unsubdivide()
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
QuadtreeNode2& QuadtreeNode2::nwr() {
	QuadtreeNode2* cur = this;
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
QuadtreeNode2& QuadtreeNode2::ner() {
	QuadtreeNode2* cur = this;
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
QuadtreeNode2& QuadtreeNode2::nsr() {
	QuadtreeNode2* cur = this;
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
QuadtreeNode2& QuadtreeNode2::nnr() {
	QuadtreeNode2* cur = this;
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

QuadtreeNode2& QuadtreeNode2::nner() {
	//get direct neighbour reference
	QuadtreeNode2& a = nnr();
	QuadtreeNode2& b = ner();

	//get diagonal neighnour reference
	QuadtreeNode2& c = a.ner();
	QuadtreeNode2& d = b.nnr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode2& QuadtreeNode2::nnwr() {
	//get direct neighbour reference
	QuadtreeNode2& a = nnr();
	QuadtreeNode2& b = nwr();

	//get diagonal neighnour reference
	QuadtreeNode2& c = a.nwr();
	QuadtreeNode2& d = b.nnr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode2& QuadtreeNode2::nser() {
	//get direct neighbour reference
	QuadtreeNode2& a = nsr();
	QuadtreeNode2& b = ner();

	//get diagonal neighnour reference
	QuadtreeNode2& c = a.ner();
	QuadtreeNode2& d = b.nsr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}
QuadtreeNode2& QuadtreeNode2::nswr() {
	//get direct neighbour reference
	QuadtreeNode2& a = nsr();
	QuadtreeNode2& b = nwr();

	//get diagonal neighnour reference
	QuadtreeNode2& c = a.nwr();
	QuadtreeNode2& d = b.nsr();

	//figure out best if different
	if (c.id.level >= d.id.level) return c;
	else return d;
}

void QuadtreeNode2::decrementUsers()
{
	if (users) {
		users--;
	}
	else {
		logError("Cant have less than 0 users, error must have happened!");
	}
}
void QuadtreeNode2::incrementUsers()
{
	users++;
}
void QuadtreeNode2::removeSelfFromNeighbours() {
	//remove reference from north neighbour
	if (nn) nn->ns = nullptr;
	//remove reference from south neighbour
	if (ns) ns->nn = nullptr;
	//remove reference from east neighbour
	if (ne) ne->nw = nullptr;
	//remove reference from west neighbour
	if (nw) nw->ne = nullptr;
}
void QuadtreeNode2::createRootNode(TerrainSystem& terrainSystem)
{
	parent = nullptr;
	id.level = 0;
	id.location = ULLVec2(0);

	data.terrain = new Terrain(terrainSystem, id, nullptr, 0, 0);

}
void QuadtreeNode2::subdivide(TerrainSystem& terrainSystem)
{
	if ((!subdivided) && (id.level < MAX_CHUNK_LEVEL - 1)) {

		c00 = new QuadtreeNode2();
		c01 = new QuadtreeNode2();
		c10 = new QuadtreeNode2();
		c11 = new QuadtreeNode2();

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
QuadtreeNode2& QuadtreeNode2::get(TerrainSystem& terrainSystem, QuadtreeID id)
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
void QuadtreeNode2::update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem)
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
void QuadtreeNode2::create(TerrainSystem& terrainSystem, QuadtreeNode2* parent, Bool x, Bool y)
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