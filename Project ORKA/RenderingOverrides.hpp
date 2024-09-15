#pragma once

#include "Renderer.hpp"

struct Renderer;

class DepthTestOverride
{
public:
	~DepthTestOverride();
	DepthTestOverride() = delete;
	DepthTestOverride(Renderer& renderer, const Bool value);

private:
	Bool      stored;
	Renderer* renderer;
};

class AlphaBlendOverride
{
public:
	~AlphaBlendOverride();
	AlphaBlendOverride() = delete;
	AlphaBlendOverride(Renderer& renderer, const Bool value);

private:
	Bool      stored;
	Renderer* renderer;
};
