#include "UiTextEditModel.hpp"

namespace Ui
{
	namespace
	{
		struct DecodedCodepoint
		{
			U32 value = 0;
			SizeT bytes = 0;
		};

		DecodedCodepoint decode(const String& text, const SizeT offset)
		{
			if (offset >= text.size()) return {};
			const U8 first = static_cast<U8>(text[offset]);
			if (first < 0x80) return { first, 1 };
			SizeT length = first >= 0xf0 ? 4 : first >= 0xe0 ? 3 : first >= 0xc0 ? 2 : 0;
			if (!length || offset + length > text.size()) return {};
			U32 value = first & (0x7f >> length);
			for (SizeT index = 1; index < length; ++index)
			{
				const U8 next = static_cast<U8>(text[offset + index]);
				if ((next & 0xc0) != 0x80) return {};
				value = (value << 6) | (next & 0x3f);
			}
			if ((length == 2 && value < 0x80) || (length == 3 && value < 0x800) ||
				(length == 4 && value < 0x10000) || value > 0x10ffff ||
				(value >= 0xd800 && value <= 0xdfff)) return {};
			return { value, length };
		}

		Bool isExtend(const U32 value)
		{
			return (value >= 0x0300 && value <= 0x036f) ||
				(value >= 0x1ab0 && value <= 0x1aff) ||
				(value >= 0x1dc0 && value <= 0x1dff) ||
				(value >= 0x20d0 && value <= 0x20ff) ||
				(value >= 0xfe00 && value <= 0xfe0f) ||
				(value >= 0xfe20 && value <= 0xfe2f) ||
				(value >= 0xe0100 && value <= 0xe01ef) ||
				(value >= 0x1f3fb && value <= 0x1f3ff);
		}

		Bool isRegionalIndicator(const U32 value)
		{
			return value >= 0x1f1e6 && value <= 0x1f1ff;
		}

		Bool isWhitespace(const U32 value)
		{
			return value == ' ' || value == '\t' || value == '\n' || value == '\r' ||
				value == 0x00a0 || value == 0x1680 || (value >= 0x2000 && value <= 0x200a) ||
				value == 0x2028 || value == 0x2029 || value == 0x202f || value == 0x205f || value == 0x3000;
		}

	}

	UiTextEditModel::UiTextEditModel(UiTextEditLimits limits) : editLimits(limits)
	{
		editLimits.maximumUndoEntries = std::max(SizeT(1), editLimits.maximumUndoEntries);
	}

	String UiTextEditModel::displayText() const
	{
		if (!editMetadata.password) return value;
		String result;
		for (SizeT index = 0; index < codepointCount(value); ++index) result += "\xe2\x80\xa2";
		return result;
	}

	String UiTextEditModel::semanticValue() const
	{
		return editMetadata.password ? String{} : value;
	}

	void UiTextEditModel::setMetadata(const UiTextEditMetadata metadata)
	{
		editMetadata = metadata;
		if (!editMetadata.multiline)
		{
			const SizeT newline = value.find_first_of("\r\n");
			if (newline != String::npos) (void)setText(value.substr(0, newline));
		}
	}

	void UiTextEditModel::setValidationCallback(UiTextValidationCallback callback)
	{
		validator = std::move(callback);
	}

	void UiTextEditModel::setClusterBoundaryHook(UiTextClusterBoundaryHook hook)
	{
		clusterBoundaryProvider = std::move(hook);
		setSelection(selected.start, selected.end);
	}

	void UiTextEditModel::setWordMovementHook(UiTextBoundaryHook hook)
	{
		wordMovement = std::move(hook);
	}

	void UiTextEditModel::setVerticalMovementHook(UiTextVerticalMovementHook hook)
	{
		verticalMovement = std::move(hook);
	}

