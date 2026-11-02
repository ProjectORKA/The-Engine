#pragma once

#include "UiBundleManager.hpp"

struct Window;

namespace Ui
{
	struct UiLogicalWindowEntry
	{
		String id;
		String title;
		UInt blueprintIndex = 0;
	};

	void buildLogicalWindowCatalog(
		const UiMarkupDocument& blueprint,
		Vector<UiLogicalWindowEntry>& outEntries);

	[[nodiscard]] const UiLogicalWindowEntry* findLogicalWindow(
		const Vector<UiLogicalWindowEntry>& entries,
		const String& windowId);

	// When the bundle declares exactly one <window title="…">, apply it to the OS window.
	void syncPrimaryOsWindowTitle(Window& osWindow, const UiMarkupDocument& blueprint);
}
