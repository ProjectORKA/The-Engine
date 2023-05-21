#include "BitmapTextRenderSystem.hpp"

#include "Renderer.hpp"
#include "ResourceManager.hpp"

void BitmapTextRenderSystem::destroy() {
	gpuText.unload();
	textTexture.unload();
}

void BitmapTextRenderSystem::create(ResourceManager& resourceManager, Renderer& renderer) {
	CPUTexture cpuTextTexture;
	cpuTextTexture.load(resourceManager, "font", Filter::nearest, Filter::linear, Wrapping::repeat);
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add(resourceManager, "text");
}

void BitmapTextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text,
                                    const Vec2 position, const Alignment x, const Alignment y, const FontStyle style) {
	renderer.setDepthTest(false);
	const UInt length = text.size();
	const Float size = style.absoluteSize;
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

	if (x == Alignment::start) { start = -(length * style.letterSpacing); }

	if (x == Alignment::center) { start = -(static_cast<Float>(length) * style.letterSpacing) / 2.0f; }

	if (x == Alignment::end) { start = 0; }

	cpuText.name = "text";
	cpuText.clearGeometry();
	cpuText.drawMode = MeshDrawMode::DynamicMode;

	for (UInt i = 0; i < length; i++) {
		left = start + static_cast<Float>(i) * style.letterSpacing;
		right = left + 1.0f;

		const Int character = static_cast<unsigned char>(text[i]);

		const Float uvUp = 1 - static_cast<Float>(character / 16) / 16.0f;
		const Float uvLeft = static_cast<Float>(character % 16) / 16.0f;
		const Float uvRight = uvLeft + 1.0f / 16.0f;
		const Float uvDown = uvUp - 1.0f / 16.0f;

		cpuText.positions.push_back(Vec3(position.x + size * left, position.y + size * up, 0));
		cpuText.positions.push_back(Vec3(position.x + size * left, position.y + size * down, 0));
		cpuText.positions.push_back(Vec3(position.x + size * right, position.y + size * up, 0));
		cpuText.positions.push_back(Vec3(position.x + size * right, position.y + size * down, 0));

		cpuText.textureCoordinates.push_back(Vec2(uvLeft, uvUp));
		cpuText.textureCoordinates.push_back(Vec2(uvLeft, uvDown));
		cpuText.textureCoordinates.push_back(Vec2(uvRight, uvUp));
		cpuText.textureCoordinates.push_back(Vec2(uvRight, uvDown));

		cpuText.indices.push_back(i * 4);
		cpuText.indices.push_back(i * 4 + 1);
		cpuText.indices.push_back(i * 4 + 2);
		cpuText.indices.push_back(i * 4 + 1);
		cpuText.indices.push_back(i * 4 + 3);
		cpuText.indices.push_back(i * 4 + 2);
	}

	cpuText.checkIntegrity();

	gpuText.unload();
	gpuText.upload(cpuText);

	renderer.useShader(resourceManager, "text");
	textTexture.use(0);

	renderer.setAlphaBlending(true);

	gpuText.render(renderer.uniforms());
}
