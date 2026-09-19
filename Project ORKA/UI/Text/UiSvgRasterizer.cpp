#include "UiSvgRasterizer.hpp"

#include "UiSvgPathParser.hpp"

#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>

namespace Ui
{
	namespace
	{
		using Vec2 = glm::vec2;

		[[nodiscard]] Bool extractAttribute(
			const String& source, const String& attribute, String& value)
		{
			String lowerSource = source;
			for (Char& character : lowerSource)
				character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
			const String lowerAttribute = [&]
				{
					String copy = attribute;
					for (Char& character : copy)
						character = static_cast<Char>(std::tolower(static_cast<unsigned char>(character)));
					return copy;
				}();
			const String token = lowerAttribute + "=\"";
			const SizeT start = lowerSource.find(token);
			if (start == String::npos) return false;
			const SizeT contentStart = start + token.size();
			const SizeT end = source.find('"', contentStart);
			if (end == String::npos) return false;
			value = source.substr(contentStart, end - contentStart);
			return true;
		}

		[[nodiscard]] Bool extractViewBox(const String& source, Float& width, Float& height)
		{
			String value;
			if (!extractAttribute(source, "viewBox", value)) return false;
			Float x = 0, y = 0;
			if (std::sscanf(value.c_str(), "%f %f %f %f", &x, &y, &width, &height) != 4)
				return false;
			return width > 0 && height > 0;
		}

		[[nodiscard]] Bool extractStrokeWidth(const String& source, Float& strokeWidth)
		{
			String value;
			if (!extractAttribute(source, "stroke-width", value)) return false;
			strokeWidth = std::strtof(value.c_str(), nullptr);
			return strokeWidth > 0;
		}

		void collectPathData(const String& source, Vector<String>& paths)
		{
			SizeT searchFrom = 0;
			while (true)
			{
				const SizeT pathStart = source.find("<path", searchFrom);
				if (pathStart == String::npos) break;
				const SizeT pathEnd = source.find("/>", pathStart);
				const SizeT closeEnd = source.find("</path>", pathStart);
				SizeT end = pathEnd;
				if (closeEnd != String::npos && (pathEnd == String::npos || closeEnd < pathEnd))
					end = closeEnd;
				if (end == String::npos) break;

				const String pathTag = source.substr(pathStart, end - pathStart);
				String data;
				if (extractAttribute(pathTag, "d", data) && !data.empty())
					paths.push_back(data);
				searchFrom = end + 1;
			}
		}

		[[nodiscard]] Float distanceToSegment(const Vec2& point, const Vec2& a, const Vec2& b)
		{
			const Vec2 ab = b - a;
			const Float lengthSquared = ab.x * ab.x + ab.y * ab.y;
			if (lengthSquared <= 1e-6f) return glm::length(point - a);
			const Float t = std::clamp(
				((point.x - a.x) * ab.x + (point.y - a.y) * ab.y) / lengthSquared, 0.0f, 1.0f);
			const Vec2 projection = a + ab * t;
			return glm::length(point - projection);
		}

		void stampDisk(
			Vector<Byte>& pixels, UInt width, UInt height, Vec2 center, Float radius)
		{
			const Int minX = std::max(0, static_cast<Int>(std::floor(center.x - radius - 1)));
			const Int maxX = std::min(static_cast<Int>(width) - 1,
				static_cast<Int>(std::ceil(center.x + radius + 1)));
			const Int minY = std::max(0, static_cast<Int>(std::floor(center.y - radius - 1)));
			const Int maxY = std::min(static_cast<Int>(height) - 1,
				static_cast<Int>(std::ceil(center.y + radius + 1)));
			for (Int y = minY; y <= maxY; ++y)
			{
				for (Int x = minX; x <= maxX; ++x)
				{
					const Float distance = glm::length(
						Vec2(static_cast<Float>(x) + 0.5f, static_cast<Float>(y) + 0.5f) - center);
					const Float coverage = std::clamp(radius + 0.75f - distance, 0.0f, 1.0f);
					if (coverage <= 0) continue;
					const SizeT index = (static_cast<SizeT>(y) * width + static_cast<SizeT>(x)) * 4;
					const Byte alpha = static_cast<Byte>(std::min(255.0f, coverage * 255.0f));
					pixels[index + 3] = std::max(pixels[index + 3], alpha);
					if (pixels[index + 3] > 0)
					{
						pixels[index + 0] = 255;
						pixels[index + 1] = 255;
						pixels[index + 2] = 255;
					}
				}
			}
		}

