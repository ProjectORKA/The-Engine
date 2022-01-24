
#include "Sandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

void Sandbox::render(Renderer& renderer) {
	//renderer.clearColor(Color(0.008, 0.008, 0.009, 0));
	renderer.clearColor(Color(0,0,0,0));
	renderer.useShader("color");
	renderer.uniforms().customColor(Color(0.218, 0.018, 0.009, 1));
	renderer.screenSpace();

	Transform t;
	t.scale = 512;
	t.render(renderer);
	renderer.renderMesh("plane");
}
void Sandbox::mouseIsMoving(Window& window, IVec2 position) {
	
}
void Sandbox::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	
}
void Sandbox::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {

}