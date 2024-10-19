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

	void add(const Name& name, Index id);
	Bool find(const Name& name, Index& id) const;
};

struct BasicMeshes
{
	GpuMesh fullscreenMesh;

	void create(Renderer& renderer);
	void destroy(Renderer& renderer);
};

struct MeshSystem
{
	NameTable       meshNames;
	Vector<GpuMesh> gpuMeshes;
	BasicMeshes     basicMeshes;
	Index           currentMeshId = 0;

	GpuMesh& currentMesh();
	void     use(Index meshId);
	void     create(Renderer& renderer);
	void     destroy(Renderer& renderer);
	void     render(Uniforms& uniforms, Index meshId);
	void     use(Renderer& renderer, const Name& name);
	void     renderFullscreen(Uniforms& uniforms) const;
	void     addMesh(Renderer& renderer, const CpuMesh& cpuMesh);
	void     render(Renderer& renderer, Uniforms& uniforms, const Name& meshName);
	void     renderInstanced(Renderer& renderer, Uniforms& uniforms, const Name& meshName, const Vector<Matrix>& transforms);
};
