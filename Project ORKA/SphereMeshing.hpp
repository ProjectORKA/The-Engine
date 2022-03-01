#include "Random.hpp"
#include "Algorithm.hpp"
#include "PointCloud.hpp"

using Face = Index[3];

struct Mesh {
	Vector<Vec3> positions;
	Vector<Index> faces;

	//void removeVerticesInsideSphere(Sphere s){
	//	for (Int i = 0; i < positions.size(); i++) {
	//		if (pointInsideSphere(positions[i], s)) {

	//			for (int j = 0; j < faces.size(); j++) {
	//				if (faces[j][0] == i || faces[j][1] == i || faces[j][2] == i) faces.erase(faces.begin() + j);
	//			}
	//		}
	//	}
	//}
};

struct SphereMeshing {

	Bool loaded = false;
	PointCloud terrain;
	Vector<Vec3> air;
	Vector<Vec4> spheres;
	PointCloudRenderer pcRenderer;
	GPUBuffer sphereBuffer;

	void render(Renderer& renderer) {

		if (!sphereBuffer.loaded)sphereBuffer.create(spheres, 0);

		//sphereBuffer.use(0);
		//renderer.useShader("sphereRendering");
		//renderer.renderMesh("flippedCube");

		pcRenderer.render(terrain, renderer);

		//renderer.renderMeshInstanced("sphereLowPoly", spheres);

	};

	void rebuildMesh() {
		spheres.clear();
		for (auto t : terrain.points) {
			Float size = getDistanceToClosestPoint(t, air);
			spheres.push_back(Vec4(t, size));
		}

		CPUMesh icoSphere;
		icoSphere.load("lowPolyIcoSphere");

		Mesh m;
		m.positions = icoSphere.vertices;
		m.faces = icoSphere.indices;
	};

	void update() {
		if (!loaded) {
			UInt gridSize = 64;
			Float noiseSize = 4;
			for (int x = 0; x < gridSize; x++) {
				for (int y = 0; y < gridSize; y++) {
					for (int z = 0; z < gridSize; z++) {
						//Vec3 point = (Vec3(x, y, z) + randomVec3(-0.5, 0.5)) / Vec3(gridSize);
						Vec3 point = Vec3(x, y, z) / Vec3(gridSize);
						if (noise.octaveNoise0_1(point.x * noiseSize, point.y * noiseSize, point.z * noiseSize, 8) + point.z - 0.5 > 0.5) air.push_back(point);
						else terrain.add(point);
					}
				}
			}

			if (terrain.points.size() && air.size()) rebuildMesh();
			loaded = true;
		}

		//for(int i = 0; i < 100; i++) generatePoint();
	};

	//void generatePoint() {
	//	Vec3 point = randomVec3(1);

	//	if (point.z > 0.5) {
	//		air.push_back(point);

	//		for (auto s : spheres) {
	//			s.w = min(s.w, distance(Vec3(s), point)/2);
	//		}
	//	}
	//	else {
	//		terrain.push_back(point);
	//		spheres.push_back(Vec4(point, getDistanceToClosestPoint(point, air) / 2));
	//	}
	//}

};