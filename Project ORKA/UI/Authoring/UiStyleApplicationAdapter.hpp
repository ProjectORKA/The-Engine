#pragma once

#include "UiBundleManager.hpp"
#include "../Core/UiDocument.hpp"
#include "../Layout/UiLayoutEngine.hpp"

namespace Ui
{
	struct UiPaintStyleTokens
	{
		Bool hasBackground = false;
		Color background = Color(0);
		Bool hasForeground = false;
		Color foreground = Color(0);
		Bool hasBorder = false;
		Color border = Color(0);
		Float borderWidth = 1;
		Float borderRadius = 0;
	};

	class UiStyleApplicationAdapter
	{
	public:
		UiStyleApplicationAdapter(UiDocument& document, UiLayoutEngine& layout);

		void apply(const UiCompiledStyleSheet& sheet, UiNodeHandle root = {});
		[[nodiscard]] const UiPaintStyleTokens* paintTokens(UiNodeHandle node) const;
		[[nodiscard]] const Vector<UiDiagnostic>& diagnostics() const { return styleDiagnostics; }

	private:
		UiDocument& document;
		UiLayoutEngine& layout;
		Map<UiNodeHandle, UiPaintStyleTokens> tokens;
		Vector<UiDiagnostic> styleDiagnostics;

		void diagnose(String message, const SourceSpan& span);
	};
}
