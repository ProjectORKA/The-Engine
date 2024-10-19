#pragma once

#include "Basics.hpp"
#include "ShaderSystem.hpp"
#include "MeshSystem.hpp"
#include "TextureSystem.hpp"

struct Renderer;

struct RenderObject
{
	Index meshId    = 0;
	Index shaderId  = 0;
	Index textureId = 0;
};

struct RenderObjectNames
{
	Name renderObjectName = "default";
	Name meshName         = "default";
	Name shaderName       = "default";
	Name textureName      = "default";

	RenderObjectNames();
	RenderObjectNames(const String& name, const String& mesh, const String& texture, const String& shader);
};

struct RenderObjectSystem
{
	Map<Name, Index>          nameToIndex;
	MeshSystem*               meshSystemPtr;
	Vector<RenderObject>      renderObjects;
	ShaderSystem*             shaderSystemPtr;
	TextureSystem*            textureSystemPtr;
	Vector<RenderObjectNames> renderObjectNamesQueue;
	Index                     currentRenderObjectId = 0;

	RenderObject& current();
	void          destroy();
	void          renderCurrent();
	void          select(Index id);
	void          render(Index id);
	void          create(Renderer& renderer);
	void          render(Renderer& renderer, const Name& name);
	void          select(Renderer& renderer, const Name& name);
	Index         addRenderObject(const RenderObject& renderObject);
	void          addRenderObject(const RenderObjectNames& renderObject);
	void          addRenderObject(const Name& name, const Name& meshName, const Name& textureName, const Name& shaderName);
};
