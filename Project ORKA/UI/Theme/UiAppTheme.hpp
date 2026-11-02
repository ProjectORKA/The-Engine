#pragma once

#include "UiAppFlex.hpp"
#include "../Render/UiPaintBuilder.hpp"
#include "../Text/UiTextPainter.hpp"

namespace Ui
{
	class UiSvgIconService;
	struct UiPaintStyleTokens;
	using UiPaintStyleTokenProvider = Function<const UiPaintStyleTokens*(UiNodeHandle)>;

	enum class UiThemeMode : Byte
	{
		Light,
		Dark,
	};

	// Typography scale used by all app surfaces — body for everything except titles.
	struct UiAppTypography
	{
		static constexpr Float bodySize = 16;
		static constexpr Float bodyLine = 24;
		static constexpr Float titleSize = 28;
		static constexpr Float titleLine = 36;
	};

	// Layout metrics shared by launcher, demo, and future app shells
	struct UiAppLayout
	{
		static constexpr Float cornerRadius = 0;
		static constexpr Float maxContentWidth = 960;
		static constexpr Float controlRowHeight = UiAppTypography::bodyLine + UiAppFlex::p10 * 2;
	};

	// Theme paint classes — typography, swatches, and chrome painted by UiAppTheme (layout uses UiAppFlexClass)
	namespace UiAppClass
	{
		inline constexpr const Char* root = "app-root";
		inline constexpr const Char* shell = "app-shell";
		inline constexpr const Char* scroll = "app-scroll";
		inline constexpr const Char* panel = "panel";
		// Translucent black modal dim. Same in light and dark so it reads over a bright game.
		inline constexpr const Char* scrim = "scrim";
		inline constexpr const Char* section = "section";
		inline constexpr const Char* badge = "app-badge";
		inline constexpr const Char* textBody = "text-body";
		inline constexpr const Char* textBadge = "text-badge";
		inline constexpr const Char* entryDescription = "entry-description";
		inline constexpr const Char* fieldLabel = "field-label";
		inline constexpr const Char* valueReadout = "value-readout";
		inline constexpr const Char* statusText = "status-text";
		inline constexpr const Char* iconMuted = "icon-muted";
		inline constexpr const Char* themeSwatch = "theme-swatch";
		inline constexpr const Char* spacingSample = "spacing-sample";
		inline constexpr const Char* typographySample = "typography-sample";
	}

	struct UiAppColors
	{
		UiLinearColor surfaceRoot;
		// Mode-aware overlay: white highlight (dark) or black darkening (light). Stack on parent fills.
		UiLinearColor surfaceHighlight;
		UiLinearColor surfacePanel;
		UiLinearColor surfaceControl;
		UiLinearColor surfaceControlHover;
		UiLinearColor surfaceControlActive;
		UiLinearColor surfaceListItem;
		UiLinearColor surfaceListItemHover;
		UiLinearColor surfaceListItemActive;
		UiLinearColor surfaceBadge;
		UiLinearColor surfaceMenu;
		UiLinearColor surfaceMenuHover;
		UiLinearColor surfaceMenuSelected;
		UiLinearColor surfaceTrack;
		UiLinearColor surfaceTrackFill;
		UiLinearColor border;
		UiLinearColor borderHover;
		UiLinearColor borderFocus;
		UiLinearColor shadow;
		UiLinearColor accent;
		UiLinearColor accentFocus;
		UiLinearColor toggleOn;
		UiLinearColor toggleOff;
		UiLinearColor thumb;
		UiLinearColor textPrimary;
		UiLinearColor textSecondary;
		UiLinearColor textMuted;
		UiLinearColor textInverse;
		UiLinearColor textAccent;
		UiLinearColor iconPrimary;
		UiLinearColor iconMuted;
		UiLinearColor iconOnAccent;
	};

	struct UiAppThemeToken
	{
		const Char* id;
		const Char* label;
		const Char* category;
		UiLinearColor color;
		Float numericValue = 0;
		Bool isColor = true;
	};

	struct UiAppTheme
	{
		UiThemeMode mode = UiThemeMode::Light;
		UiAppColors colors;

		[[nodiscard]] static UiAppTheme light();
		[[nodiscard]] static UiAppTheme dark();
		void setMode(const UiThemeMode newMode);
		[[nodiscard]] UiThemeMode getMode() const { return mode; }
		[[nodiscard]] const Vector<UiAppThemeToken>& tokens() const;

		[[nodiscard]] const UiAppThemeToken* findToken(const String& id) const;

	private:
		mutable Vector<UiAppThemeToken> cachedTokens;
		void rebuildTokenCache() const;
	};

	[[nodiscard]] UiTextStyleProvider makeAppTextStyleProvider(const UiAppTheme& theme);

	[[nodiscard]] UiNodePainter makeAppThemePainter(
		const UiAppTheme& theme,
		const UiDocument* document = nullptr,
		UiTextStyleProvider textStyleProvider = {},
		UiTextGlyphProvider glyphProvider = {},
		UiTextShapeProvider textShapeProvider = {},
		UiSvgIconService* iconService = nullptr,
		UiPaintStyleTokenProvider paintStyleTokens = {});

	// Scrollbars render above descendant content, so paint them after the tree.
	[[nodiscard]] Bool appendAppScrollbarOverlays(
		const UiAppColors& colors, UiDocument& document, UiDisplayList& output);
}
