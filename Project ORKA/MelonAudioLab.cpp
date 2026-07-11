#include "MelonAudioLab.hpp"

#include "AudioEngine.hpp"
#include "Window.hpp"

namespace
{
	void layoutGraph(WaveformGraph& graph, const Float x, const Float y, const Int width, const Int height)
	{
		graph.positionFromBottomLeft = Vec2(x, y);
		graph.width                  = width;
		graph.height                 = height;
	}

	Vector<Float> boostForDisplay(const Vector<Float>& samples, const Float targetPeak = 0.9f, const Float maxGain = 48.0f)
	{
		Vector<Float> boosted = samples;
		Float peak = 0.0f;
		for(const Float sample : boosted) peak = max(peak, abs(sample));
		if(peak <= 0.000001f) return boosted;
		const Float gain = clamp(targetPeak / peak, 1.0f, maxGain);
		for(Float& sample : boosted) sample *= gain;
		return boosted;
	}
}

void MelonAudioLabRenderer::destroy(Window& window)
{
	audioEngine.destroyProceduralMotion();
	GameRenderer::destroy(window);
}

void MelonAudioLabRenderer::connect(GameSimulation& simulation) {}

void MelonAudioLabRenderer::update(Window& window)
{
	const Float dt = max(window.renderer.deltaTime(), 0.0001f);

	ProceduralMotionTuning tuning;
	tuning.humNoiseMix    = humNoiseMix;
	tuning.humPitchScale  = humPitchScale;
	tuning.windPitchScale = windPitchScale;
	tuning.humGain        = humGain;
	tuning.windGain       = windGain;
	tuning.breathGain     = breathGain;
	tuning.responseCurve  = responseCurve;
	tuning.enableHum      = enableHum;
	tuning.enableWind     = enableWind;
	tuning.enableBreath   = enableBreath;
	audioEngine.setProceduralMotionTuning(tuning);
	audioEngine.updateProceduralMotion(simulatedSpeed01, simulatedDopplerDirection, dt);

	Vector<Float> oscillator;
	Vector<Float> control;
	Vector<Float> combined;
	Vector<Float> hum;
	Vector<Float> wind;
	Vector<Float> breath;
	audioEngine.getProceduralMotionDebugWaveform(oscillator, control, combined, &hum, &wind, &breath);
	combinedGraph.setSamples({}, control, boostForDisplay(combined, 0.95f, 32.0f));
	humGraph.setSamples({}, {}, boostForDisplay(hum, 0.9f, 48.0f));
	windGraph.setSamples({}, {}, boostForDisplay(wind, 0.9f, 48.0f));
	heartbeatGraph.setSamples({}, {}, boostForDisplay(breath, 0.9f, 48.0f));
}

void MelonAudioLabRenderer::create(Window& window)
{
	GameRenderer::create(window);
	audioEngine.createProceduralMotion();
	audioEngine.setProceduralMotionEnabled(true);

	ProceduralMotionTuning tuning = audioEngine.getProceduralMotionTuning();
	tuning.responseCurve  = responseCurve;
	tuning.windPitchScale = windPitchScale;
	tuning.windGain       = windGain;
	audioEngine.setProceduralMotionTuning(tuning);
}

void MelonAudioLabRenderer::inputEvent(Window& window, InputEvent& input) {}

void MelonAudioLabRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.setWireframeMode(false);
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(true);
	r.screenSpaceFromBottomLeft();
	r.useShader("color");

	const IVec2 canvas = getSize();
	r.fill(0.06f, 0.06f, 0.06f, 1.0f);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(canvas.x * 0.5f, canvas.y * 0.5f, static_cast<Float>(canvas.x), static_cast<Float>(canvas.y)));
	r.plane();

	const Float graphX      = 30.0f;
	const Int   graphWidth  = max(canvas.x - 60, 120);
	const Float graphGap    = 10.0f;
	const Float stackBottom = 30.0f;
	const Float usableHeight = max(static_cast<Float>(canvas.y) - 230.0f, 220.0f);
	const Int   combinedHeight = max(static_cast<Int>(usableHeight * 0.38f), 90);
	const Int   componentHeight = max(static_cast<Int>((usableHeight - static_cast<Float>(combinedHeight) - graphGap * 3.0f) / 3.0f), 55);

	layoutGraph(heartbeatGraph, graphX, stackBottom, graphWidth, componentHeight);
	layoutGraph(windGraph, graphX, stackBottom + static_cast<Float>(componentHeight) + graphGap, graphWidth, componentHeight);
	layoutGraph(humGraph, graphX, stackBottom + static_cast<Float>(componentHeight * 2) + graphGap * 2.0f, graphWidth, componentHeight);
	layoutGraph(combinedGraph, graphX, stackBottom + static_cast<Float>(componentHeight * 3) + graphGap * 3.0f, graphWidth, combinedHeight);

	heartbeatGraph.render(window);
	windGraph.render(window);
	humGraph.render(window);
	combinedGraph.render(window);

	auto drawGraphLabel = [&](const Float y, const String& label)
	{
		r.useShader("text");
		r.uniforms().setMMatrix(Matrix(1));
		r.fill(0.85f, 0.85f, 0.85f, 1.0f);
		r.fontSize(14.0f);
		r.letterSpacing(0.45f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		r.text(label, Vec2(graphX + static_cast<Float>(graphWidth) + 12.0f, y + static_cast<Float>(componentHeight) * 0.5f));
	};

	drawGraphLabel(stackBottom, "Heartbeat");
	drawGraphLabel(stackBottom + static_cast<Float>(componentHeight) + graphGap, "Wind");
	drawGraphLabel(stackBottom + static_cast<Float>(componentHeight * 2) + graphGap * 2.0f, "Hum");
	drawGraphLabel(stackBottom + static_cast<Float>(componentHeight * 3) + graphGap * 3.0f, "Combined");

	r.useShader("text");
	r.uniforms().setMMatrix(Matrix(1));
	r.fill(1.0f, 1.0f, 1.0f, 1.0f);
	r.fontSize(22.0f);
	r.letterSpacing(0.55f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("Melon Audio Lab", Vec2(30, static_cast<Float>(canvas.y - 40)));

	r.fontSize(16.0f);
	r.text("Procedural motion audio tuned for Project Melon (max speed ~15000 u/s).", Vec2(30, static_cast<Float>(canvas.y - 70)));
	r.text("At rest: heartbeat only. In motion: hum + wind scale with speed; Doppler affects wind pitch slightly.", Vec2(30, static_cast<Float>(canvas.y - 95)));
	r.text("Wind brightness opens via a smoothed low-pass (~320 Hz at rest to ~14 kHz at max). Toggle layers on the right.", Vec2(30, static_cast<Float>(canvas.y - 120)));
	r.text("Combined graph: mixed output (magenta), speed (orange).", Vec2(30, static_cast<Float>(canvas.y - 145)));
	r.text("Motion speed: " + toString(simulatedSpeed01) + " | Doppler: " + toString(simulatedDopplerDirection), Vec2(30, static_cast<Float>(canvas.y - 170)));
}

void MelonAudioLabRenderer::renderInteractive(Window& window) {}

MelonAudioLabToggle::MelonAudioLabToggle(Bool& boundValue, String componentLabel)
	: value(&boundValue)
	, label(std::move(componentLabel))
{
	row();
	growX(false);
	setWidth(336);
	setHeight(28);
	padding(6);
	border(1, Color(1, 1, 1, 0.12f));
	background(Color(0.08f, 0.08f, 0.1f, 0.95f));
	hoveredBackground(Color(0.12f, 0.12f, 0.14f, 0.95f));
	activeBackground(Color(0.16f, 0.16f, 0.18f, 0.95f));
	setText(label + ": " + (boundValue ? "On" : "Off"));
}

void MelonAudioLabToggle::doThis()
{
	if(value) *value = !*value;
}

void MelonAudioLabToggle::update(Window& window)
{
	UIElement::update(window);
	if(!value) return;
	setText(label + ": " + (*value ? "On" : "Off"));
}

MelonAudioLabControls::MelonAudioLabControls(MelonAudioLabRenderer& renderer)
	: humToggle(renderer.enableHum, "Hum")
	, windToggle(renderer.enableWind, "Wind")
	, breathToggle(renderer.enableBreath, "Heartbeat")
{
	setName("MelonAudioLabControls");
	column();
	growX(false);
	growY(true);
	setWidth(360);
	gap(10);
	padding(12);
	background(Color(0.02f, 0.02f, 0.025f, 0.95f));
	hoveredBackground(Color(0.02f, 0.02f, 0.025f, 0.95f));
	activeBackground(Color(0.02f, 0.02f, 0.025f, 0.95f));
	border(1, Color(1, 1, 1, 0.12f));

	addElement(humToggle);
	addElement(windToggle);
	addElement(breathToggle);

	humNoiseMixSlider
		.setLabel("Hum/Noise Mix")
		.setRange(0.0f, 1.0f)
		.bind(renderer.humNoiseMix);
	humNoiseMixSlider.growX(false);
	humNoiseMixSlider.setWidth(336);

	humPitchSlider
		.setLabel("Hum Pitch Scale")
		.setRange(0.1f, 0.5f)
		.bind(renderer.humPitchScale);
	humPitchSlider.growX(false);
	humPitchSlider.setWidth(336);

	windPitchSlider
		.setLabel("Wind Pitch Scale")
		.setRange(0.05f, 3.0f)
		.bind(renderer.windPitchScale);
	windPitchSlider.growX(false);
	windPitchSlider.setWidth(336);

	humGainSlider
		.setLabel("Hum Gain")
		.setRange(0.0f, 2.5f)
		.bind(renderer.humGain);
	humGainSlider.growX(false);
	humGainSlider.setWidth(336);

	windGainSlider
		.setLabel("Wind Gain")
		.setRange(0.0f, 2.5f)
		.bind(renderer.windGain);
	windGainSlider.growX(false);
	windGainSlider.setWidth(336);

	heartbeatGainSlider
		.setLabel("Heartbeat Gain")
		.setRange(0.0f, 2.5f)
		.bind(renderer.breathGain);
	heartbeatGainSlider.growX(false);
	heartbeatGainSlider.setWidth(336);

	responseCurveSlider
		.setLabel("Speed Response Curve")
		.setRange(0.0f, 0.5f)
		.bind(renderer.responseCurve);
	responseCurveSlider.growX(false);
	responseCurveSlider.setWidth(336);

	speedSlider
		.setLabel("Motion Speed")
		.setRange(0.0f, 1.0f)
		.bind(renderer.simulatedSpeed01);
	speedSlider.growX(false);
	speedSlider.setWidth(336);

	dopplerDirectionSlider
		.setLabel("Doppler Direction")
		.setRange(-1.0f, 1.0f)
		.bind(renderer.simulatedDopplerDirection);
	dopplerDirectionSlider.growX(false);
	dopplerDirectionSlider.setWidth(336);

	addElement(humNoiseMixSlider);
	addElement(humPitchSlider);
	addElement(windPitchSlider);
	addElement(humGainSlider);
	addElement(windGainSlider);
	addElement(heartbeatGainSlider);
	addElement(responseCurveSlider);
	addElement(speedSlider);
	addElement(dopplerDirectionSlider);
}

void MelonAudioLab::run() const
{
	MelonAudioLabRenderer renderer;
	MelonAudioLabControls controls(renderer);

	Window& window = ui.window("Melon Audio Lab", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.row();
	window.gap(10);
	window.padding(10);

	renderer.growX(true);
	renderer.growY(true);
	controls.growY(true);

	window.addElement(renderer);
	window.addElement(controls);
	ui.run();
}
