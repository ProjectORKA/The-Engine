#include "SimpleRtsSettingsPanel.hpp"

#include "Debug.hpp"
#include "ResourceManager.hpp"
#include "SimpleRTS.hpp"
#include "UI/Theme/UiAppHelpers.hpp"
#include "UserInterface.hpp"

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

	void copySettingsToDraft(SimpleRtsWorldSettings& draft, const SimpleRtsWorldSettings& source)
	{
		draft = source;
	}

	void publishDraft(UiPropertyRegistry& properties, const SimpleRtsWorldSettings& draft,
		const String& selectedPreset, const String& presetName, const String& statusLine, const Bool worldRunning)
	{
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
}

SimpleRtsSettingsPanel::SimpleRtsSettingsPanel(SimpleRtsRenderer& owner)
{
	bindRenderer(owner);
	addElement(host);
	growX(true);
	growY(true);
	background(Color(0, 0, 0, 0.55f));
	(void)SimpleRtsWorldPresets::loadLastSettings(draftSettings);
	selectedPreset = SimpleRtsWorldPresets::loadLastPresetId();
	presetName = selectedPreset;
	statusLine = "Adjust settings, load a preset, or save your own.";
}

void SimpleRtsSettingsPanel::bindRenderer(SimpleRtsRenderer& owner)
{
	renderer = &owner;
}

void SimpleRtsSettingsPanel::setVisiblePanel(const Bool visible)
{
	panelVisible = visible;
	if (!owningWindow) return;
	if (!visible && renderer)
		ui.focus(*owningWindow, renderer);
	else if (visible)
		ui.focus(*owningWindow, this);
}

void SimpleRtsSettingsPanel::syncOverlayLayout(Window& window)
{
	const IVec2 size = window.getSize();
	setWidth(size.x);
	setHeight(size.y);
	setPositionX(0);
	setPositionY(0);
	host.setWidth(size.x);
	host.setHeight(size.y);
}

void SimpleRtsSettingsPanel::togglePanel()
{
	setVisiblePanel(!panelVisible);
	if (authoredReady) refreshBoundState();
}

void SimpleRtsSettingsPanel::publishPresetsList()
{
	if (!authored) return;
	(void)authored->properties().set("presets.list", presetOptions(SimpleRtsWorldPresets::listPresets()));
}

void SimpleRtsSettingsPanel::refreshBoundState()
{
	if (!authored) return;
	(void)authored->properties().set("ui.themeToggleLabel",
		UiValue::fromString(theme.getMode() == UiThemeMode::Light ? "Dark mode" : "Light mode"));
	publishPresetsList();
	publishDraft(authored->properties(), draftSettings, selectedPreset, presetName, statusLine, worldRunning);
	authored->syncFromProperties();
}

void SimpleRtsSettingsPanel::applyTheme()
{
	host.useAppTheme(theme);
	if (!authoredReady) return;
	refreshBoundState();
}

void SimpleRtsSettingsPanel::syncDraftFromProperties(const String& path, const UiValue& value)
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

	if (path.starts_with("state."))
		refreshBoundState();
}

void SimpleRtsSettingsPanel::loadSelectedPresetIntoDraft()
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

void SimpleRtsSettingsPanel::applyWorldFromDraft()
{
	if (!renderer || !renderer->sim) return;

	SimpleRtsSimulation& sim = *renderer->sim;
	sim.worldSettings = draftSettings;
	SimpleRtsWorldPresets::setLastPresetId(selectedPreset);

	if (sim.isLoaded() || sim.getKeepRunning())
		renderer->restartWorld();
	else
		renderer->startWorld();

	worldRunning = true;
	setVisiblePanel(false);
	statusLine = "World is building…";
	refreshBoundState();
}

void SimpleRtsSettingsPanel::create(Window& window)
{
	owningWindow = &window;
	const IVec2 size = window.getSize();
	setWidth(size.x);
	setHeight(size.y);
	host.setWidth(size.x);
	host.setHeight(size.y);
	setVisiblePanel(panelVisible);

	host.useAppTheme(theme);
	UIElement::create(window);

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
			copySettingsToDraft(draftSettings, SimpleRtsWorldSettings::defaults());
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
			setVisiblePanel(false);
			return true;
		});

	const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
	if (!authored->loadFromFiles(uiDirectory / "simple-rts-world.ui.html", uiDirectory / "simple-rts-world.ui.scss"))
	{
		logError("Simple RTS setup: failed to load authored UI bundle.");
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
		logError("Simple RTS setup: failed to mount authored UI.");
		return;
	}

	authored->ensureNodeLayout("simple-rts-setup-root", [](UiLayoutStyle& style)
		{
			style.width = UiSizeSpec::percent(1);
			style.height = UiSizeSpec::percent(1);
		});
	authored->ensureNodeLayout("simple-rts-setup-shell", [](UiLayoutStyle& style)
		{
			style.width = UiSizeSpec::percent(1);
			style.height = UiSizeSpec::percent(1);
		});
	authored->ensureNodeLayout("simple-rts-setup-scroll", [](UiLayoutStyle& style)
		{
			style.flexGrow = 1;
			style.flexShrink = 1;
		});

	authoredReady = true;
	refreshBoundState();
	if (owningWindow) ui.focus(*owningWindow, this);
}

void SimpleRtsSettingsPanel::render(Window& window)
{
	if (!panelVisible) return;
	syncOverlayLayout(window);
	UIElement::render(window);
}

void SimpleRtsSettingsPanel::inputEvent(Window& window, InputEvent& input)
{
	if (!panelVisible) return;
	host.inputEvent(window, input);
}

Bool SimpleRtsSettingsPanel::isPointInside(const Window& window, const DVec2& point) const
{
	if (!panelVisible) return false;
	return UIElement::isPointInside(window, point);
}

void SimpleRtsSettingsPanel::update(Window& window)
{
	if (!panelVisible) return;

	syncOverlayLayout(window);

	if (authoredReady)
		authored->syncControlLayouts();
	host.update(window);
	UIElement::update(window);
}

void SimpleRtsSettingsPanel::destroy(Window& window)
{
	authored.reset();
	authoredReady = false;
	host.destroy(window);
	UIElement::destroy(window);
}
