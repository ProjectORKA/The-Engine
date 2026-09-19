#include "UiDocument.hpp"

namespace Ui
{
	void UiViewportState::set(const IVec2 newLogicalSize, const IVec2 newFramebufferSize)
	{
		const IVec2 safeLogical(std::max(newLogicalSize.x, 0), std::max(newLogicalSize.y, 0));
		const IVec2 safeFramebuffer(std::max(newFramebufferSize.x, 0), std::max(newFramebufferSize.y, 0));
		Vec2 newScale(1);
		if (safeLogical.x > 0) newScale.x = static_cast<Float>(safeFramebuffer.x) / static_cast<Float>(safeLogical.x);
		if (safeLogical.y > 0) newScale.y = static_cast<Float>(safeFramebuffer.y) / static_cast<Float>(safeLogical.y);

		if (safeLogical == logicalSize && safeFramebuffer == framebufferSize && newScale == contentScale) return;
		logicalSize = safeLogical;
		framebufferSize = safeFramebuffer;
		contentScale = newScale;
		dpi = 96.0f * std::max(contentScale.x, contentScale.y);
		++revision;
	}

	UiDocument::UiDocument()
	{
		rootHandle = allocateNode({}, UiAuthoredData{ .typeName = "root", .id = "root" });
	}

	Bool UiDocument::isValid(const UiNodeHandle handle) const
	{
		return handle.isValid() &&
			static_cast<SizeT>(handle.index) < slots.size() &&
			slots[handle.index].occupied &&
			slots[handle.index].generation == handle.generation;
	}

	UiNode* UiDocument::get(const UiNodeHandle handle)
	{
		return isValid(handle) ? &slots[handle.index].node : nullptr;
	}

	const UiNode* UiDocument::get(const UiNodeHandle handle) const
	{
		return isValid(handle) ? &slots[handle.index].node : nullptr;
	}

	UiNodeHandle UiDocument::allocateNode(const UiNodeHandle parent, UiAuthoredData authored)
	{
		if (liveNodeCount >= maxNodes || (parent.isValid() && !isValid(parent))) return {};

		UInt index = 0;
		if (!freeSlots.empty())
		{
			index = freeSlots.back();
			freeSlots.pop_back();
		}
		else
		{
			index = static_cast<UInt>(slots.size());
			slots.emplace_back();
		}

		Slot& slot = slots[index];
		slot.node = {};
		slot.node.parent = parent;
		slot.node.authored = std::move(authored);
		slot.occupied = true;
		++liveNodeCount;

		const UiNodeHandle handle{ index, slot.generation };
		if (UiNode* parentNode = get(parent)) parentNode->children.push_back(handle);
		return handle;
	}

	UiNodeHandle UiDocument::createNode(const UiNodeHandle parent, UiAuthoredData authored)
	{
		if (isTraversing() || !isValid(parent)) return {};
		const UiNodeHandle handle = allocateNode(parent, std::move(authored));
		if (handle.isValid()) markDirty(parent, DirtyFlag::Measure);
		return handle;
	}

	void UiDocument::destroySubtree(const UiNodeHandle handle)
	{
		UiNode* node = get(handle);
		if (!node) return;

		const Vector<UiNodeHandle> children = node->children;
		for (const UiNodeHandle child : children) destroySubtree(child);

		const UiNodeHandle parent = node->parent;
		if (UiNode* parentNode = get(parent))
		{
			const auto position = std::find(parentNode->children.begin(), parentNode->children.end(), handle);
			if (position != parentNode->children.end()) parentNode->children.erase(position);
		}

		Slot& slot = slots[handle.index];
		slot.node = {};
		slot.occupied = false;
		++slot.generation;
		if (slot.generation == 0) slot.generation = 1;
		freeSlots.push_back(handle.index);
		--liveNodeCount;
	}

	Bool UiDocument::destroyNode(const UiNodeHandle handle)
	{
		if (isTraversing() || !isValid(handle) || handle == rootHandle) return false;
		const UiNodeHandle parent = get(handle)->parent;
		destroySubtree(handle);
		if (isValid(parent)) markDirty(parent, DirtyFlag::Measure);
		return true;
	}

	Bool UiDocument::wouldCreateCycle(const UiNodeHandle handle, UiNodeHandle parent) const
	{
		while (isValid(parent))
		{
			if (parent == handle) return true;
			parent = get(parent)->parent;
		}
		return false;
	}

	Bool UiDocument::reparentNode(const UiNodeHandle handle, const UiNodeHandle newParent)
	{
		if (isTraversing() || !isValid(handle) || !isValid(newParent) ||
			handle == rootHandle || handle == newParent || wouldCreateCycle(handle, newParent)) return false;

		UiNode& node = *get(handle);
		const UiNodeHandle oldParent = node.parent;
		if (oldParent == newParent) return true;

		UiNode& oldParentNode = *get(oldParent);
		const auto position = std::find(oldParentNode.children.begin(), oldParentNode.children.end(), handle);
		if (position != oldParentNode.children.end()) oldParentNode.children.erase(position);
		get(newParent)->children.push_back(handle);
		node.parent = newParent;

		markDirty(oldParent, DirtyFlag::Measure);
		markDirty(newParent, DirtyFlag::Measure);
		return true;
	}

