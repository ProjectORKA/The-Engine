#include "UiSvgPathParser.hpp"

#include "../../Math.hpp"

#include <cmath>
#include <cctype>
#include <functional>

namespace Ui
{
	namespace
	{
		using Vec2 = glm::vec2;

		struct PathCursor
		{
			const Char* data = nullptr;
			SizeT index = 0;
			SizeT length = 0;
		};

		[[nodiscard]] Bool atEnd(const PathCursor& cursor)
		{
			return cursor.index >= cursor.length;
		}

		void skipSeparators(PathCursor& cursor)
		{
			while (!atEnd(cursor))
			{
				const Char value = cursor.data[cursor.index];
				if (value == ' ' || value == '\t' || value == '\n' || value == '\r' ||
					value == ',')
					++cursor.index;
				else break;
			}
		}

		[[nodiscard]] Bool readNumber(PathCursor& cursor, Float& value)
		{
			skipSeparators(cursor);
			if (atEnd(cursor)) return false;
			const Char* start = cursor.data + cursor.index;
			Char* end = nullptr;
			value = std::strtof(start, &end);
			if (end == start) return false;
			cursor.index += static_cast<SizeT>(end - start);
			return true;
		}

		[[nodiscard]] Bool readCommand(PathCursor& cursor, Char& command)
		{
			skipSeparators(cursor);
			if (atEnd(cursor)) return false;
			const Char value = cursor.data[cursor.index];
			if (!std::isalpha(static_cast<unsigned char>(value))) return false;
			command = value;
			++cursor.index;
			return true;
		}

		[[nodiscard]] Bool readPoint(PathCursor& cursor, Vec2& point)
		{
			return readNumber(cursor, point.x) && readNumber(cursor, point.y);
		}

		void flattenCubic(
			const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3,
			Vector<Vec2>& output, const Float tolerance = 0.25f)
		{
			const auto flatEnough = [&](const Vec2& a, const Vec2& b, const Vec2& c) -> Bool
				{
					const Vec2 ab = b - a;
					const Vec2 bc = c - b;
					const Float cross = std::abs(ab.x * bc.y - ab.y * bc.x);
					return cross <= tolerance;
				};

			const std::function<void(Vec2, Vec2, Vec2, Vec2)> subdivide =
				[&](const Vec2 a, const Vec2 b, const Vec2 c, const Vec2 d)
				{
					if (flatEnough(a, b, d) && flatEnough(a, c, d))
					{
						output.push_back(d);
						return;
					}
					const Vec2 ab = (a + b) * 0.5f;
					const Vec2 bc = (b + c) * 0.5f;
					const Vec2 cd = (c + d) * 0.5f;
					const Vec2 abc = (ab + bc) * 0.5f;
					const Vec2 bcd = (bc + cd) * 0.5f;
					const Vec2 abcd = (abc + bcd) * 0.5f;
					subdivide(a, ab, abc, abcd);
					subdivide(abcd, bcd, cd, d);
				};

			subdivide(p0, p1, p2, p3);
		}

		void flattenQuadratic(
			const Vec2& p0, const Vec2& p1, const Vec2& p2,
			Vector<Vec2>& output, const Float tolerance = 0.25f)
		{
			const Vec2 cp1 = p0 + (p1 - p0) * (2.0f / 3.0f);
			const Vec2 cp2 = p2 + (p1 - p2) * (2.0f / 3.0f);
			flattenCubic(p0, cp1, cp2, p2, output, tolerance);
		}

		[[nodiscard]] Float angleBetween(const Vec2& vector)
		{
			return std::atan2(vector.y, vector.x);
		}

