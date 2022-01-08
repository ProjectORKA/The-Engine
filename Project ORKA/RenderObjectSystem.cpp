
#include "RenderObjectSystem.hpp"
#include "Renderer.hpp"

RenderObjectNames::RenderObjectNames(){}
void RenderObjectSystem::destroy()
{
	nameToIndex.clear();
	renderObjects.clear();
	renderObjectNamesQueue.clear();
}
void RenderObjectSystem::renderCurrent() {
	shaderSystemPtr->use(current().shaderID);
	textureSystemPtr->use(current().textureID);
	meshSystemPtr->render(shaderSystemPtr->uniforms, current().meshID);
}
void RenderObjectSystem::select(Index id)
{
	currentRenderobjectID = id;
}
void RenderObjectSystem::render(Index id) {
	select(id);
	renderCurrent();
}
void RenderObjectSystem::render(Name name) {
	select(name);
	renderCurrent();
}
void RenderObjectSystem::select(Name name)
{
	auto it = nameToIndex.find(name);
	if (it != nameToIndex.end()) {
		//id found, set to current
		currentRenderobjectID = it->second;
	}
	else {
		//id not found search in names list
		for (Index i = 0; i < renderObjectNamesQueue.size(); i++) {
			if (renderObjectNamesQueue[i].renderObjectName == name) {
				
				
				shaderSystemPtr->use(renderObjectNamesQueue[i].shaderName);
				textureSystemPtr->use(renderObjectNamesQueue[i].textureName);
				meshSystemPtr->use(renderObjectNamesQueue[i].meshName);

				if (shaderSystemPtr->currentShaderProgram().isLoaded && textureSystemPtr->currentTexture().loaded && meshSystemPtr->currentMesh().loaded) {

					RenderObject renderObject;
					renderObject.shaderID = shaderSystemPtr->currentShaderProgramID;
					renderObject.textureID = textureSystemPtr->currentTextureID;
					renderObject.meshID = meshSystemPtr->currentMeshID;
					renderObjects.push_back(renderObject);
					
					currentRenderobjectID = renderObjects.size() - 1;
					nameToIndex[name] = currentRenderobjectID;

					//remove from queue
					renderObjectNamesQueue.erase(renderObjectNamesQueue.begin() + i);
					return;
				}
				else {
					logError("RenderObject found, but could not be loaded!");
				}
			}
		}
		logError(String("RenderObject With this name doesent exist. (").append(name.data).append(")"));
	}
}
RenderObject& RenderObjectSystem::current()
{
	return renderObjects[currentRenderobjectID];
}
void RenderObjectSystem::create(Renderer & renderer)
{
	meshSystemPtr = &renderer.meshSystem;
	textureSystemPtr = &renderer.textureSystem;
	shaderSystemPtr = &renderer.shaderSystem;
}
Index RenderObjectSystem::addRenderObject(RenderObject& renderObject) {
	renderObjects.push_back(renderObject);
	currentRenderobjectID = renderObjects.size() - 1;
	return currentRenderobjectID;
}
void RenderObjectSystem::addRenderObject(RenderObjectNames renderobject)
{
	addRenderObject(renderobject.renderObjectName, renderobject.meshName, renderobject.textureName, renderobject.shaderName);
}
RenderObjectNames::RenderObjectNames(String name, String mesh, String texture, String shader)
{
	renderObjectName = name;
	meshName = mesh;
	shaderName = texture;
	textureName = shader;
}
void RenderObjectSystem::addRenderObject(Name name, Name meshName, Name textureName, Name shaderName) {
	RenderObjectNames renderObjectNames;
	renderObjectNames.renderObjectName = name;
	renderObjectNames.meshName = meshName;
	renderObjectNames.shaderName = shaderName;
	renderObjectNames.textureName = textureName;
	
	renderObjectNamesQueue.push_back(renderObjectNames);
}