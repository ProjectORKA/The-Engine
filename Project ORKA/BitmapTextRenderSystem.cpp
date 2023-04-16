
#include "BitmapTextRenderSystem.hpp"

#include "Renderer.hpp"
#include "ResourceManager.hpp"

void BitmapTextRenderSystem::destroy() {
	gpuText.unload();
	textTexture.unload();
}
void BitmapTextRenderSystem::create(ResourceManager& resourceManager, Renderer& renderer) {
	CPUTexture cpuTextTexture;
	cpuTextTexture.load(resourceManager, "font");
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add(resourceManager, "text");
}

void BitmapTextRenderSystem::render(ResourceManager& resourceManager, Renderer & renderer, String text, Vec2 position, Alignment x, Alignment y, FontStyle style) {

	renderer.setDepthTest(false);
	UInt length = text.size();
	Float size = style.absoluteSize;
	Float up = 1.0;
	Float down = 0.0;
	Float left = 0;
	Float right = 0;
	Float start = 0;

	if (y == Alignment::start) {
		up = 0.0;
		down = -1;
	}

	if (y == Alignment::center) {
		up = 0.5;
		down = -0.5;
	}

	if (x == Alignment::start) {
		start = -(length * style.letterSpacing);
	}

	if (x == Alignment::center) {
		start = -(length * style.letterSpacing)/2;
	}

	if (x == Alignment::end) {
		start = 0;
	}

	cpuText.name = "text";
	cpuText.clearGeometry();	
	cpuText.drawMode = MeshDrawMode::dynamicMode;

	for (UInt i = 0; i < length; i++) {
		//[TODO] use 4 vertex and index pushbacks instead of 6
		left = start + Float(i) * style.letterSpacing;
		right = left + 1.0;

		Int character = text[i];

		Float uvUp = 1 - Float(character / 16) / 16.0;
		Float uvLeft = Float(character % 16) / 16.0;
		Float uvRight = uvLeft + 1.0 / 16.0;
		Float uvDown = uvUp - 1.0 / 16.0;

		cpuText.positions.pushBack(Vec3(position.x + size * left,  position.y + size * up, 0));
		cpuText.positions.pushBack(Vec3(position.x + size * left,  position.y + size * down, 0));
		cpuText.positions.pushBack(Vec3(position.x + size * right, position.y + size * up, 0));
		cpuText.positions.pushBack(Vec3(position.x + size * left,  position.y + size * down, 0));
		cpuText.positions.pushBack(Vec3(position.x + size * right, position.y + size * down, 0));
		cpuText.positions.pushBack(Vec3(position.x + size * right, position.y + size * up, 0));

		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvUp));
		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvUp));
		cpuText.textureCoordinates.pushBack(Vec2(uvLeft, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvDown));
		cpuText.textureCoordinates.pushBack(Vec2(uvRight, uvUp));

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

	renderer.useShader(resourceManager, "text");
	textTexture.use(0);

	renderer.setAlphaBlending(true);

	gpuText.render(renderer.uniforms());
}