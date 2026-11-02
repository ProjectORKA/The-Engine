#pragma once

#include "../../OrkaTypes.hpp"

namespace Ui
{
	struct UiRegisteredComponent
	{
		String tag;
		Path markupPath;
	};

	// Maps authored component tag names to fragment markup files (under Data/UI).
	class UiComponentRegistry
	{
	public:
		[[nodiscard]] static UiComponentRegistry& global();

		void registerComponent(String tag, Path markupPath);
		[[nodiscard]] const UiRegisteredComponent* find(const String& tag) const;
		void registerBuiltInComponents(const Path& uiDirectory);
		void clear();

	private:
		Vector<UiRegisteredComponent> components;
	};
}
