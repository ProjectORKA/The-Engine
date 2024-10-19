#include "RenderObjectSystem.hpp"
#include "Renderer.hpp"

void RenderObjectSystem::destroy()
{
	nameToIndex.clear();
	renderObjects.clear();
	renderObjectNamesQueue.clear();
}

RenderObjectNames::RenderObjectNames() = default;

void RenderObjectSystem::renderCurrent()
{
	shaderSystemPtr->use(current().shaderId);
	textureSystemPtr->use(current().textureId);
	meshSystemPtr->render(shaderSystemPtr->uniforms, current().meshId);
}

RenderObject& RenderObjectSystem::current()
{
	return renderObjects[currentRenderObjectId];
}

void RenderObjectSystem::select(const Index id)
{
	currentRenderObjectId = id;
}

void RenderObjectSystem::render(const Index id)
{
	select(id);
	renderCurrent();
}

void RenderObjectSystem::create(Renderer& renderer)
{
	meshSystemPtr    = &renderer.meshSystem;
	textureSystemPtr = &renderer.textureSystem;
	shaderSystemPtr  = &renderer.shaderSystem;
}

Index RenderObjectSystem::addRenderObject(const RenderObject& renderObject)
{
	renderObjects.push_back(renderObject);
	currentRenderObjectId = renderObjects.size() - 1;
	return currentRenderObjectId;
}

void RenderObjectSystem::addRenderObject(const RenderObjectNames& renderObject)
{
	addRenderObject(renderObject.renderObjectName, renderObject.meshName, renderObject.textureName, renderObject.shaderName);
}

void RenderObjectSystem::render(Renderer& renderer, const Name& name)
{
	select(renderer, name);
	renderCurrent();
}

void RenderObjectSystem::select(Renderer& renderer, const Name& name)
{
	const auto it = nameToIndex.find(name);
	if(it != nameToIndex.end())
	{
		// id found, set to current
		currentRenderObjectId = it->second;
	}
	else
	{
		// id not found search in names list
		for(Index i = 0; i < renderObjectNamesQueue.size(); i++)
		{
			if(renderObjectNamesQueue[i].renderObjectName == name)
			{
				shaderSystemPtr->use(renderObjectNamesQueue[i].shaderName);
				textureSystemPtr->use(renderer, renderObjectNamesQueue[i].textureName);
				meshSystemPtr->use(renderer, renderObjectNamesQueue[i].meshName);

				if(shaderSystemPtr->currentShaderProgram().isLoaded() && textureSystemPtr->currentTexture().isLoaded() && meshSystemPtr->currentMesh().isLoaded())
				{
					RenderObject renderObject;
					renderObject.shaderId  = shaderSystemPtr->currentShaderProgramId;
					renderObject.textureId = textureSystemPtr->currentTextureId;
					renderObject.meshId    = meshSystemPtr->currentMeshId;
					renderObjects.push_back(renderObject);

					currentRenderObjectId = renderObjects.size() - 1;
					nameToIndex[name]     = currentRenderObjectId;

					// remove from queue
					renderObjectNamesQueue.erase(renderObjectNamesQueue.begin() + i);
					return;
				}
				logError("RenderObject found, but could not be loaded!");
			}
		}
		logError("RenderObject With this name doesn't exist. (" + toString(name) + ")");
	}
}

RenderObjectNames::RenderObjectNames(const String& name, const String& mesh, const String& texture, const String& shader)
{
	renderObjectName = name;
	meshName         = mesh;
	shaderName       = texture;
	textureName      = shader;
}

void RenderObjectSystem::addRenderObject(const Name& name, const Name& meshName, const Name& textureName, const Name& shaderName)
{
	RenderObjectNames renderObjectNames;
	renderObjectNames.renderObjectName = name;
	renderObjectNames.meshName         = meshName;
	renderObjectNames.shaderName       = shaderName;
	renderObjectNames.textureName      = textureName;

	renderObjectNamesQueue.push_back(renderObjectNames);
}
