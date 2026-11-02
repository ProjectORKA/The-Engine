#pragma once

#include "UiBundleManager.hpp"
#include "UiLogicalWindowCatalog.hpp"
#include "UiStyleApplicationAdapter.hpp"
#include "UiTemplateInstantiator.hpp"
#include "UiWidgetBridge.hpp"
#include "../UiHost.hpp"

namespace Ui
{
	// Loads .ui.html + .ui.scss bundles and mounts them on a UiHost.
	class UiAuthoredHost
	{
	public:
		explicit UiAuthoredHost(UiHost& host);
		~UiAuthoredHost();

		[[nodiscard]] UiPropertyRegistry& properties() { return propertyRegistry; }
		[[nodiscard]] UiCommandRegistry& commands() { return commandRegistry; }
		[[nodiscard]] UiWidgetBridge& widgetBridge() { return *widgetBridgeImpl; }
		[[nodiscard]] UiWidgetFactory& widgets() { return *widgetFactory; }
		[[nodiscard]] UiStyleApplicationAdapter& styles() { return styleAdapter; }
		[[nodiscard]] const UiCompiledBundle* bundle() const { return activeBundle.get(); }
		[[nodiscard]] const Vector<UiDiagnostic>& diagnostics() const { return diagnosticsLog; }

		[[nodiscard]] Bool loadFromFiles(const Path& markupPath, const Path& stylePath);
		[[nodiscard]] Bool mount(UiNodeHandle parent = {});
		[[nodiscard]] Bool mountLogicalWindow(const String& windowId = {});
		[[nodiscard]] Vector<UiLogicalWindowEntry> logicalWindows() const;
		void syncFromProperties();
		void syncControlLayouts();
		void ensureNodeLayout(const String& nodeId, const Function<void(UiLayoutStyle&)>& configure);

	private:
		UiHost& host;
		UiPropertyRegistry propertyRegistry;
		UiCommandRegistry commandRegistry;
		UniquePointer<UiWidgetFactory> widgetFactory;
		UniquePointer<UiWidgetBridge> widgetBridgeImpl;
		UiTemplateInstantiator instantiator;
		UiStyleApplicationAdapter styleAdapter;
		SharedPointer<const UiCompiledBundle> activeBundle;
		Vector<UiDiagnostic> diagnosticsLog;
		U64 bundleGeneration = 0;
		UInt mountedLogicalBlueprintIndex = maxUInt;
		UiNodeHandle domViewerPickedNode;
	};

	// Phase-1 app shell name; same type as UiAuthoredHost until a wider rename lands.
	using AppHost = UiAuthoredHost;
}
