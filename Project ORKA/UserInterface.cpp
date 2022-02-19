
#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "FileSystem.hpp"
#include "GameSystem.hpp"

UserInterface ui;

//UIImage
void UIImage::render(TiledRectangle renderArea, Renderer& renderer) {
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

//TextBox
UITextBox::UITextBox(String& data) {
	this->data = &data;
}
void UITextBox::render(TiledRectangle renderArea, Renderer& renderer){
	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.y)));
	renderer.uniforms().mMatrix(m);
	renderer.renderText(*data, Vec2(0), fonts.paragraph);
}

//Checkbox
UICheckBox::UICheckBox(Boolean& data) {
	this->data = &data;
}
void UICheckBox::render(TiledRectangle renderArea, Renderer& renderer) {}

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
void UIContainer::render(TiledRectangle renderArea, Renderer& renderer) {
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
		contents[i]->render(a, renderer);
	}
}
void UIContainer::renderInteractive(TiledRectangle renderArea, Renderer& renderer)
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
		contents[i]->renderInteractive(a, renderer);
	}
}
void UIContainer::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers)
{
	for (auto c : contents)c->mouseIsPressed(window, button, action, modifiers);
}

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
	glfwSetMonitorCallback(whenMonitorChanged);
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
Window& window(String title, Area size, Bool decorated, WindowState state) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, size, decorated, state);
	return ui.windows.back();
};

Window& window(String title, Area size, Bool decorated, WindowState state, UIElement & element) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, size, decorated, state);
	ui.windows.back().content = &element;
	return ui.windows.back();
};