#include "UiAppHelpers.hpp"

namespace Ui
{
	void applyScrollShift(
		UiLayoutEngine& layout, const UiNodeHandle contentNode, const Float scrollOffsetY)
	{
		UiLayoutStyle contentStyle = layout.style(contentNode);
		const Float shift = -scrollOffsetY;
		if (contentStyle.margin.top == shift) return;
		contentStyle.margin.top = shift;
		layout.setStyle(contentNode, contentStyle);
	}

	void setHitTestInvisibleRecursive(
		UiDocument& document, UiEventRouter& router, const UiNodeHandle root,
		const UiNodeHandle except)
	{
		const UiNode* node = document.get(root);
		if (!node) return;
		for (const UiNodeHandle child : node->childHandles())
		{
			if (child == except) continue;
			UiInteractionData interaction = router.interaction(child);
			interaction.hitTestVisible = false;
			router.setInteraction(child, interaction);
			setHitTestInvisibleRecursive(document, router, child, except);
		}
	}

	Float listContentHeight(const SizeT itemCount, const Float itemHeight, const Float gap)
	{
		if (itemCount == 0) return 0;
		return static_cast<Float>(itemCount) * itemHeight +
			static_cast<Float>(itemCount - 1) * gap;
	}

	Vec2 nodeGlobalOrigin(const UiDocument& document, UiNodeHandle handle)
	{
		Vec2 origin(0);
		while (handle.isValid())
		{
			const UiNode* node = document.get(handle);
			if (!node) break;
			origin += node->computed.position;
			handle = node->parentHandle();
		}
		return origin;
	}
}
