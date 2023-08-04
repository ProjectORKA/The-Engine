#pragma once

#include "Basics.hpp"
# include <random>

struct PerlinNoise
{

	std::int32_t p[512];

	void          reseed(std::uint32_t seed);
	static Double Fade(Double t) noexcept;
	static Double Lerp(Double t, Double a, Double b) noexcept;
	static Double Grad(std::int32_t hash, Double x, Double y, Double z) noexcept;

	explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed);

	[[nodiscard]] Double noise(Double x) const;
	[[nodiscard]] Double noise0_1(Double x) const;
	[[nodiscard]] Double noise(Double x, Double y) const;
	[[nodiscard]] Double noise0_1(Double x, Double y) const;
	[[nodiscard]] Double noise(Double x, Double y, Double z) const;
	[[nodiscard]] Double noise0_1(Double x, Double y, Double z) const;
	[[nodiscard]] Double octaveNoise(Double x, std::int32_t octaves) const;
	[[nodiscard]] Double octaveNoise0_1(Double x, std::int32_t octaves) const;
	[[nodiscard]] Double octaveNoise(Double x, Double y, std::int32_t octaves) const;
	[[nodiscard]] Double octaveNoise0_1(Double x, Double y, std::int32_t octaves) const;
	[[nodiscard]] Double octaveNoise(Double x, Double y, Double z, std::int32_t octaves) const;
	[[nodiscard]] Double octaveNoise0_1(Double x, Double y, Double z, std::int32_t octaves) const;

	template <class URNG> explicit PerlinNoise(URNG& urng)
	{
		reseed(urng);
	}

	template <class URNG> void reseed(URNG& urng)
	{
		for(size_t i = 0; i < 256; ++i) p[i] = i;

		std::shuffle(std::begin(p), std::begin(p) + 256, urng);

		for(size_t i = 0; i < 256; ++i) p[256 + i] = p[i];
	}
};

extern PerlinNoise noise;