	Bool UiTextEditModel::setText(String text)
	{
		if (!accepts(text)) return false;
		// Keep caret/selection when a binding refresh republishes the same string.
		if (value == text) return true;
		value = std::move(text);
		selected = { value.size(), value.size() };
		composing = false;
		composition = {};
		clearHistory();
		return true;
	}

	void UiTextEditModel::setSelection(const SizeT anchor, const SizeT caretPosition)
	{
		selected = { snap(anchor), snap(caretPosition) };
	}

	void UiTextEditModel::clearHistory()
	{
		undoEntries.clear();
		redoEntries.clear();
	}

	Bool UiTextEditModel::insert(String utf8, const U64 timestampMicroseconds)
	{
		return replace(selected, std::move(utf8), timestampMicroseconds);
	}

	Bool UiTextEditModel::replace(const UiTextRange range, String utf8, const U64 timestampMicroseconds)
	{
		if (editMetadata.readOnly) return false;
		const EditKind kind = utf8.empty() ? EditKind::Delete : EditKind::Insert;
		return applyReplace(range, std::move(utf8), kind, timestampMicroseconds, true);
	}

	Bool UiTextEditModel::deleteBackward(const U64 timestampMicroseconds)
	{
		if (editMetadata.readOnly) return false;
		if (!selected.empty()) return applyReplace(selected, {}, EditKind::Delete, timestampMicroseconds, true);
		const Vector<SizeT> clusterOffsets = boundaries();
		const auto current = std::lower_bound(clusterOffsets.begin(), clusterOffsets.end(), selected.end);
		if (current == clusterOffsets.begin()) return false;
		return applyReplace({ *(current - 1), selected.end }, {}, EditKind::Delete, timestampMicroseconds, true);
	}

	Bool UiTextEditModel::deleteForward(const U64 timestampMicroseconds)
	{
		if (editMetadata.readOnly) return false;
		if (!selected.empty()) return applyReplace(selected, {}, EditKind::Delete, timestampMicroseconds, true);
		const Vector<SizeT> clusterOffsets = boundaries();
		const auto next = std::upper_bound(clusterOffsets.begin(), clusterOffsets.end(), selected.end);
		if (next == clusterOffsets.end()) return false;
		return applyReplace({ selected.end, *next }, {}, EditKind::Delete, timestampMicroseconds, true);
	}

	void UiTextEditModel::collapseOrExtend(const SizeT target, const Bool extendSelection)
	{
		if (extendSelection) selected.end = snap(target);
		else selected = { snap(target), snap(target) };
	}

	void UiTextEditModel::moveByCluster(const Int direction, const Bool extendSelection)
	{
		if (!extendSelection && !selected.empty())
		{
			const SizeT edge = direction < 0 ? selected.minimum() : selected.maximum();
			selected = { edge, edge };
			return;
		}
		const Vector<SizeT> clusterOffsets = boundaries();
		auto current = std::lower_bound(clusterOffsets.begin(), clusterOffsets.end(), selected.end);
		if (direction < 0 && current != clusterOffsets.begin()) --current;
		else if (direction > 0 && current + 1 != clusterOffsets.end()) ++current;
		collapseOrExtend(*current, extendSelection);
	}

	void UiTextEditModel::moveByWord(const Int direction, const Bool extendSelection)
	{
		if (wordMovement)
		{
			collapseOrExtend(wordMovement(value, selected.end, direction), extendSelection);
			return;
		}
		const Vector<SizeT> clusterOffsets = boundaries();
		auto position = std::lower_bound(clusterOffsets.begin(), clusterOffsets.end(), selected.end);
		if (direction < 0)
		{
			while (position != clusterOffsets.begin())
			{
				const auto previous = position - 1;
				const DecodedCodepoint point = decode(value, *previous);
				if (!isWhitespace(point.value)) break;
				position = previous;
			}
			while (position != clusterOffsets.begin())
			{
				const auto previous = position - 1;
				const DecodedCodepoint point = decode(value, *previous);
				if (isWhitespace(point.value)) break;
				position = previous;
			}
		}
		else
		{
			while (position + 1 != clusterOffsets.end())
			{
				const DecodedCodepoint point = decode(value, *position);
				++position;
				if (isWhitespace(point.value)) break;
			}
			while (position + 1 != clusterOffsets.end())
			{
				const DecodedCodepoint point = decode(value, *position);
				if (!isWhitespace(point.value)) break;
				++position;
			}
		}
		collapseOrExtend(*position, extendSelection);
	}

