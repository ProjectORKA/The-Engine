#pragma once

#include "../Core/UiDocument.hpp"

namespace Ui::Inspector
{
	struct UiDomViewerDump
	{
		String text;
		// One entry per output line; invalid handle for placeholder lines (e.g. omitted subtree).
		Vector<UiNodeHandle> lineHandles;
	};

	[[nodiscard]] UiDomViewerDump buildDomViewerDump(
		const UiDocument& document,
		UiNodeHandle excludeRoot = {},
		UiNodeHandle focused = {},
		UiNodeHandle selected = {},
		SizeT maximumLines = 4096);

	// Live retained-DOM tree dump for `<dom-viewer />`. Skips `excludeRoot` and its descendants.
	[[nodiscard]] String buildDomViewerText(
		const UiDocument& document,
		UiNodeHandle excludeRoot = {},
		UiNodeHandle focused = {},
		SizeT maximumLines = 4096);

	[[nodiscard]] SizeT domViewerLineIndexAtCaret(const String& dumpText, SizeT caretByte);
}
