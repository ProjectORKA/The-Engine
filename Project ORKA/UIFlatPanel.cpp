
#include "UIFlatPanel.hpp"

//ui flat panel
//void createUIFlatPanel(UIElement* element, Window& window)
//{
//	window.renderer.renderObjectSystem.addRenderObject(RenderObjectNames("fullScreenQuad", "fullScreenQuad", "default", "default"));
//
//	element->createChildren(window);
//}
//void renderUIFlatPanel(UIElement* element, Window& window, TiledRectangle screenArea)
//{
//	UIFlatPanel* flatPanel = static_cast<UIFlatPanel*>(element);
//	Renderer& renderer = window.renderer;
//
//	//render settings
//	renderer.setCulling(false);
//	renderer.setDepthTest(false);
//
//	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
//	renderer.shaderSystem.uniforms.data.vpMatrix = Matrix(1);
//	renderer.shaderSystem.uniforms.data.customColor = Vec4(0, 0, 0, 1);
//	renderer.shaderSystem.uniforms.data.custom1 = 0.01;
//	renderer.shaderSystem.uniforms.update();
//
//	Index imageID = 0;
//	Index blurID = 1;
//	Index maskID = 2;
//	Index dropShadowID = 3;
//
//	RenderRegion wholeArea = renderer.renderRegion;
//
//	//create mask
//	renderer.renderRegion.set(wholeArea.region);
//	renderer.framebufferSystem.use(maskID);
//	renderer.clearColor(Color(1));
//	renderer.renderRegion.set(screenArea);
//	renderer.shaderSystem.use("unlit");
//	renderer.meshSystem.render("fullScreenQuad");
//
//	//create drop shadow
//	renderer.renderRegion.set(wholeArea.region);
//	renderer.createBlurTexture(maskID, dropShadowID);
//
//	//create blur texture
//	renderer.createBlurTexture(imageID, blurID);
//
//	//render ui
//	renderer.framebufferSystem.use(0);
//	renderer.framebufferSystem.framebuffers[imageID].colorTexture.use(0);
//	renderer.framebufferSystem.framebuffers[maskID].colorTexture.use(1);
//	renderer.framebufferSystem.framebuffers[dropShadowID].colorTexture.use(2);
//	renderer.framebufferSystem.framebuffers[blurID].colorTexture.use(3);
//	renderer.textureSystem.use("noise", 4);
//	renderer.shaderSystem.use("blurredUI");
//	renderer.meshSystem.render("fullScreenQuad");
//
//	//advance rendering
//	element->renderChildren(window, screenArea);
//}
//
//UIFlatPanel::UIFlatPanel() {
//	destroyFunction = destroyUIElement;
//	updateFunction = updateUIElement;
//	createFunction = createUIFlatPanel;
//	renderFunction = renderUIFlatPanel;
//}
