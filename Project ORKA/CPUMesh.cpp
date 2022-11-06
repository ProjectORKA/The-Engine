
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"

CPUMesh::CPUMesh(Name name) {
	load(name);
}
void CPUMesh::saveMeshFile()
{
	MeshHeader2 header;
	header.version = 2;
	header.meshName = name;
	header.primitiveMode = PrimitiveMode::Triangles;
	header.vertexCount = vertices.size();
	header.uvCount = uvs.size();
	header.normalCount = normals.size();
	header.colorCount = colors.size();
	header.indexCount = indices.size();

	OutFile mesh(String("Data/meshes/").append(name.data).append(".mesh"));
	mesh.write((Char*)&header, sizeof(MeshHeader2));
	mesh.write((Char*)&vertices[0], vertices.size() * sizeof(Vec3));
	mesh.write((Char*)&uvs[0], uvs.size() * sizeof(Vec2));
	mesh.write((Char*)&normals[0], normals.size() * sizeof(Vec3));
	mesh.write((Char*)&colors[0], colors.size() * sizeof(Vec3));
	mesh.write((Char*)&indices[0], indices.size() * sizeof(Index));
}
void CPUMesh::clearGeometry() {
	vertices.clear();
	uvs.clear();
	normals.clear();
	colors.clear();
	indices.clear();
}
void CPUMesh::load(Name name) {
	auto it{ resourceManager.meshResources.find(name) };
	if (it != std::end(resourceManager.meshResources))
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

				vertices.resize(header.vertexCount);
				uvs.resize(header.uvCount);
				normals.resize(header.normalCount);
				indices.resize(header.indexCount);
				colors.resize(header.colorCount);

				file.read((char*)&vertices[0], header.vertexCount * sizeof(Vec3));
				file.read((char*)&uvs[0], header.uvCount * sizeof(Vec2));
				file.read((char*)&normals[0], header.normalCount * sizeof(Vec3));
				file.read((char*)&colors[0], header.colorCount * sizeof(Vec3));
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
void CPUMesh::removeDoubles() {
	for (Int i = 0; i < vertices.size(); i++) {

		Vec3 current = vertices[i];
		Index currentID = i;

		for (Int j = 0; j < vertices.size(); j++) {
			if (vertices[j] == current) {

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

		vertices.push_back(Vec3(pointPos, 0));
		uvs.push_back(pointPos);
		normals.push_back(Vec3(0, 0, 1));

		indices.push_back(index);
	}

	checkIntegrity();
}
void CPUMesh::checkIntegrity() {
	dataFlags = MeshData::Positions; // we assume a mesh always has positions

	if (uvs.size()) dataFlags += MeshData::TextureCoords;
	if (normals.size()) dataFlags += MeshData::Normals;
	if (tangents.size()) dataFlags += MeshData::Tangents;
	if (bitangents.size()) dataFlags += MeshData::Bitangents;
	if (colors.size()) dataFlags += MeshData::VertexColor;

	UInt count = countBitsInFlags(dataFlags);

	if (uvs.size() + vertices.size() + normals.size() + vertices.size()) loaded = true;
	else loaded = false;
}
void CPUMesh::merge(CPUMesh source) {
	for (Int i = 0; i < source.vertices.size(); i++) {
		vertices.push_back(source.vertices[i]);
		uvs.push_back(source.uvs[i]);
		normals.push_back(source.normals[i]);
	}

	Int indexOffset = vertices.size();

	for (Int i = 0; i < source.indices.size(); i++) {
		indices.push_back(source.indices[i] + indexOffset);
	}
}
void CPUMesh::move(Vec3 moveVector) {
	for (Int i = 0; i < vertices.size(); i++) {
		vertices[i] += moveVector;
	}
}
void CPUMesh::calculateSmoothNormals()
{
	switch (primitiveMode) {
	case PrimitiveMode::Triangles:
		normals.resize(vertices.size());
		for (Vec3& normal : normals) {
			normal = Vec3(0);
		}

		for (UInt f = 0; f < (indices.size() / 3); f++) {
			Index indexA = indices[f * 3];
			Index indexB = indices[f * 3 + 1];
			Index indexC = indices[f * 3 + 2];

			//get vertex position for each face
			Vec3 vertexA = vertices[indexA];
			Vec3 vertexB = vertices[indexB];
			Vec3 vertexC = vertices[indexC];

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
		normals.resize(vertices.size());

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
			Vec3 vertexA = vertices[a];
			Vec3 vertexB = vertices[b];
			Vec3 vertexC = vertices[c];

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
void CPUMesh::meshFromHeightmap(Array2D<Float>& heightmap, UInt size) {
	name = "terrain";
	primitiveMode = PrimitiveMode::TriangleStrip;

	clearGeometry();

	for (Int y = 0; y < size; y++) {
		for (Int x = 0; x < size; x++) {
			Vec3 position = Vec3(x, y, 0) / Vec3(size);
			position.z = heightmap.get(x, y);
			vertices.push_back(position);
			normals.push_back(Vec3(0, 0, 1));
			uvs.push_back(Vec2(position.x, position.y));
		}
	}

	//create indices
	for (UInt y = 0; y < size - 1; y++) {	//very top row doesn't have triangle strip
		for (UInt x = 0; x < size; x++) {
			if ((x == 0) && (y > 0))indices.push_back(x + size * (y + 1));
			indices.push_back(x + (size * (y + 1)));
			indices.push_back(x + (size * y));
			if ((x == (size - 1)) && (y < (size - 1))) indices.push_back(x + size * y);
		}
	}

	checkIntegrity();
}