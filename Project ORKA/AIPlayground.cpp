
#include "AIPlayground.hpp"
#include "Window.hpp"

void AIPlayground::render(Window& window)
{
	Renderer& renderer = window.renderer;
	renderer.screenSpace();

	

	renderer.uniforms().data.mMatrix = scale(translate(Matrix(1), Vec3(inputManager.cursorPosition.x, inputManager.cursorPosition.y, 0)),Vec3(50));
	renderer.useShader("primitive");
	renderer.renderMesh("sphere");

	renderer.textRenderSystem.render(String(toString(inputManager.cursorPosition.y)), 0, 0, fonts.debug);
}
