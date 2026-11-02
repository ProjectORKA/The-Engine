#pragma once

#include "UiBundleManager.hpp"
#include "UiComponentRegistry.hpp"

namespace Ui
{
	// Strips @import registration lines and expands registered component tags into fragment markup.
	String preprocessUiImports(
		String markup,
		const Path& uiDirectory,
		UiComponentRegistry& registry,
		Vector<UiDiagnostic>& diagnostics);

	Bool expandRegisteredComponents(
		UiMarkupDocument& document,
		const Path& uiDirectory,
		UiComponentRegistry& registry,
		const UiTemplateBudgets& budgets,
		Vector<UiDiagnostic>& diagnostics);
}
