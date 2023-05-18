#pragma once

#include "Basics.hpp"
#include "Keymap.hpp"

using Action = Bool&;

struct InputID {
	Int type = InputType::UNKNOWN;
	Int id;

	InputID();
	InputID(Int type, Int id);
	auto operator<=>(const InputID&) const = default;
};

struct InputEvent {
	InputID id;
	Bool state = false;

	InputEvent(Int type, Int id, Bool state);
	auto operator<=>(const InputEvent&) const = default;
};
