#include "UiTextShaper.hpp"

#include <bit>
#include <mutex>
#include <sstream>
#include <unordered_map>

#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
// Optional focused-library backend. These headers and their link dependencies
// are intentionally absent from default builds.
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <hb-ft.h>
#include <hb.h>
#include <hb-ot.h>
#include <unicode/ubidi.h>
#include <unicode/ubrk.h>
#include <unicode/uscript.h>
#endif

namespace Ui
{
	namespace
	{
		struct DecodeResult
		{
			Vector<UiDecodedCodepoint> codepoints;
			Bool malformed = false;
			Bool truncated = false;
		};

		Bool continuation(const Byte value) { return (value & 0xc0) == 0x80; }

		DecodeResult decodeUtf8(const String& text, const UiTextLimits& limits)
		{
			DecodeResult result;
			const SizeT byteLimit = std::min({
				text.size(), limits.maximumInputBytes, static_cast<SizeT>(maxUInt)
			});
			const SizeT pointLimit = std::min(
				limits.maximumCodepoints, static_cast<SizeT>(maxUInt));
			result.truncated = byteLimit < text.size();
			result.codepoints.reserve(std::min(byteLimit, pointLimit));
			SizeT offset = 0;
			while (offset < byteLimit && result.codepoints.size() < pointLimit)
			{
				const SizeT start = offset++;
				const Byte first = static_cast<Byte>(text[start]);
				U32 value = 0xfffd;
				UInt length = 1;
				UInt wanted = 0;
				U32 minimum = 0;
				if (first < 0x80) value = first;
				else if ((first & 0xe0) == 0xc0) { wanted = 2; value = first & 0x1f; minimum = 0x80; }
				else if ((first & 0xf0) == 0xe0) { wanted = 3; value = first & 0x0f; minimum = 0x800; }
				else if ((first & 0xf8) == 0xf0) { wanted = 4; value = first & 0x07; minimum = 0x10000; }
				else result.malformed = true;

				if (wanted)
				{
					Bool valid = start + wanted <= byteLimit;
					U32 candidate = value;
					for (UInt index = 1; valid && index < wanted; ++index)
					{
						const Byte next = static_cast<Byte>(text[start + index]);
						if (!continuation(next)) valid = false;
						else candidate = (candidate << 6) | (next & 0x3f);
					}
					valid = valid && candidate >= minimum && candidate <= 0x10ffff &&
						!(candidate >= 0xd800 && candidate <= 0xdfff);
					if (valid)
					{
						value = candidate;
						length = wanted;
						offset = start + wanted;
					}
					else result.malformed = true;
				}
				result.codepoints.push_back({
					value, static_cast<UInt>(start), length
				});
			}
			if (offset < byteLimit) result.truncated = true;
			return result;
		}

		Bool isNewline(const U32 value) { return value == '\n' || value == '\r'; }
		Bool isSpace(const U32 value)
		{
			return value == ' ' || value == '\t' || value == '\v' || value == '\f';
		}

		UInt tagFrom(const String& text)
		{
			if (text.size() != 4) return 0;
			return static_cast<UInt>(static_cast<Byte>(text[0])) << 24 |
				static_cast<UInt>(static_cast<Byte>(text[1])) << 16 |
				static_cast<UInt>(static_cast<Byte>(text[2])) << 8 |
				static_cast<UInt>(static_cast<Byte>(text[3]));
		}

		void makeFallbackBoundaries(UiShapedText& shaped)
		{
			const UInt count = static_cast<UInt>(shaped.codepoints.size());
			shaped.boundaries.grapheme.assign(count + 1, 1);
			shaped.boundaries.word.assign(count + 1, 0);
			shaped.boundaries.line.assign(count + 1, UiTextBreak::Grapheme);
			if (!shaped.boundaries.line.empty())
				shaped.boundaries.line[0] = UiTextBreak::Mandatory;
			for (UInt index = 1; index <= count; ++index)
			{
				const U32 previous = shaped.codepoints[index - 1].value;
				if (isNewline(previous))
					shaped.boundaries.line[index] = UiTextBreak::Mandatory;
				else if (isSpace(previous) || previous == '-' || previous == 0x200b)
					shaped.boundaries.line[index] = UiTextBreak::Opportunity;
				if (isSpace(previous) || index == count ||
					(index < count && isSpace(shaped.codepoints[index].value)))
					shaped.boundaries.word[index] = 1;
			}
			if (!shaped.boundaries.word.empty()) shaped.boundaries.word[0] = 1;
		}

