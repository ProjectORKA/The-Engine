#include "RenderObjectSystem.hpp"
#include "Renderer.hpp"

RenderObjectNames::RenderObjectNames() {}

void RenderObjectSystem::destroy() {
	nameToIndex.clear();
	renderObjects.clear();
	renderObjectNamesQueue.clear();
}

void RenderObjectSystem::renderCurrent() {
	shaderSystemPtr->use(current().shaderId);
	textureSystemPtr->use(current().textureId);
	meshSystemPtr->render(shaderSystemPtr->uniforms, current().meshId);
}

void RenderObjectSystem::select(const Index id) { currentRenderobjectId = id; }

void RenderObjectSystem::render(const Index id) {
	select(id);
	renderCurrent();
}

void RenderObjectSystem::render(ResourceManager& resourceManager, const Name& name) {
	select(resourceManager, name);
	renderCurrent();
}

void RenderObjectSystem::select(ResourceManager& resourceManager, const Name& name) {
	const auto it = nameToIndex.find(name);
	if (it != nameToIndex.end()) {
		//id found, set to current
		currentRenderobjectId = it->second;
	}
	else {
		//id not found search in names list
		for (Index i = 0; i < renderObjectNamesQueue.size(); i++) {
			if (renderObjectNamesQueue[i].renderObjectName == name) {
				shaderSystemPtr->use(resourceManager, renderObjectNamesQueue[i].shaderName);
				textureSystemPtr->use(resourceManager, renderObjectNamesQueue[i].textureName);
				meshSystemPtr->use(resourceManager, renderObjectNamesQueue[i].meshName);

				if (shaderSystemPtr->currentShaderProgram().isLoaded && textureSystemPtr->currentTexture().loaded &&
					meshSystemPtr->currentMesh().loaded) {
					RenderObject renderObject;
					renderObject.shaderId = shaderSystemPtr->currentShaderProgramId;
					renderObject.textureId = textureSystemPtr->currentTextureId;
					renderObject.meshId = meshSystemPtr->currentMeshId;
					renderObjects.push_back(renderObject);

					currentRenderobjectId = renderObjects.size() - 1;
					nameToIndex[name] = currentRenderobjectId;

					//remove from queue
					renderObjectNamesQueue.erase(renderObjectNamesQueue.begin() + i);
					return;
				}
				logError("RenderObject found, but could not be loaded!");
			}
		}
		logError(String("RenderObject With this name doesent exist. (").append(name.data).append(")"));
	}
}

RenderObject& RenderObjectSystem::current() { return renderObjects[currentRenderobjectId]; }

void RenderObjectSystem::create(Renderer& renderer) {
	meshSystemPtr = &renderer.meshSystem;
	textureSystemPtr = &renderer.textureSystem;
	shaderSystemPtr = &renderer.shaderSystem;
}

Index RenderObjectSystem::addRenderObject(const RenderObject& renderObject) {
	renderObjects.push_back(renderObject);
	currentRenderobjectId = renderObjects.size() - 1;
	return currentRenderobjectId;
}

void RenderObjectSystem::addRenderObject(const RenderObjectNames& renderobject) {
	addRenderObject(renderobject.renderObjectName, renderobject.meshName, renderobject.textureName,
	                renderobject.shaderName);
}

RenderObjectNames::RenderObjectNames(const String& name, const String& mesh, const String& texture,
                                     const String& shader) {
	renderObjectName = name;
	meshName = mesh;
	shaderName = texture;
	textureName = shader;
}

void RenderObjectSystem::addRenderObject(const Name& name, const Name& meshName, const Name& textureName,
                                         const Name& shaderName) {
	RenderObjectNames renderObjectNames;
	renderObjectNames.renderObjectName = name;
	renderObjectNames.meshName = meshName;
	renderObjectNames.shaderName = shaderName;
	renderObjectNames.textureName = textureName;

	renderObjectNamesQueue.push_back(renderObjectNames);
}
