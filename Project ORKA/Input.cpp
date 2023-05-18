#include "Input.hpp"

InputID::InputID() {
	type = InputType::KeyBoard;
	id = UNKNOWN;
}

InputID::InputID(const Int type, const Int id) {
	this->type = type;
	this->id = id;
}

InputEvent::InputEvent(const Int type, const Int id, const Bool state) {
	this->id = InputID(type, id);
	this->state = state;
}