		String cacheKey(const UiTextShapeRequest& request, const U64 generation)
		{
			std::ostringstream key;
			key << generation << ':' << request.contentGeneration << ':'
				<< static_cast<UInt>(request.direction) << ':' << request.family.size() << ':'
				<< request.family << ':' << request.locale.size() << ':' << request.locale << ':'
				<< request.utf8.size() << ':' << request.utf8;
			for (const UiOpenTypeFeature& feature : request.features)
				key << "|f:" << feature.tag << ':' << feature.value << ':'
					<< feature.firstCodepoint << ':' << feature.codepointCount;
			for (const UiVariableAxis& axis : request.axes)
				key << "|a:" << axis.tag << ':' << std::bit_cast<UInt>(axis.value);
			key << "|l:" << request.limits.maximumInputBytes << ':'
				<< request.limits.maximumCodepoints << ':'
				<< request.limits.maximumGlyphs << ':'
				<< request.limits.maximumSegments;
			return key.str();
		}
	}

	struct UiTextShaper::Impl
	{
		struct Face
		{
			UiFontFaceDescriptor descriptor;
			UiFontFaceMetadata metadata;
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
			FT_Face ftFace = nullptr;
			hb_font_t* hbFont = nullptr;
#endif
		};

		struct CacheEntry
		{
			String key;
			SharedPointer<const UiShapedText> value;
		};

		UiTextShaperConfig config;
		mutable Mutex mutex;
		Map<UiFontFaceId, Face> faceRegistry;
		Map<String, UiFontFamilyDescriptor> familyRegistry;
		UiFontFaceId nextFaceId = 1;
		U64 generation = 1;
		List<CacheEntry> cache;
		std::unordered_map<String, List<CacheEntry>::iterator> cacheIndex;
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		FT_Library freeType = nullptr;
		Bool advancedReady = false;
#endif

		explicit Impl(const UiTextShaperConfig supplied) : config(supplied)
		{
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
			advancedReady = FT_Init_FreeType(&freeType) == 0;
#endif
		}

		~Impl()
		{
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
			for (auto& [id, face] : faceRegistry)
			{
				if (face.hbFont) hb_font_destroy(face.hbFont);
				if (face.ftFace) FT_Done_Face(face.ftFace);
			}
			if (freeType) FT_Done_FreeType(freeType);
#endif
		}

		void invalidate()
		{
			++generation;
			cache.clear();
			cacheIndex.clear();
		}

		Vector<UiFontFaceId> candidates(const String& family) const
		{
			Vector<UiFontFaceId> result;
			std::set<String> visited;
			Function<void(const String&)> append = [&](const String& name)
				{
					if (!visited.insert(name).second) return;
					const auto found = familyRegistry.find(name);
					if (found == familyRegistry.end()) return;
					for (const UiFontFaceId id : found->second.faces)
						if (faceRegistry.contains(id) &&
							std::ranges::find(result, id) == result.end())
							result.push_back(id);
					for (const String& fallback : found->second.fallbackFamilies)
						append(fallback);
				};
			append(family);
			if (result.empty())
				for (const auto& [id, face] : faceRegistry) result.push_back(id);
			return result;
		}

		UiFontFaceId selectFace(const Vector<UiFontFaceId>& choices, const U32 codepoint) const
		{
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
			for (const UiFontFaceId id : choices)
			{
				const auto found = faceRegistry.find(id);
				if (found != faceRegistry.end() && found->second.ftFace &&
					FT_Get_Char_Index(found->second.ftFace, codepoint) != 0)
					return id;
			}
#endif
			return choices.empty() ? invalidUiFontFaceId : choices.front();
		}

