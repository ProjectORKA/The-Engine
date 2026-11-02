#include "MelonAudioLab.hpp"

#include "AudioEngine.hpp"
#include "Debug.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Authoring/UiMarkupSurfaceRegistry.hpp"
#include "UI/Surfaces/UiInteractiveSurface.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/Theme/UiLegacyTheme.hpp"
#include "UI/UiHost.hpp"
#include "UserInterface.hpp"
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
	const Color canvasFill = Ui::UiLegacyTheme::canvasBackground(Ui::UiThemeMode::Dark);
	r.fill(canvasFill.r, canvasFill.g, canvasFill.b, canvasFill.a);
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

namespace
{
	using namespace Ui;

	void publishMelonAudioState(UiPropertyRegistry& properties, const MelonAudioLabRenderer& renderer)
	{
		(void)properties.set("state.enableHum", UiValue::fromBool(renderer.enableHum));
		(void)properties.set("state.enableWind", UiValue::fromBool(renderer.enableWind));
		(void)properties.set("state.enableBreath", UiValue::fromBool(renderer.enableBreath));
		(void)properties.set("state.humNoiseMix", UiValue::fromNumber(renderer.humNoiseMix));
		(void)properties.set("state.humPitchScale", UiValue::fromNumber(renderer.humPitchScale));
		(void)properties.set("state.windPitchScale", UiValue::fromNumber(renderer.windPitchScale));
		(void)properties.set("state.humGain", UiValue::fromNumber(renderer.humGain));
		(void)properties.set("state.windGain", UiValue::fromNumber(renderer.windGain));
		(void)properties.set("state.breathGain", UiValue::fromNumber(renderer.breathGain));
		(void)properties.set("state.responseCurve", UiValue::fromNumber(renderer.responseCurve));
		(void)properties.set("state.simulatedSpeed", UiValue::fromNumber(renderer.simulatedSpeed01));
		(void)properties.set("state.dopplerDirection", UiValue::fromNumber(renderer.simulatedDopplerDirection));
	}

	struct MelonAudioLabShell final : UIElement
	{
		UiAppTheme theme = UiAppTheme::dark();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		MelonAudioLabRenderer renderer;
		Window* owningWindow = nullptr;
		Bool ready = false;

		MelonAudioLabShell()
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
		}

		void refresh()
		{
			if (!authored) return;
			publishMelonAudioState(authored->properties(), renderer);
			authored->syncFromProperties();
		}

		void create(Window& window) override
		{
			owningWindow = &window;
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);
			host.useAppTheme(theme);
			UIElement::create(window);

			renderer.renderToFramebuffer = false;
			renderer.create(window);
			UiMarkupSurfaceRegistry::global().registerInteractiveSurfaceTag("melon-audio-view");
			authored = std::make_unique<UiAuthoredHost>(host);
			authored->widgetBridge().setPropertyChangeHandler(
				[this](const String& path, const UiValue& value)
				{
					if (value.type == UiValueType::Bool)
					{
						if (path == "state.enableHum") renderer.enableHum = value.boolean;
						else if (path == "state.enableWind") renderer.enableWind = value.boolean;
						else if (path == "state.enableBreath") renderer.enableBreath = value.boolean;
					}
					else if (value.type == UiValueType::Number)
					{
						const Float number = static_cast<Float>(value.number);
						if (path == "state.humNoiseMix") renderer.humNoiseMix = number;
						else if (path == "state.humPitchScale") renderer.humPitchScale = number;
						else if (path == "state.windPitchScale") renderer.windPitchScale = number;
						else if (path == "state.humGain") renderer.humGain = number;
						else if (path == "state.windGain") renderer.windGain = number;
						else if (path == "state.breathGain") renderer.breathGain = number;
						else if (path == "state.responseCurve") renderer.responseCurve = number;
						else if (path == "state.simulatedSpeed") renderer.simulatedSpeed01 = number;
						else if (path == "state.dopplerDirection") renderer.simulatedDopplerDirection = number;
					}
					refresh();
				});

			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(
				uiDirectory / "melon-audio-lab.ui.html",
				uiDirectory / "melon-audio-lab.ui.scss") || !authored->mount())
			{
				logError("Melon Audio Lab: failed to mount authored UI.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			const UiWidgetHandle surface = authored->widgetBridge().findById("melon-audio-view");
			authored->widgets().setInteractiveSurfaceRender(surface,
				[this](const UiInteractiveSurfaceContext& context)
				{
					if (!owningWindow) return;
					renderer.setPositionX(static_cast<Int>(context.position.x));
					renderer.setPositionY(static_cast<Int>(context.position.y));
					renderer.setWidth(static_cast<Int>(context.size.x));
					renderer.setHeight(static_cast<Int>(context.size.y));
					renderer.renderInteractive(*owningWindow);
					renderer.render(*owningWindow);
				});
			ready = true;
			refresh();
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);
			renderer.update(window);
			if (ready)
			{
				refresh();
				authored->syncControlLayouts();
			}
			host.update(window);
		}

		void inputEvent(Window& window, InputEvent& input) override
		{
			renderer.inputEvent(window, input);
		}

		void destroy(Window& window) override
		{
			renderer.destroy(window);
			authored.reset();
			ready = false;
			owningWindow = nullptr;
			host.destroy(window);
		}
	};
}

void MelonAudioLab::run() const
{
	MelonAudioLabShell shell;
	Window& window = ui.window("Melon Audio Lab", settings.defaultWindowWidth,
		settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(shell);
	ui.run();
}
