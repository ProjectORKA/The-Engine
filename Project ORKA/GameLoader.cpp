#include "GameLoader.hpp"

#include "Debug.hpp"
#include "ResourceManager.hpp"
#include "UI/Authoring/UiAuthoredHost.hpp"
#include "UI/Binding/UiBindingRegistry.hpp"
#include "UI/Theme/UiAppFlex.hpp"
#include "UI/Theme/UiAppHelpers.hpp"
#include "UI/Theme/UiAppTheme.hpp"
#include "UI/Samples/UiControlsDemo.hpp"
#include "UI/Samples/UiLogicalWindowsDemo.hpp"
#include "UI/Widgets/UiWidgets.hpp"
#include "Window.hpp"
#include "WindowAPI.hpp"

#include "Pong.hpp"
#include "ProjectMelon.hpp"
#include "Sandbox.hpp"
#include "ShadertoyRenderer.hpp"
#include "SoftwareRendererGame.hpp"
#include "TripleNine.hpp"
#include "Voxels.hpp"
#include "Mooncrash.hpp"
#include "RayTracer.hpp"
#include "FunctionFinder.hpp"
#include "MelonAudioLab.hpp"
#include "SoundAnalysisLab.hpp"
#include "SimpleRTS.hpp"
#include "SdfPointcloud.hpp"
#include "AI/AiChat.hpp"

namespace
{
	using namespace Ui;

	UiValue gamesCatalog(const Vector<GameEntry>& entries)
	{
		Vector<UiValue> games;
		games.reserve(entries.size());
		for (Int index = 0; index < static_cast<Int>(entries.size()); ++index)
		{
			const GameEntry& entry = entries[static_cast<SizeT>(index)];
			games.push_back(UiValue::fromObject({
				{"id", UiValue::fromString("game-" + toString(index))},
				{"name", UiValue::fromString(entry.displayName)},
				{"description", UiValue::fromString(entry.description)}
				}));
		}
		return UiValue::fromArray(std::move(games));
	}

	void publishUiCopy(UiPropertyRegistry& properties, const UiAppTheme& theme, const SizeT projectCount)
	{
		(void)properties.set("ui.themeToggleLabel",
			UiValue::fromString(theme.getMode() == UiThemeMode::Light ? "Dark mode" : "Light mode"));
		(void)properties.set("ui.projectCount",
			UiValue::fromString(toString(projectCount) + " total"));
	}

	void publishLauncherCatalog(UiPropertyRegistry& properties, const Vector<GameEntry>& entries)
	{
		(void)properties.set("launcher.games", gamesCatalog(entries));
	}
}

Vector<GameEntry> GameLoader::buildDefaultEntries()
{
	return {
		{"UI Controls Demo", "Interactive reference for the unified ORKA design system and controls.", []() { UiControlsDemo game; game.run(); }},
		{"Logical Windows Demo", "Two OS windows from one authored document via <window> nodes and mountLogicalWindow.", []() { UiLogicalWindowsDemo demo; demo.run(); }},
		{"AI Chat", "Local Qwen-class chat via engine-managed llama.cpp — baseline for NPCs, tools, and voice.", []() { AiChat game; game.run(); }},
		{"Sandbox", "A small playground to experiment with engine features.", []() { Sandbox game; game.run(); }},
		{"Pong", "Classic Pong built with ORKA's UI and input systems.", []() { Pong game; game.run(); }},
		{"Voxels", "Ray-marched voxel world with camera controls and lighting.", []() { Voxels game; game.run(); }},
		{"MoonCrash", "Planet-scale terrain and atmosphere simulation.", []() { MoonCrash game; game.run(); }},
		{"Project Melon", "Stylized melon world prototype with player movement and rendering.", []() { ProjectMelon game; game.run(); }},
		{"Melon Audio Lab", "Procedural motion audio testbed with hum, wind, and heartbeat layers.", []() { MelonAudioLab game; game.run(); }},
		{"Sound Analysis Lab", "Import and inspect audio: waveform zoom/scrub, FFT spectrum, and spectrogram.", []() { SoundAnalysisLab game; game.run(); }},
		{"Simple RTS", "Lightweight RTS sim with humans, wildlife, terrain, and resource systems.", []() { SimpleRts game; game.run(); }},
		{"SDF Pointcloud", "Screenspace pointcloud SDF with temporal reprojection and compute hole-fill.", []() { SdfPointcloud game; game.run(); }},
		{"Shadertoy (Erosion)", "Shader-based erosion effect inspired by Shadertoy.", []() { ShadertoyGame game; game.run(); }},
		{"Software Renderer", "CPU rasterizer demo showcasing the software pipeline.", []() { SoftwareRendererGame game; game.run(); }},
		{"Function Finder", "Curve-fitting playground that searches function parameters against sampled data.", []() { FunctionFinder game; game.run(); }},
		{"Triple Nine", "Arena shooter prototype with bloom rendering and intro support.", []() { TripleNine game; game.run(); }},
		{"Cornell Box Ray Tracer", "GPU PBR path tracer with GGX metals, glass, area-light NEE, and progressive accumulation.", []() { RayTracerGame game; game.run(); }}
	};
}

