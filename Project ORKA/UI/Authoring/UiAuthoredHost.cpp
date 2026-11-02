#include "UiAuthoredHost.hpp"

#include "../Theme/UiAppTheme.hpp"

namespace Ui
{
	UiAuthoredHost::UiAuthoredHost(UiHost& newHost)
		: host(newHost),
		widgetFactory(std::make_unique<UiWidgetFactory>(host.document(), host.eventRouter())),
		widgetBridgeImpl(std::make_unique<UiWidgetBridge>(*widgetFactory, propertyRegistry)),
		instantiator(host.document(), host.eventRouter(), propertyRegistry, commandRegistry),
		styleAdapter(host.document(), host.layoutEngine())
	{
		widgetFactory->attachLayoutEngine(host.layoutEngine());
		widgetFactory->attachTextMetrics(
			host.fontService().shapeProvider(),
			makeAppTextStyleProvider(host.appTheme()));
		host.registerWidgetFactory(widgetFactory.get());
		host.setAgentSurface({
			&commandRegistry,
			&propertyRegistry,
			widgetBridgeImpl.get(),
			widgetFactory.get(),
			[this]
			{
				syncFromProperties();
				syncControlLayouts();
			}
		});
		widgetBridgeImpl->setDomViewerPickHandler([this](const UiNodeHandle node)
			{
				domViewerPickedNode = node;
				host.setDomViewerHighlight(node);
			});
	}

	UiAuthoredHost::~UiAuthoredHost()
	{
		host.setDomViewerHighlight({});
		host.setAgentSurface({});
		host.registerWidgetFactory(nullptr);
	}

	Bool UiAuthoredHost::loadFromFiles(const Path& markupPath, const Path& stylePath)
	{
		diagnosticsLog.clear();
		const UiBundleLoadResult loaded = UiBundleCompiler::loadFile(markupPath, stylePath);
		if (!loaded.valid)
		{
			diagnosticsLog = loaded.diagnostics;
			activeBundle.reset();
			return false;
		}

		++bundleGeneration;
		UiCompiledBundle compiled = UiBundleCompiler::compile(
			bundleGeneration, loaded.sources, commandRegistry.snapshot());
		diagnosticsLog = compiled.diagnostics;
		if (!compiled.valid)
		{
			activeBundle.reset();
			return false;
		}

		activeBundle = std::make_shared<const UiCompiledBundle>(std::move(compiled));
		return true;
	}

	void UiAuthoredHost::ensureNodeLayout(
		const String& nodeId, const Function<void(UiLayoutStyle&)>& configure)
	{
		host.document().traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				if (node.authored.id != nodeId) return;
				UiLayoutStyle style = host.layoutEngine().style(handle);
				configure(style);
				host.layoutEngine().setStyle(handle, style);
			});
	}

	Vector<UiLogicalWindowEntry> UiAuthoredHost::logicalWindows() const
	{
		Vector<UiLogicalWindowEntry> entries;
		if (activeBundle && activeBundle->valid)
			buildLogicalWindowCatalog(activeBundle->blueprint, entries);
		return entries;
	}

	Bool UiAuthoredHost::mountLogicalWindow(const String& windowId)
	{
		if (!activeBundle || !activeBundle->valid) return false;
		Vector<UiLogicalWindowEntry> entries;
		buildLogicalWindowCatalog(activeBundle->blueprint, entries);
		const UiLogicalWindowEntry* entry = findLogicalWindow(entries, windowId);
		if (!entry)
		{
			diagnosticsLog.push_back({ DiagnosticSeverity::Error,
				"Logical window not found in bundle: " + windowId, {} });
			return false;
		}
		instantiator.setWidgetBridge(widgetBridgeImpl.get());
		mountedLogicalBlueprintIndex = entry->blueprintIndex;
		if (!instantiator.reconcileLogicalWindowSubtree(
			*activeBundle, entry->blueprintIndex, host.document().root()))
			return false;
		styleAdapter.apply(activeBundle->styles);
		host.setPaintStyleTokens([this](const UiNodeHandle handle)
			{ return styleAdapter.paintTokens(handle); });
		for (const UiDiagnostic& diagnostic : instantiator.diagnostics())
			diagnosticsLog.push_back(diagnostic);
		for (const UiDiagnostic& diagnostic : styleAdapter.diagnostics())
			diagnosticsLog.push_back(diagnostic);
		return true;
	}

	Bool UiAuthoredHost::mount(const UiNodeHandle parent)
	{
		if (!activeBundle || !activeBundle->valid) return false;
		mountedLogicalBlueprintIndex = maxUInt;
		instantiator.setWidgetBridge(widgetBridgeImpl.get());
		if (!instantiator.reconcile(*activeBundle, parent.isValid() ? parent : host.document().root()))
			return false;
		styleAdapter.apply(activeBundle->styles);
		host.setPaintStyleTokens([this](const UiNodeHandle handle)
			{ return styleAdapter.paintTokens(handle); });
		for (const UiDiagnostic& diagnostic : instantiator.diagnostics())
			diagnosticsLog.push_back(diagnostic);
		for (const UiDiagnostic& diagnostic : styleAdapter.diagnostics())
			diagnosticsLog.push_back(diagnostic);
		return true;
	}

	void UiAuthoredHost::syncFromProperties()
	{
		if (!activeBundle) return;
		instantiator.setWidgetBridge(widgetBridgeImpl.get());
		if (mountedLogicalBlueprintIndex != maxUInt)
		{
			(void)instantiator.reconcileLogicalWindowSubtree(
				*activeBundle, mountedLogicalBlueprintIndex, host.document().root());
		}
		else (void)instantiator.reconcile(*activeBundle, host.document().root());
		styleAdapter.apply(activeBundle->styles);
		host.setPaintStyleTokens([this](const UiNodeHandle handle)
			{ return styleAdapter.paintTokens(handle); });
		widgetBridgeImpl->syncDomViewers(
			host.document(), host.eventRouter().focusedNode(), domViewerPickedNode);
	}

	void UiAuthoredHost::syncControlLayouts()
	{
		widgetFactory->attachTextMetrics(
			host.fontService().shapeProvider(),
			makeAppTextStyleProvider(host.appTheme()));
		widgetBridgeImpl->syncControlLayouts();
	}
}
