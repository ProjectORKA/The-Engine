
#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "FileSystem.hpp"

//UIImage
UIImage::UIImage(Name name) {
	this->name = name;
}
void UIImage::update(Window& window) {}
void UIImage::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) {
	Renderer& renderer = window.renderer;

	renderer.renderRegion.set(renderArea);
	renderer.useShader(resourceManager, "unlit");
	renderer.useTexture(resourceManager, "default");
	renderer.setColor(Color(1.0f));
	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize(Vec4(renderArea.position.x, renderArea.position.y, 0, min(renderArea.size.x, renderArea.size.x)));
	renderer.uniforms().mMatrix(m);
	renderer.renderMesh(resourceManager, "plane");
}

//TextBox
UITextBox::UITextBox(String& data) {
	this->data = &data;
}
void UITextBox::update(Window& window) {}
void UITextBox::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) {
	Renderer& renderer = window.renderer;

	renderer.screenSpaceFromTopLeft();
	Matrix m = matrixFromLocation(Vec3(renderArea.position.x, renderArea.position.y, 0));
	renderer.uniforms().mMatrix(m);
	renderer.textRenderSystem.setStyle(fonts.paragraph);
	renderer.textRenderSystem.render(resourceManager,renderer, *data);
}

//Checkbox
UICheckBox::UICheckBox(Boolean& data) {
	this->data = &data;
}
void UICheckBox::update(Window& window) {}
void UICheckBox::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) {}

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
	contents.pushBack(&element);
	return *this;
}
void UIContainer::update(Window& window) {}
void UIContainer::render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) {
	for (UInt i = 0; i < contents.size(); i++) {
		TiledRectangle a = renderArea;
		if (renderVertical) {
			a.size.y /= contents.size();
			a.position.y -= a.size.y * i;
		}
		else {
			a.size.x /= contents.size();
			a.position.x += a.size.x * i;
		}
		contents[i]->render(resourceManager, window, a);
	}
}
void UIContainer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea)
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
		contents[i]->renderInteractive(resourceManager, window, a);
	}
}

//User Interface Elements
UIButton& UserInterface::button() {
	buttons.emplace_back();
	return buttons.back();
}
UIContainer& UserInterface::container() {
	containers.emplace_back();
	return containers.back();
}
UIImage& UserInterface::image(Name name) {
	images.emplace_back(name);
	return images.back();
}
UICheckBox& UserInterface::checkBox(Bool& data) {
	checkBoxes.emplace_back(data);
	return checkBoxes.back();
}
UITextBox& UserInterface::textBox(String& data) {
	textBoxes.emplace_back(data);
	return textBoxes.back();
}
Window& UserInterface::window(String title, Area size, Bool decorated, WindowState state, ResourceManager& resourceManager) {
	windows.emplace_back();
	windows.back().create(title, size, decorated, state, resourceManager);
	windows.back().content = nullptr;
	return windows.back();
};
Window& UserInterface::window(String title, Area size, Bool decorated, WindowState state, UIElement& element, ResourceManager& resourceManager) {
	windows.emplace_back();
	windows.back().create(title, size, decorated, state, resourceManager);
	windows.back().content = &element;
	return windows.back();
};

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

	glfwPollEvents();

	glfwTerminate();
}