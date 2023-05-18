#pragma once

#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Scene.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct Uniforms;

struct NameTable {
	Vector<Name> names;
	Vector<Index> indices;

	void add(const Name& name, Index id);
	Bool find(const Name& name, Index& id) const;
};

struct BasicMeshes {
	GPUMesh fullscreenMesh;

	void create(ResourceManager& resourceManager) {
		CpuMesh fullScreenQuad;
		fullScreenQuad.load(resourceManager, "fullScreenQuad");
		fullscreenMesh.upload(fullScreenQuad);
	}

	void destroy() { fullscreenMesh.unload(); }
};

struct MeshSystem {
	NameTable meshNames;
	BasicMeshes basicMeshes;
	Vector<GPUMesh> gpuMeshes; //uploaded meshes ready to be drawn
	VertexBufferObject transforms;

	Index currentMeshID = 0;

	void destroy();
	void create(ResourceManager& resourceManager);

	//mesh
	void use(Index meshID);
	GPUMesh& currentMesh();
	void addMesh(const CpuMesh& cpuMesh);
	void use(ResourceManager& resourceManager, const Name& name);
	void renderFullscreen(Uniforms& uniforms);
	void render(Uniforms& uniforms, Index meshID);
	void render(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName);
	void renderInstanced(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName,
	                     Vector<Matrix>& transformations);
};
