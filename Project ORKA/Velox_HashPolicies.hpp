#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include "Velox_MathUtils.hpp"

struct FibonacciHashPolicy {
	usize indexForHash(const usize hash, usize /*slotsCount*/) const noexcept {
		return (11400714819323198485ull * hash) >> shift;
	}

	usize keepInRange(const usize index, const usize slotsCount) const noexcept { return index & slotsCount; }

	uint8 nextSize(usize& size) const noexcept {
		size = std::max(static_cast<usize>(2), Math::NextPow2(size));
		return (NB_BITS + 1) - Math::Log2OfPow2(size);
	}

	void commit(const uint8 shift) noexcept { this->shift = shift; }

	void reset() noexcept { shift = NB_BITS; }

private:
	constexpr static auto NB_BITS = sizeof(usize) * CHAR_BIT - 1;
	uint8 shift = NB_BITS;
};

struct PowerOfTwoHashPolicy {
	usize indexForHash(const usize hash, const usize slotsCount) const { return hash & slotsCount; }

	usize keepInRange(const usize index, const usize slotsCount) const { return indexForHash(index, slotsCount); }

	uint8 nextSize(usize& size) const {
		size = Math::NextPow2(size);
		return 0;
	}

	void commit(uint8) { }

	void reset() { }
};
