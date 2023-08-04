#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"

struct Renderer;

struct Plot
{
	Vector<Vec2> points;

	void                  add(Vec2 value);
	[[nodiscard]] CPUMesh createMesh() const;
	void                  add(Float a, Float b);
	void                  render(Renderer& renderer) const;
	void                  render(Renderer& renderer, Float lineWidth) const;
};
