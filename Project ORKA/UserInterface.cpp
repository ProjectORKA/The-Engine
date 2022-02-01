
#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "FileSystem.hpp"
#include "GameSystem.hpp"

UserInterface ui;

//TextBox
TextBox::TextBox(String& data) {
	this->data = &data;
}
void TextBox::render(Renderer& renderer) {
	std::cout << "TextBox(" << *data << ")";
}

//Checkbox
CheckBox::CheckBox(Boolean& data) {
	this->data = &data;
}
void CheckBox::render(Renderer& renderer) {
	std::cout << "CheckBox(" << *data << ")";
}

//Button
void Button::render(Renderer& renderer) {
	std::cout << "Button(" << *data << ",";
	content->render(renderer);
	std::cout << ")";
}
Button& Button::insert(UIElement* element) {
	content = element;
	return *this;
}

//Container
Container& Container::horizontal() {
	vertical = false;
	return *this;
}
void  Container::render(Renderer& renderer) {
	std::cout << "Container(";

	for (auto element : contents) {
		element->render(renderer);
		if (element != contents.back())std::cout << ",";
	}

	std::cout << ")";
}
Container& Container::insert(UIElement* element) {
	contents.push_back(element);
	return *this;
}

//GameView
GameView::GameView(Index gameID) {
	this->gameID = gameID;
}
void GameView::render(Renderer& renderer) {
	if (gameSystem.games[gameID]->selfReplace) {
		Game* tmp = gameSystem.games[gameID];
		gameSystem.games[gameID] = gameSystem.games[gameID]->selfReplace;
		delete tmp;
	}

	gameSystem.games[gameID]->render(renderer);
}
void GameView::mouseIsMoving(Window& window, IVec2 position) {
	gameSystem.games[gameID]->mouseIsMoving(window, position);
}
void GameView::filesDropped(Window& window, Vector<Path> paths) {
	gameSystem.games[gameID]->filesDropped(window, paths);
};
void GameView::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	gameSystem.games[gameID]->buttonIsPressed(window, keyID, action, modifiers);
}
void GameView::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	gameSystem.games[gameID]->mouseIsScrolled(window, xAxis, yAxis);
}
void GameView::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	gameSystem.games[gameID]->mouseIsPressed(window, button, action, modifiers);
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

GameView& gameView() {
	ui.gameViews.emplace_back();
	return ui.gameViews.back();
}
Container& container() {
	ui.containers.emplace_back();
	return ui.containers.back();
}
Button& button(Bool& data) {
	ui.buttons.emplace_back(data);
	return ui.buttons.back();
}
Window& window(String title, UIElement* element, Area size, Bool decorated, Window::WindowState state) {
	ui.windows.emplace_back();
	ui.windows.back().create(title, element, size, decorated, state);
	return ui.windows.back();
};
CheckBox& checkBox(Bool& data) {
	ui.checkBoxes.emplace_back(data);
	return ui.checkBoxes.back();
}
TextBox& textBox(String& data) {
	ui.textBoxes.emplace_back(data);
	return ui.textBoxes.back();
}
GameView& gameView(Index gameID) {
	ui.gameViews.emplace_back();
	ui.gameViews.back().gameID = gameID;
	return ui.gameViews.back();
};

//void exampleCode() {
//
//	String stringVariable = "Hello World";
//	Bool isEnabled = true;
//	Bool pressed = false;
//
//	window("Example", &container()
//		.insert(
//			&button(pressed)
//			.insert(
//				&textBox(stringVariable)
//			)
//		)
//		.insert(
//			&checkBox(isEnabled)
//		)
//		.horizontal()
//	);
//
//	ui.run();
//}