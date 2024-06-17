#include "NeighborOctree.hpp"
#include "Renderer.hpp"
#include "PerlinNoise.hpp"

void NeighborOctree::create()
{
	root.nl = &root;
	root.nr = &root;
	root.nb = &root;
	root.nf = &root;
	root.nt = &root;
	root.nd = &root;

	root.position  = Vec3(10, 10, 0);
	root.isTerrain = true;
	root.isSurface = true;
}

void NeighborOctree::destroy()
{
	root.unSubdivide();
}

void NeighborOctreeNode::subdivide()
{
	if(!subdivided && level < 12)
	{
		c000 = new NeighborOctreeNode();
		c001 = new NeighborOctreeNode();
		c010 = new NeighborOctreeNode();
		c011 = new NeighborOctreeNode();
		c100 = new NeighborOctreeNode();
		c101 = new NeighborOctreeNode();
		c110 = new NeighborOctreeNode();
		c111 = new NeighborOctreeNode();

		c000->create(*this, false, false, false);
		c001->create(*this, false, false, true);
		c010->create(*this, false, true, false);
		c011->create(*this, false, true, true);
		c100->create(*this, true, false, false);
		c101->create(*this, true, false, true);
		c110->create(*this, true, true, false);
		c111->create(*this, true, true, true);

		subdivided = true;
	}
}

void NeighborOctreeNode::unSubdivide()
{
	if(subdivided)
	{
		subdivided = false;

		c000->unSubdivide();
		c001->unSubdivide();
		c010->unSubdivide();
		c011->unSubdivide();
		c100->unSubdivide();
		c101->unSubdivide();
		c110->unSubdivide();
		c111->unSubdivide();

		c000->removeSelfFromNeighbors();
		c001->removeSelfFromNeighbors();
		c010->removeSelfFromNeighbors();
		c011->removeSelfFromNeighbors();
		c100->removeSelfFromNeighbors();
		c101->removeSelfFromNeighbors();
		c110->removeSelfFromNeighbors();
		c111->removeSelfFromNeighbors();

		// c000->data.destroy();
		// c001->data.destroy();
		// c010->data.destroy();
		// c011->data.destroy();
		// c100->data.destroy();
		// c101->data.destroy();
		// c110->data.destroy();
		// c111->data.destroy();

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
	}
}

void NeighborOctreeNode::updateIsSurface()
{
	if(hasAllNeighbors)
	{
		if(isTerrain) isSurface = !(nfr().isTerrain && nbr().isTerrain && nlr().isTerrain && nrr().isTerrain);
		else isSurface          = nfr().isTerrain || nbr().isTerrain || nlr().isTerrain || nrr().isTerrain;
	}
	else isSurface = true;

	if(isSurface)
	{
		auto curr = this;
		while(curr)
		{
			curr->isSurface = true;
			curr            = curr->parent;
		}
	}
}

void NeighborOctree::update(const Vec3 position)
{
	root.update(position);
}

void NeighborOctreeNode::updateHasAllNeighbors()
{
	hasAllNeighbors = nl && nr && nb && nf && nt && nd;
}

NeighborOctreeNode& NeighborOctreeNode::nlr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nl)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nl;
}

NeighborOctreeNode& NeighborOctreeNode::nrr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nr)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nr;
}

NeighborOctreeNode& NeighborOctreeNode::nbr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nb)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nb;
}

NeighborOctreeNode& NeighborOctreeNode::nfr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nf)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nf;
}

NeighborOctreeNode& NeighborOctreeNode::ntr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nt)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!"); // will probably crash at root, because it doesn't loop
	}
	return *cur->nt;
}

NeighborOctreeNode& NeighborOctreeNode::ndr() const
{
	const NeighborOctreeNode* cur = this;
	while(!cur->nd)
	{
		if(cur->parent) cur = cur->parent;
		else logError("Quadtree Critical Failure!");
	}
	return *cur->nd;
}

void NeighborOctreeNode::update(const Vec3 pos)
{
	inRenderDistance = withinDiamondArea(position - pos, powf(2, 5 - level + 1));

	// if (!inRenderDistance) unSubdivide();

	if(subdivided && inRenderDistance)
	{
		c000->update(pos);
		c001->update(pos);
		c010->update(pos);
		c011->update(pos);
		c100->update(pos);
		c101->update(pos);
		c110->update(pos);
		c111->update(pos);
	}

	if(inRenderDistance && isSurface) subdivide();
}

void NeighborOctreeNode::removeSelfFromNeighbors() const
{
	if(nf) nf->nb = nullptr;
	if(nb) nb->nf = nullptr;
	if(nr) nr->nl = nullptr;
	if(nl) nl->nr = nullptr;
	if(nt) nt->nd = nullptr;
	if(nd) nd->nt = nullptr;
}

