#include "Input.hpp"

InputId::InputId()
{
	type = InputType::KeyBoard;
	id   = UNKNOWN;
}

InputId::InputId(const InputType type, const Int id)
{
	this->type = type;
	this->id   = id;
}

InputEvent::InputEvent(const InputType type, const Int id, const Bool state)
{
	this->id    = InputId(type, id);
	this->state = state;
}
