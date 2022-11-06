
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include <algorithm>
#include "Velox_Common.hpp"
#include "Velox_MathUtils.hpp"

TRE_NS_START

template<typename HashMapType>
struct LinkedListItr
{
    using KeyType = typename HashMapType::KeyType;
    using ValueType = typename HashMapType::ValueType;
    using value_type = typename HashMapType::value_type;
    using iterator = typename HashMapType::iterator;
    using BlockPointer = typename HashMapType::BlockPointer;
    using Constants = typename HashMapType::Constants;
    constexpr static auto BLOCK_SIZE = HashMapType::BLOCK_SIZE;

    BlockPointer block = nullptr;
    usize index = 0;

    FORCEINLINE LinkedListItr()
    {
    }

    FORCEINLINE LinkedListItr(usize index, BlockPointer block)
        : block(block), index(index)
    {
    }

    FORCEINLINE iterator Iterator() const
    {
        return { block, index };
    }

    FORCEINLINE uint32 IndexInBlock() const
    {
        return index % BLOCK_SIZE;
    }

    FORCEINLINE bool IsDirectHit() const
    {
        return (this->GetMetadata() & Constants::BITS_FOR_DIRECT_HIT) == Constants::MAGIC_FOR_DIRECT_HIT;
    }

    FORCEINLINE bool IsEmpty() const
    {
        return this->GetMetadata() == Constants::MAGIC_FOR_EMPTY;
    }

    FORCEINLINE bool HasNext() const
    {
        return this->JumpIndex() != 0;
    }

    FORCEINLINE uint8 JumpIndex() const
    {
        return Constants::DistanceFromMetadata(this->GetMetadata());
    }

    FORCEINLINE uint8 GetMetadata() const
    {
        return block->controlBytes[this->IndexInBlock()];
    }

    FORCEINLINE void SetMetadata(uint8 metadata)
    {
        block->controlBytes[this->IndexInBlock()] = metadata;
    }

    FORCEINLINE LinkedListItr Next(HashMapType& table) const
    {
        uint8 distance = this->JumpIndex();
        usize nextIdx = table.m_HashPolicy.KeepInRange(index + Constants::JUMP_DISTANCES[distance], table.m_SlotsCount);
        return { nextIdx, table.m_Entries + nextIdx / BLOCK_SIZE };
    }

    FORCEINLINE void SetNext(uint8 jumpIdx)
    {
        uint8& metadata = block->controlBytes[this->IndexInBlock()];
        metadata = (metadata & ~Constants::BITS_FOR_DISTANCE) | jumpIdx;
    }

    FORCEINLINE void ClearNext()
    {
        this->SetNext(0);
    }

    FORCEINLINE value_type& operator*() const
    {
        usize idx = this->IndexInBlock();
        value_type* pair = block->GetPair(idx);
        return *pair;
    }

    FORCEINLINE bool operator!() const
    {
        return !block;
    }

    FORCEINLINE explicit operator bool() const
    {
        return block != nullptr;
    }

    FORCEINLINE bool operator==(const LinkedListItr& other) const
    {
        return index == other.index;
    }

    FORCEINLINE bool operator!=(const LinkedListItr& other) const
    {
        return !(*this == other);
    }
};

TRE_NS_END
