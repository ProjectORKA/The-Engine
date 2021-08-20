
#include "TextRenderSystem.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	gpuText.unload();
	textTexture.unload();
}
void TextRenderSystem::create(Renderer & renderer) {
	
	rendererPtr = &renderer;
	
	CPUTexture cpuTextTexture;
	cpuTextTexture.load("fontSDF");
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add("text");
}
void TextRenderSystem::render(String text, Float x, Float y, FontStyle style) {

	UInt length = text.size();

	cpuText.name = "text";
	cpuText.vertices.clear();
	cpuText.uvs.clear();
	cpuText.indices.clear();
	cpuText.normals.clear();
	cpuText.drawMode = MeshDrawMode::dynamicMode;
	gpuText.unload();

	for (UInt i = 0; i < length; i++) {

		//Float up = yPos;
		//Float left = xPos + Float(i) * size;
		//Float down = up + size;
		//Float right = left + size;

		Float up = 1.0;
		Float left = Float(i) * style.letterSpacing;
		Float down = 0.0;
		Float right = left + 1.0;

		Int character = text[i];
		
		Float uvUp = 1 - Float(character / 16) / 16.0;
		Float uvLeft = Float(character % 16) / 16.0;
		Float uvRight = uvLeft + 1.0 / 16.0;
		Float uvDown = uvUp - 1.0 / 16.0;

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

	gpuText.unload();
	gpuText.upload(cpuText);
	//gpuText.update(cpuText);

	rendererPtr->shaderSystem.use("text");
	textTexture.use(0);

	apiEnable(GL_BLEND);
	apiBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	rendererPtr->screenSpace();
	rendererPtr->shaderSystem.uniforms.data.mMatrix = scale(translate(Matrix(1), Vec3(x, y, 0)),Vec3(style.size));
	rendererPtr->shaderSystem.uniforms.update();

	apiDisable(GL_CULL_FACE);
	gpuText.render();
}