		const UiFontFaceMetadata* metadata(const UiFontFaceId id) const
		{
			const auto found = faceRegistry.find(id);
			return found == faceRegistry.end() ? nullptr : &found->second.metadata;
		}

		void fallbackShape(UiShapedText& output, const UiTextShapeRequest& request)
		{
			const Vector<UiFontFaceId> choices = candidates(request.family);
			const UInt glyphLimit = static_cast<UInt>(std::min(
				request.limits.maximumGlyphs, static_cast<SizeT>(maxUInt)));
			UiTextDirection resolved = request.direction;
			if (resolved == UiTextDirection::Auto)
			{
				resolved = UiTextDirection::LeftToRight;
				for (const UiDecodedCodepoint& point : output.codepoints)
					if ((point.value >= 0x0590 && point.value <= 0x08ff) ||
						(point.value >= 0xfb1d && point.value <= 0xfdff) ||
						(point.value >= 0xfe70 && point.value <= 0xfeff))
					{
						resolved = UiTextDirection::RightToLeft;
						break;
					}
					else if ((point.value >= 'A' && point.value <= 'Z') ||
						(point.value >= 'a' && point.value <= 'z')) break;
			}
			const UInt count = static_cast<UInt>(output.codepoints.size());
			for (UInt visualIndex = 0;
				visualIndex < count && output.glyphs.size() < glyphLimit; ++visualIndex)
			{
				const UInt index = resolved == UiTextDirection::RightToLeft ?
					count - visualIndex - 1 : visualIndex;
				const U32 codepoint = output.codepoints[index].value;
				const UiFontFaceId faceId = selectFace(choices, codepoint);
				const UiFontFaceMetadata* face = metadata(faceId);
				const Float units = face ? face->unitsPerEm : 1000;
				const Float advance = isNewline(codepoint) ? 0 :
					(isSpace(codepoint) ? units * 0.333f : units * 0.6f);
				output.glyphs.push_back({
					codepoint, faceId, index, advance, 0, 0, 0
				});
				UiTextRun run;
				run.firstCodepoint = index;
				run.codepointCount = 1;
				run.firstGlyph = static_cast<UInt>(output.glyphs.size() - 1);
				run.glyphCount = 1;
				run.faceId = faceId;
				run.bidiLevel = resolved == UiTextDirection::RightToLeft ? 1 : 0;
				run.direction = resolved;
				if (face)
				{
					run.unitsPerEm = face->unitsPerEm;
					run.ascent = face->ascent;
					run.descent = face->descent;
					run.lineGap = face->lineGap;
				}
				output.runs.push_back(run);
			}
			if (output.glyphs.size() < output.codepoints.size()) output.wasTruncated = true;
			makeFallbackBoundaries(output);
		}

#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		static void appendUtf16(const Vector<UiDecodedCodepoint>& points,
			Vector<UChar>& utf16, Vector<Int>& pointToUtf16)
		{
			pointToUtf16.reserve(points.size() + 1);
			for (const UiDecodedCodepoint& point : points)
			{
				pointToUtf16.push_back(static_cast<Int>(utf16.size()));
				if (point.value <= 0xffff)
					utf16.push_back(static_cast<UChar>(point.value));
				else
				{
					const U32 value = point.value - 0x10000;
					utf16.push_back(static_cast<UChar>(0xd800 + (value >> 10)));
					utf16.push_back(static_cast<UChar>(0xdc00 + (value & 0x3ff)));
				}
			}
			pointToUtf16.push_back(static_cast<Int>(utf16.size()));
		}

		static UInt pointAtUtf16(const Vector<Int>& pointToUtf16, const Int offset)
		{
			const auto found = std::lower_bound(pointToUtf16.begin(), pointToUtf16.end(), offset);
			return static_cast<UInt>(found - pointToUtf16.begin());
		}

