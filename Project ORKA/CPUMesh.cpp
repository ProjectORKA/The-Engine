#include "CpuMesh.hpp"
#include "GPUMesh.hpp"
#include "Renderer.hpp"

void CpuMesh::clearGeometry() {
	positions.clear();
	textureCoordinates.clear();
	normals.clear();
	vertexColors.clear();
	indices.clear();
}

void CpuMesh::removeDoubles() {
	for (Int i = 0; i < positions.size(); i++) {
		Vec3 current = positions[i];
		const Index currentId = i;

		for (Int j = 0; j < positions.size(); j++) {
			if (positions[j] == current) { for (unsigned int& index : indices) { if (index == j) index = currentId; } }
		}
	}
}

void CpuMesh::checkIntegrity() {
	dataFlags = 0;
	if (!positions.empty()) dataFlags += Positions;
	if (!textureCoordinates.empty()) dataFlags += TextureCoords;
	if (!normals.empty()) dataFlags += Normals;
	if (!tangents.empty()) dataFlags += Tangents;
	if (!biTangents.empty()) dataFlags += BiTangents;
	if (!vertexColors.empty()) dataFlags += VertexColor;

	if (countBitsInFlags(dataFlags)) loaded = true;
	else loaded = false;
}

CpuMesh::CpuMesh(const Graph& graph) {
	drawMode = MeshDrawMode::DynamicMode;
	name = "graph";
	primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < graph.points.size(); index++) {
		auto pointPos = Vec2(static_cast<Float>(index) / static_cast<Float>(graph.points.size()), graph.points[index]);

		positions.push_back(Vec3(pointPos, 0));
		textureCoordinates.push_back(pointPos);
		normals.push_back(Vec3(0, 0, 1));

		indices.push_back(index);
	}

	checkIntegrity();
}

void CpuMesh::calculateSmoothNormals() {
	switch (primitiveMode) {
		case PrimitiveMode::Triangles: calculateSmoothNormalsForTriangleMesh();
			break;
		case PrimitiveMode::TriangleStrip: calculateSmoothNormalsForTriangleStrip();
			break;
		default: logError("Cant compute normals for this mesh! Primitive type not supported!");
			break;
	}
}

void CpuMesh::move(const Vec3 moveVector) { for (auto& position : positions) { position += moveVector; } }

void CpuMesh::merge(const CpuMesh& source) {
	for (ULL i = 0; i < source.positions.size(); i++) {
		positions.push_back(source.positions[i]);
		textureCoordinates.push_back(source.textureCoordinates[i]);
		normals.push_back(source.normals[i]);
	}

	const ULL indexOffset = positions.size();

	for (const ULL index : source.indices) { indices.push_back(index + indexOffset); }
}

void CpuMesh::render(Renderer& renderer) const {
	GpuMesh mesh;
	mesh.upload(*this);
	mesh.render(renderer.uniforms());
	mesh.unload();
}

void CpuMesh::calculateSmoothNormalsForTriangleMesh() {
	normals.resize(positions.size());

	for (Vec3& normal : normals) { normal = Vec3(0); }

	for (UInt f = 0; f < (indices.size() / 3); f++) {
		const Index indexA = indices[f * 3];
		const Index indexB = indices[f * 3 + 1];
		const Index indexC = indices[f * 3 + 2];

		//get vertex position for each face
		Vec3 vertexA = positions[indexA];
		Vec3 vertexB = positions[indexB];
		Vec3 vertexC = positions[indexC];

		//get face normal
		const Vec3 faceNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

		//add face normal to every vertex
		normals[indexA] += faceNormal;
		normals[indexB] += faceNormal;
		normals[indexC] += faceNormal;
	}

	for (Vec3& normal : normals) { normal = normalize(normal); }
};

void CpuMesh::calculateSmoothNormalsForTriangleStrip() {
	normals.resize(positions.size());

	for (Vec3& normal : normals) { normal = Vec3(0); }

	for (UInt f = 2; f < indices.size(); f++) {
		Index a, b, c;
		if (isEven(f)) {
			a = indices[f - 2];
			b = indices[f - 1];
			c = indices[f - 0];
		}
		else {
			a = indices[f - 1];
			b = indices[f - 2];
			c = indices[f - 0];
		}

		//get vertex position for each face
		Vec3 vertexA = positions[a];
		Vec3 vertexB = positions[b];
		Vec3 vertexC = positions[c];

		if (vertexA == vertexB || vertexB == vertexC || vertexA == vertexC)continue;

		//get face normal
		const Vec3 faceNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

		//add face normal to every vertex
		normals[a] += faceNormal;
		normals[b] += faceNormal;
		normals[c] += faceNormal;
	}
	for (Vec3& normal : normals) { normal = normalize(normal); }
};

