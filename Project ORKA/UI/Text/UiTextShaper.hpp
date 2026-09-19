#pragma once

#include "UiTextLayout.hpp"

namespace Ui
{
	using UiFontFaceId = U64;
	constexpr UiFontFaceId invalidUiFontFaceId = 0;

	enum class UiTextDirection : Byte { Auto, LeftToRight, RightToLeft };
	enum class UiGlyphColorFormat : UInt
	{
		None = 0,
		Layered = 1 << 0, // COLR/CPAL
		Bitmap = 1 << 1,  // CBDT/CBLC or sbix
		Svg = 1 << 2,
	};

	struct UiFontFaceDescriptor
	{
		String name;         // ResourceManager-style stable resource name.
		Path resourcePath;   // Resolved path; no absolute path is baked into the service.
		String family;
		String style;
		Int faceIndex = 0;
	};

	struct UiFontFaceMetadata
	{
		UiFontFaceId id = invalidUiFontFaceId;
		String name;
		String family;
		String style;
		Float unitsPerEm = 1000;
		Float ascent = 800;
		Float descent = 200;
		Float lineGap = 0;
		UInt colorFormats = static_cast<UInt>(UiGlyphColorFormat::None);
		Bool scalable = true;
		Bool loaded = false;
	};

	struct UiFontFamilyDescriptor
	{
		String name;
		Vector<UiFontFaceId> faces;
		Vector<String> fallbackFamilies;
	};

	struct UiOpenTypeFeature
	{
		String tag; // Four-character OpenType tag, for example "liga".
		UInt value = 1;
		UInt firstCodepoint = 0;
		UInt codepointCount = maxUInt;
	};

	struct UiVariableAxis
	{
		String tag; // Four-character OpenType axis tag, for example "wght".
		Float value = 0;
	};

	struct UiTextShapeRequest
	{
		String utf8;
		String family;
		String locale = "und";
		UiTextDirection direction = UiTextDirection::Auto;
		Vector<UiOpenTypeFeature> features;
		Vector<UiVariableAxis> axes;
		U64 contentGeneration = 0;
		UiTextLimits limits;
	};

	struct UiTextGlyph
	{
		UInt glyphId = 0;
		UiFontFaceId faceId = invalidUiFontFaceId;
		UInt cluster = 0; // Logical codepoint index, never a UTF-8 byte offset.
		Float advanceX = 0;
		Float advanceY = 0;
		Float offsetX = 0;
		Float offsetY = 0;
	};

	struct UiTextRun
	{
		UInt firstCodepoint = 0;
		UInt codepointCount = 0;
		UInt firstGlyph = 0;
		UInt glyphCount = 0;
		UiFontFaceId faceId = invalidUiFontFaceId;
		Byte bidiLevel = 0;
		UiTextDirection direction = UiTextDirection::LeftToRight;
		Float unitsPerEm = 1000;
		Float ascent = 800;
		Float descent = 200;
		Float lineGap = 0;
	};

	struct UiTextBoundaryMetadata
	{
		// Arrays have codepointCount + 1 entries and describe boundaries before
		// each codepoint (plus the terminal boundary).
		Vector<Byte> grapheme;
		Vector<Byte> word;
		Vector<UiTextBreak> line;
	};

	struct UiShapedText
	{
		String utf8;
		Vector<UiDecodedCodepoint> codepoints;
		Vector<UiTextRun> runs;
		Vector<UiTextGlyph> glyphs;
		UiTextBoundaryMetadata boundaries;
		Bool hadMalformedUtf8 = false;
		Bool wasTruncated = false;
		U64 registryGeneration = 0;

		[[nodiscard]] UiPreparedText prepareForLayout(
			const UiTextLimits& limits = {}) const;
	};

	struct UiTextShaperConfig
	{
		SizeT maximumCacheEntries = 256;
	};

	// Backend-neutral font registry and shaping service. Define
	// ORKA_UI_ENABLE_ADVANCED_TEXT when compiling UiTextShaper.cpp to use the
	// optional FreeType + HarfBuzz + ICU implementation.
	class UiTextShaper
	{
	public:
		explicit UiTextShaper(UiTextShaperConfig config = {});
		~UiTextShaper();
		UiTextShaper(UiTextShaper&&) noexcept;
		UiTextShaper& operator=(UiTextShaper&&) noexcept;
		UiTextShaper(const UiTextShaper&) = delete;
		UiTextShaper& operator=(const UiTextShaper&) = delete;

		[[nodiscard]] UiFontFaceId registerFace(const UiFontFaceDescriptor& descriptor);
		[[nodiscard]] Bool unregisterFace(UiFontFaceId id);
		void registerFamily(const UiFontFamilyDescriptor& descriptor);
		[[nodiscard]] Bool unregisterFamily(const String& name);
		void clearRegistry();

		[[nodiscard]] UiFontFaceId findFace(const String& resourceName) const;
		[[nodiscard]] UiFontFaceMetadata faceMetadata(UiFontFaceId id) const;
		[[nodiscard]] Vector<UiFontFaceMetadata> faces() const;
		[[nodiscard]] U64 registryGeneration() const;
		[[nodiscard]] Bool advancedBackendAvailable() const;
		[[nodiscard]] String backendName() const;

		[[nodiscard]] SharedPointer<const UiShapedText> shape(
			const UiTextShapeRequest& request);
		void clearCache();
		void setMaximumCacheEntries(SizeT count);

	private:
		struct Impl;
		UniquePointer<Impl> impl;
	};
}
