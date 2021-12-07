#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f

template<typename T>
T sq(T a) {
	return (a * a);
}

template<typename T>
T max(T a, T b) {
	return (a < b) ? b : a;
}

template <typename T>
T min(T a, T b) {
	return !(b < a) ? a : b;
}

template <typename T>
T clamp(T a, T min, T max) {
	if (a > max) return max;
	if (a < min) return min;
	return a;
}

Float distance(Float a, Float b);

Float distance(Vec2 a, Vec2 b);

Float lerp(Float a, Float b, Float alpha);
bool isFloatNearOther(Float a, Float b, Float error);
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);

UInt nextPowerOfTwo(UInt& value);


struct Graph {
	Vector<Float> points;

	void add(Float value);
};

struct Plot {
	Vector<Vec2> points;
	void add(Vec2 value);
	void add(Float a, Float b);
};


//perlin noise by Ryo Suzuki
//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2018 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

# include <cstdint>
# include <numeric>
# include <algorithm>
# include <random>


class PerlinNoise
{
private:

	std::int32_t p[512];

	static Double Fade(Double t) noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	static Double Lerp(Double t, Double a, Double b) noexcept
	{
		return a + t * (b - a);
	}

	static Double Grad(std::int32_t hash, Double x, Double y, Double z) noexcept
	{
		const std::int32_t h = hash & 15;
		const Double u = h < 8 ? x : y;
		const Double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

public:

	explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
	{
		reseed(seed);
	}

	template <class URNG>
	explicit PerlinNoise(URNG& urng)
	{
		reseed(urng);
	}

	void reseed(std::uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = i;
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	}

	template <class URNG>
	void reseed(URNG& urng)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = i;
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, urng);

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	}

	Double noise(Double x) const
	{
		return noise(x, 0.0, 0.0);
	}

	Double noise(Double x, Double y) const
	{
		return noise(x, y, 0.0);
	}

	Double noise(Double x, Double y, Double z) const
	{
		LL X = static_cast<LL>(std::floor(x)) & 255;
		LL Y = static_cast<LL>(std::floor(y)) & 255;
		LL Z = static_cast<LL>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const Double u = Fade(x);
		const Double v = Fade(y);
		const Double w = Fade(z);

		LL A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		LL B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
									Grad(p[BA], x - 1, y, z)),
							Lerp(u, Grad(p[AB], x, y - 1, z),
								 Grad(p[BB], x - 1, y - 1, z))),
					Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
								 Grad(p[BA + 1], x - 1, y, z - 1)),
						 Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
							  Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	Double octaveNoise(Double x, std::int32_t octaves) const
	{
		Double result = 0.0;
		Double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x) * amp;
			x *= 2.0;
			amp *= 0.5;
		}

		return result;
	}

	Double octaveNoise(Double x, Double y, std::int32_t octaves) const
	{
		Double result = 0.0;
		Double amp = 1.0;
		Double roughness = 1.749;
		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x, y) * amp;
			x *= roughness;
			y *= roughness;
			amp *= 1 / roughness;
		}

		return result;
	}

	Double octaveNoise(Double x, Double y, Double z, std::int32_t octaves) const
	{
		Double result = 0.0;
		Double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += noise(x, y, z) * amp;
			x *= 2.0;
			y *= 2.0;
			z *= 2.0;
			amp *= 0.5;
		}

		return result;
	}

	Double noise0_1(Double x) const
	{
		return noise(x) * 0.5 + 0.5;
	}

	Double noise0_1(Double x, Double y) const
	{
		return noise(x, y) * 0.5 + 0.5;
	}

	Double noise0_1(Double x, Double y, Double z) const
	{
		return noise(x, y, z) * 0.5 + 0.5;
	}

	Double octaveNoise0_1(Double x, std::int32_t octaves) const
	{
		return octaveNoise(x, octaves) * 0.5 + 0.5;
	}

	Double octaveNoise0_1(Double x, Double y, std::int32_t octaves) const
	{
		return octaveNoise(x, y, octaves) * 0.5 + 0.5;
	}

	Double octaveNoise0_1(Double x, Double y, Double z, std::int32_t octaves) const
	{
		return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
	}
};