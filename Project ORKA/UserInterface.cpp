#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

void UserInterface::run()
{
	if(initialized)
	{
		if(!windows.empty())
			while(!windows.empty())
			{
				glfwPollEvents();
				for(auto it = windows.begin(); it != windows.end(); ++it)
				{
					if(it->shouldClose())
					{
						it->destroy();
						windows.erase(it);
						break;
					}
				}
			}
		else logError("No windows to render!");

		for(Window& window : windows) window.destroy();
		windows.clear();
		glfwPollEvents();
		glfwTerminate();
	}
	else
	{
		logError("UserInterface not initialized!");
	}
}

void UserInterface::create()
{
	if(glfwInit() != GLFW_TRUE)
	{
		logError("GLFW could not be initialized");
		return;
	}
	glfwSetErrorCallback(whenWindowAPIThrowsError);

	initialized = true;
}

void UIImage::update(Window& window) {}

void UIImage::destroy(Window& window) {}

void UITextBox::update(Window& window) {}

void UICheckBox::update(Window& window) {}

void UITextBox::destroy(Window& window) {}

UIButton& UserInterface::button()
{
	if(initialized)
	{
		buttons.emplace_back();
		return buttons.back();
	}
	logError("UserInterface not initialized!");
}

void UICheckBox::destroy(Window& window) {}

void UIContainer::update(Window& window) {}

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

void UIContainer::destroy(Window& window) {}

UIContainer& UIContainer::vertical()
{
	renderVertical = true;
	return *this;
}

UIContainer& UIContainer::horizontal()
{
	renderVertical = false;
	return *this;
}

UIContainer& UserInterface::container()
{
	if(initialized)
	{
		containers.emplace_back();
		return containers.back();
	}
	logError("UserInterface not initialized!");
}

UIImage& UserInterface::image(Name name)
{
	if(initialized)
	{
		images.emplace_back(name);
		return images.back();
	}
	logError("UserInterface not initialized!");
}

UICheckBox& UserInterface::checkBox(Bool& data)
{
	if(initialized)
	{
		checkBoxes.emplace_back(data);
		return checkBoxes.back();
	}
	logError("UserInterface not initialized!");
}

UITextBox& UserInterface::textBox(String& data)
{
	if(initialized)
	{
		textBoxes.emplace_back(data);
		return textBoxes.back();
	}
	logError("UserInterface not initialized!");
}

void UIImage::inputEvent(Window& window, InputEvent input) {}

UIContainer& UIContainer::insert(UIElement& element)
{
	contents.push_back(&element);
	return *this;
}

void UITextBox::inputEvent(Window& window, InputEvent input) {}

void UICheckBox::inputEvent(Window& window, InputEvent input) {}

void UIContainer::inputEvent(Window& window, InputEvent input) {}

void UIImage::create(ResourceManager& resourceManager, Window& window) {}

void UITextBox::create(ResourceManager& resourceManager, Window& window) {}

void UICheckBox::create(ResourceManager& resourceManager, Window& window) {}

void UIContainer::create(ResourceManager& resourceManager, Window& window) {}

void UICheckBox::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) {}

void UIImage::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void UITextBox::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void UICheckBox::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void UIImage::render(ResourceManager& resourceManager, Window& window, const TiledRectangle renderArea)
{
	Renderer& renderer = window.renderer;

	renderer.setRenderRegion(renderArea);
	renderer.useShader(resourceManager, "unlit");
	renderer.useTexture(resourceManager, "default");
	renderer.setColor(Color(1.0f));
	renderer.screenSpace();
	const Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.x)));
	renderer.uniforms().setMMatrix(m);
	renderer.renderMesh(resourceManager, "plane");
}

void UITextBox::render(ResourceManager& resourceManager, Window& window, const TiledRectangle renderArea)
{
	Renderer& renderer = window.renderer;

	renderer.screenSpaceFromTopLeft();
	const Matrix m = matrixFromLocation(Vec3(renderArea.position.x, renderArea.position.y, 0));
	renderer.uniforms().setMMatrix(m);
	renderer.textRenderSystem.setStyle(fonts.paragraph);
	renderer.textRenderSystem.render(resourceManager, renderer, *data);
}

void UIContainer::render(ResourceManager& resourceManager, Window& window, const TiledRectangle renderArea)
{
	for(UInt i = 0; i < contents.size(); i++)
	{
		TiledRectangle a = renderArea;
		if(renderVertical)
		{
			a.size.y /= contents.size();
			a.position.y -= a.size.y * i;
		}
		else
		{
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->render(resourceManager, window, a);
	}
}

void UIContainer::renderInteractive(ResourceManager& resourceManager, Window& window, const TiledRectangle renderArea)
{
	for(UInt i = 0; i < contents.size(); i++)
	{
		TiledRectangle a = renderArea;
		if(renderVertical)
		{
			a.size.y /= contents.size();
			a.position.y += a.size.y * i;
		}
		else
		{
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->renderInteractive(resourceManager, window, a);
	}
}

Window& UserInterface::window(const String& title, const Area size, const Bool decorated, const Bool visible, const WindowState state, ResourceManager& resourceManager)
{
	if(initialized)
	{
		windows.emplace_back();
		windows.back().create(title, size, decorated, visible, state, resourceManager);
		windows.back().content = nullptr;
		return windows.back();
	}
	logError("UserInterface not initialized!");
}

Window& UserInterface::window(const String& title, const Area size, const Bool decorated, const Bool visible, const WindowState state, UIElement& element, ResourceManager& resourceManager)
{
	if(initialized)
	{
		windows.emplace_back();
		windows.back().create(title, size, decorated, visible, state, resourceManager);
		windows.back().content = &element;
		return windows.back();
	}
	logError("UserInterface not initialized!");
}
