#include "Input.hpp"

InputID::InputID() {
	type = InputType::KeyBoard;
	id = UNKNOWN;
}
InputID::InputID(Int type, Int id) {
	this->type = type;
	this->id = id;
}

InputEvent::InputEvent(Int type, Int id, Bool state) {
	this->id = InputID(type,id);
	this->state = state;
}