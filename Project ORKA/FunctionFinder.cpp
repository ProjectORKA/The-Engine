#include "FunctionFinder.hpp"

#include "JobSystem.hpp"
#include "Random.hpp"
#include "UserInterface.hpp"

void testParameters(FunctionFinderRenderer& ff)
{
	Float fitness = 0;

	Vector<Vec2>  estimate;
	estimate.resize(ff.data.size());
	Vector<Float> difference;
	difference.resize(ff.data.size());

	const Float a = ff.bestA + randomFloatSlow(-ff.learnRate, ff.learnRate);
	const Float b = ff.bestB + randomFloatSlow(-ff.learnRate, ff.learnRate);
	const Float c = ff.bestC + randomFloatSlow(-ff.learnRate, ff.learnRate);
	//const Float d = ff.bestD + randomFloatFast(-0.1, 0.1);

	for (Int i = 0; i < ff.data.size(); i++)
	{
		estimate[i].x = ff.data[i].x;
		estimate[i].y = FunctionFinderRenderer::function3(ff.data[i].x, a, b, c);
		const Float diff = abs(ff.data[i].y - estimate[i].y);
		difference[i] = diff;
		fitness -= diff;
	}

	if (fitness > ff.bestScore)
	{
		ff.best      = estimate;
		ff.bestScore = fitness;
		ff.bestA     = a;
		ff.bestB     = b;
		ff.bestC     = c;
	}

	ff.learnRate *= 0.9999999;

	if(ff. threading) jobSystem.enqueue(testParameters, std::ref(ff));
}

void FunctionFinderRenderer::create(Window& window)
{
	constexpr Int samples = 100;

	for (int i = 0; i < samples; i++)
	{
		Vec2 dataPoint = bezier2D(Vec2(0, 0), Vec2(50, 5), 10, static_cast<Float>(i) / samples);
		logDebug(dataPoint);
		data.push_back(dataPoint);
	}

	for (int i = 0; i < samples; i++)
	{
		data.push_back(Vec2(50 + 50 * i/samples, 5));
	}

	for (const Vec2 point : data)
	{
		if (point.x < functionBounds.x) functionBounds.x = point.x;
		if (point.x > functionBounds.y) functionBounds.y = point.x;
		if (point.y < functionBounds.z) functionBounds.z = point.y;
		if (point.y > functionBounds.w) functionBounds.w = point.y;
	}

	//estimate.resize(data.size());
	//difference.resize(data.size());
	best.resize(data.size());

	player.camera.setPosition(Vec3(0.0f, -5.0f, 0.0f));

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();

	for(int i = 0; i < jobSystem.numThreads(); i++)
	{
		jobSystem.enqueue(testParameters, std::ref(*this));
	}
}

void FunctionFinderRenderer::update(Window& window)
{
	player.update(window);
}

void FunctionFinderRenderer::destroy(Window& window)
{
	threading = false;
	jobSystem.stop();
	framebuffer.destroy();
}

void FunctionFinderRenderer::connect(GameSimulation& simulation) {}

void FunctionFinderRenderer::renderInteractive(Window& window, TiledRectangle area) {}

Matrix FunctionFinderRenderer::plotMatrix() const
{
	const Float centerX = (functionBounds.x + functionBounds.y) / 2.0f;
	const Float centerY = (functionBounds.z + functionBounds.w) / 2.0f;
	const Float scaleX  = 2.0f / (functionBounds.y - functionBounds.x);
	const Float scaleY  = 2.0f / (functionBounds.w - functionBounds.z);
	return scale(Matrix(0.95f), Vec3(scaleX, scaleY, 1.0f)) * translate(Matrix(1.0f), Vec3(-centerX, -centerY, 0.0f));
}

void FunctionFinderRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// render scene
	player.render(window); // sets the position, rotation and projection
	r.useShader("color");  // sets the color / material for the rendered objects

	if (render2D)
	{
		r.normalizedSpace();
		r.fill(1);
		r.line(data, plotMatrix());
		//r.fill(1, 0, 0);
		//r.line(estimate, plotMatrix());
		r.fill(0, 0, 1);
		r.line(best, plotMatrix());
	}
	else
	{
		r.line(data, matrixFromPositionAndSize(Vec2(100, 100), Vec2(1.0f, 0.01f)));
	}

	// text rendering
	if (!disableUi)
	{
		r.setDepthTest(false); // disables depth to always draw on top
		r.screenSpace();       // aligns coordinate system with screenspace
		r.uniforms().setMMatrix(Matrix(1));
		r.textRenderSystem.setSize(16.0f);
		r.textRenderSystem.setLetterSpacing(0.6f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		Float spacing = 30;
		Int i = 0;
		r.textRenderSystem.render(r, "FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta())), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "Scroll to change speed", Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "F for wireframe mode", Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "W A S D Q E to move", Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "d: " + toString(bestD), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "c: " + toString(bestC), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "b: " + toString(bestB), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "a: " + toString(bestA), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "Best Score: " + toString(bestScore), Vec2(50, spacing * i++));
		r.textRenderSystem.render(r, "Learn Rate: " + toString(learnRate), Vec2(50, spacing * i++));
	}

	// render to window
	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");
}

void FunctionFinderRenderer::inputEvent(Window& window, const InputEvent input)
{
	if (input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	player.inputEvent(window, input);
}