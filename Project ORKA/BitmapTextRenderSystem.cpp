#include "BitmapTextRenderSystem.hpp"
#include "Renderer.hpp"

void BitmapTextRenderSystem::destroy()
{
	gpuText.unload();
	textTexture.unload();
}

void BitmapTextRenderSystem::create(Renderer& renderer)
{
	CPUTexture cpuTextTexture;
	cpuTextTexture.load(Name("font"), Filter::Nearest, Filter::Linear, Wrapping::Repeat);
	textTexture.load(cpuTextTexture);
	renderer.shaderSystem.add("text");
}

void BitmapTextRenderSystem::render(Renderer& renderer, const String& text, const Vec2 position, const Alignment x, const Alignment y, const Float absoluteSize, const Float letterSpacing)
{
	renderer.setDepthTest(false);
	const UInt  length = static_cast<UInt>(text.size());
	const Float size   = absoluteSize;
	Float       up     = 0, down = 0, start = 0;

	if(y == Alignment::top)
	{
		up   = 0.0;
		down = -1;
	}

	if(y == Alignment::middle)
	{
		up   = 0.5;
		down = -0.5;
	}

	if(y == Alignment::bottom)
	{
		up   = 1.0;
		down = 0.0;
	}

	if(x == Alignment::left) start = 0;

	if(x == Alignment::center) start = -(static_cast<Float>(length) * letterSpacing) / 2.0f;

	if(x == Alignment::right) start = -(static_cast<Float>(length) * letterSpacing);

	cpuText.name = "text";
	cpuText.clearGeometry();
	cpuText.drawMode = BufferUsage::DynamicDraw;

	for(UInt i = 0; i < length; i++)
	{
		const Float left  = start + static_cast<Float>(i) * letterSpacing;
		const Float right = left + 1.0f;

		const Int character = static_cast<unsigned char>(text[i]);

		const Float uvUp    = 1.0f - static_cast<Float>(character / 16) / 16.0f;
		const Float uvLeft  = static_cast<Float>(character % 16) / 16.0f;
		const Float uvRight = uvLeft + 1.0f / 16.0f;
		const Float uvDown  = uvUp - 1.0f / 16.0f;

		cpuText.positions.emplace_back(position.x + size * left, position.y + size * up, 0);
		cpuText.positions.emplace_back(position.x + size * left, position.y + size * down, 0);
		cpuText.positions.emplace_back(position.x + size * right, position.y + size * up, 0);
		cpuText.positions.emplace_back(position.x + size * right, position.y + size * down, 0);

		cpuText.textureCoordinates.emplace_back(uvLeft, uvUp);
		cpuText.textureCoordinates.emplace_back(uvLeft, uvDown);
		cpuText.textureCoordinates.emplace_back(uvRight, uvUp);
		cpuText.textureCoordinates.emplace_back(uvRight, uvDown);

		cpuText.indices.emplace_back(i * 4);
		cpuText.indices.emplace_back(i * 4 + 1);
		cpuText.indices.emplace_back(i * 4 + 2);
		cpuText.indices.emplace_back(i * 4 + 1);
		cpuText.indices.emplace_back(i * 4 + 3);
		cpuText.indices.emplace_back(i * 4 + 2);
	}

	cpuText.checkIntegrity();

	gpuText.unload();
	gpuText.upload(cpuText);

	renderer.useShader("text");
	textTexture.useTextureInSlot(0);

	renderer.setAlphaBlending(true);

	gpuText.render(renderer.uniforms());
}
