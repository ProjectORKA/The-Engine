#include "Game.hpp"
#include "Window.hpp"
#include "Debug.hpp"

namespace
{
	[[nodiscard]] Bool hasValidArea(const IVec2 size)
	{
		return size.x > 0 && size.y > 0;
	}
}

void GameSimulation::stop()
{
	if (loaded)
	{
		logDebug("Stopping game simulation thread!");
		keepRunning = false;
		destroy();
		loaded = false;
		thread.join();
	}
	else logError("Simulation not initialized!");
}

void GameSimulation::start()
{
	if (!loaded)
	{
		keepRunning = true;
		logDebug("Starting game simulation thread!");
		thread = Thread(gameSimulationThread, std::ref(*this));
	}
	else logError("Simulation already initialized!");
}

Bool GameSimulation::isLoaded() const
{
	return loaded;
}

Float GameSimulation::getTickRate() const
{
	return tickRate;
}

Bool GameSimulation::getKeepRunning() const
{
	return keepRunning;
}

void gameSimulationThread(GameSimulation& sim)
{
	sim.create();

	if (sim.isLoaded())
	{
		logDebug("Game simulation started!");
		while (sim.getKeepRunning())
		{
			TimePoint t = Clock::now() + Milliseconds(static_cast<Int>(1000.0f / sim.getTickRate()));
			sim.update(1.0f / sim.getTickRate());
			sleepUntil(t);
		}
		logDebug("Game simulation stopped!");
	}
	else logError("Simulation not initialized!");
}

void GameRenderer::create(Window& window)
{
	IVec2 size = getSize();

	// When UI layout has not run yet, the element size can still be zero.
	// Fall back to the current window framebuffer size so the game framebuffer
	// is created immediately and ready for the first render pass.
	if (!hasValidArea(size))
	{
		size = window.getContentSize();
		if (hasValidArea(size))
		{
			setWidth(size.x);
			setHeight(size.y);
		}
	}

	if (hasValidArea(size))
	{
		framebuffer.create(window.renderer, "GameFramebuffer", size);
		framebuffer.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
		framebuffer.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
		framebuffer.checkComplete(window.renderer);
		framebufferInitialized = true;
	}
	else logWarning("GameRenderer framebuffer not created: invalid size " + toString(size));
	UIElement::create(window);
}

void GameRenderer::destroy(Window& window)
{
	if (framebufferInitialized)
	{
		framebuffer.destroy(window.renderer);
		framebufferInitialized = false;
	}
	UIElement::destroy(window);
}

void GameRenderer::render(Window& window)
{
	Renderer& r    = window.renderer;
	IVec2     sz   = getSize();

	// If layout has not yet provided a size, fall back to the window.
	if (!hasValidArea(sz))
	{
		const IVec2 windowSize = window.getContentSize();
		if (hasValidArea(windowSize))
		{
			setWidth(windowSize.x);
			setHeight(windowSize.y);
			sz = windowSize;
		}
	}

	if (!hasValidArea(sz))
	{
		logWarning("GameRenderer render skipped: invalid size " + toString(sz));
		return;
	}

	const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());

	//if (renderToFramebuffer)
	//{
		if (!framebufferInitialized)
		{
			framebuffer.create(r, "GameFramebuffer", sz);
			framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
			framebuffer.add(r, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
			framebuffer.checkComplete(r);
			framebufferInitialized = true;
		}

		framebuffer.resize(r, sz);
		framebuffer.bindDraw(r);
		r.setRenderRegion(Vec2(0), sz);

		renderScene(window);

		r.drawToWindow();
		r.setRenderRegion({0, 0}, window.getSize());
		r.uiSpace(window);
		r.setDepthTest(false);
		r.setCulling(false);
		r.uniforms().setCustomInt1(1); // flip framebuffer vertically for UI blit
		framebuffer.setAsTexture(r);
		r.useShader("texture");
		// Rectangle mesh goes from y=0 down to y=-1; uiSpace uses bottom-left origin,
		// so anchor at the render-space top-left and let the mesh extend downward.
		r.uniforms().setMMatrix(matrixFromPositionAndSize(static_cast<Float>(renderPos.x), static_cast<Float>(renderPos.y), static_cast<Float>(sz.x), static_cast<Float>(sz.y)));
		r.rectangle();
		r.uniforms().setCustomInt1(0); // reset
	//}
	//else
	//{
	//	// convert UI (top-left origin) to OpenGL viewport coords (bottom-left origin)
	//	const IVec2 viewportPosition(getPosition().x, window.getHeight() - getPosition().y - sz.y);
	//	r.setRenderRegion(viewportPosition, sz);

	//	renderScene(window);

	//	// restore UI space for following UI elements
	//	r.setRenderRegion({0, 0}, window.getSize());
	//	r.uiSpace(window);
	//	r.setDepthTest(false);
	//	r.setCulling(false);
	//}
}
