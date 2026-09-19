#include "ORKAIntroSequence.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Camera.hpp"

void ORKAIntroSequence::update(Window& window)
{
	// Perform the handoff outside of render to avoid touching GL resources while
	// the intro framebuffer is bound. This prevents incomplete framebuffer
	// states in the following frame.
	if (handoffPending && followUpElement)
	{
		handoffPending = false;

		// Clean up intro resources before handing over to the next renderer to
		// avoid leaked textures/framebuffers.
		destroy(window);

		window.decorateWindow();

		window.addElement(*followUpElement);
		window.dropElement(this);

		followUpElement->create(window);
		followUpElement = nullptr;
	}
}
void ORKAIntroSequence::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void ORKAIntroSequence::create(Window& window)
{
	GameRenderer::create(window);
	introTimerStarted = false;
	handoffPending    = false;
	renderToFramebuffer = false;
	window.unDecorateWindow();
	window.setVisible(true);
}

void ORKAIntroSequence::renderScene(Window& window)
{
	Renderer& renderer = window.renderer;

	// Keep the intro framebuffer fully transparent except where the logo draws.
	renderer.clearBackground(Color(0.0f, 0.0f, 0.0f, 0.0f));

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	renderer.setWireframeMode(false);

	// camera stuff
	Camera c;
	c.setPosition(Vec3(0.0f, -15.0f, 0.0f));
	c.render(renderer);

	// logo stuff
	constexpr Float animationLength = 1.1f;
	const Float     size            = min(pow(1.0f + 1.0f / pow(animationLength, 5.0f), pow(static_cast<Float>(renderer.time.getTotal()) - 2.0f * animationLength, 5.0f)), 100.0f);
	const Float     tint            = max(2.0f - size, 0.0f);
	renderer.uniforms().setCustomColor(Color(tint, tint, tint, 1.0f));
	Matrix modelMatrix = scale(Matrix(1.0f), Vec3(size, 1.0f, size));
	modelMatrix        = rotate(modelMatrix, 20.0f / (pow(static_cast<Float>(renderer.time.getTotal()), 4.0f) + 1.0f), Vec3(0.0f, 0.0f, 1.0f));
	renderer.uniforms().setMMatrix(modelMatrix);

	renderer.useShader("orkaIntroSequence");
	renderer.useTexture("ProjectORKABakedLogo");
	renderer.renderMesh("projectORKALogo");

	// start the time when we actually start rendering
	if (!introTimerStarted)
	{
		renderer.time.reset();
		introTimerStarted = true;
	}

	// swap the intro scene with the followup
	if (renderer.time.getTotal() > 5)
	{
		handoffPending = true;
	}
}

void ORKAIntroSequence::init(UIElement& followUp)
{
	followUpElement = &followUp;
}

void ORKAIntroSequence::renderInteractive(Window& window) {}
void ORKAIntroSequence::connect(GameSimulation& simulation) {}
void ORKAIntroSequence::inputEvent(Window& window, InputEvent& input)
{
	if (!introTimerStarted || handoffPending) return;

	const Bool skippedByMouse = input.id.type == InputType::Mouse && input.state;
	const Bool skippedByKey   = input.id.type == InputType::KeyBoard && input.state;
	if (skippedByMouse || skippedByKey)
	{
		handoffPending = true;
		input.consume();
	}
}