		void rasterizeStroke(
			Vector<Byte>& pixels, UInt width, UInt height,
			const UiSvgDocument& document, Float scale, Float strokeRadius)
		{
			for (const UiSvgSubpath& subpath : document.subpaths)
			{
				if (subpath.points.size() < 2) continue;
				for (SizeT index = 1; index < subpath.points.size(); ++index)
				{
					const Vec2 start = subpath.points[index - 1] * scale;
					const Vec2 end = subpath.points[index] * scale;
					stampDisk(pixels, width, height, start, strokeRadius);
					stampDisk(pixels, width, height, end, strokeRadius);

					const Float boundsRadius = strokeRadius + 2.0f;
					const Int minX = std::max(0, static_cast<Int>(std::floor(
						std::min(start.x, end.x) - boundsRadius)));
					const Int maxX = std::min(static_cast<Int>(width) - 1, static_cast<Int>(std::ceil(
						std::max(start.x, end.x) + boundsRadius)));
					const Int minY = std::max(0, static_cast<Int>(std::floor(
						std::min(start.y, end.y) - boundsRadius)));
					const Int maxY = std::min(static_cast<Int>(height) - 1, static_cast<Int>(std::ceil(
						std::max(start.y, end.y) + boundsRadius)));

					for (Int y = minY; y <= maxY; ++y)
					{
						for (Int x = minX; x <= maxX; ++x)
						{
							const Vec2 point(static_cast<Float>(x) + 0.5f, static_cast<Float>(y) + 0.5f);
							const Float distance = distanceToSegment(point, start, end);
							const Float coverage = std::clamp(strokeRadius + 0.75f - distance, 0.0f, 1.0f);
							if (coverage <= 0) continue;
							const SizeT pixelIndex =
								(static_cast<SizeT>(y) * width + static_cast<SizeT>(x)) * 4;
							const Byte alpha = static_cast<Byte>(std::min(255.0f, coverage * 255.0f));
							pixels[pixelIndex + 3] = std::max(pixels[pixelIndex + 3], alpha);
							if (pixels[pixelIndex + 3] > 0)
							{
								pixels[pixelIndex + 0] = 255;
								pixels[pixelIndex + 1] = 255;
								pixels[pixelIndex + 2] = 255;
							}
						}
					}
				}

				if (subpath.closed && subpath.points.size() >= 2)
				{
					const Vec2 start = subpath.points.back() * scale;
					const Vec2 end = subpath.points.front() * scale;
					stampDisk(pixels, width, height, start, strokeRadius);
					stampDisk(pixels, width, height, end, strokeRadius);
				}
			}
		}
	}

	Bool UiSvgRasterizer::loadDocument(const Path& path, UiSvgDocument& output)
	{
		std::ifstream stream(path, std::ios::binary | std::ios::ate);
		if (!stream.is_open()) return false;
		const std::streamoff size = stream.tellg();
		if (size <= 0 || size > 256 * 1024) return false;
		String source(static_cast<SizeT>(size), '\0');
		stream.seekg(0, std::ios::beg);
		stream.read(source.data(), static_cast<std::streamsize>(size));
		output = {};
		output.viewBoxWidth = 24;
		output.viewBoxHeight = 24;
		output.strokeWidth = 2;
		(void)extractViewBox(source, output.viewBoxWidth, output.viewBoxHeight);
		(void)extractStrokeWidth(source, output.strokeWidth);

		Vector<String> pathData;
		collectPathData(source, pathData);
		for (const String& data : pathData)
		{
			Vector<UiSvgSubpath> parsed;
			if (!UiSvgPathParser::parsePathData(data, parsed)) continue;
			for (UiSvgSubpath& entry : parsed)
				if (!entry.points.empty()) output.subpaths.push_back(std::move(entry));
		}
		return !output.subpaths.empty();
	}

	Bool UiSvgRasterizer::rasterize(
		const UiSvgDocument& document, const UInt pixelSize, UiSvgRasterResult& output)
	{
		if (pixelSize == 0 || document.subpaths.empty()) return false;
		output.width = pixelSize;
		output.height = pixelSize;
		output.rgba.assign(static_cast<SizeT>(pixelSize) * pixelSize * 4, 0);

		const Float scale = static_cast<Float>(pixelSize) /
			std::max(document.viewBoxWidth, document.viewBoxHeight);
		const Float strokeRadius = std::max(1.0f, document.strokeWidth * scale * 0.5f);
		rasterizeStroke(output.rgba, pixelSize, pixelSize, document, scale, strokeRadius);
		return true;
	}
} // namespace Ui
