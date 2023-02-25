
#include "BitmapTextRenderSystem.hpp"

#include "Renderer.hpp"
#include "Engine.hpp"

void BitmapTextRenderSystem::destroy() {
	gpuText.unload();
	textTexture.unload();
}
void BitmapTextRenderSystem::create(Engine & engine, Renderer& renderer) {
	CPUTexture cpuTextTexture;
	cpuTextTexture.load(engine, "fontSDF");
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add(engine, "text");

}
void BitmapTextRenderSystem::render(Engine& engine, Renderer & renderer, String text, Vec2 position, FontStyle style) {
	
	renderer.setDepthTest(false);
	UInt length = text.size();

	cpuText.name = "text";
	cpuText.clearGeometry();
	
	cpuText.drawMode = MeshDrawMode::dynamicMode;

	Float up = 1.0;
	Float down = 0.0;

	for (UInt i = 0; i < length; i++) {
		//[TODO] use 4 vertex and index pushbacks instead of 6
		Float left = Float(i) * style.letterSpacing;
		Float right = left + 1.0;

		Int character = text[i];

		Float uvUp = 1 - Float(character / 16) / 16.0;
		Float uvLeft = Float(character % 16) / 16.0;
		Float uvRight = uvLeft + 1.0 / 16.0;
		Float uvDown = uvUp - 1.0 / 16.0;

		cpuText.positions.pushBack(Vec3(left, up, 0));
		cpuText.positions.pushBack(Vec3(left, down, 0));
		cpuText.positions.pushBack(Vec3(right, up, 0));
		cpuText.positions.pushBack(Vec3(left, down, 0));
		cpuText.positions.pushBack(Vec3(right, down, 0));
		cpuText.positions.pushBack(Vec3(right, up, 0));


		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvUp));
		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvUp));
		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvUp));

		cpuText.normals.pushBack(Vec3(0, 0, 1));
		cpuText.normals.pushBack(Vec3(0, 0, 1));
		cpuText.normals.pushBack(Vec3(0, 0, 1));
		cpuText.normals.pushBack(Vec3(0, 0, 1));
		cpuText.normals.pushBack(Vec3(0, 0, 1));
		cpuText.normals.pushBack(Vec3(0, 0, 1));

		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));
		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));
		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));
		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));
		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));
		cpuText.vertexColors.pushBack(Vec3(1.0f, 1.0f, 1.0f));

		cpuText.indices.pushBack(i * 6);
		cpuText.indices.pushBack(i * 6 + 1);
		cpuText.indices.pushBack(i * 6 + 2);
		cpuText.indices.pushBack(i * 6 + 3);
		cpuText.indices.pushBack(i * 6 + 4);
		cpuText.indices.pushBack(i * 6 + 5);
	}

	cpuText.checkIntegrity();

	gpuText.unload();
	gpuText.upload(cpuText);

	renderer.useShader(engine, "text");
	textTexture.use(0);

	renderer.setAlphaBlending(true);

	//renderer.uniforms().mMatrix(scale(translate(Matrix(1), Vec3(position, 0)), Vec3(style.absoluteSize)));
	renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(position.x, position.y, style.absoluteSize, style.absoluteSize));
	gpuText.render(renderer.uniforms());
}