#include "PerformanceGraph.hpp"
#include "Renderer.hpp"
#include "RenderingOverrides.hpp"

void PerformanceGraph::addTime(const Float time)
{
	if (times.size() >= width) times.erase(times.begin());
	times.push_back(time);
	maxSample = *std::ranges::max_element(times);
}

void PerformanceGraph::render(Window& window)
{
	AlphaBlendOverride(window.renderer, true);

	// [TODO] remove opengl calls
	OpenGL::apiSetBlending(window.renderer.openGlContext, true);
	OpenGL::apiBlendFunc(window.renderer.openGlContext, BlendFunction::SrcAlpha, BlendFunction::OneMinusSrcAlpha);
	OpenGL::apiBlendEquation(window.renderer.openGlContext, BlendEquation::Add);
	if (times.empty()) return;
	window.renderer.useShader("color");
	window.renderer.screenSpaceFromBottomLeft();
	window.renderer.fill(0, 0, 0, 0.5);
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(100 + width / 2.0f, 100 + height / 2.0f, width, height));
	window.renderer.plane();
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(100, 100, 1, 1));
	window.renderer.fill(1);
	Vec2Vector line;
	for (Int i = 0; i < times.size(); i++) line.emplace_back(i, times[i] / maxSample * height);
	window.renderer.line(line);
}
