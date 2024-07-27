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

	void create();
	void destroy();
};

struct MeshSystem
{
	NameTable       meshNames;
	Vector<GpuMesh> gpuMeshes;
	BasicMeshes     basicMeshes;
	Index           currentMeshId = 0;

	void     create();
	void     destroy();
	GpuMesh& currentMesh();
	void     use(Index meshId);
	void     addMesh(const CpuMesh& cpuMesh);
	void     render(Uniforms& uniforms, Index meshId);
	void     renderFullscreen(Uniforms& uniforms) const;
	void     use(const Name& name);
	void     render(Uniforms& uniforms, const Name& meshName);
	void     renderInstanced(Uniforms& uniforms, const Name& meshName, const Vector<Matrix>& transforms);
};
