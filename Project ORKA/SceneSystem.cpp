#include "SceneSystem.hpp"
#include "FileSystem.hpp"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

void checkMesh(const aiMesh& mesh)
{
	String errorMessage;

	// check for triangulation
	for (UInt faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++) if (mesh.mFaces[faceIndex].mNumIndices != 3) errorMessage = "Mesh is not triangulated!";

	if (!errorMessage.empty()) logError(errorMessage);
}

void loadAssimpScene(Scene2& destinationScene, const aiScene& sourceScene);

void processNode(SceneNode& sceneNode, const aiNode* fileNode)
{
	sceneNode.name = fileNode->mName.C_Str();

	const aiVector3D rotation;
	// get transform
	aiVector3D position, scale;
	fileNode->mTransformation.Decompose(scale, position, position);

	sceneNode.transform.setSize(scale.x, scale.y, scale.z);
	sceneNode.transform.setPosition(position.x, position.y, position.z);
	sceneNode.transform.setRotation(rotation.x, rotation.y, rotation.z);

	// get meshes
	for (Int i = 0; i < fileNode->mNumMeshes; i++) sceneNode.meshes.push_back(fileNode->mMeshes[i]);

	// get child nodes
	for (Int i = 0; i < fileNode->mNumChildren; i++)
	{
		sceneNode.children.emplace_back();
		processNode(sceneNode.children.back(), fileNode->mChildren[i]);
	}
}

void loadAssimpMesh(CpuMesh& destinationMesh, aiMesh& sourceMesh)
{
	if (sourceMesh.HasTangentsAndBitangents())
	{
		// tangents
		for (Int tangentIndex = 0; tangentIndex < sourceMesh.mTangents->Length(); tangentIndex++)
		{
			Vec3 tangent;
			tangent.x = sourceMesh.mTangents[tangentIndex].x;
			tangent.y = sourceMesh.mTangents[tangentIndex].y;
			tangent.z = sourceMesh.mTangents[tangentIndex].z;
			destinationMesh.biTangents.push_back(tangent);
		}

		// bitangents
		for (Int bitangentIndex = 0; bitangentIndex < sourceMesh.mBitangents->Length(); bitangentIndex++)
		{
			Vec3 bitangent;
			bitangent.x = sourceMesh.mBitangents[bitangentIndex].x;
			bitangent.y = sourceMesh.mBitangents[bitangentIndex].y;
			bitangent.z = sourceMesh.mBitangents[bitangentIndex].z;
			destinationMesh.biTangents.push_back(bitangent);
		}
	}

	// vertecies
	for (Int vertexIndex = 0; vertexIndex < sourceMesh.mNumVertices; vertexIndex++)
	{
		// positions
		Vec3 position;
		position.x = sourceMesh.mVertices[vertexIndex].x;
		position.y = sourceMesh.mVertices[vertexIndex].y;
		position.z = sourceMesh.mVertices[vertexIndex].z;
		destinationMesh.positions.push_back(position);

		// normals
		Vec3 normal;
		normal.x = sourceMesh.mNormals[vertexIndex].x;
		normal.y = sourceMesh.mNormals[vertexIndex].y;
		normal.z = sourceMesh.mNormals[vertexIndex].z;
		destinationMesh.normals.push_back(normal);

		// texture coordinates
		Vec2 texCoord;
		texCoord.x = sourceMesh.mTextureCoords[0][vertexIndex].x;
		texCoord.y = sourceMesh.mTextureCoords[0][vertexIndex].y;
		destinationMesh.textureCoordinates.push_back(texCoord);
	}

	if (sourceMesh.GetNumColorChannels() > 0)
	{
		for (Int vertexIndex = 0; vertexIndex < sourceMesh.mNumVertices; vertexIndex++)
		{
			// colors
			Vec4 vertexColor;
			vertexColor.x = sourceMesh.mColors[0][vertexIndex].r;
			vertexColor.y = sourceMesh.mColors[0][vertexIndex].g;
			vertexColor.z = sourceMesh.mColors[0][vertexIndex].b;
			vertexColor.w = sourceMesh.mColors[0][vertexIndex].a;
			destinationMesh.vertexColors.emplace_back(vertexColor);
		}
	}

	Index index;
	for (UInt faceIndex = 0; faceIndex < sourceMesh.mNumFaces; faceIndex++)
	{
		index = sourceMesh.mFaces[faceIndex].mIndices[0];
		destinationMesh.indices.push_back(index);
		index = sourceMesh.mFaces[faceIndex].mIndices[1];
		destinationMesh.indices.push_back(index);
		index = sourceMesh.mFaces[faceIndex].mIndices[2];
		destinationMesh.indices.push_back(index);
	}
}

void Scene2::importFBX(Path path)
{
	path = makeAbsolute(path);
	// check if file is valid
	String errorMessage;

	if (doesPathExist(path))
	{
		logDebug("Loading mesh: (" + path.string() + ")");

		Assimp::Importer importer;
		const aiScene*   assimpScene = importer.ReadFile(path.string(), aiProcess_GenUVCoords | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_RemoveRedundantMaterials);

		errorMessage = importer.GetErrorString();

		if (errorMessage.empty()) loadAssimpScene(*this, *assimpScene);
	}
	else
	{
		errorMessage = path.parent_path().string() + " does not exist!";
	}

	if (!errorMessage.empty()) logError("The model (" + path.stem().string() + ") could not be loaded! (" + path.string() + ")" + " Error: " + errorMessage);
}

void loadAssimpScene(Scene2& destinationScene, const aiScene& sourceScene)
{
	// get meshes
	for (Int i = 0; i < sourceScene.mNumMeshes; i++)
	{
		destinationScene.meshes.emplace_back();
		CpuMesh& destinationMesh = destinationScene.meshes.back();
		aiMesh&  sourceMesh      = *sourceScene.mMeshes[i];

		checkMesh(sourceMesh);

		loadAssimpMesh(destinationMesh, sourceMesh);
	}

	// get nodes
	processNode(destinationScene.root, sourceScene.mRootNode);
}
