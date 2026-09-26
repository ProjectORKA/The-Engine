#include "UiAppFlex.hpp"

#include "../Core/UiDocument.hpp"

namespace Ui
{
	UiEdges uniformPadding(const Float value)
	{
		return {value, value, value, value};
	}

	Vec2 uniformGap(const Float value)
	{
		return Vec2(value, value);
	}

	Bool parsePaddingClass(const String& className, Float& value)
	{
		if (className == UiAppFlexClass::p5) { value = UiAppFlex::p5; return true; }
		if (className == UiAppFlexClass::p10) { value = UiAppFlex::p10; return true; }
		if (className == UiAppFlexClass::p20) { value = UiAppFlex::p20; return true; }
		if (className == UiAppFlexClass::p40) { value = UiAppFlex::p40; return true; }
		if (className == UiAppFlexClass::p80) { value = UiAppFlex::p80; return true; }
		return false;
	}

	Bool parseGapClass(const String& className, Float& value)
	{
		if (className == UiAppFlexClass::g5) { value = UiAppFlex::g5; return true; }
		if (className == UiAppFlexClass::g10) { value = UiAppFlex::g10; return true; }
		if (className == UiAppFlexClass::g20) { value = UiAppFlex::g20; return true; }
		if (className == UiAppFlexClass::g40) { value = UiAppFlex::g40; return true; }
		if (className == UiAppFlexClass::g80) { value = UiAppFlex::g80; return true; }
		return false;
	}

	Bool parseBasisClass(const String& className, Float& value)
	{
		if (className == UiAppFlexClass::b50) { value = UiAppFlex::b50; return true; }
		if (className == UiAppFlexClass::b100) { value = UiAppFlex::b100; return true; }
		if (className == UiAppFlexClass::b150) { value = UiAppFlex::b150; return true; }
		if (className == UiAppFlexClass::b200) { value = UiAppFlex::b200; return true; }
		if (className == UiAppFlexClass::b250) { value = UiAppFlex::b250; return true; }
		if (className == UiAppFlexClass::b300) { value = UiAppFlex::b300; return true; }
		if (className == UiAppFlexClass::b350) { value = UiAppFlex::b350; return true; }
		return false;
	}

	Bool parseAlignClass(const String& className, UiAlignHorizontal& horizontal, UiAlignVertical& vertical)
	{
		if (className == UiAppFlexClass::alignTopLeft)
		{
			horizontal = UiAlignHorizontal::Left;
			vertical = UiAlignVertical::Top;
			return true;
		}
		if (className == UiAppFlexClass::alignTopCenter)
		{
			horizontal = UiAlignHorizontal::Center;
			vertical = UiAlignVertical::Top;
			return true;
		}
		if (className == UiAppFlexClass::alignTopRight)
		{
			horizontal = UiAlignHorizontal::Right;
			vertical = UiAlignVertical::Top;
			return true;
		}
		if (className == UiAppFlexClass::alignLeft)
		{
			horizontal = UiAlignHorizontal::Left;
			vertical = UiAlignVertical::Center;
			return true;
		}
		if (className == UiAppFlexClass::alignCenter)
		{
			horizontal = UiAlignHorizontal::Center;
			vertical = UiAlignVertical::Center;
			return true;
		}
		if (className == UiAppFlexClass::alignRight)
		{
			horizontal = UiAlignHorizontal::Right;
			vertical = UiAlignVertical::Center;
			return true;
		}
		if (className == UiAppFlexClass::alignBotLeft)
		{
			horizontal = UiAlignHorizontal::Left;
			vertical = UiAlignVertical::Bottom;
			return true;
		}
		if (className == UiAppFlexClass::alignBotCenter)
		{
			horizontal = UiAlignHorizontal::Center;
			vertical = UiAlignVertical::Bottom;
			return true;
		}
		if (className == UiAppFlexClass::alignBotRight)
		{
			horizontal = UiAlignHorizontal::Right;
			vertical = UiAlignVertical::Bottom;
			return true;
		}
		return false;
	}

	namespace
	{
		UiJustify justifyFromHorizontal(const UiAlignHorizontal horizontal)
		{
			switch (horizontal)
			{
			case UiAlignHorizontal::Left: return UiJustify::Start;
			case UiAlignHorizontal::Center: return UiJustify::Center;
			case UiAlignHorizontal::Right: return UiJustify::End;
			}
			return UiJustify::Start;
		}

