
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include <initializer_list>
#include <iterator>
#include <utility>

#include "Velox_Common.hpp"
#include "Velox_Debug.hpp"
#include "Velox_UtilityConcepts.hpp"
#include "Velox_Memory.hpp"
#include "Velox_GenericAllocator.hpp"
#include "Velox_RandomAccessIterator.hpp"

TRE_NS_START

template<typename T>
class BasicVector
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
    using iterator          = Iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator  = const_reverse_iterator;
    using size_type         = size_t;
    using difference_type	= ptrdiff_t;

public:
    constexpr FORCEINLINE void Erease(Iterator start, Iterator end) noexcept;

    constexpr FORCEINLINE void Erease(usize start, usize end) noexcept;

    constexpr FORCEINLINE void Erease(Iterator itr) noexcept;

    constexpr FORCEINLINE void Erease(usize index) noexcept;

    constexpr FORCEINLINE void Clear() noexcept
    {
        Utils::Destroy(m_Data, m_Length);
        m_Length = 0;
    }

    constexpr FORCEINLINE bool PopBack() noexcept
    {
        if (m_Length <= 0)
            return false;
        m_Data[--m_Length].~T();
        return m_Length;
    }

    constexpr FORCEINLINE bool PopFront() noexcept
    {
        if (m_Length <= 0)
            return false;
        T* data = m_Data;
        data[0].~T();
        // This is safe slot 1 is moved to slot 0 and so on slot n+1 will be moved in slot n...
        Utils::Move(data, data + 1, --m_Length);
        return m_Length;
    }

    constexpr FORCEINLINE void FastErease(Iterator itr) noexcept;

    constexpr FORCEINLINE void FastErease(usize index) noexcept;

    constexpr FORCEINLINE bool FastPopFront() noexcept;

    constexpr FORCEINLINE usize Size() const noexcept { return m_Length; }

    constexpr FORCEINLINE usize Length() const noexcept { return this->Size(); }

    constexpr FORCEINLINE bool IsEmpty() const noexcept { return this->Size() == 0; }

    constexpr FORCEINLINE T* Back() noexcept
    {
        TRE_ASSERTF(!this->IsEmpty(), "Can't access the last element while the container is empty !");
        return &m_Data[m_Length - 1];
    }

    constexpr FORCEINLINE T* Front() noexcept
    {
        TRE_ASSERTF(!this->IsEmpty(), "Can't access the first element while the container is empty !");
        return &m_Data[0];
    }

    constexpr FORCEINLINE const T* Back() const noexcept
    {
        TRE_ASSERTF(!this->IsEmpty(), "Can't access the last element while the container is empty !");
        return &m_Data[m_Length - 1];
    }

    constexpr FORCEINLINE const T* Front() const noexcept
    {
        TRE_ASSERTF(!this->IsEmpty(), "Can't access the first element while the container is empty !");
        return &m_Data[0];
    }

    constexpr FORCEINLINE T& Last() noexcept { return *this->Back(); }

    constexpr FORCEINLINE T& First() noexcept { return *this->Front(); }

    constexpr FORCEINLINE const T& Last() const noexcept { return *this->Back(); }

    constexpr FORCEINLINE const T& First() const noexcept { return *this->Front(); }

    constexpr FORCEINLINE T* Data() noexcept { return this->Front(); }

    constexpr FORCEINLINE T Get(usize i) const noexcept
    {
        TRE_ASSERTF(i < m_Length, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Length);
        return m_Data[i];
    }

    constexpr FORCEINLINE T& At(usize i) noexcept
    {
        TRE_ASSERTF(i < m_Length, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Length);
        return m_Data[i];
    }

    constexpr FORCEINLINE const T& At(usize i) const noexcept
    {
        TRE_ASSERTF(i < m_Length, "Can't access the %" SZu " th element, out of range [0..%" SZu ") !", i, m_Length);
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

    constexpr FORCEINLINE iterator begin() const noexcept
    {
        return Iterator(m_Data);
    }

    constexpr FORCEINLINE iterator end() const noexcept
    {
        return Iterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE const_iterator cbegin() const noexcept
    {
        return CIterator(m_Data);
    }

    constexpr FORCEINLINE const_iterator cend() const noexcept
    {
        return CIterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr FORCEINLINE const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr FORCEINLINE const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr FORCEINLINE const_reverse_iterator  crend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr FORCEINLINE friend void Swap(BasicVector<T>& first, BasicVector<T>& second) noexcept
    {
        std::swap(first.m_Data, second.m_Data);
        std::swap(first.m_Length, second.m_Length);
    }

    constexpr FORCEINLINE friend void swap(BasicVector<T>& first, BasicVector<T>& second) noexcept
    {
        Swap(first, second);
    }

protected:
    constexpr FORCEINLINE BasicVector() noexcept
        : m_Data(nullptr), m_Length(0)
    {

    }

    constexpr FORCEINLINE BasicVector(T* data, usize sz = 0) noexcept
        : m_Data(data), m_Length(sz)
    {

    }

    constexpr FORCEINLINE BasicVector(BasicVector&& other) = default;

    constexpr FORCEINLINE BasicVector& operator=(BasicVector&& other) = default;

    constexpr FORCEINLINE ~BasicVector() = default;

    template<typename... Args>
    constexpr FORCEINLINE T& FastEmplace(usize i, Args&&... args) noexcept;

    constexpr FORCEINLINE void Fill(usize length, const T& obj = {}) noexcept;

    template<typename... Args>
    constexpr FORCEINLINE T& Emplace(T* oldData, usize i, Args&&... args) noexcept;

    template<typename... Args>
    constexpr FORCEINLINE T& Emplace(usize i, Args&&... args) noexcept;

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceBack(Args&&... args) noexcept;

    constexpr FORCEINLINE T& PushBack(const T& obj) noexcept
    {
        return this->EmplaceBack(obj);
    }

    constexpr FORCEINLINE void CopyFrom(const T* data, usize length)
    {
        Utils::CopyConstruct(m_Data, data, length);
    }

    constexpr FORCEINLINE void MoveFrom(T* data, usize length) noexcept
    {
        Utils::MoveConstruct(m_Data, data, length);
    }

    constexpr FORCEINLINE void Destroy() noexcept
    {
        Utils::Destroy(m_Data, m_Length);
    }

    constexpr FORCEINLINE void SetLength(usize len) noexcept
    {
        m_Length = len;
    }

    constexpr FORCEINLINE void SetData(T* data) noexcept
    {
        m_Data = data;
    }
protected:
    T*    m_Data;
    usize m_Length;
};

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& BasicVector<T>::EmplaceBack(Args&&... args) noexcept
{
    T* data = m_Data;
    auto newLen = m_Length++;
    new (data + newLen) T(std::forward<Args>(args)...);
    return data[newLen];
}

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& BasicVector<T>::Emplace(T* oldData, usize i, Args&&... args) noexcept
{
    T* data = m_Data;
    T* dest = data + i;

    if (oldData == data){
        // Shift all of this to keep place for the new element
        Utils::MoveConstructBackward(data + 1, data, m_Length - 1, i);
    }else{
        // Copy all the the elements starting from i to the new buffer
        Utils::MoveConstructForward(data + 1, oldData, i, m_Length);
    }

    new (dest) T(std::forward<Args>(args)...);
    m_Length++;
    return *dest;
}

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& BasicVector<T>::Emplace(usize i, Args&&... args) noexcept
{
    T* data = m_Data;
    T* dest = data + i;
    // Shift all of this to keep place for the new element
    // Utils::MoveConstructBackward(data + 1, data, m_Length - 1, i);
    Utils::MoveConstructForward(data + 1, data, i, m_Length);
    new (dest) T(std::forward<Args>(args)...);
    m_Length++;
    return *dest;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::Erease(Iterator start, Iterator end) noexcept
{
    auto len = m_Length;
    T* data = m_Data;
    T* sentinel = data + len;
    TRE_ASSERTF(end >= start, "end must be greater than start");
    TRE_ASSERTF(start < sentinel && end <= sentinel, "[%" SZu "..%" SZu "] interval isn't included in the range [0..%" SZu "]", start - data, end - data, len);
    const usize size = usize(end - start);
    if (size == 0)
        return;
    T* startPtr = start.GetPointer();
    T* endPtr = end.GetPointer();
    usize count = usize(sentinel - endPtr);
    Utils::Destroy(startPtr, size);
    Utils::MoveConstruct(startPtr, endPtr, count);
    m_Length -= size;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::Erease(usize start, usize end) noexcept
{
    auto begin = this->begin();
    return this->Erease(begin + start, begin + end);
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::Erease(Iterator itr) noexcept
{
    auto len = m_Length;
    T* data = m_Data;
    T* sentinel = data + len;
    T* itrPtr = itr.GetPointer();
    TRE_ASSERTF((itrPtr >= data && itrPtr < sentinel), "The given iterator doesn't belong to the Vector.");
    usize end = usize(sentinel - itrPtr);
    Utils::Destroy(itrPtr, 1);
    Utils::MoveConstruct(itrPtr, itrPtr + 1, end - 1);
    m_Length -= 1;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::Erease(usize index) noexcept
{
    this->Erease(this->begin() + index);
}

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& BasicVector<T>::FastEmplace(usize i, Args&&... args) noexcept
{
    auto len = this->m_Length;
    T* data = this->m_Data;
    T* element = data + i;

    if (i != len) [[likely]] {
        T* last = data + len;
        T temp(std::move(*element));
        new (element) T(std::forward<Args>(args)...);
        new (last) T(std::move(temp));
    } else [[unlikely]] {
        new (element) T(std::forward<Args>(args)...);
    }

    this->m_Length++;
    return *element;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::FastErease(Iterator itr) noexcept
{
    T* itrPtr = itr.GetPtr();
    T* data = this->m_Data;
    auto len = this->m_Length;
    T* sentinel = data + len;
    TRE_ASSERTF((itrPtr >= data && itrPtr < sentinel), "The given iterator doesn't belong to the Vector.");
    itrPtr->~T();
    T* lastPtr = sentinel - 1;
    new (itrPtr) T(std::move(*lastPtr));
    this->m_Length -= 1;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::FastErease(usize index) noexcept
{
    return this->FastErease(this->begin() + index);
}

template<typename T>
constexpr FORCEINLINE bool BasicVector<T>::FastPopFront() noexcept
{
    T* data = this->m_Data;
    auto len = this->m_Length;

    if (len <= 0)
        return false;

    data[0].~T();
    if (--this->m_Length != 0)
        new (data) T(std::move(data[len + 1]));
    return false;
}

template<typename T>
constexpr FORCEINLINE void BasicVector<T>::Fill(usize length, const T& obj) noexcept
{
    Utils::MemSet(m_Data, obj, length);
    this->m_Length = length;
}

TRE_NS_END
