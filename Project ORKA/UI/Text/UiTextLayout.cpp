#include "UiTextLayout.hpp"

#include "../Core/UiDocument.hpp"

namespace Ui
{
	namespace
	{
		constexpr UInt invalidTextIndex = maxUInt;

		struct DecodeResult
		{
			Vector<UiDecodedCodepoint> codepoints;
			Bool malformed = false;
			Bool truncated = false;
		};

		struct TextMetrics
		{
			Float ascent = 0;
			Float descent = 0;
			Float gap = 0;
		};

		Bool isContinuation(const Byte value) { return (value & 0xc0) == 0x80; }
		Bool isNewline(const U32 value) { return value == '\n' || value == '\r'; }
		Bool isHorizontalSpace(const U32 value)
		{
			return value == ' ' || value == '\t' || value == '\v' || value == '\f';
		}

		Float nonNegativeFinite(const Float value)
		{
			return std::isfinite(value) ? std::max(value, 0.0f) : 0.0f;
		}

		Float safePositive(const Float value, const Float fallback)
		{
			return std::isfinite(value) && value > 0 ? value : fallback;
		}

		SizeT limitedCount(const SizeT requested, const SizeT limit)
		{
			return std::min(requested, std::min(limit, static_cast<SizeT>(maxUInt)));
		}

		SizeT decodedByteCount(const DecodeResult& decoded)
		{
			if (decoded.codepoints.empty()) return 0;
			const UiDecodedCodepoint& last = decoded.codepoints.back();
			return static_cast<SizeT>(last.byteOffset) + last.byteLength;
		}

		DecodeResult decodeUtf8(const String& text, const UiTextLimits& limits)
		{
			DecodeResult result;
			const SizeT byteCount = limitedCount(text.size(), limits.maximumInputBytes);
			const SizeT codepointLimit = limitedCount(maxUInt, limits.maximumCodepoints);
			result.truncated = byteCount < text.size();
			result.codepoints.reserve(std::min(byteCount, codepointLimit));
			SizeT offset = 0;
			while (offset < byteCount && result.codepoints.size() < codepointLimit)
			{
				const SizeT start = offset;
				const Byte first = static_cast<Byte>(text[offset++]);
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
					Bool valid = start + wanted <= byteCount;
					U32 decodedValue = value;
					for (UInt index = 1; valid && index < wanted; ++index)
					{
						const Byte next = static_cast<Byte>(text[start + index]);
						if (!isContinuation(next)) valid = false;
						else decodedValue = (decodedValue << 6) | (next & 0x3f);
					}
					valid = valid && decodedValue >= minimum && decodedValue <= 0x10ffff &&
						!(decodedValue >= 0xd800 && decodedValue <= 0xdfff);
					if (valid)
					{
						value = decodedValue;
						length = wanted;
						offset = start + wanted;
					}
					else
					{
						value = 0xfffd;
						result.malformed = true;
					}
				}
				result.codepoints.push_back({
					value, static_cast<UInt>(start), length
				});
			}
			if (offset < byteCount) result.truncated = true;
			return result;
		}

		TextMetrics metricsFor(const UiPreparedText& text)
		{
			TextMetrics result;
			for (const UiShapedSegment& segment : text.segments())
			{
				result.ascent = std::max(result.ascent, nonNegativeFinite(segment.ascent));
				result.descent = std::max(result.descent, nonNegativeFinite(segment.descent));
				result.gap = std::max(result.gap, nonNegativeFinite(segment.lineGap));
			}
			if (result.ascent + result.descent <= 0)
			{
				result.ascent = text.unitsPerEm() * 0.8f;
				result.descent = text.unitsPerEm() * 0.2f;
			}
			return result;
		}

		Bool isNormalBreak(const UiTextBreak value)
		{
			return value == UiTextBreak::Opportunity || value == UiTextBreak::Mandatory;
		}

		Bool isFallbackBreak(const UiTextBreak value)
		{
			return value != UiTextBreak::None;
		}

		Float sumAdvance(const UiPreparedText& text, const UInt first, const UInt end)
		{
			Float result = 0;
			for (UInt index = first; index < end; ++index)
				result += text.codepointAdvance(index);
			return result;
		}

