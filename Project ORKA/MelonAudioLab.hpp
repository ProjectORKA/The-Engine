#pragma once

#include "AudioEngine.hpp"
#include "Game.hpp"
#include "UIButton.hpp"
#include "UISlider.hpp"
#include "WaveformGraph.hpp"

struct Window;

struct MelonAudioLabRenderer final : GameRenderer
{
	WaveformGraph combinedGraph;
	WaveformGraph humGraph;
	WaveformGraph windGraph;
	WaveformGraph heartbeatGraph;

	Float humNoiseMix           = 0.45f;
	Float humPitchScale         = 0.3f;
	Float windPitchScale        = 1.0f;
	Float humGain               = 1.0f;
	Float windGain              = 0.6f;
	Float breathGain            = 1.0f;
	Float responseCurve         = 0.25f;
	Float simulatedSpeed01      = 0.0f;
	Float simulatedDopplerDirection = 0.0f;
	Bool  enableHum               = true;
	Bool  enableWind              = true;
	Bool  enableBreath            = true;

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct MelonAudioLabToggle final : UIButton
{
	Bool*  value  = nullptr;
	String label;

	MelonAudioLabToggle(Bool& boundValue, String componentLabel);
	void doThis() override;
	void update(Window& window) override;
};

struct MelonAudioLabControls final : UIElement
{
	MelonAudioLabToggle humToggle;
	MelonAudioLabToggle windToggle;
	MelonAudioLabToggle breathToggle;
	UISlider humNoiseMixSlider;
	UISlider humPitchSlider;
	UISlider windPitchSlider;
	UISlider humGainSlider;
	UISlider windGainSlider;
	UISlider heartbeatGainSlider;
	UISlider responseCurveSlider;
	UISlider speedSlider;
	UISlider dopplerDirectionSlider;

	explicit MelonAudioLabControls(MelonAudioLabRenderer& renderer);
};

struct MelonAudioLab
{
	void run() const;
};