		void appendArcPoints(
			Vec2 current, Float rx, Float ry, Float xAxisRotation,
			Bool largeArc, Bool sweep, Vec2 target, Vector<Vec2>& output)
		{
			if (rx <= 0 || ry <= 0)
			{
				output.push_back(target);
				return;
			}

			rx = std::abs(rx);
			ry = std::abs(ry);
			const Float radians = xAxisRotation * 3.14159265358979323846f / 180.0f;
			const Float cosPhi = std::cos(radians);
			const Float sinPhi = std::sin(radians);

			const Vec2 midpoint = (current - target) * 0.5f;
			const Vec2 transformed(
				cosPhi * midpoint.x + sinPhi * midpoint.y,
				-sinPhi * midpoint.x + cosPhi * midpoint.y);

			Float rxSq = rx * rx;
			Float rySq = ry * ry;
			const Float transformedXSq = transformed.x * transformed.x;
			const Float transformedYSq = transformed.y * transformed.y;
			Float radiiCheck = transformedXSq / rxSq + transformedYSq / rySq;
			if (radiiCheck > 1.0f)
			{
				const Float scale = std::sqrt(radiiCheck);
				rx *= scale;
				ry *= scale;
				rxSq = rx * rx;
				rySq = ry * ry;
			}

			Float numerator = rxSq * rySq - rxSq * transformedYSq - rySq * transformedXSq;
			numerator = std::max(0.0f, numerator);
			Float denominator = rxSq * transformedYSq + rySq * transformedXSq;
			Float coefficient = std::sqrt(numerator / denominator);
			if (largeArc == sweep) coefficient = -coefficient;

			const Vec2 centerTransformed(
				coefficient * (rx * transformed.y / ry),
				coefficient * (-ry * transformed.x / rx));
			const Vec2 center(
				cosPhi * centerTransformed.x - sinPhi * centerTransformed.y +
					(current.x + target.x) * 0.5f,
				sinPhi * centerTransformed.x + cosPhi * centerTransformed.y +
					(current.y + target.y) * 0.5f);

			const auto vectorAngle = [](const Vec2& from, const Vec2& to) -> Float
				{
					const Float dot = std::clamp(
						(from.x * to.x + from.y * to.y) /
							(std::sqrt(from.x * from.x + from.y * from.y) *
							 std::sqrt(to.x * to.x + to.y * to.y)),
						-1.0f, 1.0f);
					Float angle = std::acos(dot);
					if (from.x * to.y - from.y * to.x < 0) angle = -angle;
					return angle;
				};

			const Vec2 startVector((current.x - center.x) / rx, (current.y - center.y) / ry);
			const Vec2 endVector((target.x - center.x) / rx, (target.y - center.y) / ry);
			Float sweepAngle = vectorAngle(startVector, endVector);
			constexpr Float twoPi = 6.28318530717958647692f;
			constexpr Float quarterPi = 0.78539816339744830961f;
			if (!sweep && sweepAngle > 0) sweepAngle -= twoPi;
			else if (sweep && sweepAngle < 0) sweepAngle += twoPi;

			const Int segments = std::max(4, static_cast<Int>(std::ceil(
				std::abs(sweepAngle) / quarterPi)));
			for (Int segment = 1; segment <= segments; ++segment)
			{
				const Float t = static_cast<Float>(segment) / static_cast<Float>(segments);
				const Float angle = angleBetween(startVector) + sweepAngle * t;
				const Vec2 pointOnEllipse(std::cos(angle) * rx, std::sin(angle) * ry);
				output.push_back(Vec2(
					cosPhi * pointOnEllipse.x - sinPhi * pointOnEllipse.y + center.x,
					sinPhi * pointOnEllipse.x + cosPhi * pointOnEllipse.y + center.y));
			}
		}
	}

