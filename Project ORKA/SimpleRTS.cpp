#include "SimpleRTS.hpp"

#include "Crosshair.hpp"
#include "Debug.hpp"
#include "Random.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "SimpleRtsWorldPresets.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Authoring/UiMarkupSurfaceRegistry.hpp"
#include "UI/Surfaces/UiInteractiveSurface.hpp"
#include "UI/Theme/UiAppHelpers.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/UiHost.hpp"
#include "UI/Widgets/UiWidgets.hpp"
#include "UIElement.hpp"
#include "UserInterface.hpp"

Bool debugSimpleRts = false;

namespace
{
	using namespace Ui;

	UiValue presetOptions(const Vector<SimpleRtsWorldPresetEntry>& presets)
	{
		Vector<UiValue> options;
		options.reserve(presets.size());
		for (const SimpleRtsWorldPresetEntry& preset : presets)
		{
			options.push_back(UiValue::fromObject({
				{"value", UiValue::fromString(preset.id)},
				{"label", UiValue::fromString(preset.label)}
				}));
		}
		return UiValue::fromArray(std::move(options));
	}

	void publishDraft(
		UiPropertyRegistry& properties,
		const SimpleRtsWorldSettings& draft,
		const String& selectedPreset,
		const String& presetName,
		const String& statusLine,
		const Bool worldRunning,
		const Bool setupVisible)
	{
		(void)properties.set("state.setupVisible", UiValue::fromBool(setupVisible));
		(void)properties.set("state.subtitle",
			UiValue::fromString("Configure map and spawn counts, save named presets, then start or rebuild the simulation."));
		(void)properties.set("state.selectedPreset", UiValue::fromString(selectedPreset));
		(void)properties.set("state.presetName", UiValue::fromString(presetName));
		(void)properties.set("state.statusLine", UiValue::fromString(statusLine));
		(void)properties.set("state.worldRunning", UiValue::fromBool(worldRunning));

		(void)properties.set("state.mapSize", UiValue::fromNumber(draft.mapSize));
		(void)properties.set("state.terrainPointCount", UiValue::fromNumber(static_cast<Double>(draft.terrainPointCount)));
		(void)properties.set("state.enableWater", UiValue::fromBool(draft.enableWater));
		(void)properties.set("state.enableElevation", UiValue::fromBool(draft.enableElevation));

		(void)properties.set("state.humans", UiValue::fromNumber(static_cast<Double>(draft.humans)));
		(void)properties.set("state.rabbits", UiValue::fromNumber(static_cast<Double>(draft.rabbits)));
		(void)properties.set("state.wolves", UiValue::fromNumber(static_cast<Double>(draft.wolves)));
		(void)properties.set("state.trees", UiValue::fromNumber(static_cast<Double>(draft.trees)));
		(void)properties.set("state.bushes", UiValue::fromNumber(static_cast<Double>(draft.bushes)));
		(void)properties.set("state.berryBushes", UiValue::fromNumber(static_cast<Double>(draft.berryBushes)));
		(void)properties.set("state.bodies", UiValue::fromNumber(static_cast<Double>(draft.bodies)));
	}

	struct SimpleRtsAppShell final : UIElement
	{
		explicit SimpleRtsAppShell(SimpleRtsSimulation& simulation)
			: sim(&simulation)
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
			(void)SimpleRtsWorldPresets::loadLastSettings(draftSettings);
			selectedPreset = SimpleRtsWorldPresets::loadLastPresetId();
			presetName = selectedPreset;
			statusLine = "Adjust settings, load a preset, or save your own.";
		}

		SimpleRtsSimulation* sim = nullptr;
		UiAppTheme theme = UiAppTheme::light();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		Mutex mutex;
		DebugPlayer player;
		PerformanceGraph performanceGraph;
		Bool wireframeMode = false;
		GPUMesh terrainMesh;
		GPUMesh oceanMesh;
		SimpleRtsWorldSettings draftSettings;
		Bool setupVisible = true;
		Bool worldRunning = false;
		Bool authoredReady = false;
		Int nextTabIndex = 0;
		String selectedPreset;
		String presetName;
		String statusLine;
		Window* owningWindow = nullptr;
		UiWidgetHandle gameViewport;

