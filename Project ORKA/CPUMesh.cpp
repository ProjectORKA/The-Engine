#include "CPUMesh.hpp"
#include "File.hpp"
#include "GPUMesh.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"

CpuMesh::CpuMesh() = default;

void CpuMesh::clearGeometry()
{
	vertexNormals.clear();
	faceNormals.clear();
	indices.clear();
	tangents.clear();
	positions.clear();
	biTangents.clear();
	vertexColors.clear();
	textureCoordinates.clear();
}

void CpuMesh::removeDoubles()
{
	for (SizeT i = 0; i < positions.size(); i++)
	{
		Vec3       current   = positions[i];
		const auto currentId = static_cast<Index>(i);

		for (Index j = 0; j < positions.size(); j++) if (positions[j] == current) for (Index& index : indices) if (index == j) index = currentId;
	}
}

void CpuMesh::checkIntegrity()
{
	dataFlags = 0;
	if (!positions.empty()) dataFlags += static_cast<Short>(MeshDataFlags::Positions);
	if (!textureCoordinates.empty()) dataFlags += static_cast<Short>(MeshDataFlags::TextureCoords);
	if (!vertexNormals.empty()) dataFlags += static_cast<Short>(MeshDataFlags::VertexNormals);
	if (!tangents.empty()) dataFlags += static_cast<Short>(MeshDataFlags::Tangents);
	if (!biTangents.empty()) dataFlags += static_cast<Short>(MeshDataFlags::BiTangents);
	if (!vertexColors.empty()) dataFlags += static_cast<Short>(MeshDataFlags::VertexColor);
	if (!faceNormals.empty()) dataFlags += static_cast<Short>(MeshDataFlags::FaceNormals);

	if (countBitsInFlags(dataFlags)) loaded = true;
	else loaded                             = false;
}

Bool CpuMesh::isLoaded() const
{
	return loaded;
}

Bool CpuMesh::hasVertexNormals() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::VertexNormals);
}

Bool CpuMesh::hasFaceNormals() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::FaceNormals);
}

Bool CpuMesh::hasTangents() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::Tangents);
}

Bool CpuMesh::hasPositions() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::Positions);
}

Bool CpuMesh::hasBiTangents() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::BiTangents);
}

CpuMesh::CpuMesh(const Graph& graph)
{
	drawMode      = BufferUsage::DynamicDraw;
	name          = "graph";
	primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < graph.points.size(); index++)
	{
		auto pointPos = Vec2(static_cast<Float>(index) / static_cast<Float>(graph.points.size()), graph.points[index]);

		positions.emplace_back(pointPos, 0);
		textureCoordinates.push_back(pointPos);
		vertexNormals.emplace_back(0, 0, 1);

		indices.push_back(index);
	}

	checkIntegrity();
}

Bool CpuMesh::hasVertexColors() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::VertexColor);
}

void CpuMesh::calculateSmoothNormals()
{
	switch (primitiveMode)
	{
	case PrimitiveMode::Triangles: calculateSmoothNormalsForTriangleMesh();
		break;
	case PrimitiveMode::TriangleStrip: calculateSmoothNormalsForTriangleStrip();
		break;
	default: logError("Cant compute normals for this mesh! Primitive type not supported!");
		break;
	}
}

void CpuMesh::move(const Vec3 moveVector)
{
	for (auto& position : positions) position += moveVector;
}

void CpuMesh::calculateFlatNormals() {
	switch (primitiveMode)
	{
	case PrimitiveMode::Triangles: calculateFlatNormalsForTriangleMesh();
		break;
	default: logError("Cant compute normals for this mesh! Primitive type not supported!");
		break;
	}	
}

void CpuMesh::merge(const CpuMesh& source)
{
	for (ULL i = 0; i < source.positions.size(); i++)
	{
		positions.push_back(source.positions[i]);
		textureCoordinates.push_back(source.textureCoordinates[i]);
		vertexNormals.push_back(source.vertexNormals[i]);
	}

	const auto indexOffset = static_cast<Index>(positions.size());

	for (const Index index : source.indices) indices.push_back(index + indexOffset);
}

Bool CpuMesh::hasTextureCoordinates() const
{
	return dataFlags & static_cast<UShort>(MeshDataFlags::TextureCoords);
}

void CpuMesh::render(Renderer& renderer) const
{
	GPUMesh mesh;
	mesh.upload(renderer, *this);
	mesh.render(renderer);
	mesh.unload(renderer);
}