		static void collectBreaks(UBreakIteratorType type, const Vector<UChar>& utf16,
			const Vector<Int>& pointToUtf16, const String& locale, Vector<Byte>& output)
		{
			UErrorCode error = U_ZERO_ERROR;
			UBreakIterator* iterator = ubrk_open(type,
				locale.empty() ? "und" : locale.c_str(), utf16.data(),
				static_cast<Int>(utf16.size()), &error);
			if (U_FAILURE(error) || !iterator) return;
			for (Int boundary = ubrk_first(iterator); boundary != UBRK_DONE;
				boundary = ubrk_next(iterator))
			{
				const UInt point = pointAtUtf16(pointToUtf16, boundary);
				if (point < output.size() && pointToUtf16[point] == boundary)
					output[point] = 1;
			}
			ubrk_close(iterator);
		}

		void advancedBoundaries(UiShapedText& output, const Vector<UChar>& utf16,
			const Vector<Int>& pointToUtf16, const String& locale)
		{
			const SizeT count = output.codepoints.size() + 1;
			output.boundaries.grapheme.assign(count, 0);
			output.boundaries.word.assign(count, 0);
			Vector<Byte> line(count, 0);
			collectBreaks(UBRK_CHARACTER, utf16, pointToUtf16, locale,
				output.boundaries.grapheme);
			collectBreaks(UBRK_WORD, utf16, pointToUtf16, locale,
				output.boundaries.word);
			collectBreaks(UBRK_LINE, utf16, pointToUtf16, locale, line);
			output.boundaries.line.assign(count, UiTextBreak::None);
			for (UInt index = 0; index < count; ++index)
			{
				if (output.boundaries.grapheme[index])
					output.boundaries.line[index] = UiTextBreak::Grapheme;
				if (line[index])
					output.boundaries.line[index] = UiTextBreak::Opportunity;
				if (index > 0 && isNewline(output.codepoints[index - 1].value))
					output.boundaries.line[index] = UiTextBreak::Mandatory;
			}
			if (!output.boundaries.line.empty())
				output.boundaries.line[0] = UiTextBreak::Mandatory;
		}

		void shapeChunk(UiShapedText& output, const UiTextShapeRequest& request,
			const Vector<hb_codepoint_t>& text,
			const UInt first, const UInt end, const UiFontFaceId faceId,
			const Byte level, const UiTextDirection direction, const UScriptCode script)
		{
			const auto found = faceRegistry.find(faceId);
			if (found == faceRegistry.end() || !found->second.hbFont) return;
			hb_font_t* font = hb_font_create(
				hb_font_get_face(found->second.hbFont));
			const Int unitsPerEm = static_cast<Int>(std::max(
				found->second.metadata.unitsPerEm, 1.0f));
			// Shape from OpenType tables in design units. A newly loaded
			// FT_Face has no active pixel size, so hb-ft positioning would
			// collapse advances to zero until a size was selected.
			hb_ot_font_set_funcs(font);
			hb_font_set_scale(font, unitsPerEm, unitsPerEm);
			Vector<hb_variation_t> variations;
			for (const UiVariableAxis& axis : request.axes)
				if (const UInt tag = tagFrom(axis.tag))
					variations.push_back({ tag, axis.value });
			if (!variations.empty())
				hb_font_set_variations(font, variations.data(),
					static_cast<UInt>(variations.size()));

			hb_buffer_t* buffer = hb_buffer_create();
			hb_buffer_set_direction(buffer, direction == UiTextDirection::RightToLeft ?
				HB_DIRECTION_RTL : HB_DIRECTION_LTR);
			hb_buffer_set_language(buffer, hb_language_from_string(
				request.locale.c_str(), static_cast<Int>(request.locale.size())));
			hb_buffer_set_script(buffer, hb_script_from_iso15924_tag(
				hb_tag_from_string(uscript_getShortName(script), -1)));
			hb_buffer_set_cluster_level(buffer,
				HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);
			// Passing the whole paragraph gives HarfBuzz the context outside the
			// item range and makes cluster values absolute codepoint indices.
			hb_buffer_add_codepoints(buffer, text.data(),
				static_cast<Int>(text.size()), first,
				static_cast<Int>(end - first));

			Vector<hb_feature_t> features;
			for (const UiOpenTypeFeature& feature : request.features)
			{
				const UInt tag = tagFrom(feature.tag);
				if (!tag) continue;
				const UInt featureEnd = feature.codepointCount == maxUInt ? maxUInt :
					feature.firstCodepoint + feature.codepointCount;
				features.push_back({
					tag, feature.value, feature.firstCodepoint, featureEnd
				});
			}
			hb_shape(font, buffer, features.data(), static_cast<UInt>(features.size()));
			UInt glyphCount = 0;
			const hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buffer, &glyphCount);
			const hb_glyph_position_t* positions =
				hb_buffer_get_glyph_positions(buffer, &glyphCount);
			const UInt allowed = static_cast<UInt>(std::min<SizeT>(glyphCount,
				request.limits.maximumGlyphs > output.glyphs.size() ?
					request.limits.maximumGlyphs - output.glyphs.size() : 0));

