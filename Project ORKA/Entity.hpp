#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Math.hpp"

struct Entity {
	Matrix transform = Matrix(1);
	Index modelID = 0;
};