		InputEvent enter           = InputEvent(InputType::Mouse, Lmb, true);
		InputEvent exit            = InputEvent(InputType::Mouse, Rmb, false);
		InputEvent pause           = InputEvent(InputType::KeyBoard, P, true);
		InputEvent toggleWireframe = InputEvent(InputType::KeyBoard, F, false);
		InputEvent reset           = InputEvent(InputType::KeyBoard, R, false);
		InputEvent toggleSetup     = InputEvent(InputType::KeyBoard, F2, false);

		void setSetupVisible(const Bool visible)
		{
			setupVisible = visible;
			if (visible && ui.capturing.load()) ui.releaseCapture();
			if (owningWindow) ui.focus(*owningWindow, this);
			refreshBoundState();
		}

		void toggleSetupPanel() { setSetupVisible(!setupVisible); }

		void publishPresetsList()
		{
			if (!authored) return;
			(void)authored->properties().set("presets.list", presetOptions(SimpleRtsWorldPresets::listPresets()));
		}

		void refreshBoundState()
		{
			if (!authored) return;
			(void)authored->properties().set("ui.themeToggleLabel",
				UiValue::fromString(theme.getMode() == UiThemeMode::Light ? "Dark mode" : "Light mode"));
			publishPresetsList();
			publishDraft(authored->properties(), draftSettings, selectedPreset, presetName, statusLine,
				worldRunning, setupVisible);
			authored->syncFromProperties();
			ensureInteractiveSurfaceWired();
		}

		void applyTheme()
		{
			host.useAppTheme(theme);
			if (!authoredReady) return;
			refreshBoundState();
		}

		void syncDraftFromProperties(const String& path, const UiValue& value)
		{
			if (path == "state.selectedPreset" && value.type == UiValueType::String)
				selectedPreset = value.string;
			else if (path == "state.presetName" && value.type == UiValueType::String)
				presetName = value.string;
			else if (path == "state.mapSize" && value.type == UiValueType::Number)
				draftSettings.mapSize = static_cast<Float>(value.number);
			else if (path == "state.terrainPointCount" && value.type == UiValueType::Number)
				draftSettings.terrainPointCount = static_cast<Int>(value.number);
			else if (path == "state.enableWater" && value.type == UiValueType::Bool)
				draftSettings.enableWater = value.boolean;
			else if (path == "state.enableElevation" && value.type == UiValueType::Bool)
				draftSettings.enableElevation = value.boolean;
			else if (path == "state.humans" && value.type == UiValueType::Number)
				draftSettings.humans = static_cast<Int>(value.number);
			else if (path == "state.rabbits" && value.type == UiValueType::Number)
				draftSettings.rabbits = static_cast<Int>(value.number);
			else if (path == "state.wolves" && value.type == UiValueType::Number)
				draftSettings.wolves = static_cast<Int>(value.number);
			else if (path == "state.trees" && value.type == UiValueType::Number)
				draftSettings.trees = static_cast<Int>(value.number);
			else if (path == "state.bushes" && value.type == UiValueType::Number)
				draftSettings.bushes = static_cast<Int>(value.number);
			else if (path == "state.berryBushes" && value.type == UiValueType::Number)
				draftSettings.berryBushes = static_cast<Int>(value.number);
			else if (path == "state.bodies" && value.type == UiValueType::Number)
				draftSettings.bodies = static_cast<Int>(value.number);

			if (path.starts_with("state.")) refreshBoundState();
		}

		void loadSelectedPresetIntoDraft()
		{
			String error;
			if (!SimpleRtsWorldPresets::loadPreset(selectedPreset, draftSettings, &error))
			{
				statusLine = error;
				refreshBoundState();
				return;
			}
			presetName = selectedPreset;
			SimpleRtsWorldPresets::setLastPresetId(selectedPreset);
			statusLine = "Loaded preset \"" + selectedPreset + "\".";
			refreshBoundState();
		}

