#include "UiComponentRegistry.hpp"

#include <cctype>

namespace Ui
{
	namespace
	{
		String lower(String value)
		{
			for (Char& character : value)
				character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
			return value;
		}
	}

	UiComponentRegistry& UiComponentRegistry::global()
	{
		static UiComponentRegistry registry;
		return registry;
	}

	void UiComponentRegistry::clear()
	{
		components.clear();
	}

	void UiComponentRegistry::registerBuiltInComponents(const Path& uiDirectory)
	{
		registerComponent("pong-score", uiDirectory / "pong-score.ui.html");
		registerComponent("pong-game-view", uiDirectory / "pong-game-view.ui.html");
		registerComponent("pong-playfield", uiDirectory / "pong-playfield.ui.html");
	}

	void UiComponentRegistry::registerComponent(String tag, Path markupPath)
	{
		tag = lower(std::move(tag));
		if (tag.empty()) return;
		for (UiRegisteredComponent& entry : components)
		{
			if (entry.tag == tag)
			{
				entry.markupPath = std::move(markupPath);
				return;
			}
		}
		components.push_back({ std::move(tag), std::move(markupPath) });
	}

	const UiRegisteredComponent* UiComponentRegistry::find(const String& tag) const
	{
		const String folded = lower(tag);
		for (const UiRegisteredComponent& entry : components)
			if (entry.tag == folded) return &entry;
		return nullptr;
	}
}