			UiTextRun run;
			run.firstCodepoint = first;
			run.codepointCount = end - first;
			run.firstGlyph = static_cast<UInt>(output.glyphs.size());
			run.glyphCount = allowed;
			run.faceId = faceId;
			run.bidiLevel = level;
			run.direction = direction;
			run.unitsPerEm = found->second.metadata.unitsPerEm;
			run.ascent = found->second.metadata.ascent;
			run.descent = found->second.metadata.descent;
			run.lineGap = found->second.metadata.lineGap;
			for (UInt index = 0; index < allowed; ++index)
				output.glyphs.push_back({
					infos[index].codepoint, faceId,
					std::min(infos[index].cluster,
						static_cast<UInt>(output.codepoints.size() - 1)),
					static_cast<Float>(positions[index].x_advance),
					static_cast<Float>(positions[index].y_advance),
					static_cast<Float>(positions[index].x_offset),
					static_cast<Float>(positions[index].y_offset)
				});
			output.runs.push_back(run);
			if (allowed < glyphCount) output.wasTruncated = true;
			hb_buffer_destroy(buffer);
			hb_font_destroy(font);
		}

		Bool advancedShape(UiShapedText& output, const UiTextShapeRequest& request)
		{
			if (!advancedReady || faceRegistry.empty() || output.codepoints.empty())
				return false;
			Vector<hb_codepoint_t> text;
			text.reserve(output.codepoints.size());
			for (const UiDecodedCodepoint& point : output.codepoints)
				text.push_back(point.value);
			Vector<UChar> utf16;
			Vector<Int> pointToUtf16;
			appendUtf16(output.codepoints, utf16, pointToUtf16);
			advancedBoundaries(output, utf16, pointToUtf16, request.locale);

			UErrorCode error = U_ZERO_ERROR;
			UBiDi* bidi = ubidi_openSized(static_cast<Int>(utf16.size()), 0, &error);
			if (U_FAILURE(error) || !bidi) return false;
			UBiDiLevel paragraphLevel = UBIDI_DEFAULT_LTR;
			if (request.direction == UiTextDirection::LeftToRight) paragraphLevel = 0;
			else if (request.direction == UiTextDirection::RightToLeft) paragraphLevel = 1;
			ubidi_setPara(bidi, utf16.data(), static_cast<Int>(utf16.size()),
				paragraphLevel, nullptr, &error);
			if (U_FAILURE(error)) { ubidi_close(bidi); return false; }

			struct Chunk
			{
				UInt first;
				UInt end;
				UiFontFaceId face;
				UScriptCode script;
			};
			const Vector<UiFontFaceId> choices = candidates(request.family);
			const Int runCount = ubidi_countRuns(bidi, &error);
			for (Int visualRun = 0; U_SUCCESS(error) && visualRun < runCount; ++visualRun)
			{
				Int logicalStart = 0;
				Int length = 0;
				const UBiDiDirection bidiDirection =
					ubidi_getVisualRun(bidi, visualRun, &logicalStart, &length);
				const UInt first = pointAtUtf16(pointToUtf16, logicalStart);
				const UInt end = pointAtUtf16(pointToUtf16, logicalStart + length);
				Vector<Chunk> chunks;
				for (UInt index = first; index < end;)
				{
					UErrorCode scriptError = U_ZERO_ERROR;
					UScriptCode script = uscript_getScript(
						output.codepoints[index].value, &scriptError);
					if (U_FAILURE(scriptError)) script = USCRIPT_UNKNOWN;
					const UiFontFaceId face = selectFace(
						choices, output.codepoints[index].value);
					UInt next = index + 1;
					while (next < end)
					{
						UErrorCode nextError = U_ZERO_ERROR;
						UScriptCode nextScript = uscript_getScript(
							output.codepoints[next].value, &nextError);
						if (U_FAILURE(nextError)) nextScript = script;
						if (nextScript == USCRIPT_COMMON || nextScript == USCRIPT_INHERITED)
							nextScript = script;
						if (selectFace(choices, output.codepoints[next].value) != face ||
							nextScript != script) break;
						++next;
					}
					chunks.push_back({ index, next, face, script });
					index = next;
				}
				if (bidiDirection == UBIDI_RTL) std::ranges::reverse(chunks);
				const UiTextDirection direction = bidiDirection == UBIDI_RTL ?
					UiTextDirection::RightToLeft : UiTextDirection::LeftToRight;
				for (const Chunk& chunk : chunks)
					shapeChunk(output, request, text, chunk.first, chunk.end, chunk.face,
						ubidi_getLevelAt(bidi, pointToUtf16[chunk.first]),
						direction, chunk.script);
			}
			ubidi_close(bidi);
			return !output.runs.empty();
		}
