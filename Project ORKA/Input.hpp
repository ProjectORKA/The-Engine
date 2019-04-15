#pragma once

class Key {
public:
	bool holding = false; //active every frame while button is pressed
	bool activated = false; //activated once every press
	bool toggled = false; //active every toggle
	bool toggleStatus = false; //do not confuse with holding! toggles with every press down

	void set(bool pressed) {
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
	void reset() {
		holding = false;
		activated = false;
		toggled = false;
		toggleStatus = false;
	}
};

//class Axis {
//public:
//	float current = 0.0f;
//	float previous = 0.0f;
//	float delta = 0.0f;
//
//	void set(float value) {
//		previous = current;
//		current = value;
//		delta = current - previous;
//	}
//};

class InputHandler {
public:
	Key action;
	Key interact;
	Key forward;
	Key backward;
	Key right;
	Key left;
	Key up;
	Key down;
	Key escape;
	Key wireframe;
	//Axis yaw;
	//Axis pitch;
};