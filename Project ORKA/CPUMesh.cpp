
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"
#include "Renderer.hpp"

void CPUMesh::clearGeometry() {
	positions.clear();
	textureCoordinates.clear();
	normals.clear();
	vertexColors.clear();
	indices.clear();
}
void CPUMesh::removeDoubles() {
	for (Int i = 0; i < positions.size(); i++) {

		Vec3 current = positions[i];
		Index currentID = i;

		for (Int j = 0; j < positions.size(); j++) {
			if (positions[j] == current) {

				for (Int k = 0; k < indices.size(); k++) {
					if (indices[k] == j)  indices[k] = currentID;
				}
			}
		}
	}
}
CPUMesh::CPUMesh(Graph& graph) {
	drawMode = MeshDrawMode::dynamicMode;
	name = "graph";
	primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < graph.points.size(); index++) {
		Vec2 pointPos = Vec2(Float(index) / Float(graph.points.size()), graph.points[index]);

		positions.pushBack(Vec3(pointPos, 0));
		textureCoordinates.pushBack(pointPos);
		normals.pushBack(Vec3(0, 0, 1));

		indices.pushBack(index);
	}

	checkIntegrity();
}
void CPUMesh::checkIntegrity() {
	dataFlags = MeshData::Positions; // we assume a mesh always has positions

	if (textureCoordinates.size()) dataFlags += MeshData::TextureCoords;
	if (normals.size()) dataFlags += MeshData::Normals;
	if (tangents.size()) dataFlags += MeshData::Tangents;
	if (bitangents.size()) dataFlags += MeshData::Bitangents;
	if (vertexColors.size()) dataFlags += MeshData::VertexColor;

	UInt count = countBitsInFlags(dataFlags);

	if (textureCoordinates.size() + positions.size() + normals.size() + positions.size()) loaded = true;
	else loaded = false;
}
void CPUMesh::merge(CPUMesh source) {
	for (Int i = 0; i < source.positions.size(); i++) {
		positions.pushBack(source.positions[i]);
		textureCoordinates.pushBack(source.textureCoordinates[i]);
		normals.pushBack(source.normals[i]);
	}

	Int indexOffset = positions.size();

	for (Int i = 0; i < source.indices.size(); i++) {
		indices.pushBack(source.indices[i] + indexOffset);
	}
}
void CPUMesh::move(Vec3 moveVector) {
	for (Int i = 0; i < positions.size(); i++) {
		positions[i] += moveVector;
	}
}
void CPUMesh::calculateSmoothNormals()
{
	switch (primitiveMode) {
	case PrimitiveMode::Triangles:
		normals.resize(positions.size());
		for (Vec3& normal : normals) {
			normal = Vec3(0);
		}

		for (UInt f = 0; f < (indices.size() / 3); f++) {
			Index indexA = indices[f * 3];
			Index indexB = indices[f * 3 + 1];
			Index indexC = indices[f * 3 + 2];

			//get vertex position for each face
			Vec3 vertexA = positions[indexA];
			Vec3 vertexB = positions[indexB];
			Vec3 vertexC = positions[indexC];

			//get face normal
			Vec3 faceNormal = glm::normalize(glm::cross(vertexB - vertexA, vertexC - vertexA));

			//add face normal to every vertex
			normals[indexA] += faceNormal;
			normals[indexB] += faceNormal;
			normals[indexC] += faceNormal;
		}
		for (Vec3& normal : normals) {
			normal = glm::normalize(normal);
		}
		break;
	case PrimitiveMode::TriangleStrip:
		normals.resize(positions.size());

		for (Vec3& normal : normals) {
			normal = Vec3(0);
		}

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
			Vec3 faceNormal = glm::normalize(glm::cross(vertexB - vertexA, vertexC - vertexA));

			//add face normal to every vertex
			normals[a] += faceNormal;
			normals[b] += faceNormal;
			normals[c] += faceNormal;
		}
		for (Vec3& normal : normals) {
			normal = glm::normalize(normal);
		}
		break;
	default:
		logError("Cant compute normals for this mesh! Primitive type not supported!");
		break;
	}
}
void CPUMesh::render(Renderer& renderer) {
	GPUMesh mesh;
	mesh.upload(*this);
	mesh.render(renderer.uniforms());
	mesh.unload();
}
CPUMesh::CPUMesh(Engine& engine, Name name) {
	load(engine, name);
}
void CPUMesh::load(Engine& engine, Name name) {
	auto it{ engine.resourceManager.meshResources.find(name) };
	if (it != std::end(engine.resourceManager.meshResources))
	{
		Path path = it->second;

		bool error = false;

		InFile file(path);

		logDebug(String("Loading mesh file from(").append(path.string()).append(")"));

		this->name = name;

		if (file.isOpen) {
			MeshHeader2 header;
			file.read((char*)&header, sizeof(MeshHeader2));

			if (header.version == 2) {
				name = header.meshName;
				primitiveMode = header.primitiveMode;

				positions.resize(header.vertexCount);
				textureCoordinates.resize(header.uvCount);
				normals.resize(header.normalCount);
				indices.resize(header.indexCount);
				vertexColors.resize(header.colorCount);

				file.read((char*)&positions[0], header.vertexCount * sizeof(Vec3));
				file.read((char*)&textureCoordinates[0], header.uvCount * sizeof(Vec2));
				file.read((char*)&normals[0], header.normalCount * sizeof(Vec3));
				file.read((char*)&vertexColors[0], header.colorCount * sizeof(Vec3));
				file.read((char*)&indices[0], header.indexCount * sizeof(Index));
			}
			else {
				logError("Mesh version not supported!");
				error = true;
			}
		}
		else error = true;

		if (error) {
			logEvent(String("The model (").append(path.filename().replace_extension().string()).append(") could not be loaded! (").append(path.string()).append(")"));
			loaded = false;
			return;
		}
		else {
			checkIntegrity();
		}
	}
	else {
		logWarning(String("Mesh (").append(name.data).append(") not found as resource!"));
	}
};
void CPUMesh::saveMeshFile(ResourceManager& resourceManager)
{
	MeshHeader2 header;
	header.version = 2;
	header.meshName = name;
	header.primitiveMode = PrimitiveMode::Triangles;
	header.vertexCount = positions.size();
	header.uvCount = textureCoordinates.size();
	header.normalCount = normals.size();
	header.colorCount = vertexColors.size();
	header.indexCount = indices.size();

	String meshName = String(name.data).append(".mesh");

	Path meshPath = resourceManager.orkaCachedMeshLocation;
	meshPath.append(meshName);

	OutFile mesh(meshPath);
	mesh.write((Char*)&header, sizeof(MeshHeader2));
	mesh.write((Char*)&positions[0], positions.size() * sizeof(Vec3));
	mesh.write((Char*)&textureCoordinates[0], textureCoordinates.size() * sizeof(Vec2));
	mesh.write((Char*)&normals[0], normals.size() * sizeof(Vec3));
	mesh.write((Char*)&vertexColors[0], vertexColors.size() * sizeof(Vec3));
	mesh.write((Char*)&indices[0], indices.size() * sizeof(Index));
}
void CPUMesh::meshFromHeightmap(Array2D<Float>& heightmap, UInt size) {
	name = "terrain";
	primitiveMode = PrimitiveMode::TriangleStrip;

	clearGeometry();

	for (Int y = 0; y < size; y++) {
		for (Int x = 0; x < size; x++) {
			Vec3 position = Vec3(x, y, 0) / Vec3(size);
			position.z = heightmap.get(x, y);
			positions.pushBack(position);
			normals.pushBack(Vec3(0, 0, 1));
			textureCoordinates.pushBack(Vec2(position.x, position.y));
		}
	}

	//create indices
	for (UInt y = 0; y < size - 1; y++) {	//very top row doesn't have triangle strip
		for (UInt x = 0; x < size; x++) {
			if ((x == 0) && (y > 0))indices.pushBack(x + size * (y + 1));
			indices.pushBack(x + (size * (y + 1)));
			indices.pushBack(x + (size * y));
			if ((x == (size - 1)) && (y < (size - 1))) indices.pushBack(x + size * y);
		}
	}

	checkIntegrity();
}