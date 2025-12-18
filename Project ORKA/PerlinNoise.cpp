#include "PerlinNoise.hpp"
#include <random>

#include "Math.hpp"

struct PerlinNoise
{
	Int precomputed[512];

	PerlinNoise()
	{
		for (size_t i = 0; i < 256; ++i) precomputed[i] = i;
		std::shuffle(std::begin(precomputed), std::begin(precomputed) + 256, std::default_random_engine());
		for (size_t i = 0; i < 256; ++i) precomputed[256 + i] = precomputed[i];
	}
};

PerlinNoise noise;

Double perlinNoiseFade(const Double t) noexcept
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

Double grad1D(const Int hash, const Double x)
{
	return ((hash & 1) == 0 ? x : -x);
}

Double grad2D(const Int hash, const Double x, const Double y)
{
	switch (hash & 3)
	{
	case 0: return x + y;
	case 1: return x - y;
	case 2: return -x + y;
	case 3: return -x - y;
	}
	return 0.0;
}

Double grad3D(const Int hash, const Double x, const Double y, const Double z)
{
	const Int    h  = hash & 15;
	const Double u  = (h < 8) ? x : y;
	const Double v  = (h < 4) ? y : (h == 12 || h == 14) ? x : z;
	const Double r1 = ((h & 1) == 0) ? u : -u;
	const Double r2 = ((h & 2) == 0) ? v : -v;
	return r1 + r2;
}

Double grad4D(const Int hash, const Double x, const Double y, const Double z, const Double w)
{
	switch (const int h = hash & 31; h & 3)
	{
	case 0: return x + y + z + w;
	case 1: return x - y + z - w;
	case 2: return -x + y - z + w;
	case 3: return x + y - z - w;
	default: return 0.0;
	}
}

Double noise1D(Double x)
{
	const LL lx = static_cast<LL>(std::floor(x)) & 255;
	x -= std::floor(x);
	const Double u  = perlinNoiseFade(x);
	const Int    a0 = noise.precomputed[lx + 0];
	const Int    b0 = noise.precomputed[lx + 1];
	const Double g0 = grad1D(a0, x);
	const Double g1 = grad1D(b0, x - 1.0);
	return lerp(u, g0, g1);
}

Double noise2D(Double x, Double y)
{
	const LL lx = static_cast<LL>(std::floor(x)) & 255;
	const LL ly = static_cast<LL>(std::floor(y)) & 255;
	x -= std::floor(x);
	y -= std::floor(y);
	const Double u      = perlinNoiseFade(x);
	const Double v      = perlinNoiseFade(y);
	const LL     a0     = noise.precomputed[lx + 0] + ly;
	const LL     b0     = noise.precomputed[lx + 1] + ly;
	const int    aa     = noise.precomputed[a0 + 0];
	const int    ab     = noise.precomputed[a0 + 1];
	const int    ba     = noise.precomputed[b0 + 0];
	const int    bb     = noise.precomputed[b0 + 1];
	const Double g00    = grad2D(aa, x, y);
	const Double g10    = grad2D(ba, x - 1, y);
	const Double g01    = grad2D(ab, x, y - 1);
	const Double g11    = grad2D(bb, x - 1, y - 1);
	const Double lerpX1 = lerp(u, g00, g10);
	const Double lerpX2 = lerp(u, g01, g11);
	return lerp(v, lerpX1, lerpX2);
}

Double noise3D(Double x, Double y, Double z)
{
	const LL lx = static_cast<LL>(std::floor(x)) & 255;
	const LL ly = static_cast<LL>(std::floor(y)) & 255;
	const LL lz = static_cast<LL>(std::floor(z)) & 255;
	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);
	const Double u      = perlinNoiseFade(x);
	const Double v      = perlinNoiseFade(y);
	const Double w      = perlinNoiseFade(z);
	const LL     a0     = noise.precomputed[lx + 0] + ly;
	const LL     b0     = noise.precomputed[lx + 1] + ly;
	const LL     aa     = noise.precomputed[a0 + 0] + lz;
	const LL     ab     = noise.precomputed[a0 + 1] + lz;
	const LL     ba     = noise.precomputed[b0 + 0] + lz;
	const LL     bb     = noise.precomputed[b0 + 1] + lz;
	const Double g000   = grad3D(noise.precomputed[aa], x, y, z);
	const Double g100   = grad3D(noise.precomputed[ba], x - 1, y, z);
	const Double g010   = grad3D(noise.precomputed[ab], x, y - 1, z);
	const Double g110   = grad3D(noise.precomputed[bb], x - 1, y - 1, z);
	const Double g001   = grad3D(noise.precomputed[aa + 1], x, y, z - 1);
	const Double g101   = grad3D(noise.precomputed[ba + 1], x - 1, y, z - 1);
	const Double g011   = grad3D(noise.precomputed[ab + 1], x, y - 1, z - 1);
	const Double g111   = grad3D(noise.precomputed[bb + 1], x - 1, y - 1, z - 1);
	const Double lerpX1 = lerp(u, g000, g100);
	const Double lerpX2 = lerp(u, g010, g110);
	const Double lerpY1 = lerp(v, lerpX1, lerpX2);
	const Double lerpX3 = lerp(u, g001, g101);
	const Double lerpX4 = lerp(u, g011, g111);
	const Double lerpY2 = lerp(v, lerpX3, lerpX4);
	return lerp(w, lerpY1, lerpY2);
}

Double noise01(const Double x)
{
	return noise1D(x) * 0.5 + 0.5;
}

