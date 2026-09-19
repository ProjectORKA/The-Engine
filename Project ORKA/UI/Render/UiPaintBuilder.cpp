#include "UiPaintBuilder.hpp"

namespace Ui
{
	namespace
	{
		[[nodiscard]] Bool hasLayoutClip(const UiNode& node)
		{
			return node.computed.clipSize.x > 0 && node.computed.clipSize.y > 0;
		}
	}

	UiDirtyPaintBuilder::UiDirtyPaintBuilder(const UiRenderBudgets budgets)
		: limits(budgets)
	{
	}

	U64 UiDirtyPaintBuilder::key(const UiNodeHandle node)
	{
		return (static_cast<U64>(node.generation) << 32) | node.index;
	}

	Bool UiDirtyPaintBuilder::build(UiDocument& document, UiDisplayList& output,
		const UiNodePainter& painter, Vector<UiPaintBuildDiagnostic>& diagnostics)
	{
		diagnostics.clear();
		if (output.isFinalized())
		{
			diagnostics.push_back({ UiPaintBuildDiagnosticCode::OutputAlreadyFinalized, {},
				"Cannot replace a finalized display list." });
			return false;
		}
		if (!painter)
		{
			diagnostics.push_back({ UiPaintBuildDiagnosticCode::MissingPainter, {},
				"A node painter callback is required." });
			return false;
		}

		Vector<UiNodeHandle> traversal;
		std::set<U64> liveKeys;
		document.traverse([&](const UiNodeHandle handle, UiNode&)
			{
				traversal.push_back(handle);
				liveKeys.insert(key(handle));
			});

		for (auto iterator = cache.begin(); iterator != cache.end();)
		{
			if (!liveKeys.contains(iterator->first)) iterator = cache.erase(iterator);
			else ++iterator;
		}

		Bool succeeded = true;
		for (UInt order = 0; order < traversal.size(); ++order)
		{
			const UiNodeHandle handle = traversal[order];
			UiNode* node = document.get(handle);
			if (!node) continue;

			const U64 nodeKey = key(handle);
			const auto cached = cache.find(nodeKey);
			const Bool clipChanged = cached != cache.end() && (
				cached->second.clipPosition != node->computed.clipPosition ||
				cached->second.clipSize != node->computed.clipSize);
			const Bool needsPaint = hasDirty(node->dirtyFlags, DirtyFlag::Paint) ||
				cached == cache.end() || clipChanged;
			if (!needsPaint) continue;

			UiDisplayList fragment(limits);
			Bool painted = true;
			if (node->state.visible)
				painted = painter({ handle, *node, order }, fragment);
			if (!painted)
			{
				diagnostics.push_back({ UiPaintBuildDiagnosticCode::NodePainterFailed, handle,
					"Node painter rejected the node fragment." });
				succeeded = false;
				continue;
			}

			UiRenderPacket unusedPacket;
			Vector<UiRenderDiagnostic> renderDiagnostics;
			if (!UiDisplayListFinalizer::finalize(fragment, unusedPacket, renderDiagnostics))
			{
				diagnostics.push_back({ UiPaintBuildDiagnosticCode::InvalidNodeFragment, handle,
					"Node painter produced an invalid or unbalanced fragment.",
					std::move(renderDiagnostics) });
				succeeded = false;
				continue;
			}

			cache.insert_or_assign(nodeKey, CachedNodePaint{
				std::move(fragment),
				node->computed.clipPosition,
				node->computed.clipSize
			});
			node->dirtyFlags &= ~dirty(DirtyFlag::Paint);
		}
		if (!succeeded) return false;

		UiDisplayList assembled(limits);
		for (const UiNodeHandle handle : traversal)
		{
			const auto cached = cache.find(key(handle));
			if (cached == cache.end()) continue;
			const UiNode* node = document.get(handle);
			const Bool clipFragment = node && hasLayoutClip(*node);
			if (clipFragment && !assembled.pushClip(
				UiClipRect{node->computed.clipPosition, node->computed.clipSize}))
			{
				diagnostics.push_back({ UiPaintBuildDiagnosticCode::OutputBudgetExceeded, handle,
					"Layout clip stack exceeds the output display-list budget." });
				return false;
			}
			if (!assembled.append(cached->second.fragment))
			{
				diagnostics.push_back({ UiPaintBuildDiagnosticCode::OutputBudgetExceeded, handle,
					"Retained node fragments exceed the output display-list budget." });
				return false;
			}
			if (clipFragment && !assembled.popClip())
			{
				diagnostics.push_back({ UiPaintBuildDiagnosticCode::InvalidNodeFragment, handle,
					"Layout clip stack became unbalanced while assembling node fragments." });
				return false;
			}
		}

		output = std::move(assembled);
		return true;
	}

	void UiDirtyPaintBuilder::invalidateAll()
	{
		cache.clear();
	}

	void UiDirtyPaintBuilder::invalidate(const UiNodeHandle node)
	{
		cache.erase(key(node));
	}
}
