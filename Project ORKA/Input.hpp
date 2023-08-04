#pragma once

#include "Basics.hpp"
#include "Keymap.hpp"

using Action = Bool&;

struct InputId
{
	Int       id;
	InputType type = InputType::Unknown;

	InputId();
	InputId(InputType type, Int id);
	auto operator<=>(const InputId&) const = default;
};

struct InputEvent
{
	InputId id;
	Bool    state = false;

	InputEvent(InputType type, Int id, Bool state);
	auto operator<=>(const InputEvent&) const = default;
};
