#pragma once

#include <algorithm>
#include <cmath>
#include <exception>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#include "Basics.hpp"

namespace Delaunator
{
	constexpr Double epsilon      = std::numeric_limits<Double>::epsilon();
	constexpr SizeT  invalidIndex = std::numeric_limits<SizeT>::max();

	inline SizeT fastMod(const SizeT i, const SizeT c)
	{
		return i >= c ? i % c : i;
	}

	inline Double sum(const Vector<Double>& x)
	{
		Double sum = x[0];
		Double err = 0.0;

		for (SizeT i = 1; i < x.size(); i++)
		{
			const Double k = x[i];
			const Double m = sum + k;
			err += std::fabs(sum) >= std::fabs(k) ? sum - m + k : k - m + sum;
			sum = m;
		}
		return sum + err;
	}

	inline Double dist(const Double ax, const Double ay, const Double bx, const Double by)
	{
		const Double dx = ax - bx;
		const Double dy = ay - by;
		return dx * dx + dy * dy;
	}

	inline Double circumRadius(const Double ax, const Double ay, const Double bx, const Double by, const Double cx, const Double cy)
	{
		const Double dx = bx - ax;
		const Double dy = by - ay;
		const Double ex = cx - ax;
		const Double ey = cy - ay;

		const Double bl = dx * dx + dy * dy;
		const Double cl = ex * ex + ey * ey;
		const Double d  = dx * ey - dy * ex;

		const Double x = (ey * bl - dy * cl) * 0.5 / d;
		const Double y = (dx * cl - ex * bl) * 0.5 / d;

		if ((bl > 0.0 || bl < 0.0) && (cl > 0.0 || cl < 0.0) && (d > 0.0 || d < 0.0)) return x * x + y * y;
		else return std::numeric_limits<Double>::max();
	}

	inline Bool orient(const Double px, const Double py, const Double qx, const Double qy, const Double rx, const Double ry)
	{
		return (qy - py) * (rx - qx) - (qx - px) * (ry - qy) < 0.0;
	}

	inline std::pair<Double, Double> circumCenter(const Double ax, const Double ay, const Double bx, const Double by, const Double cx, const Double cy)
	{
		const Double dx = bx - ax;
		const Double dy = by - ay;
		const Double ex = cx - ax;
		const Double ey = cy - ay;

		const Double bl = dx * dx + dy * dy;
		const Double cl = ex * ex + ey * ey;
		const Double d  = dx * ey - dy * ex;

		const Double x = ax + (ey * bl - dy * cl) * 0.5 / d;
		const Double y = ay + (dx * cl - ex * bl) * 0.5 / d;

		return std::make_pair(x, y);
	}

	struct Compare
	{
		const Vector<Double>& coords;
		Double                cx;
		Double                cy;

		Bool operator()(const SizeT i, const SizeT j) const
		{
			const Double d1    = dist(coords[2 * i], coords[2 * i + 1], cx, cy);
			const Double d2    = dist(coords[2 * j], coords[2 * j + 1], cx, cy);
			const Double diff1 = d1 - d2;
			const Double diff2 = coords[2 * i] - coords[2 * j];
			const Double diff3 = coords[2 * i + 1] - coords[2 * j + 1];

			if (diff1 > 0.0 || diff1 < 0.0) return diff1 < 0;
			if (diff2 > 0.0 || diff2 < 0.0) return diff2 < 0;
			return diff3 < 0;
		}
	};

	inline Bool inCircle(const Double ax, const Double ay, const Double bx, const Double by, const Double cx, const Double cy, const Double px, const Double py)
	{
		const Double dx = ax - px;
		const Double dy = ay - py;
		const Double ex = bx - px;
		const Double ey = by - py;
		const Double fx = cx - px;
		const Double fy = cy - py;

		const Double ap = dx * dx + dy * dy;
		const Double bp = ex * ex + ey * ey;
		const Double cp = fx * fx + fy * fy;

		return dx * (ey * cp - bp * fy) - dy * (ex * cp - bp * fx) + ap * (ex * fy - ey * fx) < 0.0;
	}

	inline Bool checkPtsEqual(const Double x1, const Double y1, const Double x2, const Double y2)
	{
		return std::fabs(x1 - x2) <= epsilon && std::fabs(y1 - y2) <= epsilon;
	}

