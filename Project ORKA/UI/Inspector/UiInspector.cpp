#include "UiInspector.hpp"

#include <cmath>

namespace Ui::Inspector
{
	namespace
	{
		Vector<String> inferDirtyReasons(const DirtyFlags flags)
		{
			Vector<String> result;
			if (hasDirty(flags, DirtyFlag::Bindings)) result.push_back("bindings");
			if (hasDirty(flags, DirtyFlag::Style)) result.push_back("style");
			if (hasDirty(flags, DirtyFlag::Measure)) result.push_back("measure");
			if (hasDirty(flags, DirtyFlag::Arrange)) result.push_back("arrange");
			if (hasDirty(flags, DirtyFlag::Paint)) result.push_back("paint");
			if (hasDirty(flags, DirtyFlag::HitTest)) result.push_back("hit-test");
			return result;
		}

		Bool finiteRect(const Vec2 position, const Vec2 size)
		{
			return std::isfinite(position.x) && std::isfinite(position.y) &&
				std::isfinite(size.x) && std::isfinite(size.y) &&
				size.x >= 0 && size.y >= 0;
		}

		Bool addOutline(UiDisplayList& output, const Vec2 position, const Vec2 size,
			const Float width, const UiLinearColor color)
		{
			if (size.x <= 0 || size.y <= 0 || width <= 0) return true;
			const Float horizontal = std::min(width, size.y);
			const Float vertical = std::min(width, size.x);
			return output.addSolidQuad({ position, Vec2(size.x, horizontal) }, color) &&
				output.addSolidQuad({
					Vec2(position.x, position.y + size.y - horizontal),
					Vec2(size.x, horizontal) }, color) &&
				output.addSolidQuad({
					Vec2(position.x, position.y + horizontal),
					Vec2(vertical, std::max(0.0f, size.y - horizontal * 2)) }, color) &&
				output.addSolidQuad({
					Vec2(position.x + size.x - vertical, position.y + horizontal),
					Vec2(vertical, std::max(0.0f, size.y - horizontal * 2)) }, color);
		}
	}

	void UiInspectorModel::setHooks(UiInspectorHooks hooks)
	{
		inspectorHooks = std::move(hooks);
	}

	UiInspectorSnapshot UiInspectorModel::capture(
		UiDocument& document,
		const UiNodeHandle focused,
		const UiRenderPacket* packet,
		const Vector<UiRenderDiagnostic>* diagnostics)
	{
		UiInspectorSnapshot snapshot;
		snapshot.revision = nextRevision++;
		snapshot.focused = focused;
		UInt depth = 0;
		Vector<UInt> childEnds;
		document.traverse([&](const UiNodeHandle handle, UiNode& node)
			{
				while (!childEnds.empty() && childEnds.back() == 0)
				{
					childEnds.pop_back();
					if (depth) --depth;
					if (!childEnds.empty()) --childEnds.back();
				}
				UiInspectorNode inspected;
				inspected.handle = handle;
				inspected.parent = node.parentHandle();
				inspected.depth = depth;
				inspected.authored = node.authored;
				inspected.state = node.state;
				inspected.computed = node.computed;
				inspected.dirtyFlags = node.dirtyFlags;
				inspected.dirtyReasons = inspectorHooks.dirtyReasons
					? inspectorHooks.dirtyReasons(handle)
					: inferDirtyReasons(node.dirtyFlags);
				if (inspectorHooks.authoredProperties)
					inspected.authoredProperties =
						inspectorHooks.authoredProperties(handle);
				if (inspectorHooks.computedProperties)
					inspected.computedProperties =
						inspectorHooks.computedProperties(handle);
				if (inspectorHooks.bindings)
					inspected.bindings = inspectorHooks.bindings(handle);
				if (inspectorHooks.styleSources)
					inspected.styleSources = inspectorHooks.styleSources(handle);
				snapshot.nodes.push_back(std::move(inspected));
				if (!node.childHandles().empty())
				{
					childEnds.push_back(static_cast<UInt>(node.childHandles().size()));
					++depth;
				}
				else if (!childEnds.empty()) --childEnds.back();
			});
		if (packet)
		{
			snapshot.render.commandCount = packet->commands().size();
			snapshot.render.instanceCount = packet->instances().size();
			snapshot.render.meshVertexCount = packet->meshVertices().size();
			snapshot.render.meshIndexCount = packet->meshIndices().size();
			snapshot.render.resourceCount = packet->resources().size();
			snapshot.render.batchCount = packet->batches().size();
		}
		if (diagnostics) snapshot.render.diagnostics = *diagnostics;
		if (inspectorHooks.atlasStats) snapshot.atlas = inspectorHooks.atlasStats();
		if (inspectorHooks.timings) snapshot.timings = inspectorHooks.timings();
		return snapshot;
	}

	UiInspectorPaintResult UiInspectorOverlayPainter::paint(
		const UiInspectorSnapshot& snapshot,
		UiDisplayList& output,
		const UiInspectorOverlayStyle& style,
		const UiNodeHandle selected)
	{
		UiInspectorPaintResult result;
		if (output.isFinalized())
		{
			result.valid = false;
			result.diagnostics.push_back("Inspector output display list is already finalized.");
			return result;
		}
		for (const UiInspectorNode& node : snapshot.nodes)
		{
			if (selected.isValid() && node.handle != selected) continue;
			const Vec2 position = node.computed.position;
			const Vec2 size = node.computed.size;
			if (!finiteRect(position, size))
			{
				result.valid = false;
				result.diagnostics.push_back(
					"Skipped non-finite layout box at node " +
					std::to_string(node.handle.index) + ".");
				continue;
			}
			UiLinearColor layoutColor = style.layoutColor;
			if (style.showDirty && node.dirtyFlags != 0) layoutColor = style.dirtyColor;
			if (style.showFocus && (node.handle == snapshot.focused || node.state.focused))
				layoutColor = style.focusColor;
			if (style.showLayout &&
				!addOutline(output, position, size, style.lineWidth, layoutColor))
			{
				result.valid = false;
				result.diagnostics.push_back("Display-list budget rejected a layout outline.");
				break;
			}
			if (style.showClip &&
				!addOutline(output, node.computed.clipPosition,
					node.computed.clipSize, style.lineWidth, style.clipColor))
			{
				result.valid = false;
				result.diagnostics.push_back("Display-list budget rejected a clip outline.");
				break;
			}
			++result.boxesPainted;
		}
		return result;
	}
}
