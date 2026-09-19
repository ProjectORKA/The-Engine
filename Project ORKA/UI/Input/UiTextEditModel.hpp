#pragma once

#include "../Core/UiTypes.hpp"

namespace Ui
{
	struct UiTextRange
	{
		SizeT start = 0;
		SizeT end = 0;

		[[nodiscard]] SizeT minimum() const { return std::min(start, end); }
		[[nodiscard]] SizeT maximum() const { return std::max(start, end); }
		[[nodiscard]] Bool empty() const { return start == end; }
	};

	enum class UiTextEditCommand : Byte
	{
		InsertText,
		DeleteBackward,
		DeleteForward,
		MoveBackward,
		MoveForward,
		MoveWordBackward,
		MoveWordForward,
		MoveLineUp,
		MoveLineDown,
		MoveLineStart,
		MoveLineEnd,
		SelectAll,
		Copy,
		Cut,
		Paste,
		Undo,
		Redo,
	};

	class UiClipboard
	{
	public:
		virtual ~UiClipboard() = default;
		[[nodiscard]] virtual String readText() const = 0;
		virtual void writeText(const String& text) = 0;
	};

	using UiTextValidationCallback = Function<Bool(const String&)>;
	using UiTextBoundaryHook = Function<SizeT(const String&, SizeT, Int)>;
	using UiTextClusterBoundaryHook = Function<Vector<SizeT>(const String&)>;
	using UiTextVerticalMovementHook = Function<SizeT(const String&, SizeT, Int)>;

	struct UiTextEditLimits
	{
		SizeT maximumBytes = 1024 * 1024;
		SizeT maximumCodepoints = 262144;
		SizeT maximumUndoEntries = 128;
		U64 coalescingWindowMicroseconds = 750000;
	};

	struct UiTextEditMetadata
	{
		Bool readOnly = false;
		Bool password = false;
		Bool multiline = false;
	};

	class UiTextEditModel
	{
	public:
		explicit UiTextEditModel(UiTextEditLimits limits = {});

		[[nodiscard]] const String& text() const { return value; }
		[[nodiscard]] String displayText() const;
		// Password values are intentionally omitted from accessibility/semantic output.
		[[nodiscard]] String semanticValue() const;
		[[nodiscard]] UiTextRange selection() const { return selected; }
		[[nodiscard]] SizeT caret() const { return selected.end; }
		[[nodiscard]] UiTextRange compositionRange() const { return composition; }
		[[nodiscard]] Bool isComposing() const { return composing; }
		[[nodiscard]] const UiTextEditMetadata& metadata() const { return editMetadata; }

		void setMetadata(UiTextEditMetadata metadata);
		void setValidationCallback(UiTextValidationCallback callback);
		void setClusterBoundaryHook(UiTextClusterBoundaryHook hook);
		void setWordMovementHook(UiTextBoundaryHook hook);
		void setVerticalMovementHook(UiTextVerticalMovementHook hook);
		[[nodiscard]] Bool setText(String text);
		void setSelection(SizeT anchor, SizeT caret);
		void clearHistory();

		[[nodiscard]] Bool insert(String utf8, U64 timestampMicroseconds = 0);
		[[nodiscard]] Bool replace(UiTextRange range, String utf8, U64 timestampMicroseconds = 0);
		[[nodiscard]] Bool deleteBackward(U64 timestampMicroseconds = 0);
		[[nodiscard]] Bool deleteForward(U64 timestampMicroseconds = 0);
		void moveByCluster(Int direction, Bool extendSelection = false);
		void moveByWord(Int direction, Bool extendSelection = false);
		void moveVertically(Int direction, Bool extendSelection = false);
		void moveToLineBoundary(Bool end, Bool extendSelection = false);

		void compositionStart();
		[[nodiscard]] Bool compositionUpdate(String utf8);
		[[nodiscard]] Bool compositionCommit(String utf8 = {}, U64 timestampMicroseconds = 0);
		void compositionCancel();

		[[nodiscard]] Bool execute(UiTextEditCommand command, const String& argument = {},
			UiClipboard* clipboard = nullptr, Bool extendSelection = false, U64 timestampMicroseconds = 0);
		[[nodiscard]] Bool undo();
		[[nodiscard]] Bool redo();

		[[nodiscard]] static Vector<SizeT> clusterBoundaries(const String& utf8);
		[[nodiscard]] static SizeT codepointCount(const String& utf8);
		[[nodiscard]] static Bool isValidUtf8(const String& utf8);

	private:
		enum class EditKind : Byte { None, Insert, Delete };
		struct Snapshot
		{
			String text;
			UiTextRange selection;
		};
		struct HistoryEntry
		{
			Snapshot before;
			Snapshot after;
			EditKind kind = EditKind::None;
			U64 timestamp = 0;
		};

		String value;
		UiTextRange selected;
		UiTextRange composition;
		Snapshot compositionBefore;
		UiTextEditLimits editLimits;
		UiTextEditMetadata editMetadata;
		UiTextValidationCallback validator;
		UiTextClusterBoundaryHook clusterBoundaryProvider;
		UiTextBoundaryHook wordMovement;
		UiTextVerticalMovementHook verticalMovement;
		Vector<HistoryEntry> undoEntries;
		Vector<HistoryEntry> redoEntries;
		Bool composing = false;

		[[nodiscard]] SizeT snap(SizeT byteOffset, Int bias = 0) const;
		[[nodiscard]] Vector<SizeT> boundaries() const;
		[[nodiscard]] Bool accepts(const String& candidate) const;
		[[nodiscard]] Bool applyReplace(UiTextRange range, String utf8, EditKind kind,
			U64 timestampMicroseconds, Bool recordHistory);
		void record(const Snapshot& before, EditKind kind, U64 timestampMicroseconds);
		void collapseOrExtend(SizeT target, Bool extendSelection);
		[[nodiscard]] static U64 nowMicroseconds();
	};
}
