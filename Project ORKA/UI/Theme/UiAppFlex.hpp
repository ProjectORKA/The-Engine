#pragma once

#include "../Layout/UiLayoutTypes.hpp"

namespace Ui
{
	struct UiNode;

	// Flex-first spacing scale. Padding and gap use only these class tokens app-wide.
	struct UiAppFlex
	{
		static constexpr Float p5 = 5;
		static constexpr Float p10 = 10;
		static constexpr Float p20 = 20;
		static constexpr Float p40 = 40;
		static constexpr Float p80 = 80;

		static constexpr Float g5 = 5;
		static constexpr Float g10 = 10;
		static constexpr Float g20 = 20;
		static constexpr Float g40 = 40;
		static constexpr Float g80 = 80;

		static constexpr Float b50 = 50;
		static constexpr Float b100 = 100;
		static constexpr Float b150 = 150;
		static constexpr Float b200 = 200;
		static constexpr Float b250 = 250;
		static constexpr Float b300 = 300;
		static constexpr Float b350 = 350;
	};

	namespace UiAppFlexClass
	{
		inline constexpr const char* col = "col";
		inline constexpr const char* row = "row";
		inline constexpr const char* flexWrap = "flex-wrap";
		inline constexpr const char* flexWrapBalance = "flex-wrap-balance";
		inline constexpr const char* flexGrow = "flex-grow";
		inline constexpr const char* minW0 = "min-w-0";

		inline constexpr const char* p5 = "p5";
		inline constexpr const char* p10 = "p10";
		inline constexpr const char* p20 = "p20";
		inline constexpr const char* p40 = "p40";
		inline constexpr const char* p80 = "p80";

		inline constexpr const char* g5 = "g5";
		inline constexpr const char* g10 = "g10";
		inline constexpr const char* g20 = "g20";
		inline constexpr const char* g40 = "g40";
		inline constexpr const char* g80 = "g80";

		inline constexpr const char* b50 = "b50";
		inline constexpr const char* b100 = "b100";
		inline constexpr const char* b150 = "b150";
		inline constexpr const char* b200 = "b200";
		inline constexpr const char* b250 = "b250";
		inline constexpr const char* b300 = "b300";
		inline constexpr const char* b350 = "b350";

		inline constexpr const char* alignTopLeft = "align-top-left";
		inline constexpr const char* alignTopCenter = "align-top-center";
		inline constexpr const char* alignTopRight = "align-top-right";
		inline constexpr const char* alignLeft = "align-left";
		inline constexpr const char* alignCenter = "align-center";
		inline constexpr const char* alignRight = "align-right";
		inline constexpr const char* alignBotLeft = "align-bot-left";
		inline constexpr const char* alignBotCenter = "align-bot-center";
		inline constexpr const char* alignBotRight = "align-bot-right";
	}

	enum class UiAlignHorizontal : Byte { Left, Center, Right };
	enum class UiAlignVertical : Byte { Top, Center, Bottom };

	[[nodiscard]] UiEdges uniformPadding(const Float value);
	[[nodiscard]] Vec2 uniformGap(const Float value);

	[[nodiscard]] Bool parsePaddingClass(const String& className, Float& value);
	[[nodiscard]] Bool parseGapClass(const String& className, Float& value);
	[[nodiscard]] Bool parseBasisClass(const String& className, Float& value);
	[[nodiscard]] Bool parseAlignClass(const String& className, UiAlignHorizontal& horizontal, UiAlignVertical& vertical);

	// Applies row (column is the layout default), wrap, p*, g*, b*, and align-* tokens from authored classes.
	void applyFlexClasses(const Vector<String>& classes, UiLayoutStyle& style);

	// Default p10 layout padding for chrome controls when no padding is authored.
	void applyControlLayoutDefaults(const UiNode& node, UiLayoutStyle& style);

	[[nodiscard]] UiLayoutStyle flexColumnStyle();
	[[nodiscard]] UiLayoutStyle flexRowStyle();
}
