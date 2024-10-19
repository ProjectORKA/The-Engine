#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

UserInterface ui;

void UIImage::update(Window& window) {}

void UIImage::destroy(Window& window) {}

void UITextBox::update(Window& window) {}

void UICheckBox::update(Window& window) {}

void UITextBox::destroy(Window& window) {}

//UIButton& UserInterface::button()
//{
//	if(initialized)
//	{
//		buttons.emplace_back();
//		return buttons.back();
//	}
//	logError("UserInterface not initialized!");
//}

void UICheckBox::destroy(Window& window) {}

UIImage::UIImage(const Name& name)
{
	this->name = name;
}

UITextBox::UITextBox(String& data)
{
	this->data = &data;
}

UICheckBox::UICheckBox(Bool& data)
{
	this->data = &data;
}

//UIContainer& UserInterface::container()
//{
//	if(initialized)
//	{
//		containers.emplace_back();
//		return containers.back();
//	}
//	logError("UserInterface not initialized!");
//}
//
//UIImage& UserInterface::image(Name name)
//{
//	if(initialized)
//	{
//		images.emplace_back(name);
//		return images.back();
//	}
//	logError("UserInterface not initialized!");
//}
//
//UICheckBox& UserInterface::checkBox(Bool& data)
//{
//	if(initialized)
//	{
//		checkBoxes.emplace_back(data);
//		return checkBoxes.back();
//	}
//	logError("UserInterface not initialized!");
//}
//
//UITextBox& UserInterface::textBox(String& data)
//{
//	if(initialized)
//	{
//		textBoxes.emplace_back(data);
//		return textBoxes.back();
//	}
//	logError("UserInterface not initialized!");
//}

void UIImage::inputEvent(Window& window, InputEvent input) {}

void UITextBox::inputEvent(Window& window, InputEvent input) {}

void UICheckBox::inputEvent(Window& window, InputEvent input) {}

void UIImage::create(Window& window) {}

void UITextBox::create(Window& window) {}

void UICheckBox::create(Window& window) {}

void UICheckBox::render(Window& window, TiledRectangle renderArea) {}

void UIImage::renderInteractive(Window& window, TiledRectangle area) {}

void UITextBox::renderInteractive(Window& window, TiledRectangle area) {}

void UICheckBox::renderInteractive(Window& window, TiledRectangle area) {}

void UserInterface::run()
{
	// as long as windows exist
	while(!windows.empty())
	{
		// collect input
		External::glfwPollEvents();

		// iterate over all windows while being able to delete them at the same time
		for(auto window = windows.begin(); window != windows.end();)
		{
			if(window->shouldClose())
			{
				window->destroy();
				window = windows.erase(window);
			}
			else
			{
				++window;
			}
		}
	}

	for(Window& window : windows) window.destroy();
	windows.clear();
	External::glfwPollEvents();
	External::glfwTerminate();
}

void UIImage::render(Window& window, const TiledRectangle renderArea)
{
	Renderer& renderer = window.renderer;

	renderer.setRenderRegion(renderArea);
	renderer.useShader("unlit");
	renderer.useTexture("default");
	renderer.setColor(Color(1.0f));
	renderer.screenSpace();
	const Matrix m = matrixFromPositionAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.x)));
	renderer.uniforms().setMMatrix(m);
	renderer.renderMesh("plane");
}

void UITextBox::render(Window& window, const TiledRectangle renderArea)
{
	Renderer& renderer = window.renderer;

	renderer.screenSpaceFromTopLeft();
	const Matrix m = matrixFromPosition(Vec3(renderArea.position.x, renderArea.position.y, 0));
	renderer.uniforms().setMMatrix(m);
	renderer.textRenderSystem.setSize(16.0f);
	renderer.textRenderSystem.setLetterSpacing(0.6f);
	renderer.textRenderSystem.render(renderer, *data);
}

Window& UserInterface::window(const String& title, const Area size, const Bool decorated, const Bool visible, const WindowState state)
{
	windows.emplace_back();
	windows.back().create(title, size, decorated, visible, state);
	return windows.back();
}

Window& UserInterface::window(const String& title, const Area size, const Bool decorated, const Bool visible, const WindowState state, UIElement& content)
{
	windows.emplace_back();
	windows.back().create(title, size, decorated, visible, state);
	windows.back().add(content);
	return windows.back();
}
