#pragma once

#include "Basics.hpp"
#include "PrimitiveModes.hpp"

struct MeshFileInfo {
	String name = "empty";
	PrimitiveMode primitiveMode = Triangles;
	String path = "";
};