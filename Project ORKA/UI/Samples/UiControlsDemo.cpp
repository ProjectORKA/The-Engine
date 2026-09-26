#include "UiControlsDemo.hpp"

#include "../../Debug.hpp"
#include "../../ResourceManager.hpp"
#include "../../Settings.hpp"
#include "../../UserInterface.hpp"
#include "../../Window.hpp"
#include "../Authoring/UiAuthoredHost.hpp"
#include "../Theme/UiAppHelpers.hpp"
#include "../Theme/UiAppTheme.hpp"

namespace
{
	using namespace Ui;

	struct DemoState
	{
		Bool lightingEnabled = true;
		Bool debugOverlay = false;
		Bool vsyncEnabled = true;
		Bool notificationsEnabled = false;
		Double volume = 65;
		Double brightness = 0.75;
		Double particleCap = 2500;
		Double temperature = 21.5;
		Double unboundOffset = 0;
		String qualityPreset = "high";
		String renderMode = "forward";
		String playerName = "Pilot";
		String notes = "Adjust controls and watch the status panel update.";
	};

	UiValue tokenArray(const UiAppTheme& theme, const String& category)
	{
		Vector<UiValue> entries;
		for (const UiAppThemeToken& token : theme.tokens())
		{
			if (category != token.category) continue;
			entries.push_back(UiValue::fromObject({
				{"id", UiValue::fromString(token.id)},
				{"label", UiValue::fromString(token.label)}
				}));
		}
		return UiValue::fromArray(std::move(entries));
	}

	UiValue typographySamples()
	{
		return UiValue::fromArray({
			UiValue::fromObject({
				{"id", UiValue::fromString("type-body")},
				{"text", UiValue::fromString("Body — The quick brown fox jumps over the lazy dog.")}
				}),
			UiValue::fromObject({
				{"id", UiValue::fromString("type-title")},
				{"text", UiValue::fromString("Title — The quick brown fox")}
				}),
			});
	}

	UiValue selectOptions(const Vector<std::pair<String, String>>& entries)
	{
		Vector<UiValue> options;
		for (const auto& [value, label] : entries)
		{
			options.push_back(UiValue::fromObject({
				{"value", UiValue::fromString(value)},
				{"label", UiValue::fromString(label)}
				}));
		}
		return UiValue::fromArray(std::move(options));
	}

	String statusText(const UiAppTheme& theme, const DemoState& state)
	{
		return
			"Theme: " + String(theme.getMode() == UiThemeMode::Light ? "Light" : "Dark") +
			"  |  Lighting: " + String(state.lightingEnabled ? "ON" : "OFF") +
			"  |  Debug: " + String(state.debugOverlay ? "ON" : "OFF") +
			"  |  VSync: " + String(state.vsyncEnabled ? "ON" : "OFF") +
			"\nVolume: " + toString(static_cast<Int>(state.volume)) + "%" +
			"  |  Brightness: " + toString(static_cast<Int>(state.brightness * 100)) + "%" +
			"\nQuality: " + state.qualityPreset + "  |  Render: " + state.renderMode +
			"\nPlayer: \"" + state.playerName + "\"";
	}

	void publishState(UiPropertyRegistry& properties, const UiAppTheme& theme, const DemoState& state)
	{
		(void)properties.set("state.lightingEnabled", UiValue::fromBool(state.lightingEnabled));
		(void)properties.set("state.debugOverlay", UiValue::fromBool(state.debugOverlay));
		(void)properties.set("state.vsyncEnabled", UiValue::fromBool(state.vsyncEnabled));
		(void)properties.set("state.notificationsEnabled", UiValue::fromBool(state.notificationsEnabled));
		(void)properties.set("state.volume", UiValue::fromNumber(state.volume));
		(void)properties.set("state.brightness", UiValue::fromNumber(state.brightness));
		(void)properties.set("state.particleCap", UiValue::fromNumber(state.particleCap));
		(void)properties.set("state.temperature", UiValue::fromNumber(state.temperature));
		(void)properties.set("state.unboundOffset", UiValue::fromNumber(state.unboundOffset));
		(void)properties.set("state.qualityPreset", UiValue::fromString(state.qualityPreset));
		(void)properties.set("state.renderMode", UiValue::fromString(state.renderMode));
		(void)properties.set("state.playerName", UiValue::fromString(state.playerName));
		(void)properties.set("state.notes", UiValue::fromString(state.notes));
		(void)properties.set("state.volumeLabel", UiValue::fromString(toString(static_cast<Int>(state.volume)) + "%"));
		(void)properties.set("state.brightnessLabel",
			UiValue::fromString(toString(static_cast<Int>(state.brightness * 100)) + "%"));
		(void)properties.set("state.statusText", UiValue::fromString(statusText(theme, state)));
	}

	void publishUiCopy(UiPropertyRegistry& properties, const UiAppTheme& theme)
	{
		(void)properties.set("ui.themeToggleLabel",
			UiValue::fromString(theme.getMode() == UiThemeMode::Light ? "Dark mode" : "Light mode"));
		(void)properties.set("ui.subtitle",
			UiValue::fromString("Authored UI from Data/UI/controls-demo.ui.html + global app-master.ui.scss."));
	}

	void publishCatalog(UiPropertyRegistry& properties, const UiAppTheme& theme)
	{
		(void)properties.set("tokens.colors", tokenArray(theme, "color"));
		(void)properties.set("tokens.padding", tokenArray(theme, "padding"));
		(void)properties.set("tokens.gap", tokenArray(theme, "gap"));
		(void)properties.set("tokens.layout", tokenArray(theme, "layout"));
		(void)properties.set("tokens.typography", typographySamples());
		(void)properties.set("options.quality", selectOptions({
			{"low", "Low"}, {"medium", "Medium"}, {"high", "High"}, {"ultra", "Ultra"}}));
		(void)properties.set("options.renderMode", selectOptions({
			{"forward", "Forward"}, {"deferred", "Deferred"}, {"path", "Path traced"}}));
	}