GameLoader::GameLoader()
{
	entries = buildDefaultEntries();
	selectionIndex.store(-1);

	column();
	growX(true);
	growY(true);
	background(Color(0, 0, 0, 0));
	addElement(host);
}

void GameLoader::applyTheme()
{
	host.useAppTheme(theme);
	if (!authoredReady || !authored) return;
	publishUiCopy(authored->properties(), theme, entries.size());
	authored->syncFromProperties();
	wireLauncherInteractions();
}

void GameLoader::toggleTheme()
{
	theme.setMode(theme.getMode() == UiThemeMode::Light ? UiThemeMode::Dark : UiThemeMode::Light);
	applyTheme();
}

void GameLoader::applyLauncherLayout()
{
	if (!authored) return;
	authored->ensureNodeLayout("game-loader", [](UiLayoutStyle& style)
		{
			style.width = UiSizeSpec::percent(1);
			style.height = UiSizeSpec::percent(1);
		});
	authored->ensureNodeLayout("game-loader-shell", [](UiLayoutStyle& style)
		{
			style.width = UiSizeSpec::percent(1);
			style.height = UiSizeSpec::percent(1);
		});
	authored->ensureNodeLayout("game-list-panel", [](UiLayoutStyle& style)
		{
			style.flexGrow = 1;
			style.flexShrink = 1;
		});
	authored->ensureNodeLayout("game-list", [](UiLayoutStyle& style)
		{
			style.flexGrow = 1;
			style.flexShrink = 1;
		});
}

void GameLoader::wireLauncherInteractions()
{
	if (!authored) return;

	UiWidgetFactory& widgets = authored->widgets();
	UiDocument& document = host.document();
	UiEventRouter& router = host.eventRouter();

	for (Int index = 0; index < static_cast<Int>(entries.size()); ++index)
	{
		const String buttonId = "game-" + toString(index);
		const UiWidgetHandle button = authored->widgetBridge().findById(buttonId);
		if (!widgets.isValid(button)) continue;

		if (UiButtonState* buttonState = widgets.state<UiButtonState>(button))
		{
			buttonState->onActivate = [this, index](UiWidgetHandle)
				{
					if (owningWindow) onGameSelected(*owningWindow, index);
				};
		}

		if (UiNode* buttonNode = document.get(widgets.node(button)))
			buttonNode->state.semantics.label = entries[static_cast<SizeT>(index)].displayName;

		setHitTestInvisibleRecursive(document, router, widgets.node(button));
	}
}

void GameLoader::onGameSelected(Window& window, const Int index)
{
	selectionIndex.store(index);

	if (index >= 0 && index < static_cast<Int>(entries.size()))
		selectedGame = entries[static_cast<SizeT>(index)].launch;
	else selectedGame = {};

	window.keepRunning = false;
	apiSetWindowShouldClose(window.apiWindow, true);
}

void GameLoader::create(Window& window)
{
	owningWindow = &window;
	const IVec2 size = window.getSize();
	setWidth(size.x);
	setHeight(size.y);
	host.setWidth(size.x);
	host.setHeight(size.y);

	host.useAppTheme(theme);
	UIElement::create(window);

	authored = std::make_unique<UiAuthoredHost>(host);

	(void)authored->commands().registerCommand("Launcher.ToggleTheme", {},
		[this](const Vector<UiValue>&)
		{
			toggleTheme();
			return true;
		});

	const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
	if (!authored->loadFromFiles(
		uiDirectory / "game-loader.ui.html",
		uiDirectory / "game-loader.ui.scss"))
	{
		logError("GameLoader: failed to load authored UI bundle.");
		for (const UiDiagnostic& diagnostic : authored->diagnostics())
			logError("  " + diagnostic.message);
		return;
	}

	publishUiCopy(authored->properties(), theme, entries.size());
	publishLauncherCatalog(authored->properties(), entries);

	if (!authored->mount())
	{
		logError("GameLoader: failed to mount authored UI.");
		for (const UiDiagnostic& diagnostic : authored->diagnostics())
			logError("  " + diagnostic.message);
		return;
	}

	applyLauncherLayout();
	authoredReady = true;
	applyTheme();
}

void GameLoader::update(Window& window)
{
	const IVec2 size = window.getSize();
	setWidth(size.x);
	setHeight(size.y);
	host.setWidth(size.x);
	host.setHeight(size.y);
	if (authoredReady) authored->syncControlLayouts();
	UIElement::update(window);
}

void GameLoader::destroy(Window& window)
{
	UIElement::destroy(window);
	authored.reset();
	owningWindow = nullptr;
	authoredReady = false;
}

Function<void()> GameLoader::consumeSelectedGame()
{
	Function<void()> result = std::move(selectedGame);
	selectedGame            = {};
	selectionIndex.store(-1);
	return result;
}
