
#include "AIPlayground.hpp"
#include "Window.hpp"

void AIPlayground::render(TiledRectangle area, Renderer& renderer)
{
	renderer.screenSpace();

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(inputManager.cursorPosition.x, inputManager.cursorPosition.y, 0)), Vec3(50));
	renderer.useShader("primitive");
	renderer.renderMesh("sphere");

	renderer.renderText(String(toString(inputManager.cursorPosition.y)), Vec2(0), fonts.debug);
}
