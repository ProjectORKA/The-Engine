#pragma once

#include "../Layout/UiLayoutTypes.hpp"

namespace Ui
{
	struct UiNode;

	enum class UiTextWrap : Byte
	{
		Wrap,
		KeepAll,
		GraphemeFallback,
		BreakWord = GraphemeFallback,
	};
	enum class UiWhitespaceMode : Byte { Collapse, Preserve, PreserveBreaks };
	enum class UiTextFit : Byte { None, Shrink, GrowShrink };

	// Breaks are stored before a codepoint. Grapheme opportunities are only
	// used by GraphemeFallback; normal wrapping uses Opportunity.
	enum class UiTextBreak : Byte
	{
		None,
		Grapheme,
		Opportunity,
		Mandatory,
	};

	struct UiTextLimits
	{
		SizeT maximumInputBytes = 16 * 1024 * 1024;
		SizeT maximumCodepoints = 1024 * 1024;
		SizeT maximumGlyphs = 1024 * 1024;
		SizeT maximumSegments = 1024 * 1024;
		UInt maximumLines = 65536;
	};

	struct UiDecodedCodepoint
	{
		U32 value = 0xfffd;
		UInt byteOffset = 0;
		UInt byteLength = 0;
	};

	struct UiShapedGlyph
	{
		UInt glyph = 0;
		UInt codepointIndex = 0;
		Float advance = 0; // Font design units.
	};

	// A segment is a width-independent shaped run. Break opportunities and
	// grapheme fallback boundaries are supplied with it by a future shaper.
	struct UiShapedSegment
	{
		UInt firstCodepoint = 0;
		UInt codepointCount = 0;
		UInt firstGlyph = 0;
		UInt glyphCount = 0;
		Float ascent = 0;  // Design units.
		Float descent = 0; // Positive design units below the baseline.
		Float lineGap = 0;
		UiTextBreak breakBefore = UiTextBreak::None;
		Vector<UInt> graphemeBreaks; // Relative codepoint offsets inside this segment.
	};

	using UiGlyphAdvanceProvider = Function<Float(U32 codepoint)>;
	using UiBreakOpportunityProvider = Function<UiTextBreak(
		U32 previous, U32 current, UInt currentCodepointIndex)>;

	struct UiTextPrepareOptions
	{
		Float designUnitsPerEm = 1000;
		Float ascent = 800;
		Float descent = 200;
		Float lineGap = 0;
		UiWhitespaceMode whitespace = UiWhitespaceMode::Collapse;
		UiGlyphAdvanceProvider glyphAdvance;
		UiBreakOpportunityProvider breakOpportunity;
		UiTextLimits limits;
	};

	class UiPreparedText
	{
	public:
		[[nodiscard]] static UiPreparedText prepareUtf8(
			const String& utf8, const UiTextPrepareOptions& options = {});

		// Glyph cluster indices and segment ranges are sanitized and clamped.
		[[nodiscard]] static UiPreparedText fromShaped(
			const String& utf8, Vector<UiShapedGlyph> glyphs,
			Vector<UiShapedSegment> segments, Float designUnitsPerEm,
			const UiTextLimits& limits = {});

		[[nodiscard]] const String& utf8() const { return source; }
		[[nodiscard]] const Vector<UiDecodedCodepoint>& codepoints() const { return decoded; }
		[[nodiscard]] const Vector<UiShapedGlyph>& glyphs() const { return shapedGlyphs; }
		[[nodiscard]] const Vector<UiShapedSegment>& segments() const { return shapedSegments; }
		[[nodiscard]] const Vector<UiTextBreak>& breaks() const { return breaksBefore; }
		[[nodiscard]] Float unitsPerEm() const { return designUnitsPerEm; }
		[[nodiscard]] Float codepointAdvance(UInt index) const;
		[[nodiscard]] Float segmentAdvance(UInt index) const;
		[[nodiscard]] UInt byteOffset(UInt codepointIndex) const;
		[[nodiscard]] UInt segmentAt(UInt codepointIndex) const;
		[[nodiscard]] Bool empty() const { return decoded.empty(); }
		[[nodiscard]] Bool hadMalformedUtf8() const { return malformedUtf8; }
		[[nodiscard]] Bool wasTruncated() const { return truncated; }
		[[nodiscard]] const UiTextLimits& appliedLimits() const { return limits; }

	private:
		String source;
		Vector<UiDecodedCodepoint> decoded;
		Vector<UiShapedGlyph> shapedGlyphs;
		Vector<UiShapedSegment> shapedSegments;
		Vector<UiTextBreak> breaksBefore;
		Vector<Float> codepointAdvances;
		Vector<Float> segmentAdvances;
		Vector<UInt> codepointSegments;
		Float designUnitsPerEm = 1000;
		UiTextLimits limits;
		Bool malformedUtf8 = false;
		Bool truncated = false;

		void rebuildPreparedData();
	};

	struct UiTextLayoutOptions
	{
		UiTextWrap wrap = UiTextWrap::Wrap;
		Float fontSize = 16;
		Float lineHeight = 0; // Zero selects prepared font metrics.
		UInt maximumLines = maxUInt;
		Bool ellipsis = false;
		Float ellipsisAdvance = 0; // Design units; zero uses three periods.
	};

	struct UiTextLine
	{
		UInt firstCodepoint = 0;
		UInt codepointCount = 0;
		UInt firstGlyph = 0;
		UInt glyphCount = 0;
		UInt firstSegment = 0;
		UInt segmentCount = 0;
		UInt firstByte = 0;
		UInt byteCount = 0;
		Float width = 0;
		Float y = 0;
		Float height = 0;
		Float baseline = 0; // Offset from y.
		Bool ellipsized = false;
	};

	struct UiTextLayoutResult
	{
		Vector<UiTextLine> lines;
		Vec2 size = Vec2(0);
		Float baseline = -1;
		Bool overflowed = false;
		Bool limited = false;
		Float fontSize = 0;
	};

	struct UiTextIntrinsicWidths
	{
		Float minimum = 0;
		Float maximum = 0;
	};

	struct UiTextFitOptions
	{
		UiTextFit mode = UiTextFit::None;
		Float minimumSize = 8;
		Float preferredSize = 16;
		Float maximumSize = 64;
		Float quantization = 1.0f / 64.0f;
		UInt maximumIterations = 20;
	};

	class UiTextLineLayout
	{
	public:
		explicit UiTextLineLayout(const UiPreparedText& prepared) : text(prepared) {}

		[[nodiscard]] UiTextLayoutResult layout(
			Float availableWidth, const UiTextLayoutOptions& options) const;
		[[nodiscard]] UiTextLayoutResult fit(
			Vec2 definiteBox, const UiTextLayoutOptions& options,
			const UiTextFitOptions& fitOptions) const;
		[[nodiscard]] UiTextIntrinsicWidths intrinsicWidths(
			Float fontSize, UiTextWrap wrap = UiTextWrap::Wrap) const;
		[[nodiscard]] UiIntrinsicMeasure measure(
			const UiConstraints& constraints, const UiTextLayoutOptions& options,
			const UiTextFitOptions& fitOptions = {}) const;

	private:
		const UiPreparedText& text;
	};

	// Directly consumable by UiLayoutEngine::setIntrinsicMeasurer.
	[[nodiscard]] Function<UiIntrinsicMeasure(const UiNode&, const UiConstraints&)>
		makeUiTextIntrinsicMeasurer(const UiPreparedText& text,
			UiTextLayoutOptions options, UiTextFitOptions fitOptions = {});
}
