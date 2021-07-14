
#include "TextRenderSystem.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	gpuText.unload();
	textTexture.unload();
}
void TextRenderSystem::create(Renderer & renderer) {
	
	rendererPtr = &renderer;
	
	CPUTexture cpuTextTexture;
	cpuTextTexture.load("font");
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add("text");
}
void TextRenderSystem::render(String text, Float xPos, Float yPos, Float size) {

	UInt length = text.size();

	cpuText.name = "text";
	cpuText.vertices.clear();
	cpuText.uvs.clear();
	cpuText.indices.clear();
	cpuText.drawMode = MeshDrawMode::dynamicMode;
	gpuText.unload();

	for (UInt i = 0; i < length; i++) {

		Float up = yPos;
		Float left = xPos + Float(i) * size;
		Float down = up + size;
		Float right = left + size;

		Int character = text[i]+16;
		Float uvUp = (Float((character) / 16) / -16.0f); //[TODO] there is an error, the y coordinate is inverted, maybe fix this, maybe not
		Float uvLeft = Float(character % 16) / 16.0f;
		Float uvDown = uvUp + 0.0625;
		Float uvRight = uvLeft + 0.0625;

		cpuText.vertices.push_back(Vec3(left,up,0));
		cpuText.vertices.push_back(Vec3(left,down,0));
		cpuText.vertices.push_back(Vec3(right,up,0));
		cpuText.vertices.push_back(Vec3(left,down,0));
		cpuText.vertices.push_back(Vec3(right,down,0));
		cpuText.vertices.push_back(Vec3(right,up,0));


		cpuText.uvs.push_back(Vec2(uvLeft, uvUp));
		cpuText.uvs.push_back(Vec2(uvLeft, uvDown));
		cpuText.uvs.push_back(Vec2(uvRight, uvUp));
		cpuText.uvs.push_back(Vec2(uvLeft, uvDown));
		cpuText.uvs.push_back(Vec2(uvRight,uvDown));
		cpuText.uvs.push_back(Vec2(uvRight,uvUp));

		cpuText.normals.push_back(Vec3(0, 0, 1));
		cpuText.normals.push_back(Vec3(0, 0, 1));
		cpuText.normals.push_back(Vec3(0, 0, 1));
		cpuText.normals.push_back(Vec3(0, 0, 1));
		cpuText.normals.push_back(Vec3(0, 0, 1));
		cpuText.normals.push_back(Vec3(0, 0, 1));

		cpuText.indices.push_back(i * 6);
		cpuText.indices.push_back(i * 6 + 1);
		cpuText.indices.push_back(i * 6 + 2);
		cpuText.indices.push_back(i * 6 + 3);
		cpuText.indices.push_back(i * 6 + 4);
		cpuText.indices.push_back(i * 6 + 5);
	}

	cpuText.readyForUpload = true;

	gpuText.upload(cpuText);

	rendererPtr->shaderSystem.use("text");
	textTexture.use(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	rendererPtr->shaderSystem.uniforms.data.vpMatrix = scale(translate(Matrix(1),Vec3(-1,-1,0)), Vec3(1)/Vec3(rendererPtr->renderRegion.region.size, 1));

	rendererPtr->shaderSystem.uniforms.data.mMatrix = scale(Matrix(1),Vec3(1000));
	rendererPtr->shaderSystem.uniforms.update();

	glDisable(GL_CULL_FACE);
	gpuText.render();
}
