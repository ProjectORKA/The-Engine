
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
TextBox::TextBox(String& data) {
	this->data = &data;
}
void TextBox::render(TiledRectangle renderArea, Renderer& renderer){
	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.y)));
	renderer.uniforms().mMatrix(m);
	renderer.renderText(*data, Vec2(0), fonts.paragraph);
}

//Checkbox
CheckBox::CheckBox(Boolean& data) {
	this->data = &data;
}
void CheckBox::render(TiledRectangle renderArea, Renderer& renderer) {}

//Button
Button::Button(Bool& data) {
	this->data = &data;
}
Button& Button::insert(UIElement* element) {
	content = element;
	return *this;
}
void Button::render(TiledRectangle renderArea, Renderer& renderer) {
	
	if(content) content->render(renderArea,renderer);
}

//Test Button
void TestButton::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::L) {
		if (action == ActionState::Press) {
			if (window.renderer.framebufferSystem.idFramebuffer().currentIds.objectID == id) {
				pressed = true;
			}
			else {
				pressed = false;
			}
		}
		else {

			if (window.renderer.framebufferSystem.idFramebuffer().currentIds.objectID == id) {
				if (pressed) {
					pressed = false;
					beep();
				}
			}
			else {
				pressed = false;
			}
		}
	}
	else pressed = false;
}

//Container
Container& Container::horizontal() {
	vertical = false;
	return *this;
}
Container& Container::insert(UIElement& element) {
	contents.push_back(&element);
	return *this;
}
void  Container::render(TiledRectangle renderArea, Renderer& renderer) {
	for (UInt i = 0; i < contents.size();i++) {
		TiledRectangle a = renderArea;
		if (vertical) {
			a.size.y /= contents.size();
			a.position.y = a.size.y * i;
		}
		else {
			a.size.x /= contents.size();
			a.position.x = a.size.x * i;
		}
		contents[i]->render(a,renderer);
	}
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

Container& container() {
	ui.containers.emplace_back();
	return ui.containers.back();
}
UIImage& image(Name name) {
	ui.images.emplace_back(name);
	return ui.images.back();
}
Button& button(Bool& data) {
	ui.buttons.emplace_back(data);
	return ui.buttons.back();
}
CheckBox& checkBox(Bool& data) {
	ui.checkBoxes.emplace_back(data);
	return ui.checkBoxes.back();
}
TextBox& textBox(String& data) {
	ui.textBoxes.emplace_back(data);
	return ui.textBoxes.back();
}
Window& window(String title, Area size, Bool decorated, WindowState state) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, size, decorated, state);
	return ui.windows.back();
};