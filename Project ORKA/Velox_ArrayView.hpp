
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include "Velox_Common.hpp"
#include "Velox_Debug.hpp"
#include "Velox_Utils.hpp"
#include "Velox_UtilityConcepts.hpp"
#include "Velox_RandomAccessIterator.hpp"

TRE_NS_START

template<typename T>
class ArrayView
{
public:
    using Iterator          = RandomAccessIterator<T>;
    using CIterator         = RandomAccessIterator<const T>;
    using value_type        = T;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference	= const value_type&;
    using const_iterator	= CIterator;
    using iterator          = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator  = const_reverse_iterator;
    using size_type         = size_t;
    using difference_type	= ptrdiff_t;

public:
    constexpr FORCEINLINE ArrayView() noexcept = default;

    constexpr FORCEINLINE ~ArrayView() = default;

    constexpr FORCEINLINE ArrayView(const ArrayView& other) noexcept = default;

    constexpr FORCEINLINE ArrayView(ArrayView&& other) noexcept = default;

    constexpr FORCEINLINE ArrayView& operator=(const ArrayView& other) noexcept = default;

    constexpr FORCEINLINE ArrayView& operator=(ArrayView&& other) noexcept = default;

    constexpr FORCEINLINE ArrayView(T* data, usize size) noexcept;

    template<ContainerConcept ContainerType>
    constexpr FORCEINLINE ArrayView(const ContainerType& arr) noexcept;

    template<usize S>
    constexpr FORCEINLINE ArrayView(const T(&arr)[S]) noexcept;

    constexpr FORCEINLINE void RemovePrefix(usize skip) { m_Data += skip; }

    constexpr FORCEINLINE void RemoveSuffix(usize skip) { m_Size -= skip; }

    constexpr FORCEINLINE ArrayView SubArray(usize start, usize end) const noexcept;

    constexpr FORCEINLINE const T* Data() const { return m_Data; }

    constexpr FORCEINLINE usize Length() const { return m_Size; }

    constexpr FORCEINLINE usize Size() const { return m_Size; }

    constexpr FORCEINLINE T Get(usize i) const noexcept
    {
        TRE_ASSERTF(i < m_Size, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE T& At(usize i) noexcept
    {
        TRE_ASSERTF(i < m_Size, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE const T& At(usize i) const noexcept
    {
        TRE_ASSERTF(i < m_Size, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE T& operator[](usize i) noexcept
    {
        return this->At(i);
    }

    constexpr FORCEINLINE const T& operator[](usize i) const noexcept
    {
        return this->At(i);
    }

    constexpr iterator begin() const noexcept
    {
        return Iterator(m_Data);
    }

    constexpr iterator end() const noexcept
    {
        return Iterator(m_Data + m_Size);
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return CIterator(m_Data);
    }

    constexpr const_iterator cend() const noexcept
    {
        return CIterator(m_Data + m_Size);
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator  crend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

private:
    T* m_Data;
    usize m_Size;
};

template<typename T>
constexpr FORCEINLINE ArrayView<T>::ArrayView(T* data, usize size) noexcept
    : m_Data(data), m_Size(size)
{

}

template<typename T>
template<ContainerConcept ContainerType>
constexpr FORCEINLINE ArrayView<T>::ArrayView(const ContainerType& arr) noexcept
    : ArrayView(arr.Data(), arr.Size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE ArrayView<T>::ArrayView(const T(&arr)[S]) noexcept
    : ArrayView(arr, S)
{

}

template<typename T>
constexpr FORCEINLINE ArrayView<T> ArrayView<T>::SubArray(usize index, usize count) const noexcept
{
    if (count > m_Size - index)
        count = m_Size - index;
    return ArrayView(m_Data + index, count);
}

TRE_NS_END
