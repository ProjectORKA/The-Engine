#pragma once

#include "../../OrkaTypes.hpp"

namespace Ui
{
	// Maps authored markup tag names to interactive surface widgets (see UiInteractiveSurface.hpp).
	class UiMarkupSurfaceRegistry
	{
	public:
		[[nodiscard]] static UiMarkupSurfaceRegistry& global();

		void registerInteractiveSurfaceTag(String tag);
		[[nodiscard]] Bool isInteractiveSurfaceTag(const String& tag) const;
		void registerBuiltInTags();

	private:
		UiMarkupSurfaceRegistry() = default;
		Vector<String> tags;
	};
}
