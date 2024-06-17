#include "QuadtreeNode.hpp"
#include "Debug.hpp"
#include "QuadtreeSystem.hpp"

void QuadtreeNode::destroy()
{
	unsubdivide();
}

void QuadtreeNode::count() const
{
	static Int nodeCount = 0;
	if(id.level == 0) nodeCount = 0;
	nodeCount++;
	if(subdivided)
	{
		c00->count();
		c01->count();
		c10->count();
		c11->count();
	}
	if(id.level == 0) logDebug("QuadTreeNodeCount  : " + std::to_string(nodeCount));
}

void QuadtreeNode::unsubdivide()
{
	if(subdivided)
	{
		c00->destroy();
		c01->destroy();
		c10->destroy();
		c11->destroy();

		c00->removeSelfFromNeighbors();
		c01->removeSelfFromNeighbors();
		c10->removeSelfFromNeighbors();
		c11->removeSelfFromNeighbors();

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

void QuadtreeNode::decrementUsers()
{
	if(users) users--;
	else logError("Cant have less than 0 users, error must have happened!");
}

void QuadtreeNode::incrementUsers()
{
	users++;
}

QuadtreeNode& QuadtreeNode::nlr() const
{
	const QuadtreeNode* cur = this;
	while(!cur->nl)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nl;
}

QuadtreeNode& QuadtreeNode::nrr() const
{
	const QuadtreeNode* cur = this;
	while(!cur->nr)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nr;
}

QuadtreeNode& QuadtreeNode::nbr() const
{
	const QuadtreeNode* cur = this;
	while(!cur->nb)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nb;
}

QuadtreeNode& QuadtreeNode::nfr() const
{
	const QuadtreeNode* cur = this;
	while(!cur->nf)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nf;
}

void QuadtreeNode::removeSelfFromNeighbors() const
{
	if(nf) nf->nb = nullptr;
	if(nb) nb->nf = nullptr;
	if(nr) nr->nl = nullptr;
	if(nl) nl->nr = nullptr;
}

void QuadtreeNode::subdivide(const TerrainSystem& terrainSystem)
{
	if(!subdivided && id.level < MAX_CHUNK_LEVEL - 1)
	{
		c00 = new QuadtreeNode();
		c01 = new QuadtreeNode();
		c10 = new QuadtreeNode();
		c11 = new QuadtreeNode();

		c00->create(terrainSystem, this, false, false);
		c01->create(terrainSystem, this, false, true);
		c10->create(terrainSystem, this, true, false);
		c11->create(terrainSystem, this, true, true);

		subdivided = true;
	}
}

void QuadtreeNode::createRootNode(const TerrainSystem& terrainSystem)
{
	parent      = nullptr;
	id.level    = 0;
	id.position = UllVec2(0);

	data.terrain = new Terrain(terrainSystem, id, nullptr, false, false);
}

QuadtreeNode& QuadtreeNode::get(TerrainSystem& terrainSystem, const QuadtreeId& id)
{
	if(this->id.level == id.level) return *this;
	const Bool x = BitSet<64>(id.position.x)[this->id.level];
	const Bool y = BitSet<64>(id.position.y)[this->id.level];

	if(!subdivided) subdivide(terrainSystem);

	if(x)
	{
		if(y) return c11->get(terrainSystem, id);
		return c10->get(terrainSystem, id);
	}
	if(y) return c01->get(terrainSystem, id);
	return c00->get(terrainSystem, id);
}

void QuadtreeNode::update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem)
{
	if(users)
	{
		if(subdivided)
		{
			c00->update(quadtreeSystem, terrainSystem);
			c01->update(quadtreeSystem, terrainSystem);
			c10->update(quadtreeSystem, terrainSystem);
			c11->update(quadtreeSystem, terrainSystem);
		}
		else subdivide(terrainSystem);
	}
	else unsubdivide();
}

void QuadtreeNode::create(const TerrainSystem& terrainSystem, QuadtreeNode* parent, const Bool x, const Bool y)
{
	// set parent node
	this->parent = parent;
	// set level
	id.level = parent->id.level + 1;
	// set position
	id.position.x = parent->id.position.x | static_cast<ULL>(x) << static_cast<UShort>(64) - id.level;
	id.position.y = parent->id.position.y | static_cast<ULL>(y) << static_cast<UShort>(64) - id.level;
	// calculate neighbours
	if(x && y)
	{
		if(parent->nr && parent->nr->subdivided) nr = parent->nr->c01;
		nl = parent->c01;
		if(parent->nf && parent->nf->subdivided) nf = parent->nf->c10;
		nb = parent->c10;
	}
	if(x && !y)
	{
		if(parent->nr && parent->nr->subdivided) nr = parent->nr->c00;
		nl = parent->c00;
		nf = parent->c11;
		if(parent->nb && parent->nb->subdivided) nb = parent->nb->c11;
	}
	if(!x && !y)
	{
		nr = parent->c10;
		if(parent->nl && parent->nl->subdivided) nl = parent->nl->c10;
		nf = parent->c01;
		if(parent->nb && parent->nb->subdivided) nb = parent->nb->c01;
	}
	if(!x && y)
	{
		nr = parent->c11;
		if(parent->nl && parent->nl->subdivided) nl = parent->nl->c11;
		if(parent->nf && parent->nf->subdivided) nf = parent->nf->c00;
		nb = parent->c00;
	}
	// make sure neighbours have connection
	if(nf) nf->nb = this;
	if(nb) nb->nf = this;
	if(nr) nr->nl = this;
	if(nl) nl->nr = this;
	// generate terrain
	data.terrain = new Terrain(terrainSystem, id, parent->data.terrain, x, y);
}
