
#pragma once

#include "Game.hpp"
#include "Basics.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"

struct Renderer;
struct Window;

#include "NeighborQuadtree.hpp"
#include "DynameshTerrain.hpp"


#include "Random.hpp"
#include "Algorithm.hpp"
#include "PointCloud.hpp"

using Face = Index[3];

struct Mesh {
	Vector<Vec3> positions;
	Vector<Face> faces;
};

struct SphereMeshing {

	Bool loaded = false;
	Vector<Vec3> terrain;
	Vector<Vec3> air;
	Vector<Vec4> spheres;

	void render(Renderer & renderer) {
		renderer.useShader("debug");
		renderer.renderMeshInstanced("sphereLowPoly", spheres);
	};

	void rebuildMesh() {
		spheres.clear();
		for (auto t : terrain) {
			Float size = getDistanceToClosestPoint(t, air);
			spheres.push_back(Vec4(t, size));
		}
	};

	void update() {
		if (!loaded) {

			UInt gridSize = 64;

			for (int x = 0; x < gridSize; x++) {
				for (int y = 0; y < gridSize; y++) {
					for (int z = 0; z < gridSize; z++) {
						Vec3 point = Vec3(x, y, z) / Vec3(gridSize);
						if (point.z > sin(point.x) * sin(point.y)) air.push_back(point);
						else terrain.push_back(point);
					}
				}
			}

			if (terrain.size() && air.size()) rebuildMesh();
			loaded = true;
		}

		//for(int i = 0; i < 100; i++) generatePoint();
	};

	void generatePoint() {
		Vec3 point = randomVec3(1);

		if (point.z > 0.5) {
			air.push_back(point);

			for (auto s : spheres) {
				s.w = min(s.w, distance(Vec3(s), point));
			}
		}
		else {
			terrain.push_back(point);
			spheres.push_back(Vec4(point, getDistanceToClosestPoint(point, air)));
		}
	}

};

struct Sandbox : public Game {

	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
};