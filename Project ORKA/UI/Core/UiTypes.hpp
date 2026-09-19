#pragma once

#include "../../OrkaTypes.hpp"

namespace Ui
{
	struct UiNodeHandle
	{
		static constexpr UInt invalidIndex = maxUInt;

		UInt index = invalidIndex;
		UInt generation = 0;

		[[nodiscard]] constexpr Bool isValid() const
		{
			return index != invalidIndex && generation != 0;
		}

		constexpr auto operator<=>(const UiNodeHandle&) const = default;
	};

	enum class DirtyFlag : UInt
	{
		None     = 0,
		Bindings = 1 << 0,
		Style    = 1 << 1,
		Measure  = 1 << 2,
		Arrange  = 1 << 3,
		Paint    = 1 << 4,
		HitTest  = 1 << 5,
	};

	using DirtyFlags = UInt;

	[[nodiscard]] constexpr DirtyFlags dirty(const DirtyFlag flag)
	{
		return static_cast<DirtyFlags>(flag);
	}

	[[nodiscard]] constexpr Bool hasDirty(const DirtyFlags flags, const DirtyFlag flag)
	{
		return (flags & dirty(flag)) != 0;
	}

	struct UiAuthoredData
	{
		String typeName;
		String styleType;
		String id;
		String text;
		Vector<String> classes;
	};

	enum class UiSemanticRole : Byte
	{
		None,
		Application,
		Button,
		CheckBox,
		Dialog,
		Image,
		Link,
		List,
		ListItem,
		Menu,
		MenuItem,
		ProgressBar,
		Radio,
		Slider,
		Text,
		TextBox,
	};

	struct UiSemanticData
	{
		UiSemanticRole role = UiSemanticRole::None;
		String label;
		String value;
		Bool selected = false;
		Bool checked = false;
	};

	struct UiStateData
	{
		Bool enabled = true;
		Bool visible = true;
		Bool hovered = false;
		Bool active = false;
		Bool focused = false;
		UiSemanticData semantics;
	};

	// Scroll values published by the widget layer. Layout never writes these, so
	// painters can read the authoritative offset without owning scroll state.
	struct UiScrollPresentation
	{
		Vec2 offset = Vec2(0);
		Vec2 viewport = Vec2(0);
		Vec2 content = Vec2(0);
		// Width/height reserved inside the padded content box for scrollbar tracks.
		Float verticalGutter = 0;
		Float horizontalGutter = 0;
		Bool showVerticalScrollbar = false;
		Bool showHorizontalScrollbar = false;
	};

	struct UiComputedData
	{
		Vec2 measuredSize = Vec2(0);
		Vec2 position = Vec2(0);
		Vec2 size = Vec2(0);
		Vec2 contentPosition = Vec2(0);
		Vec2 contentSize = Vec2(0);
		Vec2 clipPosition = Vec2(0);
		Vec2 clipSize = Vec2(0);
		// Maps the arranged viewport-space box into its painted position.
		// Input applies the inverse for exact CPU hit testing.
		glm::mat3 hitTestTransform = glm::mat3(1);
		Vec2 scrollExtent = Vec2(0);
		UiScrollPresentation scroll;
		Float baseline = -1;
		Float textFontSize = 0;
		Float textLineHeight = 0;
		UInt textLineCount = 0;
		UInt styleRevision = 0;
		UInt layoutRevision = 0;
	};

	struct UiViewportState
	{
		IVec2 logicalSize = IVec2(0);
		IVec2 framebufferSize = IVec2(0);
		Vec2 contentScale = Vec2(1);
		Float dpi = 96.0f;
		UInt revision = 0;

		void set(IVec2 newLogicalSize, IVec2 newFramebufferSize);
	};
}
