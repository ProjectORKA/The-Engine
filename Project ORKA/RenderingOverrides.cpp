#include "RenderingOverrides.hpp"
#include "Renderer.hpp"

DepthTestOverride::~DepthTestOverride()
{
	this->renderer->setDepthTest(stored);
}

AlphaBlendOverride::~AlphaBlendOverride()
{
	this->renderer->setAlphaBlending(stored);
}

DepthTestOverride::DepthTestOverride(Renderer& renderer, const Bool value)
{
	this->renderer = &renderer;
	stored         = renderer.openGlContext.depthTest;
	this->renderer->setDepthTest(value);
}

AlphaBlendOverride::AlphaBlendOverride(Renderer& renderer, const Bool value)
{
	this->renderer = &renderer;
	stored         = renderer.openGlContext.blending;
	this->renderer->setAlphaBlending(value);
}