#endif
	};

	UiPreparedText UiShapedText::prepareForLayout(const UiTextLimits& limits) const
	{
		Vector<UiShapedGlyph> preparedGlyphs;
		Vector<UiShapedSegment> preparedSegments;
		preparedGlyphs.reserve(glyphs.size());
		Float unitsPerEm = runs.empty() ? 1000 : runs.front().unitsPerEm;

		// UiPreparedText expects logical, contiguous segment glyph ranges. Keep
		// this conversion separate from the visual-order glyph stream.
		for (UInt runIndex = 0; runIndex < runs.size(); ++runIndex)
		{
			const UiTextRun& run = runs[runIndex];
			Vector<UInt> cuts = { run.firstCodepoint };
			const UInt runEnd = run.firstCodepoint + run.codepointCount;
			for (UInt index = run.firstCodepoint + 1; index < runEnd; ++index)
				if (index < boundaries.line.size() &&
					static_cast<Byte>(boundaries.line[index]) >=
					static_cast<Byte>(UiTextBreak::Opportunity))
					cuts.push_back(index);
			cuts.push_back(runEnd);
			for (UInt cut = 0; cut + 1 < cuts.size(); ++cut)
			{
				UiShapedSegment segment;
				segment.firstCodepoint = cuts[cut];
				segment.codepointCount = cuts[cut + 1] - cuts[cut];
				segment.firstGlyph = static_cast<UInt>(preparedGlyphs.size());
				segment.ascent = run.ascent;
				segment.descent = run.descent;
				segment.lineGap = run.lineGap;
				if (segment.firstCodepoint < boundaries.line.size())
					segment.breakBefore = boundaries.line[segment.firstCodepoint];
				for (UInt point = segment.firstCodepoint + 1;
					point < segment.firstCodepoint + segment.codepointCount; ++point)
					if (point < boundaries.grapheme.size() && boundaries.grapheme[point])
						segment.graphemeBreaks.push_back(point - segment.firstCodepoint);
				for (UInt glyphIndex = run.firstGlyph;
					glyphIndex < run.firstGlyph + run.glyphCount &&
					glyphIndex < glyphs.size(); ++glyphIndex)
				{
					const UiTextGlyph& glyph = glyphs[glyphIndex];
					if (glyph.cluster >= cuts[cut] && glyph.cluster < cuts[cut + 1])
						preparedGlyphs.push_back({
							glyph.glyphId, glyph.cluster, std::abs(glyph.advanceX)
						});
				}
				segment.glyphCount =
					static_cast<UInt>(preparedGlyphs.size()) - segment.firstGlyph;
				preparedSegments.push_back(std::move(segment));
			}
		}
		return UiPreparedText::fromShaped(
			utf8, std::move(preparedGlyphs), std::move(preparedSegments),
			unitsPerEm, limits);
	}

	UiTextShaper::UiTextShaper(const UiTextShaperConfig config)
		: impl(std::make_unique<Impl>(config)) {}
	UiTextShaper::~UiTextShaper() = default;
	UiTextShaper::UiTextShaper(UiTextShaper&&) noexcept = default;
	UiTextShaper& UiTextShaper::operator=(UiTextShaper&&) noexcept = default;

	UiFontFaceId UiTextShaper::registerFace(const UiFontFaceDescriptor& descriptor)
	{
		LockGuard lock(impl->mutex);
		const UiFontFaceId id = impl->nextFaceId++;
		Impl::Face face;
		face.descriptor = descriptor;
		face.metadata.id = id;
		face.metadata.name = descriptor.name;
		face.metadata.family = descriptor.family;
		face.metadata.style = descriptor.style;
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		if (impl->advancedReady && FT_New_Face(impl->freeType,
			descriptor.resourcePath.string().c_str(), descriptor.faceIndex,
			&face.ftFace) == 0)
		{
			FT_Select_Charmap(face.ftFace, FT_ENCODING_UNICODE);
			face.hbFont = hb_ft_font_create_referenced(face.ftFace);
			face.metadata.loaded = face.hbFont != nullptr;
			face.metadata.unitsPerEm = static_cast<Float>(
				std::max<UInt>(face.ftFace->units_per_EM, 1));
			face.metadata.ascent = static_cast<Float>(std::max<Long>(
				face.ftFace->ascender, 0));
			face.metadata.descent = static_cast<Float>(std::max<Long>(
				-face.ftFace->descender, 0));
			face.metadata.lineGap = static_cast<Float>(std::max<Long>(
				face.ftFace->height - face.ftFace->ascender +
				face.ftFace->descender, 0));
			face.metadata.scalable = FT_IS_SCALABLE(face.ftFace) != 0;
			auto hasTable = [&](const UInt tag)
				{
					FT_ULong length = 0;
					return FT_Load_Sfnt_Table(face.ftFace, tag, 0, nullptr, &length) == 0 &&
						length > 0;
				};
			if (hasTable(FT_MAKE_TAG('C', 'O', 'L', 'R')))
				face.metadata.colorFormats |= static_cast<UInt>(UiGlyphColorFormat::Layered);
			if (hasTable(FT_MAKE_TAG('C', 'B', 'D', 'T')) ||
				hasTable(FT_MAKE_TAG('s', 'b', 'i', 'x')))
				face.metadata.colorFormats |= static_cast<UInt>(UiGlyphColorFormat::Bitmap);
			if (hasTable(FT_MAKE_TAG('S', 'V', 'G', ' ')))
				face.metadata.colorFormats |= static_cast<UInt>(UiGlyphColorFormat::Svg);
		}
#endif
		impl->faceRegistry.emplace(id, std::move(face));
		impl->invalidate();
		return id;
	}

	Bool UiTextShaper::unregisterFace(const UiFontFaceId id)
	{
		LockGuard lock(impl->mutex);
		const auto found = impl->faceRegistry.find(id);
		if (found == impl->faceRegistry.end()) return false;
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		if (found->second.hbFont) hb_font_destroy(found->second.hbFont);
		if (found->second.ftFace) FT_Done_Face(found->second.ftFace);
#endif
		impl->faceRegistry.erase(found);
		for (auto& [name, family] : impl->familyRegistry)
			std::erase(family.faces, id);
		impl->invalidate();
		return true;
	}

	void UiTextShaper::registerFamily(const UiFontFamilyDescriptor& descriptor)
	{
		LockGuard lock(impl->mutex);
		impl->familyRegistry[descriptor.name] = descriptor;
		impl->invalidate();
	}

	Bool UiTextShaper::unregisterFamily(const String& name)
	{
		LockGuard lock(impl->mutex);
		if (!impl->familyRegistry.erase(name)) return false;
		impl->invalidate();
		return true;
	}

	void UiTextShaper::clearRegistry()
	{
		LockGuard lock(impl->mutex);
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		for (auto& [id, face] : impl->faceRegistry)
		{
			if (face.hbFont) hb_font_destroy(face.hbFont);
			if (face.ftFace) FT_Done_Face(face.ftFace);
		}
#endif
		impl->faceRegistry.clear();
		impl->familyRegistry.clear();
		impl->invalidate();
	}

	UiFontFaceId UiTextShaper::findFace(const String& resourceName) const
	{
		LockGuard lock(impl->mutex);
		for (const auto& [id, face] : impl->faceRegistry)
			if (face.descriptor.name == resourceName) return id;
		return invalidUiFontFaceId;
	}

	UiFontFaceMetadata UiTextShaper::faceMetadata(const UiFontFaceId id) const
	{
		LockGuard lock(impl->mutex);
		const UiFontFaceMetadata* metadata = impl->metadata(id);
		return metadata ? *metadata : UiFontFaceMetadata{};
	}

	Vector<UiFontFaceMetadata> UiTextShaper::faces() const
	{
		LockGuard lock(impl->mutex);
		Vector<UiFontFaceMetadata> result;
		result.reserve(impl->faceRegistry.size());
		for (const auto& [id, face] : impl->faceRegistry)
			result.push_back(face.metadata);
		return result;
	}

	U64 UiTextShaper::registryGeneration() const
	{
		LockGuard lock(impl->mutex);
		return impl->generation;
	}

	Bool UiTextShaper::advancedBackendAvailable() const
	{
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		LockGuard lock(impl->mutex);
		return impl->advancedReady;
#else
		return false;
#endif
	}

	String UiTextShaper::backendName() const
	{
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		return advancedBackendAvailable() ? "FreeType + HarfBuzz + ICU" :
			"deterministic fallback";
#else
		return "deterministic fallback";
#endif
	}

	SharedPointer<const UiShapedText> UiTextShaper::shape(
		const UiTextShapeRequest& request)
	{
		LockGuard lock(impl->mutex);
		const String key = cacheKey(request, impl->generation);
		if (const auto found = impl->cacheIndex.find(key);
			found != impl->cacheIndex.end())
		{
			impl->cache.splice(impl->cache.begin(), impl->cache, found->second);
			return found->second->value;
		}

		auto output = std::make_shared<UiShapedText>();
		const DecodeResult decoded = decodeUtf8(request.utf8, request.limits);
		output->codepoints = decoded.codepoints;
		output->hadMalformedUtf8 = decoded.malformed;
		output->wasTruncated = decoded.truncated;
		output->registryGeneration = impl->generation;
		SizeT sourceBytes = 0;
		if (!output->codepoints.empty())
		{
			const UiDecodedCodepoint& last = output->codepoints.back();
			sourceBytes = static_cast<SizeT>(last.byteOffset) + last.byteLength;
		}
		output->utf8.assign(request.utf8.data(), sourceBytes);
#ifdef ORKA_UI_ENABLE_ADVANCED_TEXT
		if (!impl->advancedShape(*output, request))
			impl->fallbackShape(*output, request);
#else
		impl->fallbackShape(*output, request);
#endif
		SharedPointer<const UiShapedText> result = output;
		if (impl->config.maximumCacheEntries)
		{
			impl->cache.push_front({ key, result });
			impl->cacheIndex[key] = impl->cache.begin();
			while (impl->cache.size() > impl->config.maximumCacheEntries)
			{
				impl->cacheIndex.erase(impl->cache.back().key);
				impl->cache.pop_back();
			}
		}
		return result;
	}

	void UiTextShaper::clearCache()
	{
		LockGuard lock(impl->mutex);
		impl->cache.clear();
		impl->cacheIndex.clear();
	}

	void UiTextShaper::setMaximumCacheEntries(const SizeT count)
	{
		LockGuard lock(impl->mutex);
		impl->config.maximumCacheEntries = count;
		while (impl->cache.size() > count)
		{
			impl->cacheIndex.erase(impl->cache.back().key);
			impl->cache.pop_back();
		}
	}
}
