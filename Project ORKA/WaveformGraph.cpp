#include "WaveformGraph.hpp"

#include "Renderer.hpp"
#include "RenderingOverrides.hpp"

void WaveformGraph::setSamples(const Vector<Float>& waveformSamples, const Vector<Float>& controlSamples, const Vector<Float>& audioSamples)
{
	waveform      = waveformSamples;
	controlSignal = controlSamples;
	audioOutput   = audioSamples;
}

void WaveformGraph::render(Window& window)
{
	if(waveform.empty() && controlSignal.empty() && audioOutput.empty()) return;

	Renderer& renderer = window.renderer;
	AlphaBlendOverride alphaOverride(renderer, true);

	OpenGL::apiSetBlending(renderer.openGlContext, true);
	OpenGL::apiBlendFunc(renderer.openGlContext, BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha);
	OpenGL::apiBlendEquation(renderer.openGlContext, BlendEquation::Add);

	renderer.useShader("color");
	renderer.screenSpaceFromBottomLeft();

	const Float x = positionFromBottomLeft.x;
	const Float y = positionFromBottomLeft.y;
	const Float centerY = y + static_cast<Float>(height) * 0.5f;
	const Float scaleY  = static_cast<Float>(height) * 0.48f;
	constexpr Float visualGain = 1.55f;

	renderer.fill(0, 0, 0, 0.45f);
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(x + width / 2.0f, y + height / 2.0f, static_cast<Float>(width), static_cast<Float>(height)));
	renderer.plane();

	Vec2Vector midLine;
	midLine.emplace_back(x, centerY);
	midLine.emplace_back(x + static_cast<Float>(width), centerY);
	renderer.uniforms().setMMatrix(Matrix(1));
	renderer.fill(0.55f, 0.55f, 0.55f, 0.65f);
	renderer.line(midLine);

	auto drawCenteredLine = [&](const Vector<Float>& samples, const Float r, const Float g, const Float b, const Float a)
	{
		if(samples.empty()) return;
		Vec2Vector line;
		line.reserve(samples.size());
		const Float stepX = samples.size() > 1 ? static_cast<Float>(width) / static_cast<Float>(samples.size() - 1) : 0.0f;
		for(SizeT i = 0; i < samples.size(); ++i)
		{
			const Float clampedSample = clamp(samples[i] * visualGain, -1.0f, 1.0f);
			line.emplace_back(x + static_cast<Float>(i) * stepX, centerY + clampedSample * scaleY);
		}
		renderer.uniforms().setMMatrix(Matrix(1));
		renderer.fill(r, g, b, a);
		renderer.line(line);
	};

	drawCenteredLine(audioOutput, 0.95f, 0.35f, 0.95f, 1.0f);
	drawCenteredLine(waveform, 0.15f, 0.95f, 1.0f, 0.85f);

	if(controlSignal.empty()) return;

	Vec2Vector controlLine;
	controlLine.reserve(controlSignal.size());
	const Float controlStepX = controlSignal.size() > 1 ? static_cast<Float>(width) / static_cast<Float>(controlSignal.size() - 1) : 0.0f;
	for(SizeT i = 0; i < controlSignal.size(); ++i)
	{
		const Float clampedControl = clamp(controlSignal[i], 0.0f, 1.0f);
		controlLine.emplace_back(x + static_cast<Float>(i) * controlStepX, y + clampedControl * static_cast<Float>(height));
	}

	renderer.fill(1.0f, 0.5f, 0.2f, 0.9f);
	renderer.line(controlLine);
}
