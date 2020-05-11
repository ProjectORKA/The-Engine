
#include "RenderObjectSystem.hpp"

void RenderObjectSystem::create() {
	meshSystem.create();

	textureSystem.create();

	shaderSystem.create();

	addRenderObject("defaultObject", "skeleton", "skeleton", "default");

}
void RenderObjectSystem::addRenderObject(String name, String meshName, String textureName, String shaderName) {
	
	meshSystem.loadFromMeshFile(meshName);

	auto itM = meshSystem.meshNames.find(meshName);
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
		debugPrint("Could not find assets to create Render Object.");
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
		debugPrint("RenderObject With this name doesent exist.");
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