	void applyDemoLayout(UiAuthoredHost& authored, UiHost& host)
	{
		authored.ensureNodeLayout("controls-root", [](UiLayoutStyle& style)
			{
				style.width = UiSizeSpec::percent(1);
				style.height = UiSizeSpec::percent(1);
			});
		authored.ensureNodeLayout("controls-scroll", [](UiLayoutStyle& style)
			{
				style.flexGrow = 1;
				style.flexShrink = 1;
			});
		authored.ensureNodeLayout("controls-status-panel", [](UiLayoutStyle& style)
			{
				style.flexShrink = 0;
			});
		UiDocument& document = host.document();
		UiEventRouter& router = host.eventRouter();
		document.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				if (node.authored.classes.empty()) return;
				const Bool labeledRow = std::find(node.authored.classes.begin(), node.authored.classes.end(),
					"labeled-row") != node.authored.classes.end();
				if (!labeledRow) return;
				UiNodeHandle control;
				for (const UiNodeHandle child : node.childHandles())
				{
					if (control.isValid()) break;
					const UiNode* childNode = document.get(child);
					if (!childNode) continue;
					if (childNode->authored.typeName == "Text") continue;
					control = child;
				}
				if (control.isValid())
					setHitTestInvisibleRecursive(document, router, handle, control);
			});
	}

	struct UiControlsDemoSurface final : UIElement
	{
		UiAppTheme theme = UiAppTheme::light();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		DemoState state;
		Int nextTabIndex = 0;
		Bool authoredReady = false;

		UiControlsDemoSurface()
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
		}

		void refreshBoundState()
		{
			if (!authored) return;
			publishUiCopy(authored->properties(), theme);
			publishCatalog(authored->properties(), theme);
			publishState(authored->properties(), theme, state);
			authored->syncFromProperties();
		}

		void applyTheme()
		{
			host.useAppTheme(theme);
			if (!authoredReady) return;
			refreshBoundState();
		}

		void create(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);

			host.useAppTheme(theme);
			UIElement::create(window);

			authored = std::make_unique<UiAuthoredHost>(host);

			(void)authored->commands().registerCommand("Demo.ToggleTheme", {},
				[this](const Vector<UiValue>&)
				{
					theme.setMode(theme.getMode() == UiThemeMode::Light ? UiThemeMode::Dark : UiThemeMode::Light);
					applyTheme();
					return true;
				});
			(void)authored->commands().registerCommand("Demo.Reset", {},
				[this](const Vector<UiValue>&)
				{
					state = DemoState{};
					applyTheme();
					return true;
				});

			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(
				uiDirectory / "controls-demo.ui.html",
				uiDirectory / "controls-demo.ui.scss"))
			{
				logError("UiControlsDemo: failed to load authored UI bundle.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			publishUiCopy(authored->properties(), theme);
			publishCatalog(authored->properties(), theme);
			publishState(authored->properties(), theme, state);

			authored->widgetBridge().setTabIndexAllocator([this]() { return nextTabIndex++; });
			authored->widgetBridge().setPropertyChangeHandler(
				[this](const String& path, const UiValue& value)
				{
					if (path == "state.lightingEnabled" && value.type == UiValueType::Bool)
						state.lightingEnabled = value.boolean;
					else if (path == "state.debugOverlay" && value.type == UiValueType::Bool)
						state.debugOverlay = value.boolean;
					else if (path == "state.vsyncEnabled" && value.type == UiValueType::Bool)
						state.vsyncEnabled = value.boolean;
					else if (path == "state.notificationsEnabled" && value.type == UiValueType::Bool)
						state.notificationsEnabled = value.boolean;
					else if (path == "state.volume" && value.type == UiValueType::Number)
						state.volume = value.number;
					else if (path == "state.brightness" && value.type == UiValueType::Number)
						state.brightness = value.number;
					else if (path == "state.particleCap" && value.type == UiValueType::Number)
						state.particleCap = value.number;
					else if (path == "state.temperature" && value.type == UiValueType::Number)
						state.temperature = value.number;
					else if (path == "state.unboundOffset" && value.type == UiValueType::Number)
						state.unboundOffset = value.number;
					else if (path == "state.qualityPreset" && value.type == UiValueType::String)
						state.qualityPreset = value.string;
					else if (path == "state.renderMode" && value.type == UiValueType::String)
						state.renderMode = value.string;
					else if (path == "state.playerName" && value.type == UiValueType::String)
						state.playerName = value.string;
					else if (path == "state.notes" && value.type == UiValueType::String)
						state.notes = value.string;

					if (path.starts_with("state."))
						refreshBoundState();
				});

			if (!authored->mount())
			{
				logError("UiControlsDemo: failed to mount authored UI.");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			applyDemoLayout(*authored, host);
			authoredReady = true;
			refreshBoundState();
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);
			if (authoredReady) authored->syncControlLayouts();
			host.update(window);
		}

		void destroy(Window& window) override
		{
			authored.reset();
			authoredReady = false;
			host.destroy(window);
		}
	};
}

void UiControlsDemo::run() const
{
	UiControlsDemoSurface surface;
	Window& window = ui.window("ORKA UI Controls Demo", settings.defaultWindowWidth,
		settings.defaultWindowHeight, true, true, WindowState::Windowed);
	window.addElement(surface);
	ui.run();
}