	void UiTextEditModel::moveVertically(const Int direction, const Bool extendSelection)
	{
		if (verticalMovement)
		{
			collapseOrExtend(verticalMovement(value, selected.end, direction), extendSelection);
			return;
		}
		if (!editMetadata.multiline) return;
		const SizeT lineStart = value.rfind('\n', selected.end ? selected.end - 1 : 0);
		const SizeT currentStart = lineStart == String::npos ? 0 : lineStart + 1;
		const SizeT column = selected.end - currentStart;
		if (direction < 0)
		{
			if (!currentStart) return;
			const SizeT previousEnd = currentStart - 1;
			const SizeT previousBreak = previousEnd ? value.rfind('\n', previousEnd - 1) : String::npos;
			const SizeT previousStart = previousBreak == String::npos ? 0 : previousBreak + 1;
			collapseOrExtend(std::min(previousStart + column, previousEnd), extendSelection);
		}
		else
		{
			const SizeT currentEnd = value.find('\n', selected.end);
			if (currentEnd == String::npos) return;
			const SizeT nextStart = currentEnd + 1;
			const SizeT nextBreak = value.find('\n', nextStart);
			const SizeT nextEnd = nextBreak == String::npos ? value.size() : nextBreak;
			collapseOrExtend(std::min(nextStart + column, nextEnd), extendSelection);
		}
	}

	void UiTextEditModel::moveToLineBoundary(const Bool end, const Bool extendSelection)
	{
		SizeT target = 0;
		if (end)
		{
			target = value.find('\n', selected.end);
			if (target == String::npos) target = value.size();
		}
		else
		{
			target = selected.end ? value.rfind('\n', selected.end - 1) : String::npos;
			target = target == String::npos ? 0 : target + 1;
		}
		collapseOrExtend(target, extendSelection);
	}

	void UiTextEditModel::compositionStart()
	{
		if (editMetadata.readOnly || composing) return;
		compositionBefore = { value, selected };
		composition = { selected.minimum(), selected.maximum() };
		composing = true;
	}

	Bool UiTextEditModel::compositionUpdate(String utf8)
	{
		if (!composing || editMetadata.readOnly) return false;
		const SizeT start = composition.minimum();
		if (!applyReplace(composition, std::move(utf8), EditKind::Insert, 0, false)) return false;
		composition = { start, selected.end };
		return true;
	}

	Bool UiTextEditModel::compositionCommit(String utf8, const U64 timestampMicroseconds)
	{
		if (!composing) return false;
		if (!utf8.empty() && !compositionUpdate(std::move(utf8))) return false;
		composing = false;
		composition = {};
		record(compositionBefore, EditKind::Insert, timestampMicroseconds);
		return true;
	}

	void UiTextEditModel::compositionCancel()
	{
		if (!composing) return;
		value = std::move(compositionBefore.text);
		selected = compositionBefore.selection;
		composition = {};
		composing = false;
	}

