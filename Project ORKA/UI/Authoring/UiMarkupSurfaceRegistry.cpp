#include "UiMarkupSurfaceRegistry.hpp"

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

	UiMarkupSurfaceRegistry& UiMarkupSurfaceRegistry::global()
	{
		static UiMarkupSurfaceRegistry registry;
		static Bool builtInsRegistered = false;
		if (!builtInsRegistered)
		{
			registry.registerBuiltInTags();
			builtInsRegistered = true;
		}
		return registry;
	}

	void UiMarkupSurfaceRegistry::registerBuiltInTags()
	{
		registerInteractiveSurfaceTag("pong-game");
		registerInteractiveSurfaceTag("rts-world");
	}

	void UiMarkupSurfaceRegistry::registerInteractiveSurfaceTag(String tag)
	{
		tag = lower(std::move(tag));
		if (tag.empty()) return;
		for (const String& existing : tags)
			if (existing == tag) return;
		tags.push_back(std::move(tag));
	}

	Bool UiMarkupSurfaceRegistry::isInteractiveSurfaceTag(const String& tag) const
	{
		const String folded = lower(tag);
		for (const String& entry : tags)
			if (entry == folded) return true;
		return false;
	}
}