	Bool UiSvgPathParser::parsePathData(const String& data, Vector<UiSvgSubpath>& output)
	{
		output.clear();
		PathCursor cursor{data.c_str(), 0, data.size()};
		Char activeCommand = 0;
		Char pendingCommand = 0;
		Vec2 current(0);
		Vec2 subpathStart(0);
		Vec2 lastQuadraticControl(0);
		UiSvgSubpath* activeSubpath = nullptr;

		const auto beginSubpath = [&](const Vec2& point)
			{
				output.push_back({});
				activeSubpath = &output.back();
				activeSubpath->points.push_back(point);
				subpathStart = point;
				current = point;
			};

		const auto ensureSubpath = [&]()
			{
				if (!activeSubpath) beginSubpath(current);
			};

		while (!atEnd(cursor) || pendingCommand != 0)
		{
			Char command = pendingCommand;
			pendingCommand = 0;
			if (command == 0)
			{
				if (!readCommand(cursor, command))
				{
					if (activeCommand == 0) break;
					command = activeCommand;
				}
				else activeCommand = command;
			}

			const Bool relative = std::islower(static_cast<unsigned char>(command));
			const Char absoluteCommand = static_cast<Char>(std::toupper(
				static_cast<unsigned char>(command)));

			switch (absoluteCommand)
			{
			case 'M':
			{
				Vec2 point;
				if (!readPoint(cursor, point)) return false;
				if (relative) point += current;
				beginSubpath(point);
				skipSeparators(cursor);
				if (!atEnd(cursor) &&
					!std::isalpha(static_cast<unsigned char>(cursor.data[cursor.index])))
					pendingCommand = relative ? 'l' : 'L';
				break;
			}
			case 'L':
			{
				ensureSubpath();
				Vec2 point;
				if (!readPoint(cursor, point)) return false;
				if (relative) point += current;
				activeSubpath->points.push_back(point);
				current = point;
				break;
			}
			case 'H':
			{
				ensureSubpath();
				Float x = 0;
				if (!readNumber(cursor, x)) return false;
				if (relative) x += current.x;
				current.x = x;
				activeSubpath->points.push_back(current);
				break;
			}
			case 'V':
			{
				ensureSubpath();
				Float y = 0;
				if (!readNumber(cursor, y)) return false;
				if (relative) y += current.y;
				current.y = y;
				activeSubpath->points.push_back(current);
				break;
			}
			case 'C':
			{
				ensureSubpath();
				Vec2 control1, control2, target;
				if (!readPoint(cursor, control1) || !readPoint(cursor, control2) ||
					!readPoint(cursor, target)) return false;
				if (relative)
				{
					control1 += current;
					control2 += current;
					target += current;
				}
				flattenCubic(current, control1, control2, target, activeSubpath->points);
				lastQuadraticControl = control2;
				current = target;
				break;
			}
			case 'S':
			{
				ensureSubpath();
				Vec2 control2, target;
				if (!readPoint(cursor, control2) || !readPoint(cursor, target)) return false;
				Vec2 control1 = current * 2.0f - lastQuadraticControl;
				if (relative)
				{
					control2 += current;
					target += current;
				}
				flattenCubic(current, control1, control2, target, activeSubpath->points);
				lastQuadraticControl = control2;
				current = target;
				break;
			}
			case 'Q':
			{
				ensureSubpath();
				Vec2 control, target;
				if (!readPoint(cursor, control) || !readPoint(cursor, target)) return false;
				if (relative)
				{
					control += current;
					target += current;
				}
				flattenQuadratic(current, control, target, activeSubpath->points);
				lastQuadraticControl = control;
				current = target;
				break;
			}
			case 'T':
			{
				ensureSubpath();
				Vec2 target;
				if (!readPoint(cursor, target)) return false;
				const Vec2 control = current * 2.0f - lastQuadraticControl;
				if (relative) target += current;
				flattenQuadratic(current, control, target, activeSubpath->points);
				lastQuadraticControl = control;
				current = target;
				break;
			}
			case 'A':
			{
				ensureSubpath();
				Float rx = 0, ry = 0, rotation = 0;
				Float largeArc = 0, sweep = 0;
				Vec2 target;
				if (!readNumber(cursor, rx) || !readNumber(cursor, ry) ||
					!readNumber(cursor, rotation) || !readNumber(cursor, largeArc) ||
					!readNumber(cursor, sweep) || !readPoint(cursor, target)) return false;
				if (relative) target += current;
				appendArcPoints(
					current, rx, ry, rotation, largeArc != 0, sweep != 0, target,
					activeSubpath->points);
				current = target;
				break;
			}
			case 'Z':
				if (activeSubpath)
				{
					activeSubpath->closed = true;
					current = subpathStart;
				}
				activeSubpath = nullptr;
				break;
			default:
				return false;
			}
		}

		return true;
	}
} // namespace Ui
