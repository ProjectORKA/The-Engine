#pragma once
#include "Basics.hpp"

struct Renderer;
struct SimpleRtsSimulation;

struct SimpleRtsSystem
{
	String name = "Unknown System";

	virtual void render(Renderer& renderer) = 0;
	virtual void update(SimpleRtsSimulation& sim) = 0;
	virtual void create(const SimpleRtsSimulation& sim) = 0;
	virtual void destroy(const SimpleRtsSimulation& sim) = 0;
};