Double noise4D(Double x, Double y, Double z, Double w)
{
	const LL lx = static_cast<LL>(std::floor(x)) & 255;
	const LL ly = static_cast<LL>(std::floor(y)) & 255;
	const LL lz = static_cast<LL>(std::floor(z)) & 255;
	const LL lw = static_cast<LL>(std::floor(w)) & 255;
	x -= std::floor(x);
	y -= std::floor(y);
	z -= std::floor(z);
	w -= std::floor(w);
	const Double u     = perlinNoiseFade(x);
	const Double v     = perlinNoiseFade(y);
	const Double t     = perlinNoiseFade(z);
	const Double s     = perlinNoiseFade(w);
	const LL     a0    = noise.precomputed[lx + 0] + ly;
	const LL     b0    = noise.precomputed[lx + 1] + ly;
	const LL     a1    = noise.precomputed[a0 + 0] + lz;
	const LL     b1    = noise.precomputed[b0 + 0] + lz;
	const LL     a2    = noise.precomputed[a0 + 1] + lz;
	const LL     b2    = noise.precomputed[b0 + 1] + lz;
	const LL     aa0   = noise.precomputed[a1 + 0] + lw;
	const LL     ab0   = noise.precomputed[a1 + 1] + lw;
	const LL     ba0   = noise.precomputed[b1 + 0] + lw;
	const LL     bb0   = noise.precomputed[b1 + 1] + lw;
	const LL     aa1   = noise.precomputed[a2 + 0] + lw;
	const LL     ab1   = noise.precomputed[a2 + 1] + lw;
	const LL     ba1   = noise.precomputed[b2 + 0] + lw;
	const LL     bb1   = noise.precomputed[b2 + 1] + lw;
	const Double c0000 = grad4D(noise.precomputed[aa0], x, y, z, w);
	const Double c1000 = grad4D(noise.precomputed[ba0], x - 1, y, z, w);
	const Double c0100 = grad4D(noise.precomputed[ab0], x, y - 1, z, w);
	const Double c1100 = grad4D(noise.precomputed[bb0], x - 1, y - 1, z, w);
	const Double c0010 = grad4D(noise.precomputed[aa1], x, y, z - 1, w);
	const Double c1010 = grad4D(noise.precomputed[ba1], x - 1, y, z - 1, w);
	const Double c0110 = grad4D(noise.precomputed[ab1], x, y - 1, z - 1, w);
	const Double c1110 = grad4D(noise.precomputed[bb1], x - 1, y - 1, z - 1, w);
	const Double c0001 = grad4D(noise.precomputed[aa0 + 1], x, y, z, w - 1);
	const Double c1001 = grad4D(noise.precomputed[ba0 + 1], x - 1, y, z, w - 1);
	const Double c0101 = grad4D(noise.precomputed[ab0 + 1], x, y - 1, z, w - 1);
	const Double c1101 = grad4D(noise.precomputed[bb0 + 1], x - 1, y - 1, z, w - 1);
	const Double c0011 = grad4D(noise.precomputed[aa1 + 1], x, y, z - 1, w - 1);
	const Double c1011 = grad4D(noise.precomputed[ba1 + 1], x - 1, y, z - 1, w - 1);
	const Double c0111 = grad4D(noise.precomputed[ab1 + 1], x, y - 1, z - 1, w - 1);
	const Double c1111 = grad4D(noise.precomputed[bb1 + 1], x - 1, y - 1, z - 1, w - 1);
	const Double x00   = lerp(u, c0000, c1000);
	const Double x10   = lerp(u, c0100, c1100);
	const Double y0    = lerp(v, x00, x10);
	const Double x01   = lerp(u, c0010, c1010);
	const Double x11   = lerp(u, c0110, c1110);
	const Double y1    = lerp(v, x01, x11);
	const Double z0    = lerp(t, y0, y1);
	const Double x002  = lerp(u, c0001, c1001);
	const Double x102  = lerp(u, c0101, c1101);
	const Double y02   = lerp(v, x002, x102);
	const Double x012  = lerp(u, c0011, c1011);
	const Double x112  = lerp(u, c0111, c1111);
	const Double y12   = lerp(v, x012, x112);
	const Double z1    = lerp(t, y02, y12);
	return lerp(s, z0, z1);
}

Double noise01(const Double x, const Double y, const Double z, const Double w)
{
	return noise4D(x, y, z, w) * 0.5 + 0.5;
}

Double octaveNoise(Double x, Double y, Double z, Double w, const Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += noise4D(x, y, z, w) * amp;
		x *= 2.0;
		y *= 2.0;
		z *= 2.0;
		w *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double octaveNoise01(const Double x, const Double y, const Double z, const Double w, const Int octaves)
{
	return octaveNoise(x, y, z, w, octaves) * 0.5 + 0.5;
}

Double noise01(const Double x, const Double y)
{
	return noise2D(x, y) * 0.5 + 0.5;
}

Double octaveNoise(Double x, const Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += noise1D(x) * amp;
		x *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double noise01(const Double x, const Double y, const Double z)
{
	return noise3D(x, y, z) * 0.5 + 0.5;
}

Double octaveNoise01(const Double x, const Int octaves)
{
	return octaveNoise(x, octaves) * 0.5 + 0.5;
}

Double octaveNoise(Double x, Double y, const Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;
	for (Int i = 0; i < octaves; ++i)
	{
		result += noise2D(x, y) * amp;
		x *= 2.0;
		y *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double octaveNoise(Double x, Double y, Double z, const Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += noise3D(x, y, z) * amp;
		x *= 2.0;
		y *= 2.0;
		z *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double octaveNoise01(const Double x, const Double y, const Int octaves)
{
	return octaveNoise(x, y, octaves) * 0.5 + 0.5;
}

Double octaveNoise01(const Double x, const Double y, const Double z, const Int octaves)
{
	return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
}