	inline Double pseudoAngle(const Double dx, const Double dy)
	{
		const Double p = dx / (std::abs(dx) + std::abs(dy));
		return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
	}

	struct DelaunatorPoint
	{
		SizeT  i;
		Double x;
		Double y;
		SizeT  t;
		SizeT  prev;
		SizeT  next;
		Bool   removed;
	};

	class Delaunator
	{
	public:
		Vector<Double> const& coords;
		Vector<SizeT>         triangles;
		Vector<SizeT>         halfedges;
		Vector<SizeT>         hullPrev;
		Vector<SizeT>         hullNext;
		Vector<SizeT>         hullTri;
		SizeT                 hullStart;

		explicit Delaunator(const Vector<Double>& inCoords);

		Double getHullArea() const;

	private:
		Vector<SizeT> mHash;
		Double        mCenterX;
		Double        mCenterY;
		SizeT         mHashSize;
		Vector<SizeT> mEdgeStack;

		SizeT legalize(SizeT a);
		void  link(SizeT a, SizeT b);
		SizeT hashKey(Double x, Double y) const;
		SizeT addTriangle(SizeT i0, SizeT i1, SizeT i2, SizeT a, SizeT b, SizeT c);
	};

	inline Delaunator::Delaunator(const Vector<Double>& inCoords) : coords(inCoords), triangles(), halfedges(), hullPrev(), hullNext(), hullTri(), hullStart(), mHash(), mCenterX(), mCenterY(), mHashSize(), mEdgeStack()
	{
		SizeT n = coords.size() >> 1;

		Double        maxX = std::numeric_limits<Double>::min();
		Double        maxY = std::numeric_limits<Double>::min();
		Double        minX = std::numeric_limits<Double>::max();
		Double        minY = std::numeric_limits<Double>::max();
		Vector<SizeT> ids;
		ids.reserve(n);

		for (SizeT i = 0; i < n; i++)
		{
			const Double x = coords[2 * i];
			const Double y = coords[2 * i + 1];

			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;

			ids.push_back(i);
		}
		const Double cx      = (minX + maxX) / 2;
		const Double cy      = (minY + maxY) / 2;
		Double       minDist = std::numeric_limits<Double>::max();

		SizeT i0 = invalidIndex;
		SizeT i1 = invalidIndex;
		SizeT i2 = invalidIndex;

		// pick a seed point close to the centroid
		for (SizeT i = 0; i < n; i++)
		{
			if (const Double d = dist(cx, cy, coords[2 * i], coords[2 * i + 1]); d < minDist)
			{
				i0      = i;
				minDist = d;
			}
		}

		const Double i0X = coords[2 * i0];
		const Double i0Y = coords[2 * i0 + 1];

		minDist = std::numeric_limits<Double>::max();

		// find the point closest to the seed
		for (SizeT i = 0; i < n; i++)
		{
			if (i == i0) continue;
			if (const Double d = dist(i0X, i0Y, coords[2 * i], coords[2 * i + 1]); d < minDist && d > 0.0)
			{
				i1      = i;
				minDist = d;
			}
		}

		Double i1X = coords[2 * i1];
		Double i1Y = coords[2 * i1 + 1];

		Double minRadius = std::numeric_limits<Double>::max();

		// find the third point which forms the smallest circumcircle with the first two
		for (SizeT i = 0; i < n; i++)
		{
			if (i == i0 || i == i1) continue;

			if (const Double r = circumRadius(i0X, i0Y, i1X, i1Y, coords[2 * i], coords[2 * i + 1]); r < minRadius)
			{
				i2        = i;
				minRadius = r;
			}
		}

		if (!(minRadius < std::numeric_limits<Double>::max())) throw std::runtime_error("not triangulation");

		Double i2X = coords[2 * i2];
		Double i2Y = coords[2 * i2 + 1];

		if (orient(i0X, i0Y, i1X, i1Y, i2X, i2Y))
		{
			std::swap(i1, i2);
			std::swap(i1X, i2X);
			std::swap(i1Y, i2Y);
		}

		std::tie(mCenterX, mCenterY) = circumCenter(i0X, i0Y, i1X, i1Y, i2X, i2Y);

		// sort the points by distance from the seed triangle circumcenter
		std::ranges::sort(ids, Compare{coords, mCenterX, mCenterY});

		// initialize a hash table for storing edges of the advancing convex hull
		mHashSize = static_cast<SizeT>(std::llround(std::ceil(std::sqrt(n))));
		mHash.resize(mHashSize);
		std::ranges::fill(mHash, invalidIndex);

		// initialize arrays for tracking the edges of the advancing convex hull
		hullPrev.resize(n);
		hullNext.resize(n);
		hullTri.resize(n);

		hullStart = i0;

		SizeT hullSize = 3;

		hullNext[i0] = hullPrev[i2] = i1;
		hullNext[i1] = hullPrev[i0] = i2;
		hullNext[i2] = hullPrev[i1] = i0;

		hullTri[i0] = 0;
		hullTri[i1] = 1;
		hullTri[i2] = 2;

		mHash[hashKey(i0X, i0Y)] = i0;
		mHash[hashKey(i1X, i1Y)] = i1;
		mHash[hashKey(i2X, i2Y)] = i2;

		SizeT maxTriangles = n < 3 ? 1 : 2 * n - 5;
		triangles.reserve(maxTriangles * 3);
		halfedges.reserve(maxTriangles * 3);
		addTriangle(i0, i1, i2, invalidIndex, invalidIndex, invalidIndex);
		Double xp = std::numeric_limits<Double>::quiet_NaN();
		Double yp = std::numeric_limits<Double>::quiet_NaN();
		for (SizeT k = 0; k < n; k++)
		{
			const SizeT  i = ids[k];
			const Double x = coords[2 * i];
			const Double y = coords[2 * i + 1];

			// skip near-duplicate points
			if (k > 0 && checkPtsEqual(x, y, xp, yp)) continue;
			xp = x;
			yp = y;

			// skip seed triangle points
			if (checkPtsEqual(x, y, i0X, i0Y) || checkPtsEqual(x, y, i1X, i1Y) || checkPtsEqual(x, y, i2X, i2Y)) continue;

			// find a visible edge on the convex hull using edge hash
			SizeT start = 0;

			SizeT key = hashKey(x, y);
			for (SizeT j = 0; j < mHashSize; j++)
			{
				start = mHash[fastMod(key + j, mHashSize)];
				if (start != invalidIndex && start != hullNext[start]) break;
			}

			start   = hullPrev[start];
			SizeT e = start;
			SizeT q;

			while (q = hullNext[e], !orient(x, y, coords[2 * e], coords[2 * e + 1], coords[2 * q], coords[2 * q + 1]))
			{
				e = q;
				if (e == start)
				{
					e = invalidIndex;
					break;
				}
			}

			if (e == invalidIndex) continue; // likely a near-duplicate point; skip it

			// add the first triangle from the point
			SizeT t = addTriangle(e, i, hullNext[e], invalidIndex, invalidIndex, hullTri[e]);

			hullTri[i] = legalize(t + 2);
			hullTri[e] = t;
			hullSize++;

			// walk forward through the hull, adding more triangles and flipping recursively
			SizeT next = hullNext[e];
			while (q = hullNext[next], orient(x, y, coords[2 * next], coords[2 * next + 1], coords[2 * q], coords[2 * q + 1]))
			{
				t              = addTriangle(next, i, q, hullTri[i], invalidIndex, hullTri[next]);
				hullTri[i]     = legalize(t + 2);
				hullNext[next] = next; // mark as removed
				hullSize--;
				next = q;
			}

			// walk backward from the other side, adding more triangles and flipping
			if (e == start)
				while (q = hullPrev[e], orient(x, y, coords[2 * q], coords[2 * q + 1], coords[2 * e], coords[2 * e + 1]))
				{
					t = addTriangle(q, i, e, invalidIndex, hullTri[e], hullTri[q]);
					legalize(t + 2);
					hullTri[q]  = t;
					hullNext[e] = e; // mark as removed
					hullSize--;
					e = q;
				}

			// update the hull indices
			hullPrev[i]    = e;
			hullStart      = e;
			hullPrev[next] = i;
			hullNext[e]    = i;
			hullNext[i]    = next;

			mHash[hashKey(x, y)]                             = i;
			mHash[hashKey(coords[2 * e], coords[2 * e + 1])] = e;
		}
	}

