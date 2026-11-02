#include "UiLogicalWindowCatalog.hpp"

#include "../../Window.hpp"

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

		String literalAttribute(const UiMarkupNode& node, const String& name)
		{
			const String target = lower(name);
			for (const UiMarkupAttribute& attribute : node.attributes)
			{
				if (attribute.kind != UiAttributeKind::Literal) continue;
				if (lower(attribute.name) == target) return attribute.value;
			}
			return {};
		}
	}

	void buildLogicalWindowCatalog(
		const UiMarkupDocument& blueprint,
		Vector<UiLogicalWindowEntry>& outEntries)
	{
		outEntries.clear();
		for (UInt index = 0; index < blueprint.nodes.size(); ++index)
		{
			const UiMarkupNode& node = blueprint.nodes[index];
			if (node.kind != UiMarkupNodeKind::Element) continue;
			if (lower(node.name) != "window") continue;
			UiLogicalWindowEntry entry;
			entry.blueprintIndex = index;
			entry.id = node.id;
			entry.title = literalAttribute(node, "title");
			if (entry.title.empty()) entry.title = entry.id.empty() ? "Window" : entry.id;
			outEntries.push_back(std::move(entry));
		}
	}

	const UiLogicalWindowEntry* findLogicalWindow(
		const Vector<UiLogicalWindowEntry>& entries,
		const String& windowId)
	{
		if (entries.empty()) return nullptr;
		if (windowId.empty()) return &entries.front();
		for (const UiLogicalWindowEntry& entry : entries)
			if (entry.id == windowId) return &entry;
		return nullptr;
	}

	void syncPrimaryOsWindowTitle(Window& osWindow, const UiMarkupDocument& blueprint)
	{
		Vector<UiLogicalWindowEntry> entries;
		buildLogicalWindowCatalog(blueprint, entries);
		if (entries.size() != 1) return;
		if (entries.front().title.empty()) return;
		osWindow.setTitle(entries.front().title);
	}
}