void NeighborOctreeNode::create(NeighborOctreeNode& parent, const Bool x, const Bool y, const Bool z)
{
	this->parent = &parent;
	level        = parent.level + 1;
	if(x) position.x = parent.position.x + powf(2, 1 - level);
	else position.x  = parent.position.x - powf(2, 1 - level);
	if(y) position.y = parent.position.y + powf(2, 1 - level);
	else position.y  = parent.position.y - powf(2, 1 - level);
	if(z) position.z = parent.position.z + powf(2, 1 - level);
	else position.z  = parent.position.z - powf(2, 1 - level);

	// calculate neighbours
	if(x)
	{
		if(y)
		{
			if(z)
			{
				// 111
				if(parent.nt && parent.nt->subdivided) nt = parent.nt->c110;
				nd = parent.c110;
				if(parent.nf && parent.nf->subdivided) nf = parent.nf->c101;
				nb = parent.c101;
				if(parent.nr && parent.nr->subdivided) nr = parent.nr->c011;
				nl = parent.c011;
			}
			else
			{
				// 110
				nt = parent.c111;
				if(parent.nd && parent.nd->subdivided) nd = parent.nd->c111;
				if(parent.nf && parent.nf->subdivided) nf = parent.nf->c100;
				nb = parent.c100;
				if(parent.nr && parent.nr->subdivided) nr = parent.nr->c010;
				nl = parent.c010;
			}
		}
		else
		{
			if(z)
			{
				// 101
				if(parent.nt && parent.nt->subdivided) nt = parent.nt->c100;
				nd = parent.c100;
				nf = parent.c111;
				if(parent.nb && parent.nb->subdivided) nb = parent.nb->c111;
				if(parent.nr && parent.nr->subdivided) nr = parent.nr->c001;
				nl = parent.c001;
			}
			else
			{
				// 100
				nt = parent.c101;
				if(parent.nd && parent.nd->subdivided) nd = parent.nd->c101;
				nf = parent.c110;
				if(parent.nb && parent.nb->subdivided) nb = parent.nb->c110;
				if(parent.nr && parent.nr->subdivided) nr = parent.nr->c000;
				nl = parent.c000;
			}
		}
	}
	else
	{
		if(y)
		{
			if(z)
			{
				// 011
				if(parent.nt && parent.nt->subdivided) nt = parent.nt->c010;
				nd = parent.c010;
				if(parent.nf && parent.nf->subdivided) nf = parent.nf->c001;
				nb = parent.c001;
				nr = parent.c111;
				if(parent.nl && parent.nl->subdivided) nl = parent.nl->c111;
			}
			else
			{
				// 010
				nt = parent.c011;
				if(parent.nd && parent.nd->subdivided) nd = parent.nd->c011;
				if(parent.nf && parent.nf->subdivided) nf = parent.nf->c000;
				nb = parent.c000;
				nr = parent.c110;
				if(parent.nl && parent.nl->subdivided) nl = parent.nl->c110;
			}
		}
		else
		{
			if(z)
			{
				// 001
				if(parent.nt && parent.nt->subdivided) nt = parent.nt->c000;
				nd = parent.c000;
				nf = parent.c011;
				if(parent.nb && parent.nb->subdivided) nb = parent.nb->c011;
				nr = parent.c101;
				if(parent.nl && parent.nl->subdivided) nl = parent.nl->c101;
			}
			else
			{
				// 000
				nt = parent.c001;
				if(parent.nd && parent.nd->subdivided) nd = parent.nd->c001;
				nf = parent.c010;
				if(parent.nb && parent.nb->subdivided) nb = parent.nb->c010;
				nr = parent.c100;
				if(parent.nl && parent.nl->subdivided) nl = parent.nl->c100;
			}
		}
	}

	const Float noiseSize = 4;
	isTerrain             = noise.octaveNoise0_1(position.x * noiseSize, position.y * noiseSize, position.z * noiseSize, 8) + position.z - 0.5 < 0.5;

	// make sure neighbours have connection to this (and update them for good measure)
	if(nf)
	{
		nf->nb = this;
		nf->updateHasAllNeighbors();
		nf->updateIsSurface();
	}
	if(nb)
	{
		nb->nf = this;
		nb->updateHasAllNeighbors();
		nb->updateIsSurface();
	}
	if(nr)
	{
		nr->nl = this;
		nr->updateHasAllNeighbors();
		nr->updateIsSurface();
	}
	if(nl)
	{
		nl->nr = this;
		nl->updateHasAllNeighbors();
		nl->updateIsSurface();
	}
	if(nt)
	{
		nt->nd = this;
		nt->updateHasAllNeighbors();
		nt->updateIsSurface();
	}
	if(nd)
	{
		nd->nt = this;
		nd->updateHasAllNeighbors();
		nd->updateIsSurface();
	}

	updateHasAllNeighbors();
	updateIsSurface();
}

void renderNeighborOctreeNode(const NeighborOctreeNode& node, Renderer& renderer)
{
	if(!node.inRenderDistance || !node.subdivided)
	{
		if(node.isTerrain)
		{
			Transform t;
			t.setPosition(node.position);
			t.setSize(Vec3(pow(2, -node.level + 2)));
			t.render(renderer);
			renderer.renderMesh("centeredCube");
		}
	}
	else
	{
		renderNeighborOctreeNode(*node.c000, renderer);
		renderNeighborOctreeNode(*node.c001, renderer);
		renderNeighborOctreeNode(*node.c010, renderer);
		renderNeighborOctreeNode(*node.c011, renderer);
		renderNeighborOctreeNode(*node.c100, renderer);
		renderNeighborOctreeNode(*node.c101, renderer);
		renderNeighborOctreeNode(*node.c110, renderer);
		renderNeighborOctreeNode(*node.c111, renderer);
	}
}