	Bool UiTextEditModel::execute(const UiTextEditCommand command, const String& argument,
		UiClipboard* clipboard, const Bool extendSelection, const U64 timestampMicroseconds)
	{
		switch (command)
		{
		case UiTextEditCommand::InsertText: return insert(argument, timestampMicroseconds);
		case UiTextEditCommand::DeleteBackward: return deleteBackward(timestampMicroseconds);
		case UiTextEditCommand::DeleteForward: return deleteForward(timestampMicroseconds);
		case UiTextEditCommand::MoveBackward: moveByCluster(-1, extendSelection); return true;
		case UiTextEditCommand::MoveForward: moveByCluster(1, extendSelection); return true;
		case UiTextEditCommand::MoveWordBackward: moveByWord(-1, extendSelection); return true;
		case UiTextEditCommand::MoveWordForward: moveByWord(1, extendSelection); return true;
		case UiTextEditCommand::MoveLineUp: moveVertically(-1, extendSelection); return true;
		case UiTextEditCommand::MoveLineDown: moveVertically(1, extendSelection); return true;
		case UiTextEditCommand::MoveLineStart: moveToLineBoundary(false, extendSelection); return true;
		case UiTextEditCommand::MoveLineEnd: moveToLineBoundary(true, extendSelection); return true;
		case UiTextEditCommand::SelectAll: selected = { 0, value.size() }; return true;
		case UiTextEditCommand::Copy:
			if (!clipboard || editMetadata.password || selected.empty()) return false;
			clipboard->writeText(value.substr(selected.minimum(), selected.maximum() - selected.minimum()));
			return true;
		case UiTextEditCommand::Cut:
			if (!clipboard || editMetadata.password || editMetadata.readOnly || selected.empty()) return false;
			clipboard->writeText(value.substr(selected.minimum(), selected.maximum() - selected.minimum()));
			return applyReplace(selected, {}, EditKind::Delete, timestampMicroseconds, true);
		case UiTextEditCommand::Paste:
			return clipboard && insert(clipboard->readText(), timestampMicroseconds);
		case UiTextEditCommand::Undo: return undo();
		case UiTextEditCommand::Redo: return redo();
		}
		return false;
	}

	Bool UiTextEditModel::undo()
	{
		if (editMetadata.readOnly || undoEntries.empty()) return false;
		HistoryEntry entry = std::move(undoEntries.back());
		undoEntries.pop_back();
		value = entry.before.text;
		selected = entry.before.selection;
		redoEntries.push_back(std::move(entry));
		return true;
	}

	Bool UiTextEditModel::redo()
	{
		if (editMetadata.readOnly || redoEntries.empty()) return false;
		HistoryEntry entry = std::move(redoEntries.back());
		redoEntries.pop_back();
		value = entry.after.text;
		selected = entry.after.selection;
		undoEntries.push_back(std::move(entry));
		return true;
	}

	Vector<SizeT> UiTextEditModel::clusterBoundaries(const String& utf8)
	{
		Vector<SizeT> result{ 0 };
		SizeT offset = 0;
		UInt regionalCount = 0;
		Bool previousWasJoiner = false;
		U32 previousValue = 0;
		while (offset < utf8.size())
		{
			const DecodedCodepoint point = decode(utf8, offset);
			if (!point.bytes) { ++offset; result.push_back(offset); continue; }
			const Bool regional = isRegionalIndicator(point.value);
			const Bool joinsPrevious = offset && (isExtend(point.value) || previousWasJoiner ||
				point.value == 0x200d || (previousValue == '\r' && point.value == '\n') ||
				(regional && regionalCount % 2 == 1));
			if (!joinsPrevious && result.back() != offset) result.push_back(offset);
			previousWasJoiner = point.value == 0x200d;
			previousValue = point.value;
			regionalCount = regional ? regionalCount + 1 : 0;
			offset += point.bytes;
		}
		if (result.back() != utf8.size()) result.push_back(utf8.size());
		return result;
	}

	SizeT UiTextEditModel::codepointCount(const String& utf8)
	{
		SizeT count = 0;
		for (SizeT offset = 0; offset < utf8.size();)
		{
			const DecodedCodepoint point = decode(utf8, offset);
			if (!point.bytes) return std::numeric_limits<SizeT>::max();
			offset += point.bytes;
			++count;
		}
		return count;
	}

	Bool UiTextEditModel::isValidUtf8(const String& utf8)
	{
		return codepointCount(utf8) != std::numeric_limits<SizeT>::max();
	}

