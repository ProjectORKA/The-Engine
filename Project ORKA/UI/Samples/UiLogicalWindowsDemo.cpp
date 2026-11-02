#include "UiLogicalWindowsDemo.hpp"

#include "../../Debug.hpp"
#include "../../ResourceManager.hpp"
#include "../../Settings.hpp"
#include "../../UserInterface.hpp"
#include "../../Window.hpp"
#include "../Authoring/UiAuthoredHost.hpp"
#include "../Authoring/UiBundleManager.hpp"
#include "../Authoring/UiLogicalWindowCatalog.hpp"
#include "../Theme/UiAppTheme.hpp"

namespace
{
	using namespace Ui;

	struct LogicalWindowView final : UIElement
	{
		explicit LogicalWindowView(String logicalWindowId)
			: windowId(std::move(logicalWindowId))
		{
			addElement(host);
			growX(true);
			growY(true);
			background(Color(0, 0, 0, 0));
		}

		String windowId;
		UiAppTheme theme = UiAppTheme::light();
		UiHost host;
		UniquePointer<UiAuthoredHost> authored;
		Bool ready = false;

		void refreshBoundState()
		{
			if (!authored) return;
			authored->syncFromProperties();
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
			const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
			if (!authored->loadFromFiles(
				uiDirectory / "logical-windows-demo.ui.html",
				uiDirectory / "logical-windows-demo.ui.scss"))
			{
				logError("Logical windows demo: failed to load UI bundle.");
				return;
			}

			(void)authored->properties().set("state.windowALabel",
				UiValue::fromString("This content is mounted from <window id=\"logic-a\">."));
			(void)authored->properties().set("state.windowBLabel",
				UiValue::fromString("This content is mounted from <window id=\"logic-b\">."));

			if (!authored->mountLogicalWindow(windowId))
			{
				logError("Logical windows demo: failed to mount logical window \"" + windowId + "\".");
				for (const UiDiagnostic& diagnostic : authored->diagnostics())
					logError("  " + diagnostic.message);
				return;
			}

			authored->ensureNodeLayout(windowId, [](UiLayoutStyle& style)
				{
					style.width = UiSizeSpec::percent(1);
					style.height = UiSizeSpec::percent(1);
				});

			ready = true;
			refreshBoundState();
		}

		void update(Window& window) override
		{
			const IVec2 size = window.getSize();
			setWidth(size.x);
			setHeight(size.y);
			host.setWidth(size.x);
			host.setHeight(size.y);
			if (ready)
			{
				refreshBoundState();
				authored->syncControlLayouts();
			}
			host.update(window);
		}

		void destroy(Window& window) override
		{
			authored.reset();
			ready = false;
			host.destroy(window);
		}
	};
}

void UiLogicalWindowsDemo::run() const
{
	const Path uiDirectory = resourceManager.getOrkaDataPath() / "UI";
	const UiBundleLoadResult loaded = UiBundleCompiler::loadFile(
		uiDirectory / "logical-windows-demo.ui.html",
		uiDirectory / "logical-windows-demo.ui.scss");
	if (!loaded.valid)
	{
		logError("Logical windows demo: failed to load bundle for catalog.");
		return;
	}

	UiCommandRegistry commands;
	const UiCompiledBundle compiled = UiBundleCompiler::compile(1, loaded.sources, commands.snapshot());
	if (!compiled.valid)
	{
		logError("Logical windows demo: bundle compile failed.");
		return;
	}

	Vector<UiLogicalWindowEntry> entries;
	buildLogicalWindowCatalog(compiled.blueprint, entries);
	if (entries.empty())
	{
		logError("Logical windows demo: no <window> nodes in markup.");
		return;
	}

	Vector<UniquePointer<LogicalWindowView>> views;
	views.reserve(entries.size());
	for (const UiLogicalWindowEntry& entry : entries)
	{
		auto view = std::make_unique<LogicalWindowView>(entry.id);
		Window& osWindow = ui.window(
			entry.title,
			settings.defaultWindowWidth / 2,
			settings.defaultWindowHeight / 2,
			true,
			true,
			WindowState::Windowed,
			*view);
		views.push_back(std::move(view));
	}

	ui.run();
}
