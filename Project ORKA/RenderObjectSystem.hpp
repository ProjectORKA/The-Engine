#pragma once

#include "Basics.hpp"
#include "ShaderSystem.hpp"
#include "MeshSystem.hpp"
#include "TextureSystem.hpp"

struct RenderObject {
	Index meshID = 0;
	Index shaderID = 0;
	Index textureID = 0;
};

struct RenderObjectNames {
	Name renderObjectName = "default";
	Name meshName = "default";
	Name shaderName = "default";
	Name textureName = "default";
};

struct RenderObjectSystem {
	MeshSystem * meshSystemPtr;
	TextureSystem * textureSystemPtr;
	ShaderSystem * shaderSystemPtr;

	Map<Name, Index> nameToIndex;
	Vector<RenderObject> renderObjects;
	Vector<RenderObjectNames> renderObjectNamesQueue;

	Index currentRenderobjectID = 0;

	void destroy();
	void select(Name name);
	void select(Index id);
	void render(Name name);
	void addRenderObject(String name, Name meshName, Name textureName, Name shaderName);
	void create(MeshSystem & meshSystem, TextureSystem & textureSystem, ShaderSystem & shaderSystem);
	
	RenderObject & current();
};