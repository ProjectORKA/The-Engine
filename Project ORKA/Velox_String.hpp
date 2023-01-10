
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <string.h>

#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_Debug.hpp"
#include "Velox_RandomAccessIterator.hpp"
#include "Velox_Utils.hpp"
#include "Velox_BasicVector.hpp"

TRE_NS_START

template<typename T>
class BasicString : public BasicVector<T>
{
public:
    using iterator          = RandomAccessIterator<T>;
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
    using Base              = BasicVector<T>;

public:
    constexpr FORCEINLINE BasicString();

    constexpr FORCEINLINE BasicString(const T* data, usize size);

    constexpr FORCEINLINE BasicString(const std::basic_string<T>& str);

    template<usize S>
    constexpr FORCEINLINE BasicString(const T(&str)[S]);

    constexpr FORCEINLINE ~BasicString() noexcept;

    constexpr FORCEINLINE BasicString(const BasicString<T>& other);

    constexpr FORCEINLINE BasicString& operator=(const BasicString<T>& other);

    constexpr FORCEINLINE BasicString(BasicString<T>&& other) noexcept;

    constexpr FORCEINLINE BasicString& operator=(BasicString<T>&& other) noexcept;

    constexpr FORCEINLINE void append(T ch);

    constexpr FORCEINLINE void append(const T* str, usize sz = 0);

    constexpr FORCEINLINE void append(const BasicString<T>& str);

    constexpr void insert(usize pos, T ch);

    constexpr void insert(usize pos, const T* str, usize otherSize = 0);

    constexpr FORCEINLINE void insert(usize pos, const BasicString<T>& str);

    constexpr FORCEINLINE void pushBack(T ch);

    constexpr FORCEINLINE void popBack() noexcept;

    constexpr FORCEINLINE void erase(iterator itr, usize count) noexcept;

    constexpr FORCEINLINE void eraseRange(usize index, usize count) noexcept;

    constexpr FORCEINLINE bool startsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool startsWith(const BasicString<T>& str) const noexcept;

    constexpr FORCEINLINE bool endsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool endsWith(const BasicString<T>& str) const noexcept;

    constexpr FORCEINLINE bool reserve(usize size);

    constexpr FORCEINLINE BasicString<T> subString(usize index, usize count) const;

    constexpr FORCEINLINE bool isSmall() const noexcept { return this->m_Data == m_Buffer; }

    constexpr FORCEINLINE usize capacity() const noexcept { return this->isSmall() ? NB_CHAR_SMALL : m_Capacity; }

