#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct WaveformGraph final : UIElement
{
	Int  width  = 560;
	Int  height = 180;
	Vec2 positionFromBottomLeft = Vec2(30, 240);

	Vector<Float> waveform;
	Vector<Float> controlSignal;
	Vector<Float> audioOutput;

	void setSamples(const Vector<Float>& waveformSamples, const Vector<Float>& controlSamples, const Vector<Float>& audioSamples = {});
	void render(Window& window) override;
	void update(Window& window) override {}
	void create(Window& window) override {}
	void destroy(Window& window) override {}
	void renderInteractive(Window& window) override {}
	void inputEvent(Window& window, InputEvent& input) override {}
};
