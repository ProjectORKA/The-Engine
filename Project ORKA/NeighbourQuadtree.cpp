
#include "NeighbourQuadtree.hpp"
#include "Renderer.hpp"

NeighbourQuadtreeNode& NeighbourQuadtreeNode::nlr() {
	NeighbourQuadtreeNode* cur = this;
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
NeighbourQuadtreeNode& NeighbourQuadtreeNode::nrr() {
	NeighbourQuadtreeNode* cur = this;
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
NeighbourQuadtreeNode& NeighbourQuadtreeNode::nbr() {
	NeighbourQuadtreeNode* cur = this;
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
NeighbourQuadtreeNode& NeighbourQuadtreeNode::nfr() {
	NeighbourQuadtreeNode* cur = this;
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

void NeighbourQuadtreeNode::subdivide() {
	if (!subdivided && (level < 8)) {
		c00 = new NeighbourQuadtreeNode();
		c01 = new NeighbourQuadtreeNode();
		c10 = new NeighbourQuadtreeNode();
		c11 = new NeighbourQuadtreeNode();

		c00->create(*this,0,0);
		c01->create(*this,0,1);
		c10->create(*this,1,0);
		c11->create(*this,1,1);

		subdivided = true;
	}
}
void NeighbourQuadtreeNode::unsubdivide() {
	if (subdivided) {
		c00->unsubdivide();
		c01->unsubdivide();
		c10->unsubdivide();
		c11->unsubdivide();

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
void NeighbourQuadtreeNode::update(Vec3 location) {
	if (distance(position, location) < pow(2, 5 - level)) {
		subdivide();
	}
	else {
		unsubdivide();
	};

	if (subdivided) {
		c00->update(location);
		c01->update(location);
		c10->update(location);
		c11->update(location);
	}
}
void NeighbourQuadtreeNode::render(ResourceManager& resourceManager, Renderer& renderer) {
	if (subdivided) {
		c00->render(resourceManager, renderer);
		c01->render(resourceManager, renderer);
		c10->render(resourceManager, renderer);
		c11->render(resourceManager, renderer);
	}
	else {
		Transform t;
		t.location = position;
		t.scale = Vec3(pow(2, -level+1));
		t.render(renderer);
		renderer.fill(color);
		renderer.useShader(resourceManager, "color");
		renderer.renderMesh(resourceManager, "plane");

		//render connections
		if (level > 0) {
			renderer.arrow(resourceManager, position + Vec3(0), nrr().position + Vec3(0));
			renderer.arrow(resourceManager, position + Vec3(0), nlr().position + Vec3(0));
			renderer.arrow(resourceManager, position + Vec3(0), nfr().position + Vec3(0));
			renderer.arrow(resourceManager, position + Vec3(0), nbr().position + Vec3(0));
		}
	}
}
void NeighbourQuadtreeNode::removeSelfFromNeighbours() {
	if (nf) nf->nb = nullptr;
	if (nb) nb->nf = nullptr;
	if (nr) nr->nl = nullptr;
	if (nl) nl->nr = nullptr;
}
void NeighbourQuadtreeNode::create(NeighbourQuadtreeNode& parent, Bool x, Bool y) {
	
	this->parent = &parent;
	level = parent.level + 1;
	if (x) position.x = parent.position.x + pow(2, 1-level);
	else position.x = parent.position.x - pow(2, 1-level);
	if (y) position.y = parent.position.y + pow(2, 1-level);
	else position.y = parent.position.y - pow(2, 1-level);

	//calculate neighbours
	if (x && y) {
		if(parent.nr && parent.nr->subdivided) nr = parent.nr->c01;
		nl = parent.c01;
		if (parent.nf && parent.nf->subdivided) nf = parent.nf->c10;
		nb = parent.c10;
	}

	if (x && !y) {
		if (parent.nr && parent.nr->subdivided) nr = parent.nr->c00;
		nl = parent.c00;
		nf = parent.c11;
		if (parent.nb && parent.nb->subdivided) nb = parent.nb->c11;
	}

	if (!x && !y) {
		nr = parent.c10;
		if (parent.nl && parent.nl->subdivided) nl = parent.nl->c10;
		nf = parent.c01;
		if (parent.nb && parent.nb->subdivided) nb = parent.nb->c01;
	}

	if (!x && y) {
		nr = parent.c11;
		if (parent.nl && parent.nl->subdivided) nl = parent.nl->c11;
		if (parent.nf && parent.nf->subdivided) nf = parent.nf->c00;
		nb = parent.c00;
	}

	//make sure neighbours have connection
	if (nf) nf->nb = this;
	if (nb) nb->nf = this;
	if (nr) nr->nl = this;
	if (nl) nl->nr = this;
}

void NeighbourQuadtree::create() {
	root.nl = &root;
	root.nr = &root;
	root.nb = &root;
	root.nf = &root;
	root.position = Vec3(1, 1, 0);
}
void NeighbourQuadtree::destroy() {
	root.unsubdivide();
}
void NeighbourQuadtree::update(Vec3 location) {
	root.update(location);
}
void NeighbourQuadtree::render(ResourceManager& resourceManager, Renderer& renderer) {
	root.render(resourceManager, renderer);
}
