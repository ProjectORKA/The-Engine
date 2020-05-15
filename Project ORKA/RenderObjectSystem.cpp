
#include "RenderObjectSystem.hpp"

void RenderObjectSystem::create() {
	meshSystem.create();

	textureSystem.create();

	shaderSystem.create();

	addRenderObject("default", "default", "default", "default");
	addRenderObject("skeleton", "skeleton", "skeleton", "default");
	addRenderObject("monkey", "monkey", "default", "default");
	addRenderObject("sky", "sky", "sky", "default");

}
void RenderObjectSystem::addRenderObject(String name, String meshName, String textureName, String shaderName) {
	
	//find the specified mesh
	auto itM = meshSystem.meshNames.find(meshName);
	if (itM == meshSystem.meshNames.end()) {
		//its not already loaded
		meshSystem.loadMesh(name);
	}
	itM = meshSystem.meshNames.find(meshName);

	auto itS = shaderSystem.shaderNames.find(shaderName);
	auto itT = textureSystem.textureNames.find(textureName);

	if (itM != meshSystem.meshNames.end() && itS != shaderSystem.shaderNames.end() && itT != textureSystem.textureNames.end()) {
		renderObjects.emplace_back();
		renderObjects.back().meshID = itM->second;
		renderObjects.back().shaderID = itS->second;
		renderObjects.back().textureID = itT->second;
		nameToIndex[name] = renderObjects.size() - 1;
	}
	else {
		logDebug(String("Could not find assets to create Render Object. (").append(name).append(" = ").append(meshName).append("|").append(textureName).append("|").append(shaderName).append(")"));
	}
}
void RenderObjectSystem::render(String name) {
	auto it = nameToIndex.find(name);
	if (it != nameToIndex.end()) {
		RenderObject& renderObject = renderObjects[it->second];
		textureSystem.render(renderObject.textureID);
		shaderSystem.useShader(renderObject.shaderID);
		meshSystem.renderMesh(renderObject.meshID);
	}
	else {
		logDebug("RenderObject With this name doesent exist.");
	}
}

void RenderObjectSystem::destroy()
{
	meshSystem.destroy();
	shaderSystem.destroy();
	textureSystem.destroy();

	nameToIndex.clear();
	renderObjects.clear();
}
