#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Math.hpp"

struct Tree {
	Vec3 position = Vec3(randomFloat(), randomFloat(), randomFloat());
};

struct TreeData {
	Vector<Tree> trees;
};