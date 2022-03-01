
#include "NeighbourOctree.hpp"
#include "Renderer.hpp"

void NeighbourOctree::create() {
	root.nl = &root;
	root.nr = &root;
	root.nb = &root;
	root.nf = &root;
	root.nt = &root;
	root.nd = &root;

	root.position = Vec3(10, 10, 0);
}
void NeighbourOctree::destroy() {
	root.unsubdivide();
}
void NeighbourOctree::update(Vec3 location) {
	root.update(location);
}
void NeighbourOctree::render(Renderer& renderer) {
	root.render(renderer);
}

NeighbourOctreeNode& NeighbourOctreeNode::nlr() {
	NeighbourOctreeNode* cur = this;
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
NeighbourOctreeNode& NeighbourOctreeNode::nrr() {
	NeighbourOctreeNode* cur = this;
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
NeighbourOctreeNode& NeighbourOctreeNode::nbr() {
	NeighbourOctreeNode* cur = this;
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
NeighbourOctreeNode& NeighbourOctreeNode::nfr() {
	NeighbourOctreeNode* cur = this;
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
NeighbourOctreeNode& NeighbourOctreeNode::ntr() {
	NeighbourOctreeNode* cur = this;
	while (!cur->nt) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!"); //will probably crash at root, because it doesent loop
		}
	}
	return *cur->nt;
}
NeighbourOctreeNode& NeighbourOctreeNode::ndr() {
	NeighbourOctreeNode* cur = this;
	while (!cur->nd) {
		if (cur->parent) {
			cur = cur->parent;
		}
		else {
			logError("Quadtree Critical Failure!");
		}
	}
	return *cur->nd;
}

void NeighbourOctreeNode::subdivide() {
	if (!subdivided && (level < 8)) {
		c000 = new NeighbourOctreeNode();
		c001 = new NeighbourOctreeNode();
		c010 = new NeighbourOctreeNode();
		c011 = new NeighbourOctreeNode();
		c100 = new NeighbourOctreeNode();
		c101 = new NeighbourOctreeNode();
		c110 = new NeighbourOctreeNode();
		c111 = new NeighbourOctreeNode();

		c000->create(*this, 0, 0, 0);
		c001->create(*this, 0, 0, 1);
		c010->create(*this, 0, 1, 0);
		c011->create(*this, 0, 1, 1);
		c100->create(*this, 1, 0, 0);
		c101->create(*this, 1, 0, 1);
		c110->create(*this, 1, 1, 0);
		c111->create(*this, 1, 1, 1);

		subdivided = true;
	}
}
void NeighbourOctreeNode::unsubdivide() {
	if (subdivided) {
		c000->unsubdivide();
		c001->unsubdivide();
		c010->unsubdivide();
		c011->unsubdivide();
		c100->unsubdivide();
		c101->unsubdivide();
		c110->unsubdivide();
		c111->unsubdivide();

		c000->removeSelfFromNeighbours();
		c001->removeSelfFromNeighbours();
		c010->removeSelfFromNeighbours();
		c011->removeSelfFromNeighbours();
		c100->removeSelfFromNeighbours();
		c101->removeSelfFromNeighbours();
		c110->removeSelfFromNeighbours();
		c111->removeSelfFromNeighbours();

		c000->data.destroy();
		c001->data.destroy();
		c010->data.destroy();
		c011->data.destroy();
		c100->data.destroy();
		c101->data.destroy();
		c110->data.destroy();
		c111->data.destroy();


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
	}
}
void NeighbourOctreeNode::update(Vec3 location) {
	
	rendered = withinDiamondArea(position-location,pow(2, 5 - level));
	
	//if (!rendered) unsubdivide();

	if (subdivided) {
		c000->update(location);
		c001->update(location);
		c010->update(location);
		c011->update(location);
		c100->update(location);
		c101->update(location);
		c110->update(location);
		c111->update(location);
	}

	if(rendered) subdivide();

	data.update(*this);
}
void NeighbourOctreeNode::render(Renderer& renderer) {
	if (!rendered || !subdivided) {
		Transform t;
		t.location = position;
		t.scale = pow(2, -level + 1);
		t.render(renderer);

		data.render(renderer);

		//render connections
		//if (level > 0) {
		//	renderer.arrow(position, nrr().position);
		//	renderer.arrow(position, nlr().position);
		//	renderer.arrow(position, nfr().position);
		//	renderer.arrow(position, nbr().position);
		//	renderer.arrow(position, ntr().position);
		//	renderer.arrow(position, ndr().position);
		//}
	}
	else {
		c000->render(renderer);
		c001->render(renderer);
		c010->render(renderer);
		c011->render(renderer);
		c100->render(renderer);
		c101->render(renderer);
		c110->render(renderer);
		c111->render(renderer);
	}
}
void NeighbourOctreeNode::removeSelfFromNeighbours() {
	if (nf) nf->nb = nullptr;
	if (nb) nb->nf = nullptr;
	if (nr) nr->nl = nullptr;
	if (nl) nl->nr = nullptr;
	if (nt) nt->nd = nullptr;
	if (nd) nd->nt = nullptr;
}
void NeighbourOctreeNode::create(NeighbourOctreeNode& parent, Bool x, Bool y, Bool z) {

	this->parent = &parent;
	level = parent.level + 1;
	if (x) position.x = parent.position.x + pow(2, 1 - level);
	else position.x = parent.position.x - pow(2, 1 - level);
	if (y) position.y = parent.position.y + pow(2, 1 - level);
	else position.y = parent.position.y - pow(2, 1 - level);
	if (z) position.z = parent.position.z + pow(2, 1 - level);
	else position.z = parent.position.z - pow(2, 1 - level);

	//calculate neighbours
	if (x) {
		if (y) {
			if (z) {
				//111
				if (parent.nt && parent.nt->subdivided) nt = parent.nt->c110;
				nd = parent.c110;
				if (parent.nf && parent.nf->subdivided) nf = parent.nf->c101;
				nb = parent.c101;
				if (parent.nr && parent.nr->subdivided) nr = parent.nr->c011;
				nl = parent.c011;
			}
			else {
				//110
				nt = parent.c111;
				if (parent.nd && parent.nd->subdivided) nd = parent.nd->c111;
				if (parent.nf && parent.nf->subdivided) nf = parent.nf->c100;
				nb = parent.c100;
				if (parent.nr && parent.nr->subdivided) nr = parent.nr->c010;
				nl = parent.c010;
			}
		}
		else {
			if (z) {
				//101
				if (parent.nt && parent.nt->subdivided) nt = parent.nt->c100;
				nd = parent.c100;
				nf = parent.c111;
				if (parent.nb && parent.nb->subdivided)nb = parent.nb->c111;
				if (parent.nr && parent.nr->subdivided)nr = parent.nr->c001;
				nl = parent.c001;
			}
			else {
				//100
				nt = parent.c101;
				if (parent.nd && parent.nd->subdivided) nd = parent.nd->c101;
				nf = parent.c110;
				if (parent.nb && parent.nb->subdivided)nb = parent.nb->c110;
				if (parent.nr && parent.nr->subdivided)nr = parent.nr->c000;
				nl = parent.c000;
			}
		}
	}
	else {
		if (y) {
			if (z) {
				//011
				if (parent.nt && parent.nt->subdivided) nt = parent.nt->c010;
				nd = parent.c010;
				if (parent.nf && parent.nf->subdivided)nf = parent.nf->c001;
				nb = parent.c001;
				nr = parent.c111;
				if (parent.nl && parent.nl->subdivided)nl = parent.nl->c111;
			}
			else {
				//010
				nt = parent.c011;
				if (parent.nd && parent.nd->subdivided) nd = parent.nd->c011;
				if (parent.nf && parent.nf->subdivided)nf = parent.nf->c000;
				nb = parent.c000;
				nr = parent.c110;
				if (parent.nl && parent.nl->subdivided)nl = parent.nl->c110;
			}
		}
		else {
			if (z) {
				//001
				if (parent.nt && parent.nt->subdivided) nt = parent.nt->c000;
				nd = parent.c000;
				nf = parent.c011;
				if (parent.nb && parent.nb->subdivided)nb = parent.nb->c011;
				nr = parent.c101;
				if (parent.nl && parent.nl->subdivided)nl = parent.nl->c101;
			}
			else {
				//000
				nt = parent.c001;
				if (parent.nd && parent.nd->subdivided) nd = parent.nd->c001;
				nf = parent.c010;
				if (parent.nb && parent.nb->subdivided)nb = parent.nb->c010;
				nr = parent.c100;
				if (parent.nl && parent.nl->subdivided)nl = parent.nl->c100;
			}
		}
	}

	//make sure neighbours have connection to this
	if (nf) nf->nb = this;
	if (nb) nb->nf = this;
	if (nr) nr->nl = this;
	if (nl) nl->nr = this;
	if (nt) nt->nd = this;
	if (nd) nd->nt = this;

	data.create(*this);
}

void NeighbourOctreeNodeData::create(NeighbourOctreeNode& node) {
	//for (Int i = 0; i < 10000; i++) {
	//	Vec3 point = randomVec3(node.position - Vec3(pow(2, -node.level)), node.position + Vec3(pow(2, -node.level)));
	//	Float noiseSize = 4;
	//	if (noise.octaveNoise0_1(point.x * noiseSize, point.y * noiseSize, point.z * noiseSize, 8) + point.z - 0.5 < 0.5) points.add((point - node.position) / Vec3(pow(2, -node.level + 1)));
	//}
}
void NeighbourOctreeNodeData::update(NeighbourOctreeNode& node) {
	//if (points.points.size() < 10000) {
	//	for (UInt i = 0; i < 1; i++) {
	//		Vec3 point = randomVec3(node.position - Vec3(pow(2, -node.level)), node.position + Vec3(pow(2, -node.level)));
	//		Float noiseSize = 4;
	//		if (noise.octaveNoise0_1(point.x * noiseSize, point.y * noiseSize, point.z * noiseSize, 8) + point.z - 0.5 < 0.5) points.add((point - node.position) / Vec3(pow(2, -node.level + 1)));
	//	}
	//}
};
void NeighbourOctreeNodeData::render(Renderer& renderer) {
	//renderer.fill(color);
	renderer.useShader("debug");
	renderer.renderMesh("centeredCube");
	
	//pcr.render(points, renderer);
}