	SizeT UiTextEditModel::snap(const SizeT byteOffset, const Int bias) const
	{
		const Vector<SizeT> clusterOffsets = boundaries();
		auto found = std::lower_bound(clusterOffsets.begin(), clusterOffsets.end(),
			std::min(byteOffset, value.size()));
		if (found == clusterOffsets.end()) return value.size();
		if (*found == byteOffset || bias > 0 || found == clusterOffsets.begin()) return *found;
		return *(found - 1);
	}

	Vector<SizeT> UiTextEditModel::boundaries() const
	{
		if (clusterBoundaryProvider)
		{
			Vector<SizeT> result = clusterBoundaryProvider(value);
			if (!result.empty() && result.front() == 0 && result.back() == value.size() &&
				std::is_sorted(result.begin(), result.end()) &&
				std::adjacent_find(result.begin(), result.end()) == result.end())
			{
				Bool valid = true;
				for (const SizeT offset : result)
				{
					if (offset > value.size() || (offset < value.size() &&
						(static_cast<U8>(value[offset]) & 0xc0) == 0x80))
					{
						valid = false;
						break;
					}
				}
				if (valid) return result;
			}
		}
		return clusterBoundaries(value);
	}

	Bool UiTextEditModel::accepts(const String& candidate) const
	{
		if (!isValidUtf8(candidate) || candidate.size() > editLimits.maximumBytes ||
			codepointCount(candidate) > editLimits.maximumCodepoints) return false;
		if (!editMetadata.multiline && candidate.find_first_of("\r\n") != String::npos) return false;
		return !validator || validator(candidate);
	}

	Bool UiTextEditModel::applyReplace(UiTextRange range, String utf8, const EditKind kind,
		const U64 timestampMicroseconds, const Bool recordHistory)
	{
		if (!isValidUtf8(utf8)) return false;
		range.start = snap(range.start);
		range.end = snap(range.end);
		const SizeT start = range.minimum();
		const SizeT end = range.maximum();
		String candidate = value.substr(0, start) + utf8 + value.substr(end);
		if (!accepts(candidate)) return false;
		const Snapshot before{ value, selected };
		value = std::move(candidate);
		selected = { start + utf8.size(), start + utf8.size() };
		if (recordHistory) record(before, kind, timestampMicroseconds);
		return true;
	}

	void UiTextEditModel::record(const Snapshot& before, const EditKind kind, U64 timestampMicroseconds)
	{
		if (before.text == value && before.selection.start == selected.start && before.selection.end == selected.end)
			return;
		if (!timestampMicroseconds) timestampMicroseconds = nowMicroseconds();
		const Snapshot after{ value, selected };
		if (!undoEntries.empty())
		{
			HistoryEntry& previous = undoEntries.back();
			const Bool adjacentInsert = kind == EditKind::Insert && previous.kind == kind &&
				previous.after.selection.empty() && before.selection.empty() &&
				previous.after.selection.end == before.selection.end;
			const Bool adjacentDelete = kind == EditKind::Delete && previous.kind == kind &&
				previous.after.selection.empty() && before.selection.empty() &&
				previous.after.selection.end == before.selection.end;
			if ((adjacentInsert || adjacentDelete) && timestampMicroseconds >= previous.timestamp &&
				timestampMicroseconds - previous.timestamp <= editLimits.coalescingWindowMicroseconds)
			{
				previous.after = after;
				previous.timestamp = timestampMicroseconds;
				redoEntries.clear();
				return;
			}
		}
		undoEntries.push_back({ before, after, kind, timestampMicroseconds });
		if (undoEntries.size() > editLimits.maximumUndoEntries) undoEntries.erase(undoEntries.begin());
		redoEntries.clear();
	}

	U64 UiTextEditModel::nowMicroseconds()
	{
		return static_cast<U64>(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count());
	}
}
