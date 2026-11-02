#pragma once

#include "UiDisplayList.hpp"
#include "../Core/UiDocument.hpp"

namespace Ui
{
	struct UiNodePaintContext
	{
		UiNodeHandle handle;
		const UiNode& node;
		UInt traversalOrder = 0;
	};

	using UiNodePainter = Function<Bool(const UiNodePaintContext&, UiDisplayList&)>;

	enum class UiPaintBuildDiagnosticCode : Byte
	{
		MissingPainter,
		NodePainterFailed,
		InvalidNodeFragment,
		OutputBudgetExceeded,
		OutputAlreadyFinalized,
	};

	struct UiPaintBuildDiagnostic
	{
		UiPaintBuildDiagnosticCode code = UiPaintBuildDiagnosticCode::NodePainterFailed;
		UiNodeHandle node;
		String message;
		Vector<UiRenderDiagnostic> renderDiagnostics;
	};

	// Retains one independently valid paint fragment per node. Dirty nodes are
	// rebuilt; clean nodes are reused and emitted in document traversal order.
	class UiDirtyPaintBuilder
	{
	public:
		explicit UiDirtyPaintBuilder(UiRenderBudgets budgets = {});

		[[nodiscard]] Bool build(UiDocument& document, UiDisplayList& output,
			const UiNodePainter& painter, Vector<UiPaintBuildDiagnostic>& diagnostics);
		// Fills the fragment cache and writes document paint order. Call before assembleRange.
		[[nodiscard]] Bool rebuildCache(UiDocument& document, const UiNodePainter& painter,
			Vector<UiPaintBuildDiagnostic>& diagnostics, Vector<UiNodeHandle>& paintOrder);
		// Emits cached fragments for paintOrder[begin, end) in that order.
		[[nodiscard]] Bool assembleRange(UiDocument& document, UiDisplayList& output,
			Vector<UiPaintBuildDiagnostic>& diagnostics, const Vector<UiNodeHandle>& paintOrder,
			UInt begin, UInt end);
		void invalidateAll();
		void invalidate(UiNodeHandle node);
		[[nodiscard]] SizeT cachedNodeCount() const { return cache.size(); }

	private:
		struct CachedNodePaint
		{
			UiDisplayList fragment;
			Vec2 clipPosition = Vec2(0);
			Vec2 clipSize = Vec2(0);
		};

		[[nodiscard]] static U64 key(UiNodeHandle node);

		UiRenderBudgets limits;
		Map<U64, CachedNodePaint> cache;
	};
}