void CpuMesh::calculateFlatNormalsForTriangleMesh()
{
	if (positions.empty())
	{
		logError("Cant calculate flat normals for mesh without positions!");
		return;
	}

	Vec3Vector    newPositions;
	Vec2Vector    newTextureCoordinates;
	Vec3Vector    newVertexNormals;
	Vec3Vector    newTangents;
	Vec3Vector    newBiTangents;
	Vec3Vector    newVertexColors;
	Vector<Index> newIndices;

	for (Int f = 0; f < indices.size(); f += 3)
	{
		const Index indexA = indices[f + 0];
		const Index indexB = indices[f + 1];
		const Index indexC = indices[f + 2];

		newIndices.emplace_back(f + 0);
		newIndices.emplace_back(f + 1);
		newIndices.emplace_back(f + 2);

		Vec3 vertexA = positions[indexA];
		Vec3 vertexB = positions[indexB];
		Vec3 vertexC = positions[indexC];

		const Vec3 faceNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

		newVertexNormals.emplace_back(faceNormal);
		newVertexNormals.emplace_back(faceNormal);
		newVertexNormals.emplace_back(faceNormal);

		newPositions.emplace_back(vertexA);
		newPositions.emplace_back(vertexB);
		newPositions.emplace_back(vertexC);

		if (!textureCoordinates.empty())
		{
			newTextureCoordinates.emplace_back(textureCoordinates[indexA]);
			newTextureCoordinates.emplace_back(textureCoordinates[indexB]);
			newTextureCoordinates.emplace_back(textureCoordinates[indexC]);
		}

		if (!tangents.empty())
		{
			newTangents.emplace_back(tangents[indexA]);
			newTangents.emplace_back(tangents[indexB]);
			newTangents.emplace_back(tangents[indexC]);
		}

		if (!biTangents.empty())
		{
			newBiTangents.emplace_back(biTangents[indexA]);
			newBiTangents.emplace_back(biTangents[indexB]);
			newBiTangents.emplace_back(biTangents[indexC]);
		}

		if (!vertexColors.empty())
		{
			newVertexColors.emplace_back(vertexColors[indexA]);
			newVertexColors.emplace_back(vertexColors[indexB]);
			newVertexColors.emplace_back(vertexColors[indexC]);
		}
	}

	positions          = newPositions;
	textureCoordinates = newTextureCoordinates;
	vertexNormals      = newVertexNormals;
	tangents           = newTangents;
	biTangents         = newBiTangents;
	vertexColors       = newVertexColors;
	indices            = newIndices;
}

void CpuMesh::calculateSmoothNormalsForTriangleMesh()
{
	vertexNormals.resize(positions.size());

	for (Vec3& normal : vertexNormals) normal = Vec3(0);

	for (SizeT f = 0; f < indices.size() / 3; f++)
	{
		const Index indexA = indices[f * 3];
		const Index indexB = indices[f * 3 + 1];
		const Index indexC = indices[f * 3 + 2];

		// get vertex position for each face
		Vec3 vertexA = positions[indexA];
		Vec3 vertexB = positions[indexB];
		Vec3 vertexC = positions[indexC];

		// get face normal
		const Vec3 faceNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

		// add face normal to every vertex
		vertexNormals[indexA] += faceNormal;
		vertexNormals[indexB] += faceNormal;
		vertexNormals[indexC] += faceNormal;
	}

	for (Vec3& normal : vertexNormals) normal = normalize(normal);
}

void CpuMesh::calculateSmoothNormalsForTriangleStrip()
{
	vertexNormals.resize(positions.size());

	for (Vec3& normal : vertexNormals) normal = Vec3(0);

	for (UInt f = 2; f < indices.size(); f++)
	{
		Index a, b, c;
		if (isEven(f))
		{
			a = indices[f - 2];
			b = indices[f - 1];
			c = indices[f - 0];
		}
		else
		{
			a = indices[f - 1];
			b = indices[f - 2];
			c = indices[f - 0];
		}

		// get vertex position for each face
		Vec3 vertexA = positions[a];
		Vec3 vertexB = positions[b];
		Vec3 vertexC = positions[c];

		if (vertexA == vertexB || vertexB == vertexC || vertexA == vertexC) continue;

		// get face normal
		const Vec3 faceNormal = normalize(cross(vertexB - vertexA, vertexC - vertexA));

		// add face normal to every vertex
		vertexNormals[a] += faceNormal;
		vertexNormals[b] += faceNormal;
		vertexNormals[c] += faceNormal;
	}
	for (Vec3& normal : vertexNormals) normal = normalize(normal);
}