void CpuMesh::load(ResourceManager& resourceManager, Name name) {
	if (resourceManager.hasMeshResource(name)) {
		const Path path = resourceManager.getMeshResourcePath(name);

		bool error = false;

		InFile file(path);

		logDebug(String("Loading mesh file from(").append(path.string()).append(")"));

		this->name = name;

		if (file.isOpen) {
			MeshHeader2 header;
			file.read(reinterpret_cast<char*>(&header), sizeof(MeshHeader2));

			if (header.version == 2) {
				name = header.meshName;
				primitiveMode = header.primitiveMode;

				positions.resize(header.vertexCount);
				textureCoordinates.resize(header.uvCount);
				normals.resize(header.normalCount);
				indices.resize(header.indexCount);
				vertexColors.resize(header.colorCount);

				file.read(reinterpret_cast<char*>(positions.data()), header.vertexCount * sizeof(Vec3));
				file.read(reinterpret_cast<char*>(textureCoordinates.data()), header.uvCount * sizeof(Vec2));
				file.read(reinterpret_cast<char*>(normals.data()), header.normalCount * sizeof(Vec3));
				file.read(reinterpret_cast<char*>(vertexColors.data()), header.colorCount * sizeof(Vec3));
				file.read(reinterpret_cast<char*>(indices.data()), header.indexCount * sizeof(Index));
			}
			else {
				logError("Mesh version not supported!");
				error = true;
			}
		}
		else error = true;

		if (error) {
			logEvent(
				String("The model (").append(path.filename().replace_extension().string()).
				                      append(") could not be loaded! (").append(path.string()).append(")"));
			loaded = false;
			return;
		}
		checkIntegrity();
	}
	else { logWarning(String("Mesh (").append(name.data).append(") not found as resource!")); }
};

void CpuMesh::saveMeshFile(const ResourceManager& resourceManager) {
	MeshHeader2 header;
	header.version = 2;
	header.meshName = name;
	header.primitiveMode = PrimitiveMode::Triangles;
	header.vertexCount = positions.size();
	header.uvCount = textureCoordinates.size();
	header.normalCount = normals.size();
	header.colorCount = vertexColors.size();
	header.indexCount = indices.size();

	const String meshName = String(name.data).append(".mesh");

	Path meshPath = resourceManager.getOrkaCachedMeshLocation();
	meshPath.append(meshName);

	OutFile mesh(meshPath);
	mesh.write(reinterpret_cast<Char*>(&header), sizeof(MeshHeader2));
	mesh.write(reinterpret_cast<Char*>(positions.data()), positions.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(textureCoordinates.data()), textureCoordinates.size() * sizeof(Vec2));
	mesh.write(reinterpret_cast<Char*>(normals.data()), normals.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(vertexColors.data()), vertexColors.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(indices.data()), indices.size() * sizeof(Index));
}

void CpuMesh::meshFromHeightMap(Array2D<Float>& heightMap, const UInt size) {
	name = "terrain";
	primitiveMode = PrimitiveMode::TriangleStrip;

	clearGeometry();

	for (Int y = 0; y < size; y++) {
		for (Int x = 0; x < size; x++) {
			Vec3 position = Vec3(x, y, 0) / Vec3(size);
			position.z = heightMap.get(x, y);
			positions.push_back(position);
			normals.push_back(Vec3(0, 0, 1));
			textureCoordinates.push_back(Vec2(position.x, position.y));
		}
	}

	//create indices
	for (UInt y = 0; y < size - 1; y++) {
		//very top row doesn't have triangle strip
		for (UInt x = 0; x < size; x++) {
			if ((x == 0) && (y > 0))indices.push_back(x + size * (y + 1));
			indices.push_back(x + (size * (y + 1)));
			indices.push_back(x + (size * y));
			if ((x == (size - 1)) && (y < (size - 1))) indices.push_back(x + size * y);
		}
	}

	checkIntegrity();
}
