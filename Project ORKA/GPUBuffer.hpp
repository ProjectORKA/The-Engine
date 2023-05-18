#pragma once

#include "Basics.hpp"

struct GPUBuffer {
	Index bufferId{};
	Bool loaded = false;
	void use(Int location) const;
	void create(const Vector<Vec4>& data, Int location);
};
