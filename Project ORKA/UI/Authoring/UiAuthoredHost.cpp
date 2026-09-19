#include "UiAuthoredHost.hpp"

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

	Bool UiAuthoredHost::mount(const UiNodeHandle parent)
	{
		if (!activeBundle || !activeBundle->valid) return false;
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
		(void)instantiator.reconcile(*activeBundle, host.document().root());
		styleAdapter.apply(activeBundle->styles);
		host.setPaintStyleTokens([this](const UiNodeHandle handle)
			{ return styleAdapter.paintTokens(handle); });
	}

	void UiAuthoredHost::syncControlLayouts()
	{
		widgetBridgeImpl->syncControlLayouts();
	}
}
