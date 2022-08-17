
#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "FileSystem.hpp"

UserInterface ui;

//UIImage
void UIImage::render(Window& window, TiledRectangle renderArea) {
	Renderer& renderer = window.renderer;

	renderer.renderRegion.set(renderArea);
	renderer.useShader("unlit");
	renderer.useTexture("default");
	renderer.setColor(Color(1.0f));
	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.x)));
	renderer.uniforms().mMatrix(m);
	renderer.renderMesh("plane");
}
UIImage::UIImage(Name name) {
	this->name = name;
}

void UIImage::update(Window& window) {}

//TextBox
UITextBox::UITextBox(String& data) {
	this->data = &data;
}
void UITextBox::update(Window& window) {}
void UITextBox::render(Window& window, TiledRectangle renderArea){
	Renderer& renderer = window.renderer;

	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.y)));
	renderer.uniforms().mMatrix(m);
	renderer.renderText(*data, Vec2(0), fonts.paragraph);
}

//Checkbox
UICheckBox::UICheckBox(Boolean& data) {
	this->data = &data;
}
void UICheckBox::update(Window& window) {}
void UICheckBox::render(Window& window, TiledRectangle renderArea) {}

//Container
UIContainer& UIContainer::horizontal() {
	renderVertical = false;
	return *this;
}
UIContainer& UIContainer::vertical() {
	renderVertical = true;
	return *this;
}
UIContainer& UIContainer::insert(UIElement& element) {
	contents.push_back(&element);
	return *this;
}
void UIContainer::update(Window& window) {}
void UIContainer::render(Window& window, TiledRectangle renderArea) {
	for (UInt i = 0; i < contents.size(); i++) {
		TiledRectangle a = renderArea;
		if (renderVertical) {
			a.size.y /= contents.size();
			a.position.y += a.size.y * i;
		}
		else {
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->render(window, a);
	}
}
void UIContainer::renderInteractive(Window& window, TiledRectangle renderArea)
{
	for (UInt i = 0; i < contents.size(); i++) {
		TiledRectangle a = renderArea;
		if (renderVertical) {
			a.size.y /= contents.size();
			a.position.y += a.size.y * i;
		}
		else {
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->renderInteractive(window, a);
	}
}

//Window
Window& window(String title, Area size, Bool decorated, WindowState state) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, size, decorated, state);
	ui.windows.back().content = nullptr;
	return ui.windows.back();
};
Window& window(String title, Area size, Bool decorated, WindowState state, UIElement & element) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, size, decorated, state);
	ui.windows.back().content = &element;
	return ui.windows.back();
};

//User Interface
void UserInterface::run() {
	if (windows.size() > 0) {
		while (windows.size() > 0) {
			glfwPollEvents();
			for (auto it = windows.begin(); it != windows.end(); it++) {
				if (it->shouldClose()) {
					it->destroy();
					windows.erase(it);
					break;
				}
			}
		}
	}
	else {
		logError("No windows to render!");
	}

	for (Window& window : windows) {
		window.destroy();
	}
	windows.clear();

	glfwTerminate();
};
UserInterface::UserInterface() {
	assert(glfwInit() == GLFW_TRUE);
	glfwSetErrorCallback(whenWindowAPIThrowsError);
}

UIButton& button() {
	ui.buttons.emplace_back();
	return ui.buttons.back();
}
UIContainer& container() {
	ui.containers.emplace_back();
	return ui.containers.back();
}
UIImage& image(Name name) {
	ui.images.emplace_back(name);
	return ui.images.back();
}
UICheckBox& checkBox(Bool& data) {
	ui.checkBoxes.emplace_back(data);
	return ui.checkBoxes.back();
}
UITextBox& textBox(String& data) {
	ui.textBoxes.emplace_back(data);
	return ui.textBoxes.back();
}