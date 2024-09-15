#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"

struct Renderer;

struct Plot
{
	Vec2Vector points;

	void                  add(Vec2 value);
	[[nodiscard]] CpuMesh createMesh() const;
	void                  add(Float a, Float b);
	void                  render(Renderer& renderer) const;
};
