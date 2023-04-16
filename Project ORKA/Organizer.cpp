#include "Organizer.hpp"

void Organizer::addUser(User user) {
	users.pushBack(user);
}

void Organizer::addTask(Task task) {
	tasks.pushBack(task);
}

void Organizer::update(Window& window) {

}

void Organizer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
}