    constexpr friend void swap(BasicString<T>& first, BasicString<T>& second) noexcept
    {
        if (first.isSmall() && second.isSmall()) {
            T buffer[NB_ELEMENTS];
            Utils::copy(buffer, first.m_Data, NB_ELEMENTS);
            Utils::copy(first.m_Data, second.m_Data, NB_ELEMENTS);
            Utils::copy(second.m_Data, buffer, NB_ELEMENTS);
            std::swap(first.m_Length, second.m_Length);
        }else if ((first.isSmall() && !second.isSmall()) || (second.isSmall() && !first.isSmall())) {
            BasicString<T>* small = first.isSmall() ? &first : &second;
            BasicString<T>* notSmall = small == &first ? &second : &first;
            auto notSmallCap = notSmall->m_Capacity;
            Utils::copy(notSmall->m_Buffer, small->m_Data, small->m_Length);
            small->m_Data = notSmall->m_Data;
            small->m_Capacity = notSmallCap;
            notSmall->m_Data = notSmall->m_Buffer;
            std::swap(small->m_Length, notSmall->m_Length);
        }else{
            std::swap(first.m_Data, second.m_Data);
            std::swap(first.m_Capacity, second.m_Capacity);
            std::swap(first.m_Length, second.m_Length);
        }
    }

private:
    constexpr FORCEINLINE void reallocate(usize nCap);

private:
    constexpr static const auto SSO_SIZE        = 2 * sizeof(usize);
    constexpr static const auto NB_CHAR_SMALL   = SSO_SIZE / sizeof(T) - 1;
    constexpr static const auto NB_ELEMENTS     = NB_CHAR_SMALL + 1;
    constexpr static const T NULL_TERMINATOR    = '\0';
    constexpr static const T DEFAULT_GROW_SIZE  = 2;

private:
    union {
        T m_Buffer[NB_ELEMENTS];
        usize m_Capacity;
    };
};

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString() : Base(m_Buffer, 0), m_Buffer{}
{

}

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(const T* data, usize size)
    : Base(m_Buffer, size)
{
    if (size >= NB_CHAR_SMALL) {
        this->m_Data = Utils::allocate<T>(size + 1);
        this->m_Capacity = size;
    }

    this->copyFrom(data, size + 1);
}

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(const std::basic_string<T>& str)
    : BasicString(str.c_str(), str.size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE BasicString<T>::BasicString(const T(&str)[S])
    : BasicString(str, S - 1)
{

}

template<typename T>
constexpr FORCEINLINE BasicString<T>::~BasicString() noexcept
{
    T* data = this->m_Data;
    if (data && !this->isSmall()) {
        Utils::freeMemory(data);
    }
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::append(T ch)
{
    this->reserve(this->m_Length + 2);
    this->EmplaceBack(ch);
    this->m_Data[this->m_Length] = NULL_TERMINATOR;
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::append(const T* str, usize sz)
{
    if (str != nullptr && sz == 0)
        sz = strlen(str);

    auto len = this->m_Length;
    this->reserve(len + sz + 1);

    for (usize i = 0; i < sz; i++) {
        this->m_Data[len++] = str[i];
    }

    this->m_Data[len] = NULL_TERMINATOR;
    this->m_Length = len;
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::append(const BasicString<T>& str)
{
    return this->append(str.data(), str.size());
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::pushBack(T ch)
{
    return this->append(ch);
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::popBack() noexcept
{
    if (this->m_Length)
        this->m_Data[--this->m_Length] = NULL_TERMINATOR;
}

template<typename T>
constexpr void BasicString<T>::insert(usize pos, T ch)
{
    auto size = this->m_Length;
    TRE_ASSERTF(pos <= size, "Given index is out of bound. Index must be in  [0..%" SZu "[", size);
    usize cap = this->capacity();
    T* oldData = this->m_Data;

    if (size + 2 >= cap) {
        bool isSmall = this->isSmall();
        usize nCap = cap * DEFAULT_GROW_SIZE;
        this->m_Data = Utils::allocate<T>(nCap);
        this->copyFrom(oldData, pos);
        this->Base::emplace(oldData, pos, ch);
        // Utils::Copy(newData, m_Data, pos);
        // newData[pos] = ch;
        // Utils::Copy(newData + pos + 1, m_Data + pos, size - pos);

        if (!isSmall)
            Utils::freeMemory(oldData);

        m_Capacity = nCap;
        this->m_Data[this->m_Length] = NULL_TERMINATOR;
    } else {
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(oldData + 1, oldData, size - 1, pos);
        oldData[pos] = ch;
        oldData[++this->m_Length] = NULL_TERMINATOR;
    }

    // return this->Insert(pos, &ch, 1);
}

template<typename T>
constexpr void BasicString<T>::insert(usize pos, const T* str, usize otherSize)
{
    auto size = this->m_Length;
    TRE_ASSERTF(pos <= size, "Given index is out of bound. Index must be in  [0..%" SZu "[", size);

    if (str != nullptr && otherSize == 0)
        otherSize = strlen(str);

    usize cap = this->capacity();
    T* oldData = this->m_Data;

    if (size + otherSize + 1 >= cap) {
        bool isSmall = this->isSmall();
        usize nCap = (size + otherSize) * DEFAULT_GROW_SIZE;
        this->m_Data = Utils::allocate<T>(nCap);
        Utils::Copy(this->m_Data, oldData, pos);
        Utils::Copy(this->m_Data + pos, str, otherSize);
        Utils::Copy(this->m_Data + pos + otherSize, oldData + pos, size - pos);

        if (!isSmall)
            Utils::freeMemory(oldData);

        m_Capacity = nCap;
        this->m_Length += otherSize;
        this->m_Data[this->m_Length] = NULL_TERMINATOR;
    } else {
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(oldData + otherSize, oldData, size - 1, pos);
        Utils::Copy(oldData + pos, str, otherSize);
        this->m_Length += otherSize;
        this->m_Data[this->m_Length] = NULL_TERMINATOR;
    }
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::insert(usize pos, const BasicString<T>& str)
{
    return this->Insert(pos, str.data(), str.size());
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::erase(iterator itr, usize count) noexcept
{
    usize index = itr.getPointer() - this->m_Data;
    return this->eraseRange(index, count);
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::eraseRange(usize index, usize count) noexcept
{
    auto size = this->m_Length;
    TRE_ASSERTF(index < size, "Given index is out of bound. Index must be in  [0..%" SZu "[", size);

    if (!count)
        return;

    if (count > size - index)
        count = size - index;

    auto end = index + count;
    T* data = this->m_Data;
    Utils::MoveConstruct(data + index, data + end, size - end);
    this->m_Length -= count;
    this->m_Data[this->m_Length] = NULL_TERMINATOR;
}

template<typename T>
constexpr FORCEINLINE bool BasicString<T>::startsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::strlen(str);

    if (sz > this->m_Length)
        return false;

    // return true;
    return Utils::MemCmp(this->m_Data, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicString<T>::startsWith(const BasicString<T>& str) const noexcept
{
    return this->startsWith(str.data(), str.size());
}

template<typename T>
constexpr FORCEINLINE bool BasicString<T>::endsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::strlen(str);

    if (sz > this->m_Length)
        return false;

    // return true;
    return Utils::MemCmp(this->m_Data + this->m_Length - sz, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicString<T>::endsWith(const BasicString<T>& str) const noexcept
{
    return this->startsWith(str.data(), str.size());
}

template<typename T>
constexpr FORCEINLINE BasicString<T> BasicString<T>::subString(usize index, usize count) const
{
    auto sz = this->m_Length;
    if (count > sz - index)
        count = sz - index;
    return BasicString(this->m_Data + index, count);
}

template<typename T>
constexpr FORCEINLINE bool BasicString<T>::reserve(usize size)
{
    usize cap = this->capacity();

    if (size < cap)
        return false;

    size = size * DEFAULT_GROW_SIZE;
    this->reallocate(size);
    return true;
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::reallocate(usize nCap)
{
    bool isSmall = this->isSmall();
    T* oldData = this->m_Data;
    this->m_Data = Utils::allocate<T>(nCap);
    this->copyFrom(oldData, this->m_Length + 1);

    if (!isSmall)
        Utils::freeMemory(oldData);

    m_Capacity = nCap;
}

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(const BasicString<T>& other)
    : Base(nullptr, other.m_Length)
{
    auto sz = this->m_Length;
    if (other.isSmall()) {
        this->m_Data = m_Buffer;
    }else{
        m_Capacity = other.m_Capacity;
        this->m_Data = Utils::allocate<T>(m_Capacity);
    }
    this->copyFrom(other.m_Data, sz + 1);
}

template<typename T>
constexpr FORCEINLINE BasicString<T>& BasicString<T>::operator=(const BasicString<T>& other)
{
    BasicString<T> tmp(other);
    swap(*this, tmp);
    return *this;
}

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(BasicString<T>&& other) noexcept
    : Base(nullptr, other.m_Length)
{
    if (other.isSmall()) {
        auto sz = this->m_Length;
        this->m_Data = m_Buffer;
        this->copyFrom(other.m_Data, sz + 1);
    }else{
        m_Capacity = other.m_Capacity;
        this->m_Data = other.m_Data;
        other.m_Data = nullptr;
    }
}

template<typename T>
constexpr FORCEINLINE BasicString<T>& BasicString<T>::operator=(BasicString<T>&& other) noexcept
{
    BasicString<T> tmp(std::move(other));
    swap(*this, tmp);
    return *this;
}

using String = BasicString<char>;

TRE_NS_END