void CpuMesh::load(Name name)
{
	if (resourceManager.hasMeshResource(name))
	{
		const Path path = resourceManager.getMeshResourcePath(name);
		Bool error = false;
		const Float maxWaitTime = 10.0f; // 10 seconds max wait time
		Float currentWaitTime = 0.0f;
		const Float retryInterval = 1.0f; // Log every second

		if (debugMeshLoadingIsEnabled)
			logDebug("Attempting to load mesh file from(" + path.string() + ")");

		this->name = name;

		while (currentWaitTime < maxWaitTime)
		{
			InFile file(path);
			if (file.isOpen)
			{
				MeshHeader3 header;
				file.read(reinterpret_cast<char*>(&header), sizeof(MeshHeader3));

				if (header.version == supportedMeshVersion)
				{
					name = header.meshName;
					primitiveMode = header.primitiveMode;

					positions.resize(header.vertexCount);
					textureCoordinates.resize(header.uvCount);
					vertexNormals.resize(header.vertexNormalCount);
					faceNormals.resize(header.faceNormalCount);
					indices.resize(header.indexCount);
					vertexColors.resize(header.colorCount);

					file.read(reinterpret_cast<char*>(positions.data()), header.vertexCount * sizeof(Vec3));
					file.read(reinterpret_cast<char*>(textureCoordinates.data()), header.uvCount * sizeof(Vec2));
					file.read(reinterpret_cast<char*>(vertexNormals.data()), header.vertexNormalCount * sizeof(Vec3));
					file.read(reinterpret_cast<char*>(faceNormals.data()), header.faceNormalCount * sizeof(Vec3));
					file.read(reinterpret_cast<char*>(vertexColors.data()), header.colorCount * sizeof(Vec3));
					file.read(reinterpret_cast<char*>(indices.data()), header.indexCount * sizeof(Index));

					checkIntegrity();
					return; // Successfully loaded
				}
				else
				{
					logError("Mesh version not supported! Version: " + toString(header.version));
					error = true;
					break; // Version mismatch is a permanent error
				}
			}
			else
			{
				// File couldn't be opened, wait and retry
				currentWaitTime += retryInterval;
				if (currentWaitTime < maxWaitTime)
				{
					logDebug("Could not open mesh file (" + path.string() + "). Retrying in " + toString(retryInterval) + "s. Total wait time: " + toString(currentWaitTime) + "s");
					std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<Int>(retryInterval * 1000)));
				}
			}
		}

		if (error || currentWaitTime >= maxWaitTime)
		{
			logError("The model (" + getFileName(path) + ") could not be loaded after " + toString(maxWaitTime) + "s! (" + toString(path) + ")");
			loaded = false;
			return;
		}
	}
	else logWarning("Mesh (" + toString(name) + ") not found as resource!");
}

void CpuMesh::saveMeshFile()
{
	MeshHeader3 header;
	header.version       = 3;
	header.meshName      = name;
	header.primitiveMode = PrimitiveMode::Triangles;
	header.vertexCount   = positions.size();
	header.uvCount       = textureCoordinates.size();
	header.vertexNormalCount   = vertexNormals.size();
	header.faceNormalCount = faceNormals.size();
	header.colorCount    = vertexColors.size();
	header.indexCount    = indices.size();

	const String meshName = toString(name) + ".mesh";

	Path meshPath = resourceManager.getOrkaCachedMeshPath();
	meshPath.append(meshName);

	OutFile mesh(meshPath);
	mesh.write(reinterpret_cast<Char*>(&header), sizeof(MeshHeader3));
	mesh.write(reinterpret_cast<Char*>(positions.data()), positions.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(textureCoordinates.data()), textureCoordinates.size() * sizeof(Vec2));
	mesh.write(reinterpret_cast<Char*>(vertexNormals.data()), vertexNormals.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(faceNormals.data()), faceNormals.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(vertexColors.data()), vertexColors.size() * sizeof(Vec3));
	mesh.write(reinterpret_cast<Char*>(indices.data()), indices.size() * sizeof(Index));
}

void CpuMesh::meshFromHeightMap(Array2D<Float>& heightMap, const UInt size)
{
	name          = "terrain";
	primitiveMode = PrimitiveMode::TriangleStrip;

	clearGeometry();

	for (UInt y = 0; y < size; y++)
	{
		for (UInt x = 0; x < size; x++)
		{
			Vec3 position = Vec3(x, y, 0) / Vec3(static_cast<Float>(size));
			position.z    = heightMap.get(x, y);
			positions.push_back(position);
			vertexNormals.emplace_back(0, 0, 1);
			textureCoordinates.emplace_back(position.x, position.y);
		}
	}

	// create indices
	for (UInt y = 0; y < size - 1; y++)
	{
		// very top row doesn't have triangle strip
		for (UInt x = 0; x < size; x++)
		{
			if (x == 0 && y > 0) indices.push_back(x + size * (y + 1));
			indices.push_back(x + size * (y + 1));
			indices.push_back(x + size * y);
			if (x == size - 1 && y < size - 1) indices.push_back(x + size * y);
		}
	}

	checkIntegrity();
}
