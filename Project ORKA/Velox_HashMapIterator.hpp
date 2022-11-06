
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include <iterator>
#include "Velox_Common.hpp"

TRE_NS_START

template<typename HashMapType, typename VT>
struct TemplatedIterator
{
private:
    using KeyType = typename HashMapType::KeyType;
    using ValueType = typename HashMapType::ValueType;
    using iterator_category = std::forward_iterator_tag;
    using value_type = VT;
    using difference_type = ptrdiff_t;
    using pointer = VT*;
    using reference = VT&;
    using BlockPointer = typename HashMapType::BlockPointer;
    using Constants = typename HashMapType::Constants;
    constexpr static auto BLOCK_SIZE = HashMapType::BLOCK_SIZE;

    // using KeyType = std::remove_reference_t<typename value_type::first_type>;
    // using ValueType = std::remove_reference_t<typename value_type::second_type>;

    BlockPointer current;
    usize index;

    friend HashMapType;
public:
    TemplatedIterator() noexcept : TemplatedIterator{nullptr, 0}
    {
    }

    TemplatedIterator(BlockPointer entries, usize index) noexcept
        : current(entries), index(index)
    {
    }

    friend bool operator==(const TemplatedIterator& lhs, const TemplatedIterator& rhs) noexcept
    {
        return lhs.index == rhs.index;
    }

    friend bool operator!=(const TemplatedIterator& lhs, const TemplatedIterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    TemplatedIterator& operator++() noexcept
    {
        do {
            if (index % BLOCK_SIZE == 0)
                --current;
            if (index-- == 0)
                break;
        } while(current->controlBytes[index % BLOCK_SIZE] == Constants::MAGIC_FOR_EMPTY);
        return *this;
    }

    TemplatedIterator operator++(int) noexcept
    {
        TemplatedIterator copy(*this);
        ++*this;
        return copy;
    }

    value_type& operator*() const noexcept
    {
        usize idx = index % BLOCK_SIZE;
        value_type* pair = current->GetPair(idx);
        return *pair;
    }

    value_type* operator->() const noexcept
    {
        usize idx = index % BLOCK_SIZE;
        value_type* pair = current->GetPair(idx);
        return pair;
    }

    operator TemplatedIterator<HashMapType, const value_type>() const noexcept
    {
        return { current, index };
    }
};

TRE_NS_END
