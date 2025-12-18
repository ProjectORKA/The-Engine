#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

UserInterface ui;

void UITextBox::create(Window& window) {}
void UITextBox::update(Window& window) {}
void UITextBox::destroy(Window& window) {}
void UITextBox::renderInteractive(Window& window) {}
void UITextBox::inputEvent(Window& window, InputEvent input) {}

void UserInterface::run()
{
	// as long as windows exist
	while (!windows.empty())
	{
		// collect input
		External::glfwPollEvents();

		// iterate over all windows while being able to delete them at the same time
		for (auto window = windows.begin(); window != windows.end();)
		{
			if (window->shouldClose())
			{
				window->destroy();
				window = windows.erase(window);
			}
			else ++window;
		}
	}

	for (Window& window : windows) window.destroy();
	windows.clear();
	External::glfwPollEvents();
	External::glfwTerminate();
}

UserInterface::UserInterface()
{
	if (External::glfwInit() != GLFW_TRUE)
	{
		logError("GLFW could not be initialized");
		return;
	}
	logDebug("GLFW initialized!");
	External::glfwSetErrorCallback(whenWindowApiThrowsError);
}

UITextBox::UITextBox(String& data)
{
	this->data = &data;
}

void UITextBox::render(Window& window)
{
	Renderer& renderer = window.renderer;

	renderer.uiSpace(window);
	const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());
	const Matrix m = matrixFromPosition(Vec3(renderPos, 0));
	renderer.uniforms().setMMatrix(m);
	renderer.fontSize(16.0f);
	renderer.textRenderSystem.setLetterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	renderer.textRenderSystem.render(renderer, *data);
}

Window& UserInterface::window(const String& title, const Int width, const Int height, const Bool decorated, const Bool visible, const WindowState state)
{
	windows.emplace_back();
	windows.back().create(title, IVec2(width, height), decorated, visible, state);
	return windows.back();
}

Window& UserInterface::window(const String& title, const Int width, const Int height, const Bool decorated, const Bool visible, const WindowState state, UIElement& content)
{
	windows.emplace_back();
	windows.back().create(title, IVec2(width, height), decorated, visible, state);
	windows.back().addElement(content);
	return windows.back();
}