		void unloadWorldGpu()
		{
			if (!owningWindow) return;
			if (terrainMesh.isLoaded()) terrainMesh.unload(owningWindow->renderer);
			if (oceanMesh.isLoaded()) oceanMesh.unload(owningWindow->renderer);
		}

		void startWorld()
		{
			if (!sim) return;
			sim->worldSettings = draftSettings;
			if (!sim->getKeepRunning() && !sim->isLoaded()) sim->start();
		}

		void restartWorld()
		{
			if (!sim) return;
			unloadWorldGpu();
			if (sim->getKeepRunning() || sim->isLoaded()) sim->stop();
			sim->worldSettings = draftSettings;
			sim->start();
		}

		void applyWorldFromDraft()
		{
			if (!sim) return;

			sim->worldSettings = draftSettings;
			SimpleRtsWorldPresets::setLastPresetId(selectedPreset);

			if (sim->isLoaded() || sim->getKeepRunning()) restartWorld();
			else startWorld();

			worldRunning = true;
			setSetupVisible(false);
			statusLine = "World is building…";
			refreshBoundState();
		}

		void renderWorld(Window& window, const IVec2 viewportPosition, const IVec2 viewportSize)
		{
			if (viewportSize.x <= 0 || viewportSize.y <= 0) return;

			Renderer& r = window.renderer;
			r.setRenderRegion(viewportPosition, viewportSize);

			if (!sim || !sim->isLoaded())
			{
				r.clearBackground(Color(0.05f, 0.07f, 0.1f, 1));
				r.setRenderRegion(IVec2(0), window.getSize());
				r.uiSpace(window);
				return;
			}

			mutex.lock();

			r.setWireframeMode();
			r.clearBackground(Color(0, 0, 0, 1));
			r.uniforms().setMMatrix(Matrix(1));
			r.setWireframeMode(wireframeMode);

			r.setCulling(false);
			r.setDepthTest(false);
			r.useShader("simpleRTSSky");
			r.useTexture("simpleRTSTexture");
			player.camera.renderOnlyRot(r);
			r.uniforms().setSunDir(Vec4(normalize(Vec3(0.5, 0.25, 1)), 1));
			r.renderMesh("SimpleRTSSky");
			r.setDepthTest(true);
			r.setCulling(true);

			player.render(window);
			r.fill(1.0f);
			if (!terrainMesh.isLoaded()) terrainMesh.upload(r, sim->terrainSystem.terrainMesh);
			r.useShader("simpleRTSTerrain");
			terrainMesh.render(r);

			if (sim->worldSettings.enableWater && sim->terrainSystem.oceanMesh.isLoaded())
			{
				if (!oceanMesh.isLoaded()) oceanMesh.upload(r, sim->terrainSystem.oceanMesh);
				r.useShader("simpleRTSOcean");
				oceanMesh.render(r);
			}

			r.useTexture("simpleRTSTexture");
			r.useShader("simpleRTS");
			sim->render(r);

			r.setDepthTest(false);
			r.screenSpaceFromBottomLeft();
			r.fill(Color(1));
			r.uniforms().setMMatrix(Matrix(1));
			r.fontSize(16.0f);
			r.letterSpacing(0.6f);
			r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
			r.text(toString(1.0f / r.time.getDelta()), Vec2(50));
			r.text(toString(sim->totalTime), Vec2(50, 100));

			performanceGraph.addTime(r.time.getDelta());
			performanceGraph.render(window);
			renderCrosshairDot(window);

			mutex.unlock();

			r.setRenderRegion(IVec2(0), window.getSize());
			r.uiSpace(window);
			r.setDepthTest(false);
			r.setCulling(false);
		}

