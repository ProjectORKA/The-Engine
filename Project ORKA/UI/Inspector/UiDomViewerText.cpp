#include "UiDomViewerText.hpp"

#include <algorithm>

namespace Ui::Inspector
{
	namespace
	{
		String indent(const UInt depth)
		{
			return String(depth * 2, ' ');
		}

		String joinClasses(const Vector<String>& classes)
		{
			String joined;
			for (const String& item : classes)
			{
				if (item.empty()) continue;
				if (!joined.empty()) joined += ' ';
				joined += item;
			}
			return joined;
		}

		String truncateText(String text, const SizeT maximum)
		{
			text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
			for (Char& character : text)
				if (character == '\n') character = ' ';
			if (text.size() <= maximum) return text;
			text.resize(maximum);
			text += "...";
			return text;
		}
	}

	UiDomViewerDump buildDomViewerDump(
		const UiDocument& document,
		const UiNodeHandle excludeRoot,
		const UiNodeHandle focused,
		const UiNodeHandle selected,
		const SizeT maximumLines)
	{
		UiDomViewerDump dump;
		if (!document.root().isValid()) return dump;

		SizeT lineCount = 0;

		Function<void(UiNodeHandle, UInt)> visit = [&](const UiNodeHandle handle, const UInt depth)
		{
			if (lineCount >= maximumLines) return;
			if (!document.isValid(handle)) return;
			const UiNode* node = document.get(handle);
			if (!node) return;

			String line = indent(depth);
			if (selected.isValid() && handle == selected) line += "> ";
			line += node->authored.typeName;
			if (!node->authored.id.empty()) line += " id=\"" + node->authored.id + "\"";
			const String classes = joinClasses(node->authored.classes);
			if (!classes.empty()) line += " class=\"" + classes + "\"";
			if (focused.isValid() && handle == focused) line += " [focused]";
			if (node->state.focused) line += " focused";
			if (node->state.hovered) line += " hovered";
			if (node->state.active) line += " active";
			if (!node->state.visible) line += " hidden";
			if (!node->state.enabled) line += " disabled";
			line += " " + std::to_string(node->computed.size.x) + "x" +
				std::to_string(node->computed.size.y);
			if (!node->authored.text.empty())
				line += " text=\"" + truncateText(node->authored.text, 48) + "\"";
			dump.text += line + "\n";
			dump.lineHandles.push_back(handle);
			++lineCount;

			for (const UiNodeHandle child : node->childHandles())
			{
				if (excludeRoot.isValid() && child == excludeRoot)
				{
					dump.text += indent(depth + 1) + "(dom-viewer subtree omitted)\n";
					dump.lineHandles.push_back({});
					++lineCount;
					continue;
				}
				visit(child, depth + 1);
			}
		};

		visit(document.root(), 0);
		if (lineCount >= maximumLines) dump.text += "...(truncated)\n";
		return dump;
	}

	String buildDomViewerText(
		const UiDocument& document,
		const UiNodeHandle excludeRoot,
		const UiNodeHandle focused,
		const SizeT maximumLines)
	{
		return buildDomViewerDump(document, excludeRoot, focused, {}, maximumLines).text;
	}

	SizeT domViewerLineIndexAtCaret(const String& dumpText, const SizeT caretByte)
	{
		SizeT line = 0;
		const SizeT limit = std::min(caretByte, dumpText.size());
		for (SizeT index = 0; index < limit; ++index)
			if (dumpText[index] == '\n') ++line;
		return line;
	}
}
