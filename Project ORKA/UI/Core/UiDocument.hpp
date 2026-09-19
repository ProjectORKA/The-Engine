#pragma once

#include "UiTypes.hpp"

namespace Ui
{
	struct UiNode
	{
		UiAuthoredData authored;
		UiStateData state;
		UiComputedData computed;
		DirtyFlags dirtyFlags = dirty(DirtyFlag::Bindings) |
			dirty(DirtyFlag::Style) |
			dirty(DirtyFlag::Measure) |
			dirty(DirtyFlag::Arrange) |
			dirty(DirtyFlag::Paint) |
			dirty(DirtyFlag::HitTest);

		[[nodiscard]] UiNodeHandle parentHandle() const { return parent; }
		[[nodiscard]] const Vector<UiNodeHandle>& childHandles() const { return children; }

	private:
		friend class UiDocument;
		UiNodeHandle parent;
		Vector<UiNodeHandle> children;
	};

	class UiDocument
	{
	public:
		static constexpr SizeT maxNodes = 65536;
		static constexpr SizeT maxDeferredMutations = 4096;

		UiDocument();

		[[nodiscard]] UiNodeHandle root() const { return rootHandle; }
		[[nodiscard]] Bool isValid(UiNodeHandle handle) const;
		[[nodiscard]] UiNode* get(UiNodeHandle handle);
		[[nodiscard]] const UiNode* get(UiNodeHandle handle) const;
		[[nodiscard]] SizeT nodeCount() const { return liveNodeCount; }
		[[nodiscard]] Bool isTraversing() const { return traversalDepth != 0; }

		// Immediate mutation is rejected during traversal. Traversal callbacks
		// should use the deferred operations below.
		[[nodiscard]] UiNodeHandle createNode(UiNodeHandle parent, UiAuthoredData authored = {});
		[[nodiscard]] Bool destroyNode(UiNodeHandle handle);
		[[nodiscard]] Bool reparentNode(UiNodeHandle handle, UiNodeHandle newParent);

		[[nodiscard]] Bool deferCreate(UiNodeHandle parent, UiAuthoredData authored = {});
		[[nodiscard]] Bool deferDestroy(UiNodeHandle handle);
		[[nodiscard]] Bool deferReparent(UiNodeHandle handle, UiNodeHandle newParent);
		void applyDeferredMutations();

		void markDirty(UiNodeHandle handle, DirtyFlag flag);
		void processDirty(DirtyFlags phaseMask = ~DirtyFlags(0));
		void traverse(const Function<void(UiNodeHandle, UiNode&)>& visitor);

		UiViewportState viewport;

	private:
		struct Slot
		{
			UiNode node;
			UInt generation = 1;
			Bool occupied = false;
		};

		enum class MutationType : Byte
		{
			Create,
			Destroy,
			Reparent,
		};

		struct Mutation
		{
			MutationType type = MutationType::Create;
			UiNodeHandle node;
			UiNodeHandle parent;
			UiAuthoredData authored;
		};

		Vector<Slot> slots;
		Vector<UInt> freeSlots;
		Vector<Mutation> mutations;
		UiNodeHandle rootHandle;
		SizeT liveNodeCount = 0;
		UInt traversalDepth = 0;

		[[nodiscard]] UiNodeHandle allocateNode(UiNodeHandle parent, UiAuthoredData authored);
		void destroySubtree(UiNodeHandle handle);
		void markSubtree(UiNodeHandle handle, DirtyFlags flags);
		void markAncestors(UiNodeHandle handle, DirtyFlags flags);
		void traverseFrom(UiNodeHandle handle, const Function<void(UiNodeHandle, UiNode&)>& visitor);
		[[nodiscard]] Bool wouldCreateCycle(UiNodeHandle handle, UiNodeHandle parent) const;
	};
}
