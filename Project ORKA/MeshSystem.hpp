#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct Uniforms;

struct NameTable{
	Vector<Name> names;
	Vector<Index> indices;

	void add(Name name, Index id) {
		names.push_back(name);
		indices.push_back(id);
	};

	Bool find(Name name, Index & id) {
		id = -1;
		for (Index i = 0; i < names.size(); i++) {
			if (names[i] == name) {
				id = indices[i];
				return true;
			}
		}
		return false;
	}
};


struct MeshSystem {
	Vector<GPUMesh> gpuMeshes;	//uploaded meshes ready to be drawn
	NameTable meshNames;

	Index currentMeshID = 0;

	void create();
	void destroy();

	//mesh
	void use(Name name);
	void use(Index meshID);
	void loadMesh(Name name);
	void addMesh(CPUMesh cpuMesh);
	void render(Uniforms& uniforms, Index meshID);
	void render(Uniforms& uniforms, Name meshName);
	void renderInstanced(Uniforms& uniforms, Name meshName, Vector<Vec4>& transformations);

	GPUMesh& currentMesh();
};