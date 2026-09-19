#pragma once

#include "../Core/UiDocument.hpp"
#include "../Render/UiDisplayList.hpp"
#include "../Text/UiGlyphCache.hpp"

namespace Ui::Inspector
{
	struct UiInspectorProperty
	{
		String name;
		String value;
		String source;
	};

	struct UiInspectorTiming
	{
		String name;
		U64 microseconds = 0;
	};

	struct UiInspectorNode
	{
		UiNodeHandle handle;
		UiNodeHandle parent;
		UInt depth = 0;
		UiAuthoredData authored;
		UiStateData state;
		UiComputedData computed;
		DirtyFlags dirtyFlags = 0;
		Vector<String> dirtyReasons;
		Vector<UiInspectorProperty> authoredProperties;
		Vector<UiInspectorProperty> computedProperties;
		Vector<UiInspectorProperty> bindings;
		Vector<UiInspectorProperty> styleSources;
	};

	struct UiInspectorRenderData
	{
		SizeT commandCount = 0;
		SizeT instanceCount = 0;
		SizeT meshVertexCount = 0;
		SizeT meshIndexCount = 0;
		SizeT resourceCount = 0;
		SizeT batchCount = 0;
		Vector<UiRenderDiagnostic> diagnostics;
	};

	struct UiInspectorSnapshot
	{
		U64 revision = 0;
		UiNodeHandle focused;
		Vector<UiInspectorNode> nodes;
		UiInspectorRenderData render;
		UiGlyphCacheStats atlas;
		Vector<UiInspectorTiming> timings;
	};

	// Every hook is optional. Unset hooks leave their section empty instead of
	// reaching into style, binding, or atlas systems the inspector cannot own.
	struct UiInspectorHooks
	{
		Function<Vector<String>(UiNodeHandle)> dirtyReasons;
		Function<Vector<UiInspectorProperty>(UiNodeHandle)> authoredProperties;
		Function<Vector<UiInspectorProperty>(UiNodeHandle)> computedProperties;
		Function<Vector<UiInspectorProperty>(UiNodeHandle)> bindings;
		Function<Vector<UiInspectorProperty>(UiNodeHandle)> styleSources;
		Function<UiGlyphCacheStats()> atlasStats;
		Function<Vector<UiInspectorTiming>()> timings;
	};

	class UiInspectorModel
	{
	public:
		void setHooks(UiInspectorHooks hooks);
		[[nodiscard]] UiInspectorSnapshot capture(
			UiDocument& document,
			UiNodeHandle focused = {},
			const UiRenderPacket* packet = nullptr,
			const Vector<UiRenderDiagnostic>* diagnostics = nullptr);

	private:
		UiInspectorHooks inspectorHooks;
		U64 nextRevision = 1;
	};

	struct UiInspectorOverlayStyle
	{
		UiLinearColor layoutColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.15f, 0.65f, 1.0f, 0.9f));
		UiLinearColor clipColor =
			UiLinearColor::fromStraightSrgb(Vec4(1.0f, 0.45f, 0.15f, 0.75f));
		UiLinearColor dirtyColor =
			UiLinearColor::fromStraightSrgb(Vec4(1.0f, 0.2f, 0.25f, 0.9f));
		UiLinearColor focusColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.3f, 1.0f, 0.45f, 1.0f));
		Float lineWidth = 1.0f;
		Bool showLayout = true;
		Bool showClip = true;
		Bool showDirty = true;
		Bool showFocus = true;
	};

	struct UiInspectorPaintResult
	{
		Bool valid = true;
		SizeT boxesPainted = 0;
		Vector<String> diagnostics;
	};

	class UiInspectorOverlayPainter
	{
	public:
		[[nodiscard]] static UiInspectorPaintResult paint(
			const UiInspectorSnapshot& snapshot,
			UiDisplayList& output,
			const UiInspectorOverlayStyle& style = {},
			UiNodeHandle selected = {});
	};
}
