#pragma once

#include "Basics.hpp"
#include "Scene.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct Uniforms;

struct NameTable
{
	Vector<Name>  names;
	Vector<Index> indices;
	Vector<Bool>  failed;

	void add(const Name& name, Index id, Bool failedFlag = false);
	Bool find(const Name& name, Index& id, Bool& failedFlag) const;
	void markFailed(const Name& name);
};

struct BasicMeshes
{
	GPUMesh fullscreenMesh;

	void create(Renderer& renderer);
	void destroy(Renderer& renderer);
};

struct MeshSystem
{
	NameTable       meshNames;
	Vector<GPUMesh> gpuMeshes;
	BasicMeshes     basicMeshes;
	Index           currentMeshId = 0;

	GPUMesh& currentMesh();
	void     use(Index meshId);
	void     create(Renderer& renderer);
	void     destroy(Renderer& renderer);
	void     render(Renderer& renderer, Index meshId);
	Bool     use(Renderer& renderer, const Name& name);
	void     renderFullscreen(Renderer& renderer) const;
	void     addMesh(Renderer& renderer, const CpuMesh& cpuMesh);
	void     render(Renderer& renderer, const Name& meshName);
	void     renderInstanced(Renderer& renderer, const Name& meshName, const Vector<Matrix>& transforms);
};
