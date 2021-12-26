
#include "PerlinNoise.hpp"

PerlinNoise noise;

Double PerlinNoise::Fade(Double t) noexcept
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}
Double PerlinNoise::noise(Double x) const
{
	return noise(x, 0.0, 0.0);
}
PerlinNoise::PerlinNoise(std::uint32_t seed)
{
	reseed(seed);
}
void PerlinNoise::reseed(std::uint32_t seed)
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
Double PerlinNoise::noise0_1(Double x) const
{
	return noise(x) * 0.5 + 0.5;
}
Double PerlinNoise::noise(Double x, Double y) const
{
	return noise(x, y, 0.0);
}
Double PerlinNoise::Lerp(Double t, Double a, Double b) noexcept
{
	return a + t * (b - a);
}
Double PerlinNoise::noise0_1(Double x, Double y) const
{
	return noise(x, y) * 0.5 + 0.5;
}
Double PerlinNoise::noise(Double x, Double y, Double z) const
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
Double PerlinNoise::noise0_1(Double x, Double y, Double z) const
{
	return noise(x, y, z) * 0.5 + 0.5;
}
Double PerlinNoise::octaveNoise(Double x, std::int32_t octaves) const
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
Double PerlinNoise::octaveNoise0_1(Double x, std::int32_t octaves) const
{
	return octaveNoise(x, octaves) * 0.5 + 0.5;
}
Double PerlinNoise::Grad(std::int32_t hash, Double x, Double y, Double z) noexcept
{
	const std::int32_t h = hash & 15;
	const Double u = h < 8 ? x : y;
	const Double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
Double PerlinNoise::octaveNoise(Double x, Double y, std::int32_t octaves) const
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
Double PerlinNoise::octaveNoise0_1(Double x, Double y, std::int32_t octaves) const
{
	return octaveNoise(x, y, octaves) * 0.5 + 0.5;
}
Double PerlinNoise::octaveNoise(Double x, Double y, Double z, std::int32_t octaves) const
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
Double PerlinNoise::octaveNoise0_1(Double x, Double y, Double z, std::int32_t octaves) const
{
	return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
}