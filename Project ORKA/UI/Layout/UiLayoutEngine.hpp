#pragma once

#include "UiLayoutTypes.hpp"
#include "../Core/UiDocument.hpp"

namespace Ui
{
	using UiIntrinsicMeasurer = Function<UiIntrinsicMeasure(const UiNode&, const UiConstraints&)>;

	class UiLayoutEngine
	{
	public:
		static constexpr SizeT maximumLayoutDepth = 128;
		static constexpr SizeT maximumDiagnostics = 128;
		static constexpr SizeT maximumGridTracks = 256;

		explicit UiLayoutEngine(UiDocument& document);

		void setStyle(UiNodeHandle node, const UiLayoutStyle& style);
		[[nodiscard]] const UiLayoutStyle& style(UiNodeHandle node) const;
		// Stored style plus flex classes and theme control defaults (Button, TextField, …).
		[[nodiscard]] UiLayoutStyle resolvedStyle(UiNodeHandle node) const;
		void clearStyle(UiNodeHandle node);

		void setIntrinsicMeasurer(UiNodeHandle node, UiIntrinsicMeasurer measurer);
		void setOwnedIntrinsicMeasurer(
			UiNodeHandle node, UiIntrinsicMeasurer measurer, U64 owner);
		void clearIntrinsicMeasurer(UiNodeHandle node);
		[[nodiscard]] Bool hasIntrinsicMeasurer(UiNodeHandle node) const;
		[[nodiscard]] U64 intrinsicMeasurerOwner(UiNodeHandle node) const;
		void invalidateContent(UiNodeHandle node);

		void layout();
		void layout(const UiConstraints& rootConstraints);
		[[nodiscard]] const Vector<UiLayoutDiagnostic>& diagnostics() const { return layoutDiagnostics; }
		void clearDiagnostics() { layoutDiagnostics.clear(); }

	private:
		struct NodeState
		{
			UiNodeHandle handle;
			UiLayoutStyle style;
			UiIntrinsicMeasurer intrinsicMeasurer;
			U64 intrinsicMeasurerOwner = 0;
			UInt styleRevision = 1;
			UInt contentRevision = 1;
			UiConstraints cachedConstraints;
			UiIntrinsicMeasure cachedMeasure;
			UInt cachedStyleRevision = 0;
			UInt cachedContentRevision = 0;
			Bool hasCachedMeasure = false;
			UiRect cachedRect;
			UiRect cachedClip;
			UiRect cachedInheritedClip;
			Vec2 cachedScrollOffset = Vec2(0);
			Float cachedVerticalGutter = 0;
			Float cachedHorizontalGutter = 0;
			UInt arrangedStyleRevision = 0;
			UInt arrangedContentRevision = 0;
			Bool hasCachedArrange = false;
		};

		struct GridPlacement
		{
			UiNodeHandle node;
			UInt column = 0;
			UInt row = 0;
			UInt columnSpan = 1;
			UInt rowSpan = 1;
		};

		UiDocument& document;
		Vector<NodeState> states;
		Vector<UiNodeHandle> measureStack;
		Vector<UiLayoutDiagnostic> layoutDiagnostics;
		UiLayoutStyle defaultStyle;
		UInt layoutRevision = 0;

		[[nodiscard]] NodeState& stateFor(UiNodeHandle node);
		[[nodiscard]] const NodeState* findState(UiNodeHandle node) const;
		[[nodiscard]] UiIntrinsicMeasure measureNode(UiNodeHandle node, const UiConstraints& constraints, SizeT depth);
		void arrangeNode(UiNodeHandle node, const UiRect& rect, const UiRect& inheritedClip, SizeT depth);
		void arrangeFlex(UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, SizeT depth);
		void arrangeGrid(UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, SizeT depth);
		void arrangeOverlay(UiNodeHandle node, const UiRect& contentRect, const UiRect& clip, SizeT depth);
		void arrangeAbsolute(UiNodeHandle node, const UiRect& containingRect, const UiRect& clip, SizeT depth);

		[[nodiscard]] UiIntrinsicMeasure measureFlex(UiNodeHandle node, const UiConstraints& constraints, SizeT depth);
		[[nodiscard]] UiIntrinsicMeasure measureGrid(UiNodeHandle node, const UiConstraints& constraints, SizeT depth);
		[[nodiscard]] UiIntrinsicMeasure measureOverlay(UiNodeHandle node, const UiConstraints& constraints, SizeT depth);
		[[nodiscard]] Vector<GridPlacement> placeGridItems(UiNodeHandle node, UInt& columns, UInt& rows);

		[[nodiscard]] Float resolve(const UiSizeSpec& spec, Float reference, Float intrinsic,
			UiNodeHandle node, Bool percentageReferenceIsBounded);
		void diagnose(UiLayoutDiagnosticCode code, UiNodeHandle node, String message);
		void clearLayoutDirty(UiNodeHandle node, DirtyFlag flag);
		void clearHiddenSubtree(UiNodeHandle node);
	};
}