		void setLineRanges(const UiPreparedText& text, UiTextLine& line)
		{
			const UInt end = line.firstCodepoint + line.codepointCount;
			line.firstByte = text.byteOffset(line.firstCodepoint);
			line.byteCount = text.byteOffset(end) - line.firstByte;
			line.firstGlyph = 0;
			line.glyphCount = 0;
			Bool foundGlyph = false;
			for (UInt index = 0; index < text.glyphs().size(); ++index)
			{
				const UInt cluster = text.glyphs()[index].codepointIndex;
				if (cluster >= line.firstCodepoint && cluster < end)
				{
					if (!foundGlyph) { line.firstGlyph = index; foundGlyph = true; }
					line.glyphCount = index - line.firstGlyph + 1;
				}
			}
			line.firstSegment = 0;
			line.segmentCount = 0;
			Bool foundSegment = false;
			for (UInt index = 0; index < text.segments().size(); ++index)
			{
				const UiShapedSegment& segment = text.segments()[index];
				const UInt segmentEnd = segment.firstCodepoint + segment.codepointCount;
				if (segment.firstCodepoint < end && segmentEnd > line.firstCodepoint)
				{
					if (!foundSegment) { line.firstSegment = index; foundSegment = true; }
					line.segmentCount = index - line.firstSegment + 1;
				}
			}
		}

