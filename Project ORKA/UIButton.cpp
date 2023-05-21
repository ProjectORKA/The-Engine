#include "UIButton.hpp"
#include "Window.hpp"

UIButton::UIButton() {}

UIButton& UIButton::insert(UiElement& element)
{
	content = &element;
	return *this;
}

void UIButton::update(Window& window)
{
	content->update(window);
}

void UIButton::inputEvent(Window& window, const InputEvent input)
{
	if(input == InputEvent(InputType::Mouse, 0, true)) pressed = window.renderer.idFramebuffer.objectId == id;
	if(input == InputEvent(InputType::Mouse, 0, false))
	{
		if(pressed)
		{
			pressed = false;
			doThis();
		}
	}

	if(content) content->inputEvent(window, input);
}

void UIButton::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea)
{
	Renderer& renderer = window.renderer;

	constraints.update(renderArea);

	renderer.useShader(resourceManager, "color");
	if(pressed)
	{
		renderer.uniforms().customColor(Color(1, 1, 0, 1));
	}
	else
	{
		const UInt objectId = renderer.idFramebuffer.objectId;

		if(objectId == id)
		{
			renderer.uniforms().customColor(Color(1));
		}
		else
		{
			renderer.uniforms().customColor(Color(0.1, 0.1, 0.1, 1));
		}
	}

	renderer.screenSpace();
	renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));
	renderer.renderMesh(resourceManager, "plane");

	if(content) content->render(resourceManager, window, renderArea);
}

void UIButton::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea)
{
	constraints.update(renderArea);

	window.renderer.useShader(resourceManager, "idShader");
	window.renderer.screenSpace();
	window.renderer.uniforms().objectId(id);
	window.renderer.uniforms().mMatrix(matrixFromTiledRectangle(renderArea));

	window.renderer.renderMesh(resourceManager, "plane");
}