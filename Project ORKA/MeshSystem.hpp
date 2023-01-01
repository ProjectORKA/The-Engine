
#pragma once

#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Scene.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"
#include "Engine.hpp"

struct Uniforms;

struct NameTable {
	Vector<Name> names;
	Vector<Index> indices;

	void add(Name name, Index id);
	Bool find(Name name, Index& id);
};


struct BasicMeshes {
	GPUMesh fullscreenMesh;
	void create(Engine& engine) {
		CPUMesh fullScreenQuad;
		fullScreenQuad.load(engine, "fullScreenQuad");
		fullscreenMesh.upload(fullScreenQuad);
	}
	void destroy() {
		fullscreenMesh.unload();
	}
};

struct MeshSystem {
	BasicMeshes basicMeshes;
	VertexBufferObject transforms;
	Vector<GPUMesh> gpuMeshes;	//uploaded meshes ready to be drawn
	NameTable meshNames;

	Index currentMeshID = 0;

	void destroy();
	void create(Engine& engine);

	//mesh
	void use(Index meshID);
	GPUMesh& currentMesh();
	void addMesh(CPUMesh cpuMesh);
	void use(Engine& engine, Name name);
	void renderFullscreen(Uniforms& uniforms);
	void render(Uniforms& uniforms, Index meshID);
	void render(Engine& engine, Uniforms& uniforms, Name meshName);
	void renderInstanced(Engine& engine, Uniforms& uniforms, Name meshName, Vector<Matrix>& transformations);
};