		Bool resultFits(const UiTextLayoutResult& result, const Vec2 box)
		{
			return !result.overflowed &&
				result.size.x <= box.x + 0.001f &&
				result.size.y <= box.y + 0.001f;
		}
	}

	UiPreparedText UiPreparedText::prepareUtf8(
		const String& utf8, const UiTextPrepareOptions& rawOptions)
	{
		UiPreparedText result;
		result.limits = rawOptions.limits;
		const SizeT inputBytes = limitedCount(utf8.size(), result.limits.maximumInputBytes);
		result.source.assign(utf8.data(), inputBytes);
		result.designUnitsPerEm = safePositive(rawOptions.designUnitsPerEm, 1000);
		DecodeResult decodedSource = decodeUtf8(utf8, result.limits);
		result.source.resize(std::min(result.source.size(), decodedByteCount(decodedSource)));
		result.malformedUtf8 = decodedSource.malformed;
		result.truncated = decodedSource.truncated;
		result.decoded.reserve(decodedSource.codepoints.size());

		Bool previousWasSpace = false;
		for (SizeT sourceIndex = 0; sourceIndex < decodedSource.codepoints.size(); ++sourceIndex)
		{
			UiDecodedCodepoint codepoint = decodedSource.codepoints[sourceIndex];
			if (codepoint.value == '\r')
			{
				codepoint.value = '\n';
				if (sourceIndex + 1 < decodedSource.codepoints.size() &&
					decodedSource.codepoints[sourceIndex + 1].value == '\n')
				{
					codepoint.byteLength += decodedSource.codepoints[sourceIndex + 1].byteLength;
					++sourceIndex;
				}
			}

			const Bool collapsible = isHorizontalSpace(codepoint.value) ||
				(rawOptions.whitespace == UiWhitespaceMode::Collapse && isNewline(codepoint.value));
			if (rawOptions.whitespace != UiWhitespaceMode::Preserve && collapsible)
			{
				if (previousWasSpace) continue;
				codepoint.value = ' ';
				previousWasSpace = true;
			}
			else
			{
				previousWasSpace = false;
			}
			result.decoded.push_back(codepoint);
		}

		const SizeT itemLimit = std::min({
			result.decoded.size(),
			limitedCount(maxUInt, result.limits.maximumGlyphs),
			limitedCount(maxUInt, result.limits.maximumSegments)
		});
		if (result.decoded.size() > itemLimit)
		{
			result.decoded.resize(itemLimit);
			result.truncated = true;
		}
		result.shapedGlyphs.reserve(result.decoded.size());
		result.shapedSegments.reserve(result.decoded.size());
		for (UInt index = 0; index < result.decoded.size(); ++index)
		{
			const U32 current = result.decoded[index].value;
			UiTextBreak breakBefore = index == 0 ? UiTextBreak::Mandatory : UiTextBreak::Grapheme;
			if (index > 0)
			{
				const U32 previous = result.decoded[index - 1].value;
				if (isNewline(previous)) breakBefore = UiTextBreak::Mandatory;
				else if (rawOptions.breakOpportunity)
				{
					const UiTextBreak supplied = rawOptions.breakOpportunity(previous, current, index);
					if (supplied == UiTextBreak::Opportunity || supplied == UiTextBreak::Mandatory)
						breakBefore = supplied;
				}
				else if (previous == ' ' || previous == '-' || previous == 0x200b)
					breakBefore = UiTextBreak::Opportunity;
			}
			Float advance = 0;
			if (!isNewline(current))
			{
				advance = rawOptions.glyphAdvance ? rawOptions.glyphAdvance(current) :
					(current == ' ' ? result.designUnitsPerEm * 0.333f :
						result.designUnitsPerEm * 0.6f);
				advance = nonNegativeFinite(advance);
			}
			result.shapedGlyphs.push_back({ current, index, advance });
			UiShapedSegment segment;
			segment.firstCodepoint = index;
			segment.codepointCount = 1;
			segment.firstGlyph = index;
			segment.glyphCount = 1;
			segment.ascent = nonNegativeFinite(rawOptions.ascent);
			segment.descent = nonNegativeFinite(rawOptions.descent);
			segment.lineGap = nonNegativeFinite(rawOptions.lineGap);
			segment.breakBefore = breakBefore;
			result.shapedSegments.push_back(std::move(segment));
		}
		result.rebuildPreparedData();
		return result;
	}

	UiPreparedText UiPreparedText::fromShaped(
		const String& utf8, Vector<UiShapedGlyph> glyphs,
		Vector<UiShapedSegment> segments, const Float unitsPerEm,
		const UiTextLimits& suppliedLimits)
	{
		UiPreparedText result;
		result.limits = suppliedLimits;
		const SizeT inputBytes = limitedCount(utf8.size(), result.limits.maximumInputBytes);
		result.source.assign(utf8.data(), inputBytes);
		DecodeResult decode = decodeUtf8(utf8, result.limits);
		result.source.resize(std::min(result.source.size(), decodedByteCount(decode)));
		result.decoded = std::move(decode.codepoints);
		result.malformedUtf8 = decode.malformed;
		result.truncated = decode.truncated;
		result.designUnitsPerEm = safePositive(unitsPerEm, 1000);

		const SizeT glyphLimit = limitedCount(glyphs.size(), result.limits.maximumGlyphs);
		result.truncated = result.truncated || glyphLimit < glyphs.size();
		glyphs.resize(glyphLimit);
		for (UiShapedGlyph& glyph : glyphs)
		{
			if (result.decoded.empty()) glyph.codepointIndex = 0;
			else glyph.codepointIndex = std::min(
				glyph.codepointIndex, static_cast<UInt>(result.decoded.size() - 1));
			glyph.advance = nonNegativeFinite(glyph.advance);
		}
		if (result.decoded.empty()) glyphs.clear();
		result.shapedGlyphs = std::move(glyphs);

		const SizeT segmentLimit = limitedCount(segments.size(), result.limits.maximumSegments);
		result.truncated = result.truncated || segmentLimit < segments.size();
		segments.resize(segmentLimit);
		const UInt codepointCount = static_cast<UInt>(result.decoded.size());
		const UInt glyphCount = static_cast<UInt>(result.shapedGlyphs.size());
		for (UiShapedSegment& segment : segments)
		{
			segment.firstCodepoint = std::min(segment.firstCodepoint, codepointCount);
			segment.codepointCount = std::min(
				segment.codepointCount, codepointCount - segment.firstCodepoint);
			segment.firstGlyph = std::min(segment.firstGlyph, glyphCount);
			segment.glyphCount = std::min(
				segment.glyphCount, glyphCount - segment.firstGlyph);
			segment.ascent = nonNegativeFinite(segment.ascent);
			segment.descent = nonNegativeFinite(segment.descent);
			segment.lineGap = nonNegativeFinite(segment.lineGap);
			std::erase_if(segment.graphemeBreaks, [&](const UInt offset)
				{
					return offset == 0 || offset >= segment.codepointCount;
				});
			std::sort(segment.graphemeBreaks.begin(), segment.graphemeBreaks.end());
			segment.graphemeBreaks.erase(
				std::unique(segment.graphemeBreaks.begin(), segment.graphemeBreaks.end()),
				segment.graphemeBreaks.end());
		}
		std::stable_sort(segments.begin(), segments.end(),
			[](const UiShapedSegment& left, const UiShapedSegment& right)
			{
				return left.firstCodepoint < right.firstCodepoint;
			});
		result.shapedSegments = std::move(segments);
		result.rebuildPreparedData();
		return result;
	}

	void UiPreparedText::rebuildPreparedData()
	{
		codepointAdvances.assign(decoded.size(), 0);
		for (const UiShapedGlyph& glyph : shapedGlyphs)
			if (glyph.codepointIndex < codepointAdvances.size())
				codepointAdvances[glyph.codepointIndex] += nonNegativeFinite(glyph.advance);

		breaksBefore.assign(decoded.size() + 1, UiTextBreak::None);
		codepointSegments.assign(decoded.size(), invalidTextIndex);
		segmentAdvances.assign(shapedSegments.size(), 0);
		for (UInt segmentIndex = 0; segmentIndex < shapedSegments.size(); ++segmentIndex)
		{
			const UiShapedSegment& segment = shapedSegments[segmentIndex];
			if (segment.firstCodepoint < breaksBefore.size())
			{
				UiTextBreak value = segment.breakBefore;
				if (value == UiTextBreak::None) value = UiTextBreak::Grapheme;
				if (static_cast<Byte>(value) > static_cast<Byte>(breaksBefore[segment.firstCodepoint]))
					breaksBefore[segment.firstCodepoint] = value;
			}
			for (const UInt relativeBreak : segment.graphemeBreaks)
			{
				const UInt boundary = segment.firstCodepoint + relativeBreak;
				if (boundary < breaksBefore.size() && breaksBefore[boundary] == UiTextBreak::None)
					breaksBefore[boundary] = UiTextBreak::Grapheme;
			}
			const UInt codepointEnd = segment.firstCodepoint + segment.codepointCount;
			for (UInt index = segment.firstCodepoint; index < codepointEnd; ++index)
				if (index < codepointSegments.size() && codepointSegments[index] == invalidTextIndex)
					codepointSegments[index] = segmentIndex;
			const UInt glyphEnd = segment.firstGlyph + segment.glyphCount;
			for (UInt index = segment.firstGlyph; index < glyphEnd && index < shapedGlyphs.size(); ++index)
				segmentAdvances[segmentIndex] += shapedGlyphs[index].advance;
		}
		if (!breaksBefore.empty()) breaksBefore[0] = UiTextBreak::Mandatory;
	}

	Float UiPreparedText::codepointAdvance(const UInt index) const
	{
		return index < codepointAdvances.size() ? codepointAdvances[index] : 0;
	}

	Float UiPreparedText::segmentAdvance(const UInt index) const
	{
		return index < segmentAdvances.size() ? segmentAdvances[index] : 0;
	}

	UInt UiPreparedText::byteOffset(const UInt codepointIndex) const
	{
		return codepointIndex < decoded.size() ? decoded[codepointIndex].byteOffset :
			static_cast<UInt>(source.size());
	}

	UInt UiPreparedText::segmentAt(const UInt codepointIndex) const
	{
		return codepointIndex < codepointSegments.size() ?
			codepointSegments[codepointIndex] : invalidTextIndex;
	}

	UiTextLayoutResult UiTextLineLayout::layout(
		const Float rawAvailableWidth, const UiTextLayoutOptions& rawOptions) const
	{
		UiTextLayoutResult result;
		UiTextLayoutOptions options = rawOptions;
		options.fontSize = nonNegativeFinite(options.fontSize);
		result.fontSize = options.fontSize;
		result.limited = text.wasTruncated();
		const Float scale = options.fontSize / safePositive(text.unitsPerEm(), 1000);
		const Float availableWidth = std::isfinite(rawAvailableWidth) ?
			std::max(0.0f, rawAvailableWidth) : uiLayoutInfinity;
		const TextMetrics metrics = metricsFor(text);
		const Float naturalHeight = (metrics.ascent + metrics.descent + metrics.gap) * scale;
		const Float lineHeight = safePositive(options.lineHeight, naturalHeight);
		const Float baseline = metrics.ascent * scale +
			std::max(0.0f, lineHeight - naturalHeight) * 0.5f;
		const UInt count = static_cast<UInt>(text.codepoints().size());
		const UInt lineLimit = std::min(options.maximumLines, text.appliedLimits().maximumLines);
		result.lines.reserve(std::min<UInt>(count + 1, lineLimit));

		UInt start = 0;
		while (start < count && result.lines.size() < lineLimit)
		{
			UInt index = start;
			UInt normalBreak = invalidTextIndex;
			UInt fallbackBreak = invalidTextIndex;
			Float widthAtNormal = 0;
			Float widthAtFallback = 0;
			Float width = 0;
			Bool explicitBreak = false;
			while (index < count)
			{
				const U32 value = text.codepoints()[index].value;
				const UiTextBreak boundary = text.breaks()[index];
				if (isNewline(value) || (index > start && boundary == UiTextBreak::Mandatory))
				{
					explicitBreak = true;
					break;
				}
				if (index > start)
				{
					if (isNormalBreak(boundary))
					{
						normalBreak = index;
						widthAtNormal = width;
					}
					if (isFallbackBreak(boundary))
					{
						fallbackBreak = index;
						widthAtFallback = width;
					}
				}

				const Float nextWidth = width + text.codepointAdvance(index) * scale;
				if (std::isfinite(availableWidth) && nextWidth > availableWidth)
				{
					if (options.wrap != UiTextWrap::KeepAll && normalBreak != invalidTextIndex)
					{
						index = normalBreak;
						width = widthAtNormal;
						break;
					}
					if (options.wrap == UiTextWrap::GraphemeFallback &&
						fallbackBreak != invalidTextIndex)
					{
						index = fallbackBreak;
						width = widthAtFallback;
						break;
					}
					if (options.wrap == UiTextWrap::GraphemeFallback)
					{
						width = nextWidth;
						++index;
						break;
					}
					result.overflowed = true;
				}
				width = nextWidth;
				++index;
			}

			UiTextLine line;
			line.firstCodepoint = start;
			line.codepointCount = index - start;
			line.width = width;
			line.y = static_cast<Float>(result.lines.size()) * lineHeight;
			line.height = lineHeight;
			line.baseline = baseline;
			setLineRanges(text, line);
			result.lines.push_back(line);

			if (explicitBreak && index < count && isNewline(text.codepoints()[index].value)) ++index;
			if (index <= start) ++index;
			start = index;
		}

		const Bool trailingNewline = count > 0 && isNewline(text.codepoints().back().value);
		if ((count == 0 || trailingNewline) && result.lines.size() < lineLimit)
		{
			UiTextLine line;
			line.firstCodepoint = count;
			line.firstByte = text.byteOffset(count);
			line.y = static_cast<Float>(result.lines.size()) * lineHeight;
			line.height = lineHeight;
			line.baseline = baseline;
			result.lines.push_back(line);
		}
		if (start < count || ((count == 0 || trailingNewline) && result.lines.size() >= lineLimit))
		{
			result.overflowed = true;
			result.limited = true;
		}

		if (result.overflowed && options.ellipsis && !result.lines.empty())
		{
			UiTextLine& line = result.lines.back();
			Float ellipsis = nonNegativeFinite(options.ellipsisAdvance);
			if (ellipsis <= 0)
			{
				ellipsis = text.unitsPerEm() * 0.6f * 3;
				for (UInt index = 0; index < text.codepoints().size(); ++index)
					if (text.codepoints()[index].value == '.')
					{
						ellipsis = text.codepointAdvance(index) * 3;
						break;
					}
			}
			ellipsis *= scale;
			UInt end = line.firstCodepoint + line.codepointCount;
			while (end > line.firstCodepoint && std::isfinite(availableWidth) &&
				sumAdvance(text, line.firstCodepoint, end) * scale + ellipsis > availableWidth)
			{
				--end;
				while (end > line.firstCodepoint &&
					text.breaks()[end] == UiTextBreak::None) --end;
			}
			line.codepointCount = end - line.firstCodepoint;
			line.width = sumAdvance(text, line.firstCodepoint, end) * scale + ellipsis;
			line.ellipsized = true;
			setLineRanges(text, line);
		}

		for (const UiTextLine& line : result.lines)
			result.size.x = std::max(result.size.x, line.width);
		result.size.y = lineHeight * static_cast<Float>(result.lines.size());
		result.baseline = result.lines.empty() ? -1 : baseline;
		return result;
	}

	UiTextLayoutResult UiTextLineLayout::fit(
		const Vec2 definiteBox, const UiTextLayoutOptions& options,
		const UiTextFitOptions& rawFit) const
	{
		const Bool definite = std::isfinite(definiteBox.x) && std::isfinite(definiteBox.y) &&
			definiteBox.x >= 0 && definiteBox.y >= 0;
		if (rawFit.mode == UiTextFit::None)
		{
			UiTextLayoutResult result = layout(definite ? definiteBox.x : uiLayoutInfinity, options);
			if (definite && result.size.y > definiteBox.y + 0.001f) result.overflowed = true;
			return result;
		}
		if (!definite)
		{
			const Float width = std::isfinite(definiteBox.x) && definiteBox.x >= 0 ?
				definiteBox.x : uiLayoutInfinity;
			UiTextLayoutOptions fallbackOptions = options;
			const Float minimum = nonNegativeFinite(rawFit.minimumSize);
			const Float maximum = std::max(minimum, nonNegativeFinite(rawFit.maximumSize));
			fallbackOptions.fontSize = std::clamp(
				nonNegativeFinite(rawFit.preferredSize), minimum, maximum);
			UiTextLayoutResult result = layout(width, fallbackOptions);
			if (std::isfinite(definiteBox.y) && definiteBox.y >= 0 &&
				result.size.y > definiteBox.y + 0.001f) result.overflowed = true;
			return result;
		}

		const Float quantum = safePositive(rawFit.quantization, 1.0f / 64.0f);
		const Float minimum = nonNegativeFinite(rawFit.minimumSize);
		const Float maximum = std::max(minimum, nonNegativeFinite(rawFit.maximumSize));
		const Float preferred = std::clamp(
			nonNegativeFinite(rawFit.preferredSize), minimum, maximum);
		auto downTick = [&](const Float value)
			{
				const LDouble ratio = static_cast<LDouble>(value) / quantum + 0.0001L;
				const LDouble limit = static_cast<LDouble>(std::numeric_limits<U64>::max());
				return ratio >= limit ? std::numeric_limits<U64>::max() :
					static_cast<U64>(std::floor(ratio));
			};
		auto upTick = [&](const Float value)
			{
				const LDouble ratio = static_cast<LDouble>(value) / quantum - 0.0001L;
				const LDouble limit = static_cast<LDouble>(std::numeric_limits<U64>::max());
				return ratio >= limit ? std::numeric_limits<U64>::max() :
					static_cast<U64>(std::ceil(std::max<LDouble>(ratio, 0)));
			};
		const U64 minimumTick = upTick(minimum);
		const U64 preferredTick = std::max(minimumTick, downTick(preferred));
		const U64 maximumTick = std::max(preferredTick, downTick(maximum));
		UiTextLayoutOptions trialOptions = options;
		auto atTick = [&](const U64 tick)
			{
				trialOptions.fontSize = static_cast<Float>(tick) * quantum;
				return layout(definiteBox.x, trialOptions);
			};

		if (rawFit.mode == UiTextFit::Shrink)
		{
			UiTextLayoutResult preferredResult = atTick(preferredTick);
			if (resultFits(preferredResult, definiteBox)) return preferredResult;
		}

		UiTextLayoutResult best = atTick(minimumTick);
		if (!resultFits(best, definiteBox))
		{
			best.overflowed = true;
			return best;
		}

		U64 low = minimumTick;
		U64 high = rawFit.mode == UiTextFit::Shrink ? preferredTick : maximumTick;
		const UInt iterationLimit = std::min<UInt>(rawFit.maximumIterations, 64);
		for (UInt iteration = 0; low < high && iteration < iterationLimit; ++iteration)
		{
			const U64 distance = high - low;
			const U64 middle = low + distance / 2 + distance % 2;
			UiTextLayoutResult candidate = atTick(middle);
			if (resultFits(candidate, definiteBox))
			{
				low = middle;
				best = std::move(candidate);
			}
			else high = middle - 1;
		}
		return best;
	}

	UiTextIntrinsicWidths UiTextLineLayout::intrinsicWidths(
		const Float rawFontSize, const UiTextWrap wrap) const
	{
		const Float scale = nonNegativeFinite(rawFontSize) /
			safePositive(text.unitsPerEm(), 1000);
		Float maximum = 0;
		Float mandatoryRun = 0;
		Float minimum = 0;
		Float breakableRun = 0;
		for (UInt index = 0; index < text.codepoints().size(); ++index)
		{
			const UiTextBreak boundary = text.breaks()[index];
			const Bool mandatory = isNewline(text.codepoints()[index].value) ||
				(index > 0 && boundary == UiTextBreak::Mandatory);
			if (mandatory)
			{
				maximum = std::max(maximum, mandatoryRun);
				minimum = std::max(minimum, breakableRun);
				mandatoryRun = 0;
				breakableRun = 0;
				if (isNewline(text.codepoints()[index].value)) continue;
			}
			const Bool mayBreak = index > 0 &&
				(wrap == UiTextWrap::GraphemeFallback ?
					isFallbackBreak(boundary) : isNormalBreak(boundary));
			if (wrap != UiTextWrap::KeepAll && mayBreak)
			{
				minimum = std::max(minimum, breakableRun);
				breakableRun = 0;
			}
			const Float advance = text.codepointAdvance(index);
			mandatoryRun += advance;
			breakableRun += advance;
		}
		maximum = std::max(maximum, mandatoryRun);
		minimum = wrap == UiTextWrap::KeepAll ? maximum :
			std::max(minimum, breakableRun);
		return { minimum * scale, maximum * scale };
	}

	UiIntrinsicMeasure UiTextLineLayout::measure(
		const UiConstraints& constraints, const UiTextLayoutOptions& options,
		const UiTextFitOptions& fitOptions) const
	{
		const Float measurementSize = fitOptions.mode == UiTextFit::None ?
			options.fontSize : std::clamp(nonNegativeFinite(fitOptions.preferredSize),
				nonNegativeFinite(fitOptions.minimumSize),
				std::max(nonNegativeFinite(fitOptions.minimumSize),
					nonNegativeFinite(fitOptions.maximumSize)));
		const UiTextIntrinsicWidths widths = intrinsicWidths(measurementSize, options.wrap);
		UiTextLayoutOptions measuredOptions = options;
		measuredOptions.fontSize = measurementSize;

		UiTextLayoutResult arranged;
		if (fitOptions.mode != UiTextFit::None &&
			constraints.hasBoundedWidth() && constraints.hasBoundedHeight())
			arranged = fit(constraints.maximum, options, fitOptions);
		else
			arranged = layout(constraints.hasBoundedWidth() ?
				constraints.maximum.x : uiLayoutInfinity, measuredOptions);
		return {
			Vec2(widths.minimum, arranged.size.y),
			constraints.constrain(arranged.size),
			arranged.baseline
		};
	}

	Function<UiIntrinsicMeasure(const UiNode&, const UiConstraints&)>
		makeUiTextIntrinsicMeasurer(const UiPreparedText& text,
			UiTextLayoutOptions options, UiTextFitOptions fitOptions)
	{
		return [prepared = text, options, fitOptions](
			const UiNode&, const UiConstraints& constraints)
			{
				return UiTextLineLayout(prepared).measure(constraints, options, fitOptions);
			};
	}
}
