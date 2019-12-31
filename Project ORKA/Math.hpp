#pragma once

#define PI 3.14159265359f

float randomFloat(float low, float high);

template<typename T>
T max(T a, T b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}