		UiJustify justifyFromVertical(const UiAlignVertical vertical)
		{
			switch (vertical)
			{
			case UiAlignVertical::Top: return UiJustify::Start;
			case UiAlignVertical::Center: return UiJustify::Center;
			case UiAlignVertical::Bottom: return UiJustify::End;
			}
			return UiJustify::Start;
		}

		UiAlign alignFromHorizontal(const UiAlignHorizontal horizontal)
		{
			switch (horizontal)
			{
			case UiAlignHorizontal::Left: return UiAlign::Start;
			case UiAlignHorizontal::Center: return UiAlign::Center;
			case UiAlignHorizontal::Right: return UiAlign::End;
			}
			return UiAlign::Start;
		}

		UiAlign alignFromVertical(const UiAlignVertical vertical)
		{
			switch (vertical)
			{
			case UiAlignVertical::Top: return UiAlign::Start;
			case UiAlignVertical::Center: return UiAlign::Center;
			case UiAlignVertical::Bottom: return UiAlign::End;
			}
			return UiAlign::Start;
		}

		void applySemanticAlign(const UiAxis flexDirection, const UiAlignHorizontal horizontal,
			const UiAlignVertical vertical, UiLayoutStyle& style)
		{
			if (flexDirection == UiAxis::Column)
			{
				style.justifyContent = justifyFromVertical(vertical);
				style.alignItems = alignFromHorizontal(horizontal);
			}
			else
			{
				style.justifyContent = justifyFromHorizontal(horizontal);
				style.alignItems = alignFromVertical(vertical);
			}
		}
	}

	void applyFlexClasses(const Vector<String>& classes, UiLayoutStyle& style)
	{
		Float padding = -1;
		Float gap = -1;
		Bool hasSemanticAlign = false;
		UiAlignHorizontal semanticHorizontal = UiAlignHorizontal::Left;
		UiAlignVertical semanticVertical = UiAlignVertical::Top;
		for (const String& className : classes)
		{
			if (className == UiAppFlexClass::col)
			{
				style.display = UiDisplay::Flex;
				style.flexDirection = UiAxis::Column;
			}
			else if (className == UiAppFlexClass::row)
			{
				style.display = UiDisplay::Flex;
				style.flexDirection = UiAxis::Row;
			}
			else if (className == UiAppFlexClass::wrap)
				style.flexWrap = UiWrap::Wrap;
			else if (className == UiAppFlexClass::fg1 || className == "flex-grow")
				style.flexGrow = 1;
			else if (className == UiAppFlexClass::mw0 || className == "min-w-0")
				style.minimumWidth = UiSizeSpec::fixed(0);
			else if (className == UiAppFlexClass::mh0 || className == "min-h-0")
				style.minimumHeight = UiSizeSpec::fixed(0);
			Float paddingValue = 0;
			if (parsePaddingClass(className, paddingValue)) padding = paddingValue;
			Float gapValue = 0;
			if (parseGapClass(className, gapValue)) gap = gapValue;
			Float basisValue = 0;
			if (parseBasisClass(className, basisValue))
				style.flexBasis = UiSizeSpec::fixed(basisValue);
			UiAlignHorizontal horizontal = UiAlignHorizontal::Left;
			UiAlignVertical vertical = UiAlignVertical::Top;
			if (parseAlignClass(className, horizontal, vertical))
			{
				hasSemanticAlign = true;
				semanticHorizontal = horizontal;
				semanticVertical = vertical;
			}
		}
		if (padding >= 0) style.padding = uniformPadding(padding);
		if (gap >= 0) style.gap = uniformGap(gap);
		if (hasSemanticAlign)
			applySemanticAlign(style.flexDirection, semanticHorizontal, semanticVertical, style);
	}

	void applyControlLayoutDefaults(const UiNode& node, UiLayoutStyle& style)
	{
		if (style.padding.left != 0 || style.padding.top != 0 ||
			style.padding.right != 0 || style.padding.bottom != 0)
			return;
		const String& type = node.authored.typeName;
		if (type == "Button" || type == "TextField" || type == "NumberField" || type == "TextArea" ||
			type == "MenuItem")
			style.padding = uniformPadding(UiAppFlex::p10);
	}

	UiLayoutStyle flexColumnStyle()
	{
		UiLayoutStyle style;
		style.display = UiDisplay::Flex;
		style.flexDirection = UiAxis::Column;
		return style;
	}

	UiLayoutStyle flexRowStyle()
	{
		UiLayoutStyle style;
		style.display = UiDisplay::Flex;
		style.flexDirection = UiAxis::Row;
		return style;
	}
}
