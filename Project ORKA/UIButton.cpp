#include "UIButton.hpp"
#include "Window.hpp"

UIButton::UIButton() = default;

void UIButton::doThis()
{
	beep();
}

void UIButton::destroy(Window& window) {}

void UIButton::update(Window& window)
{
	if(content) content->update(window);
}

UIButton& UIButton::insert(UIElement& element)
{
	content = &element;
	return *this;
}

void UIButton::create(ResourceManager& resourceManager, Window& window) {}

void UIButton::inputEvent(Window& window, const InputEvent input)
{
	if(input == InputEvent(InputType::Mouse, 0, true)) pressed = window.renderer.objectId == id;
	if(input == InputEvent(InputType::Mouse, 0, false))
		if(pressed)
		{
			pressed = false;
			doThis();
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
		renderer.uniforms().setCustomColor(Color(1, 1, 0, 1));
	}
	else
	{
		const UInt objectId = renderer.objectId;

		if(objectId == id) renderer.uniforms().setCustomColor(Color(1));
		else renderer.uniforms().setCustomColor(Color(0.1, 0.1, 0.1, 1));
	}

	renderer.screenSpace();
	renderer.uniforms().setMMatrix(matrixFromTiledRectangle(renderArea));
	renderer.renderMesh(resourceManager, "plane");

	if(content) content->render(resourceManager, window, renderArea);
}

void UIButton::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea)
{
	constraints.update(renderArea);

	window.renderer.useShader(resourceManager, "idShader");
	window.renderer.screenSpace();
	window.renderer.uniforms().setObjectId(id);
	window.renderer.uniforms().setMMatrix(matrixFromTiledRectangle(renderArea));

	window.renderer.renderMesh(resourceManager, "plane");
}
