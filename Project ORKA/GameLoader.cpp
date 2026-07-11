#include "GameLoader.hpp"
#include "Window.hpp"
#include "WindowAPI.hpp"

#include "Pong.hpp"
#include "ProjectMelon.hpp"
#include "Sandbox.hpp"
#include "ShadertoyRenderer.hpp"
#include "SoftwareRendererGame.hpp"
#include "TripleNine.hpp"
#include "UISandbox.hpp"
#include "Voxels.hpp"
#include "Mooncrash.hpp"
#include "RayTracer.hpp"
#include "FunctionFinder.hpp"
#include "MelonAudioLab.hpp"
#include "SoundAnalysisLab.hpp"

Vector<GameEntry> GameLoader::buildDefaultEntries()
{
	return {
		{"UI Sandbox", "UI layout playground with embedded scene renderer.", []() { UISandbox game; game.run(); }},
		{"Sandbox", "A small playground to experiment with engine features.", []() { Sandbox game; game.run(); }},
		{"Pong", "Classic Pong built with ORKA's UI and input systems.", []() { Pong game; game.run(); }},
		{"Voxels", "Ray-marched voxel world with camera controls and lighting.", []() { Voxels game; game.run(); }},
		{"MoonCrash", "Planet-scale terrain and atmosphere simulation.", []() { MoonCrash game; game.run(); }},
		{"Project Melon", "Stylized melon world prototype with player movement and rendering.", []() { ProjectMelon game; game.run(); }},
		{"Melon Audio Lab", "Procedural motion audio testbed with hum, wind, and heartbeat layers.", []() { MelonAudioLab game; game.run(); }},
		{"Sound Analysis Lab", "Import and inspect audio: waveform zoom/scrub, FFT spectrum, and spectrogram.", []() { SoundAnalysisLab game; game.run(); }},
		{"Shadertoy (Erosion)", "Shader-based erosion effect inspired by Shadertoy.", []() { ShadertoyGame game; game.run(); }},
		{"Software Renderer", "CPU rasterizer demo showcasing the software pipeline.", []() { SoftwareRendererGame game; game.run(); }},
		{"Function Finder", "Curve-fitting playground that searches function parameters against sampled data.", []() { FunctionFinder game; game.run(); }},
		{"Triple Nine", "Arena shooter prototype with bloom rendering and intro support.", []() { TripleNine game; game.run(); }},
		{"Cornell Box Ray Tracer", "CPU path tracer with diffuse, metal, glass materials and progressive accumulation.", []() { RayTracerGame game; game.run(); }}
	};
}

GameLoader::GameLoader()
{
	entries = buildDefaultEntries();
	selectionIndex.store(-1);

	column();
	padding(40);
	gap(10);
	growX(true);
	growY(true);
}

GameLoader::LaunchButton::LaunchButton(GameLoader& loaderRef, const Int index)
{
	owner      = &loaderRef;
	entryIndex = index;
}

void GameLoader::LaunchButton::doThis()
{
	if (!owner) return;

	Window* owningWindowResolved = owningWindow;
	if (!owningWindowResolved)
	{
		UIElement* current = this;
		while (current != nullptr && owningWindowResolved == nullptr)
		{
			owningWindowResolved = dynamic_cast<Window*>(current);
			current              = current->parent;
		}
	}

	if (owningWindowResolved) owner->onGameSelected(*owningWindowResolved, entryIndex);
}

void GameLoader::buildUi()
{
	if (built) return;

	buttons.clear();
	entryContents.clear();
	nameLabels.clear();
	descriptionLabels.clear();

	buttons.reserve(entries.size());
	entryContents.reserve(entries.size());
	nameLabels.reserve(entries.size());
	descriptionLabels.reserve(entries.size());

	titleLabel.setText("Select a game to launch");
	titleLabel.color(Color(1.0f));
	titleLabel.growX(true);
	titleLabel.padding(10);
	titleLabel.growY(false);
	titleLabel.setHeight(36);
	addElement(titleLabel);

	for (Int i = 0; i < static_cast<Int>(entries.size()); ++i)
	{
		buttons.emplace_back(*this, i);
		auto& button = buttons.back();
		button.padding(12);
		button.growX(true);
		button.background(Color(1.0, 1.0, 1.0, 0.08));
		button.hoveredBackground(Color(1.0, 1.0, 1.0, 0.16));
		button.activeBackground(Color(1.0, 1.0, 1.0, 0.28));

		entryContents.emplace_back();
		UIElement& container = entryContents.back();
		container.column();
		container.gap(4);
		container.growX(true);

		nameLabels.emplace_back();
		UIElement& nameLabel = nameLabels.back();
		nameLabel.setText(entries[static_cast<SizeT>(i)].displayName);
		nameLabel.growX(true);
		nameLabel.padding(10);

		descriptionLabels.emplace_back();
		UIElement& descriptionLabel = descriptionLabels.back();
		descriptionLabel.setText(entries[static_cast<SizeT>(i)].description);
		descriptionLabel.growX(true);
		descriptionLabel.padding(10);

		container.addElement(nameLabel);
		container.addElement(descriptionLabel);

		button.insert(container);

		addElement(button);
	}

	built = true;
}

void GameLoader::onGameSelected(Window& window, const Int index)
{
	selectionIndex.store(index);

	if (index >= 0 && index < static_cast<Int>(entries.size()))
	{
		selectedGame = entries[static_cast<SizeT>(index)].launch;
	}
	else selectedGame = {};

	window.keepRunning = false;
	apiSetWindowShouldClose(window.apiWindow, true);
}

void GameLoader::create(Window& window)
{
	if (!built) buildUi();

	for (LaunchButton& button : buttons) button.owningWindow = &window;

	UIElement::create(window);
}

void GameLoader::destroy(Window& window)
{
	UIElement::destroy(window);
}

Function<void()> GameLoader::consumeSelectedGame()
{
	Function<void()> result = std::move(selectedGame);
	selectedGame            = {};
	selectionIndex.store(-1);
	return result;
}

