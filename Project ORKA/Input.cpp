
#include "Input.hpp"

void Key::set(bool pressed) {
	if (holding != pressed) {
		toggled = true;
	}

	if (toggled && pressed) {
		activated = true;
	}
	else {
		activated = false;
	}

	if (activated) {
		toggleStatus = !toggleStatus;
	}

	holding = pressed;
}
void Key::reset() {
	holding = false;
	activated = false;
	toggled = false;
	toggleStatus = false;
}

void Axis::set(double value) {
	previous = current;
	current = value;
	delta = current - previous;
}