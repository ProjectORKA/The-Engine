
#include "UIFlatPanel.hpp"

//ui flat panel
void createUIFlatPanel(UIElement* element, Window& window)
{
	window.renderer.renderObjectSystem.addRenderObject(RenderObjectNames("fullScreenQuad", "fullScreenQuad", "default", "default"));

	element->createChildren(window);
}
void renderUIFlatPanel(UIElement* element, Window& window, TiledRectangle screenArea)
{
	UIFlatPanel* flatPanel = static_cast<UIFlatPanel*>(element);
	Renderer& renderer = window.renderer;

	//render settings
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.vpMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.data.customColor = Vec4(0, 0, 0, 1);
	renderer.shaderSystem.uniforms.data.custom1 = 0.01;
	renderer.shaderSystem.uniforms.update();

	Framebuffer& image = renderer.framebufferSystem.framebuffers[0];
	Framebuffer& blur = renderer.framebufferSystem.framebuffers[1];
	Framebuffer& mask = renderer.framebufferSystem.framebuffers[2];
	Framebuffer& dropShadow = renderer.framebufferSystem.framebuffers[3];

	RenderRegion wholeArea = renderer.renderRegion;

	//create mask
	mask.use();
	renderer.clearColor(Color(1));
	renderer.renderRegion.setRenderRegion(screenArea);
	renderer.shaderSystem.use("unlit");
	renderer.meshSystem.renderMesh("fullScreenQuad");

	//create drop shadow
	renderer.renderRegion.setRenderRegion(wholeArea.region);
	renderer.createBlurTexture(mask, dropShadow);


	//create blur texture
	renderer.createBlurTexture(image, blur);

	//render ui
	renderer.framebufferSystem.select(0);
	image.colorTexture.use(0);
	mask.colorTexture.use(1);
	dropShadow.colorTexture.use(2);
	blur.colorTexture.use(3);
	renderer.textureSystem.use("noise", 4);
	renderer.shaderSystem.use("blurredUI");
	renderer.meshSystem.renderMesh("fullScreenQuad");

	//advance rendering
	element->renderChildren(window, screenArea);
}

UIFlatPanel::UIFlatPanel() {
	destroyFunction = destroyUIElement;
	updateFunction = updateUIElement;
	createFunction = createUIFlatPanel;
	renderFunction = renderUIFlatPanel;
}
