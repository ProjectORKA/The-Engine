#include "NeighborQuadtree.hpp"
#include "Renderer.hpp"

void NeighborQuadtree::create()
{
	root.nl       = &root;
	root.nr       = &root;
	root.nb       = &root;
	root.nf       = &root;
	root.position = Vec3(1, 1, 0);
}

void NeighborQuadtree::destroy()
{
	root.unSubdivide();
}

void NeighborQuadtreeNode::subdivide()
{
	if(!subdivided && level < 8)
	{
		c00 = new NeighborQuadtreeNode();
		c01 = new NeighborQuadtreeNode();
		c10 = new NeighborQuadtreeNode();
		c11 = new NeighborQuadtreeNode();

		c00->create(*this, false, false);
		c01->create(*this, false, true);
		c10->create(*this, true, false);
		c11->create(*this, true, true);

		subdivided = true;
	}
}

void NeighborQuadtreeNode::unSubdivide()
{
	if(subdivided)
	{
		c00->unSubdivide();
		c01->unSubdivide();
		c10->unSubdivide();
		c11->unSubdivide();

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

void NeighborQuadtree::update(const Vec3 position)
{
	root.update(position);
}

void NeighborQuadtreeNode::update(const Vec3 pos)
{
	if(distance(position, pos) < pow(2.0f, 5.0f - toFloat(level)))
	{
		subdivide();
	}
	else
	{
		unSubdivide();
	}

	if(subdivided)
	{
		c00->update(pos);
		c01->update(pos);
		c10->update(pos);
		c11->update(pos);
	}
}

NeighborQuadtreeNode& NeighborQuadtreeNode::nlr() const
{
	const NeighborQuadtreeNode* cur = this;
	while(!cur->nl)
	{
		if(cur->parent)
		{
			cur = cur->parent;
		}
		else
		{
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nl;
}

NeighborQuadtreeNode& NeighborQuadtreeNode::nrr() const
{
	const NeighborQuadtreeNode* cur = this;
	while(!cur->nr)
	{
		if(cur->parent)
		{
			cur = cur->parent;
		}
		else
		{
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nr;
}

NeighborQuadtreeNode& NeighborQuadtreeNode::nbr() const
{
	const NeighborQuadtreeNode* cur = this;
	while(!cur->nb)
	{
		if(cur->parent)
		{
			cur = cur->parent;
		}
		else
		{
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nb;
}

NeighborQuadtreeNode& NeighborQuadtreeNode::nfr() const
{
	const NeighborQuadtreeNode* cur = this;
	while(!cur->nf)
	{
		if(cur->parent)
		{
			cur = cur->parent;
		}
		else
		{
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nf;
}

void NeighborQuadtreeNode::removeSelfFromNeighbors() const
{
	if(nf) nf->nb = nullptr;
	if(nb) nb->nf = nullptr;
	if(nr) nr->nl = nullptr;
	if(nl) nl->nr = nullptr;
}

void NeighborQuadtree::render(Renderer& renderer) const
{
	root.render(renderer);
}

void NeighborQuadtreeNode::render(Renderer& renderer) const
{
	if(subdivided)
	{
		c00->render(renderer);
		c01->render(renderer);
		c10->render(renderer);
		c11->render(renderer);
	}
	else
	{
		Transform t;
		t.setPosition(position);
		t.setSize(Vec3(pow(2.0f, -toFloat(level) + 1.0f)));
		t.render(renderer);
		renderer.fill(color);
		renderer.useShader("color");
		renderer.renderMesh("plane");

		// render connections
		if(level > 0)
		{
			renderer.arrow(position + Vec3(0), nrr().position + Vec3(0));
			renderer.arrow(position + Vec3(0), nlr().position + Vec3(0));
			renderer.arrow(position + Vec3(0), nfr().position + Vec3(0));
			renderer.arrow(position + Vec3(0), nbr().position + Vec3(0));
		}
	}
}

void NeighborQuadtreeNode::create(NeighborQuadtreeNode& parentNode, const Bool x, const Bool y)
{
	this->parent = &parentNode;
	level        = parentNode.level + 1;
	if(x)
	{
		position.x = parentNode.position.x + pow(2.0f, 1.0f - toFloat(level));
	}
	else
	{
		position.x  = parentNode.position.x - pow(2.0f, 1.0f - toFloat(level));
	}
	if(y)
	{
		position.y = parentNode.position.y + pow(2.0f, 1.0f - toFloat(level));
	}
	else
	{
		position.y  = parentNode.position.y - pow(2.0f, 1.0f - toFloat(level));
	}

	// calculate neighbors
	if(x && y)
	{
		if(parentNode.nr && parentNode.nr->subdivided) nr = parentNode.nr->c01;
		nl = parentNode.c01;
		if(parentNode.nf && parentNode.nf->subdivided) nf = parentNode.nf->c10;
		nb = parentNode.c10;
	}

	if(x && !y)
	{
		if(parentNode.nr && parentNode.nr->subdivided) nr = parentNode.nr->c00;
		nl = parentNode.c00;
		nf = parentNode.c11;
		if(parentNode.nb && parentNode.nb->subdivided) nb = parentNode.nb->c11;
	}

	if(!x && !y)
	{
		nr = parentNode.c10;
		if(parentNode.nl && parentNode.nl->subdivided) nl = parentNode.nl->c10;
		nf = parentNode.c01;
		if(parentNode.nb && parentNode.nb->subdivided) nb = parentNode.nb->c01;
	}

	if(!x && y)
	{
		nr = parentNode.c11;
		if(parentNode.nl && parentNode.nl->subdivided) nl = parentNode.nl->c11;
		if(parentNode.nf && parentNode.nf->subdivided) nf = parentNode.nf->c00;
		nb = parentNode.c00;
	}

	// make sure neighbors have connection
	if(nf) nf->nb = this;
	if(nb) nb->nf = this;
	if(nr) nr->nl = this;
	if(nl) nl->nr = this;
}
