
#pragma once

#include "Basics.hpp"
#include "ShaderSystem.hpp"
#include "MeshSystem.hpp"
#include "TextureSystem.hpp"

struct Renderer;

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
	RenderObjectNames();
	RenderObjectNames(String name, String mesh, String texture, String shader);
};

struct RenderObjectSystem {
	MeshSystem* meshSystemPtr;
	TextureSystem* textureSystemPtr;
	ShaderSystem* shaderSystemPtr;

	Map<Name, Index> nameToIndex;
	Vector<RenderObject> renderObjects;
	Vector<RenderObjectNames> renderObjectNamesQueue;

	Index currentRenderobjectID = 0;

	void destroy();
	void renderCurrent();
	void select(Index id);
	void render(Index id);
	void create(Renderer& renderer);
	void render(ResourceManager& resourceManager,Name name);
	void select(ResourceManager& resourceManager, Name name);
	Index addRenderObject(RenderObject& renderObject);
	void addRenderObject(RenderObjectNames renderobject);
	void addRenderObject(Name name, Name meshName, Name textureName, Name shaderName);

	RenderObject& current();
};