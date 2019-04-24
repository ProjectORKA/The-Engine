#pragma once
#ifndef INPUT_HPP
#define INPUT_HPP

class Key {
public:
	bool holding = false; //active every frame while button is pressed
	bool activated = false; //activated once every press
	bool toggled = false; //active every toggle
	bool toggleStatus = false; //do not confuse with holding! toggles with every press down

	void set(bool pressed);
	void reset();
};

class Axis {
public:
	double current = 0.0f;
	double previous = 0.0f;
	double delta = 0.0f;

	void set(double value);
};

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
	Axis yaw;
	Axis pitch;
};
#endif // !INPUT_HPP