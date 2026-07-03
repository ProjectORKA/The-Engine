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
	Bool    consumed = false;

	InputEvent(InputType type, Int id, Bool state);
	void consume() { consumed = true; }
	[[nodiscard]] Bool isConsumed() const { return consumed; }
	[[nodiscard]] Bool matches(const InputEvent& other) const { return id == other.id && state == other.state; }

	auto operator<=>(const InputEvent& other) const
	{
		if (id == other.id && state == other.state) return std::strong_ordering::equal;
		if (id != other.id) return id <=> other.id;
		return state <=> other.state;
	}
	[[nodiscard]] Bool operator==(const InputEvent& other) const { return id == other.id && state == other.state; }
};