	Bool UiDocument::deferCreate(const UiNodeHandle parent, UiAuthoredData authored)
	{
		if (!isValid(parent) || mutations.size() >= maxDeferredMutations) return false;
		mutations.push_back({ MutationType::Create, {}, parent, std::move(authored) });
		return true;
	}

	Bool UiDocument::deferDestroy(const UiNodeHandle handle)
	{
		if (!isValid(handle) || handle == rootHandle || mutations.size() >= maxDeferredMutations) return false;
		mutations.push_back({ MutationType::Destroy, handle });
		return true;
	}

	Bool UiDocument::deferReparent(const UiNodeHandle handle, const UiNodeHandle newParent)
	{
		if (!isValid(handle) || !isValid(newParent) || mutations.size() >= maxDeferredMutations) return false;
		mutations.push_back({ MutationType::Reparent, handle, newParent });
		return true;
	}

	void UiDocument::applyDeferredMutations()
	{
		if (isTraversing()) return;
		Vector<Mutation> pending;
		pending.swap(mutations);
		for (Mutation& mutation : pending)
		{
			switch (mutation.type)
			{
			case MutationType::Create: (void)createNode(mutation.parent, std::move(mutation.authored)); break;
			case MutationType::Destroy: (void)destroyNode(mutation.node); break;
			case MutationType::Reparent: (void)reparentNode(mutation.node, mutation.parent); break;
			}
		}
	}

	void UiDocument::markSubtree(const UiNodeHandle handle, const DirtyFlags flags)
	{
		UiNode* node = get(handle);
		if (!node) return;
		node->dirtyFlags |= flags;
		for (const UiNodeHandle child : node->children) markSubtree(child, flags);
	}

	void UiDocument::markAncestors(UiNodeHandle handle, const DirtyFlags flags)
	{
		while (isValid(handle))
		{
			get(handle)->dirtyFlags |= flags;
			handle = get(handle)->parent;
		}
	}

	void UiDocument::markDirty(const UiNodeHandle handle, const DirtyFlag flag)
	{
		if (!isValid(handle) || flag == DirtyFlag::None) return;

		const DirtyFlags layoutDependents = dirty(DirtyFlag::Arrange) | dirty(DirtyFlag::Paint) | dirty(DirtyFlag::HitTest);
		switch (flag)
		{
		case DirtyFlag::Bindings:
			get(handle)->dirtyFlags |= dirty(flag);
			break;
		case DirtyFlag::Style:
			markSubtree(handle, dirty(DirtyFlag::Style) | dirty(DirtyFlag::Measure) | layoutDependents);
			markAncestors(get(handle)->parent, dirty(DirtyFlag::Measure) | layoutDependents);
			break;
		case DirtyFlag::Measure:
			get(handle)->dirtyFlags |= dirty(DirtyFlag::Measure);
			markAncestors(get(handle)->parent, dirty(DirtyFlag::Measure) | layoutDependents);
			markSubtree(handle, layoutDependents);
			break;
		case DirtyFlag::Arrange:
			markSubtree(handle, layoutDependents);
			// Descendants are positioned by their parent's arrange pass.
			markAncestors(get(handle)->parent, layoutDependents);
			break;
		case DirtyFlag::Paint:
			get(handle)->dirtyFlags |= dirty(DirtyFlag::Paint);
			break;
		case DirtyFlag::HitTest:
			markSubtree(handle, dirty(DirtyFlag::HitTest));
			break;
		case DirtyFlag::None:
			break;
		}
	}

	void UiDocument::traverseFrom(const UiNodeHandle handle, const Function<void(UiNodeHandle, UiNode&)>& visitor)
	{
		UiNode* node = get(handle);
		if (!node) return;
		visitor(handle, *node);
		const Vector<UiNodeHandle> children = node->children;
		for (const UiNodeHandle child : children) traverseFrom(child, visitor);
	}

	void UiDocument::traverse(const Function<void(UiNodeHandle, UiNode&)>& visitor)
	{
		if (!visitor) return;
		++traversalDepth;
		traverseFrom(rootHandle, visitor);
		--traversalDepth;
	}

	void UiDocument::processDirty(const DirtyFlags phaseMask)
	{
		// Pre-order plus fixed phase order keeps future processors deterministic.
		constexpr Array<DirtyFlag, 6> phases{
			DirtyFlag::Bindings, DirtyFlag::Style, DirtyFlag::Measure,
			DirtyFlag::Arrange, DirtyFlag::Paint, DirtyFlag::HitTest
		};
		for (const DirtyFlag phase : phases)
		{
			if ((phaseMask & dirty(phase)) == 0) continue;
			traverse([phase](UiNodeHandle, UiNode& node)
				{
					if (!hasDirty(node.dirtyFlags, phase)) return;
					if (phase == DirtyFlag::Style) ++node.computed.styleRevision;
					node.dirtyFlags &= ~dirty(phase);
				});
		}
	}
}