	inline Double Delaunator::getHullArea() const
	{
		Vector<Double> hullArea;
		SizeT          e = hullStart;
		do
		{
			hullArea.push_back((coords[2 * e] - coords[2 * hullPrev[e]]) * (coords[2 * e + 1] + coords[2 * hullPrev[e] + 1]));
			e = hullNext[e];
		}
		while (e != hullStart);
		return sum(hullArea);
	}

	inline SizeT Delaunator::legalize(SizeT a)
	{
		SizeT i = 0;
		SizeT ar;
		mEdgeStack.clear();

		// recursion eliminated with a fixed-size stack
		while (true)
		{
			const SizeT b = halfedges[a];

			/* if the pair of triangles doesn't satisfy the Delaunay condition
			* (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
			* then do the same check/flip recursively for the new pair of triangles
			*
			*           pl                    pl
			*          /||\                  /  \
			*       al/ || \bl            al/    \a
			*        /  ||  \              /      \
			*       /  a||b  \    flip    /___ar___\
			*     p0\   ||   /p1   =>   p0\---bl---/p1
			*        \  ||  /              \      /
			*       ar\ || /br             b\    /br
			*          \||/                  \  /
			*           pr                    pr
			*/
			const SizeT a0 = 3 * (a / 3);
			ar             = a0 + (a + 2) % 3;

			if (b == invalidIndex)
			{
				if (i > 0)
				{
					i--;
					a = mEdgeStack[i];
					continue;
				}
				else
				{
					//i = INVALID_INDEX;
					break;
				}
			}

			const SizeT b0 = 3 * (b / 3);
			const SizeT al = a0 + (a + 1) % 3;
			const SizeT bl = b0 + (b + 2) % 3;

			const SizeT p0 = triangles[ar];
			const SizeT pr = triangles[a];
			const SizeT pl = triangles[al];
			const SizeT p1 = triangles[bl];

			if (const Bool illegal = inCircle(coords[2 * p0], coords[2 * p0 + 1], coords[2 * pr], coords[2 * pr + 1], coords[2 * pl], coords[2 * pl + 1], coords[2 * p1], coords[2 * p1 + 1]))
			{
				triangles[a] = p1;
				triangles[b] = p0;

				const auto hbl = halfedges[bl];

				// edge swapped on the other side of the hull (rare); fix the halfedge reference
				if (hbl == invalidIndex)
				{
					SizeT e = hullStart;
					do
					{
						if (hullTri[e] == bl)
						{
							hullTri[e] = a;
							break;
						}
						e = hullNext[e];
					}
					while (e != hullStart);
				}
				link(a, hbl);
				link(b, halfedges[ar]);
				link(ar, bl);
				SizeT br = b0 + (b + 1) % 3;

				if (i < mEdgeStack.size()) mEdgeStack[i] = br;
				else mEdgeStack.push_back(br);
				i++;
			}
			else
			{
				if (i > 0)
				{
					i--;
					a = mEdgeStack[i];
					continue;
				}
				break;
			}
		}
		return ar;
	}

	inline SizeT Delaunator::hashKey(const Double x, const Double y) const
	{
		const Double dx = x - mCenterX;
		const Double dy = y - mCenterY;
		return fastMod(static_cast<SizeT>(std::llround(std::floor(pseudoAngle(dx, dy) * static_cast<Double>(mHashSize)))), mHashSize);
	}

	inline SizeT Delaunator::addTriangle(const SizeT i0, const SizeT i1, const SizeT i2, const SizeT a, const SizeT b, const SizeT c)
	{
		const SizeT t = triangles.size();
		triangles.push_back(i0);
		triangles.push_back(i1);
		triangles.push_back(i2);
		link(t, a);
		link(t + 1, b);
		link(t + 2, c);
		return t;
	}

	inline void Delaunator::link(const SizeT a, const SizeT b)
	{
		if (const SizeT s = halfedges.size(); a == s) halfedges.push_back(b);
		else if (a < s) halfedges[a] = b;
		else throw std::runtime_error("Cannot link edge");
		if (b != invalidIndex)
		{
			const SizeT s2 = halfedges.size();
			if (b == s2) halfedges.push_back(a);
			else if (b < s2) halfedges[b] = a;
			else throw std::runtime_error("Cannot link edge");
		}
	}
}
