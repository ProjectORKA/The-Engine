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

WireframeOverride::~WireframeOverride()
{
	this->renderer->setWireframeMode(stored);
}

DepthTestOverride::DepthTestOverride(Renderer& renderer, const Bool value)
{
	this->renderer = &renderer;
	stored         = renderer.openGlContext.getDepthTest();
	this->renderer->setDepthTest(value);
}

AlphaBlendOverride::AlphaBlendOverride(Renderer& renderer, const Bool value)
{
	this->renderer = &renderer;
	stored         = renderer.openGlContext.getBlending();
	this->renderer->setAlphaBlending(value);
}

WireframeOverride::WireframeOverride(Renderer& renderer, const Bool value)
{
	this->renderer = &renderer;
	stored         = renderer.openGlContext.getWireframeMode();
	this->renderer->setWireframeMode(value);
}
