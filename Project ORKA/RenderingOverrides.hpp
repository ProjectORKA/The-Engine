#pragma once

#include "Renderer.hpp"

struct Renderer;

struct DepthTestOverride
{
	~DepthTestOverride();
	DepthTestOverride() = delete;
	DepthTestOverride(Renderer& renderer, Bool value);

private:
	Bool      stored;
	Renderer* renderer;
};

struct AlphaBlendOverride
{
	~AlphaBlendOverride();
	AlphaBlendOverride() = delete;
	AlphaBlendOverride(Renderer& renderer, Bool value);

private:
	Bool      stored;
	Renderer* renderer;
};
