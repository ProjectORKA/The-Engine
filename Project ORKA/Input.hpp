#pragma once

#include "Basics.hpp"
#include "Keymap.hpp"

using Action = Bool&;

struct InputId
{
	InputType type = InputType::Unknown;
	Int id;

	InputId& operator=(const InputId& other) {
		this->type = other.type;
		this->id = other.id;
		return *this;
	};
	InputId();
	InputId(InputType type, Int id);
	auto operator<=>(const InputId&) const = default;
};

struct InputEvent
{
	InputId id;
	Bool state = false;

	InputEvent() = default;
	InputEvent& operator=(const InputEvent& other) {
		this->id = other.id;
		this->state = other.state;
		return *this;
	}

	InputEvent(InputType type, Int id, Bool state);
	auto operator<=>(const InputEvent&) const = default;
};