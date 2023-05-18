#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <algorithm>
#include "Velox_Common.hpp"
#include "Velox_MathUtils.hpp"

template <typename HashMapType>
struct LinkedListItr {
	using KeyType = typename HashMapType::KeyType;
	using ValueType = typename HashMapType::ValueType;
	using value_type = typename HashMapType::value_type;
	using iterator = typename HashMapType::iterator;
	using BlockPointer = typename HashMapType::BlockPointer;
	using Constants = typename HashMapType::Constants;
	constexpr static auto BLOCK_SIZE = HashMapType::BLOCK_SIZE;

	BlockPointer block = nullptr;
	usize index = 0;

	FORCEINLINE LinkedListItr() { }

	FORCEINLINE LinkedListItr(const usize index, BlockPointer block)
		: block(block), index(index) { }

	FORCEINLINE iterator iterator() const { return {block, index}; }

	FORCEINLINE uint32 indexInBlock() const { return index % BLOCK_SIZE; }

	FORCEINLINE bool isDirectHit() const {
		return (this->getMetadata() & Constants::BITS_FOR_DIRECT_HIT) == Constants::MAGIC_FOR_DIRECT_HIT;
	}

	FORCEINLINE bool isEmpty() const { return this->getMetadata() == Constants::MAGIC_FOR_EMPTY; }

	FORCEINLINE bool hasNext() const { return this->jumpIndex() != 0; }

	FORCEINLINE uint8 jumpIndex() const { return Constants::distanceFromMetadata(this->getMetadata()); }

	FORCEINLINE uint8 getMetadata() const { return block->controlBytes[this->indexInBlock()]; }

	FORCEINLINE void setMetadata(uint8 metadata) { block->controlBytes[this->indexInBlock()] = metadata; }

	FORCEINLINE LinkedListItr next(HashMapType& table) const {
		uint8 distance = this->jumpIndex();
		usize nextIdx = table.m_HashPolicy.keepInRange(index + Constants::JUMP_DISTANCES[distance], table.m_SlotsCount);
		return {nextIdx, table.m_Entries + nextIdx / BLOCK_SIZE};
	}

	FORCEINLINE void setNext(uint8 jumpIdx) {
		uint8& metadata = block->controlBytes[this->indexInBlock()];
		metadata = (metadata & ~Constants::BITS_FOR_DISTANCE) | jumpIdx;
	}

	FORCEINLINE void clearNext() { this->setNext(0); }

	FORCEINLINE value_type& operator*() const {
		usize idx = this->indexInBlock();
		value_type* pair = block->getPair(idx);
		return *pair;
	}

	FORCEINLINE bool operator!() const { return !block; }

	FORCEINLINE explicit operator bool() const { return block != nullptr; }

	FORCEINLINE bool operator==(const LinkedListItr& other) const { return index == other.index; }

	FORCEINLINE bool operator!=(const LinkedListItr& other) const { return !(*this == other); }
};