		void ensureInteractiveSurfaceWired()
		{
			if (!authored || setupVisible) return;

			const UiWidgetHandle handle = authored->widgetBridge().findById("rts-world");
			if (!handle.isValid()) return;
			if (handle.index == gameViewport.index && handle.generation == gameViewport.generation &&
				gameViewport.isValid()) return;

			gameViewport = handle;
			authored->widgets().setInteractiveSurfaceRender(gameViewport,
				[this](const UiInteractiveSurfaceContext& context)
				{
					if (!owningWindow) return;
					const UiInteractiveSurfaceGlViewport viewport =
						interactiveSurfaceGlViewport(context, owningWindow->getHeight());
					renderWorld(*owningWindow, viewport.position, viewport.size);
				});
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

			(void)UiMarkupSurfaceRegistry::global();
			authored = std::make_unique<UiAuthoredHost>(host);

			(void)authored->commands().registerCommand("SimpleRts.ToggleTheme", {},
				[this](const Vector<UiValue>&)
				{
					theme.setMode(theme.getMode() == UiThemeMode::Light ? UiThemeMode::Dark : UiThemeMode::Light);
					applyTheme();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.LoadPreset", {},
				[this](const Vector<UiValue>&)
				{
					loadSelectedPresetIntoDraft();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.SavePreset", {},
				[this](const Vector<UiValue>&)
				{
					String error;
					const String saveId = presetName.empty() ? selectedPreset : presetName;
					if (!SimpleRtsWorldPresets::saveUserPreset(saveId, draftSettings, &error))
						statusLine = error;
					else
					{
						selectedPreset = saveId;
						statusLine = "Saved preset \"" + saveId + "\".";
						publishPresetsList();
					}
					refreshBoundState();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.DeletePreset", {},
				[this](const Vector<UiValue>&)
				{
					String error;
					if (!SimpleRtsWorldPresets::deleteUserPreset(selectedPreset, &error))
						statusLine = error;
					else
					{
						statusLine = "Deleted preset \"" + selectedPreset + "\".";
						publishPresetsList();
					}
					refreshBoundState();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.ResetDraft", {},
				[this](const Vector<UiValue>&)
				{
					draftSettings = SimpleRtsWorldSettings::defaults();
					statusLine = "Draft reset to engine defaults (not saved).";
					refreshBoundState();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.ApplyWorld", {},
				[this](const Vector<UiValue>&)
				{
					applyWorldFromDraft();
					return true;
				});
			(void)authored->commands().registerCommand("SimpleRts.CloseSetup", {},
				[this](const Vector<UiValue>&)
				{
					setSetupVisible(false);
					return true;
				});

			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(uiDirectory / "simple-rts.ui.html", uiDirectory / "simple-rts.ui.scss"))
			{
				logError("Simple RTS: failed to load authored UI bundle.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			authored->widgetBridge().setTabIndexAllocator([this]() { return nextTabIndex++; });
			authored->widgetBridge().setPropertyChangeHandler(
				[this](const String& path, const UiValue& value)
				{
					syncDraftFromProperties(path, value);
				});

			if (!authored->mount())
			{
				logError("Simple RTS: failed to mount authored UI.");
				return;
			}

			authored->ensureNodeLayout("simple-rts-root", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("simple-rts-shell", [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});
			authored->ensureNodeLayout("simple-rts-setup-scroll", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});
			authored->ensureNodeLayout("rts-world", [](UiLayoutStyle& style)
				{
					style.flexGrow = 1;
					style.flexShrink = 1;
				});

			authoredReady = true;
			refreshBoundState();
			ui.focus(window, this);
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			if (!setupVisible)
			{
				player.update(window);
				Vec3 pos = player.camera.getPosition();
				pos.z = max(0.0f, pos.z);
				player.camera.setPosition(pos);
			}

			if (authoredReady)
			{
				refreshBoundState();
				authored->syncControlLayouts();
			}
			host.update(window);
		}

		void inputEvent(Window& window, InputEvent& input) override
		{
			if (input == toggleSetup)
			{
				toggleSetupPanel();
				return;
			}

			// routeInput already offered this event to the retained host.
			// While setup is open, leftover keys stay out of the camera.
			if (setupVisible) return;

			if (input == enter) ui.capture(*this);
			if (input == pause && sim && sim->isLoaded()) sim->paused = !sim->paused;
			if (input == exit) ui.releaseCapture();
			if (input == toggleWireframe) wireframeMode = !wireframeMode;
			if (input == reset && sim && sim->isLoaded()) restartWorld();
			player.inputEvent(window, input);
		}

		void destroy(Window& window) override
		{
			unloadWorldGpu();
			authored.reset();
			authoredReady = false;
			owningWindow = nullptr;
			host.destroy(window);
		}
	};
}

void SimpleRtsSimulation::destroy()
{
	for (const auto system : systems)
		system->destroy(*this);
	systems.clear();
	loaded = false;
}

void SimpleRtsSimulation::reset()
{
	if (loaded)
	{
		for (const auto system : systems)
			system->destroy(*this);
		systems.clear();
	}
	create();
}

void SimpleRtsSimulation::create()
{
	systems.push_back(&terrainSystem);

	systems.push_back(&bodySystem);
	systems.push_back(&bushSystem);
	systems.push_back(&treeSystem);
	systems.push_back(&wolfSystem);
	systems.push_back(&humanSystem);
	systems.push_back(&rabbitSystem);
	systems.push_back(&berryBushSystem);
	for (const auto s : systems) s->create(*this);

	quadtreeSystem.create(*this);
	quadtreeSystem.update(*this, worldSettings.mapSize);

	loaded = true;
}

Float SimpleRtsSimulation::timeStep() const
{
	if (paused) return 0.0;
	return timeScale / frameRate;
}

void SimpleRtsSimulation::update(Float delta)
{
	totalTime += timeStep();
	for (const auto s : systems) s->update(*this);
	limitFramerate(frameRate);
}

Float SimpleRtsSimulation::getTerrainHeight(const Vec2 vec) const
{
	return terrainSystem.sampleHeight(*this, vec.x / worldSettings.mapSize, vec.y / worldSettings.mapSize);
}

void SimpleRtsSimulation::render(Renderer& r) const
{
	for (const auto s : systems) s->render(r);
}

void SimpleRtsSimulation::waitUntilLoaded() const
{
	while (!loaded) {}
}

Vec3 SimpleRtsSimulation::getRandomSpawnPos(const Float radius) const
{
	Vec2 pos;
	Int  tries = 0;
	Bool doesSpawnCollide;
	do
	{
		pos              = randomVec2Fast(-worldSettings.mapSize, worldSettings.mapSize);
		doesSpawnCollide = doesCollide(pos, radius);
		tries++;
	}
	while (terrainSystem.isWater(*this, pos) || doesSpawnCollide);
	return {pos, getTerrainHeight(pos)};
}

Vec3 SimpleRtsSimulation::getRandomSpawnPos() const
{
	Vec2 pos;
	Int  tries = 0;
	do
	{
		pos = randomVec2Fast(-worldSettings.mapSize, worldSettings.mapSize);
		tries++;
	}
	while (terrainSystem.isWater(*this, pos));
	return {pos, getTerrainHeight(pos)};
}

Bool SimpleRtsSimulation::doesCollide(const Vec2 pos, const Float radius) const
{
	if (treeSystem.doesCollide(pos, radius)) return true;
	if (bushSystem.doesCollide(pos, radius)) return true;
	if (berryBushSystem.doesCollide(pos, radius)) return true;
	return false;
}

void SimpleRts::run()
{
	(void)SimpleRtsWorldPresets::loadLastSettings(sim.worldSettings);

	SimpleRtsAppShell shell(sim);
	shell.draftSettings = sim.worldSettings;
	ui.window("Simple RTS", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, shell);
	ui.run();
	if (sim.isLoaded() || sim.getKeepRunning()) sim.stop();
}
