
#include "UISandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

void UISandbox::render(TiledRectangle area, Renderer& renderer) {
	renderer.clearColor(Color(0.008, 0.008, 0.009, 1));
	//renderer.clearColor(Color(0,0,0,0));
	//renderer.useShader("color");
	//renderer.uniforms().customColor(Color(0.218, 0.018, 0.009, 1));
	//renderer.screenSpace();

	//Transform t;
	//t.scale = 512;
	//t.render(renderer);
	//renderer.renderMesh("plane");
}
void UISandbox::mouseIsMoving(Window& window, IVec2 position) {
	
}
void UISandbox::mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) {
	
}
void UISandbox::buttonIsPressed(Window& window, Key key, Int action, Int modifiers) {

}