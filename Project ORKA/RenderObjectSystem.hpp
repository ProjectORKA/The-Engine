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
	RenderObjectNames(const String& name, const String& mesh, const String& texture, const String& shader);
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
	void render(ResourceManager& resourceManager, const Name& name);
	void select(ResourceManager& resourceManager, const Name& name);
	Index addRenderObject(const RenderObject& renderObject);
	void addRenderObject(const RenderObjectNames& renderobject);
	void addRenderObject(const Name& name, const Name& meshName, const Name& textureName, const Name& shaderName);

	RenderObject& current();
};
