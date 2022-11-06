
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
#include "Velox_BasicVector.hpp"

TRE_NS_START

template<typename T, AllocConcept Alloc = GenericAllocator>
class Vector : public Alloc, public BasicVector<T>
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
    using Base              = BasicVector<T>;

public:
    CONSTEXPR const static usize DEFAULT_CAPACITY       = 8;
    CONSTEXPR const static usize DEFAULT_GROW_SIZE      = 1;
    // TODO : take into account alignement in the statment below :
    CONSTEXPR const static usize STATIC_ELEMENTS_COUNT  = Alloc::Traits::STATIC_CAP / sizeof(T);

public:
    constexpr FORCEINLINE Vector() noexcept;

    constexpr FORCEINLINE Vector(usize sz);

    constexpr FORCEINLINE Vector(usize sz, const T& obj);

    template<usize S>
    constexpr FORCEINLINE Vector(const T(&arr)[S]);

    constexpr FORCEINLINE Vector(const T* data, usize size);

    constexpr FORCEINLINE ~Vector();

    constexpr FORCEINLINE Vector(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector& operator=(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector(Vector<T, Alloc>&& other) noexcept;

    constexpr FORCEINLINE Vector& operator=(Vector<T, Alloc>&& other) noexcept;

    constexpr FORCEINLINE bool Reserve(usize sz);

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceBack(Args&&... args);

    constexpr FORCEINLINE T& PushBack(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceFront(Args&&... args);

    constexpr FORCEINLINE T& PushFront(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& FastEmplaceFront(Args&&... args);

    constexpr FORCEINLINE T& FastPushFront(const T& obj);

    template<typename... Args>
    constexpr T& Emplace(usize i, Args&&... args);

    constexpr FORCEINLINE T& Insert(usize i, const T& obj);

    constexpr FORCEINLINE T& FastInsert(usize i, const T& obj);

    template<typename... Args>
    constexpr T& FastEmplace(usize i, Args&&... args);

    constexpr FORCEINLINE void Fill(usize length, const T& obj = {}) noexcept;

    constexpr FORCEINLINE void Resize(usize newSize);

    constexpr FORCEINLINE void Append(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE void Append(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE usize Capacity() const noexcept;

    constexpr FORCEINLINE Vector& operator+=(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector& operator+=(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE T* StealPtr() noexcept;

    constexpr FORCEINLINE friend void Swap(Vector<T, Alloc>& first, Vector<T, Alloc>& second) noexcept
    {
        static_assert(SwappableConcept<Alloc>, "Can't implement Vector::Swap because the underlying allocator provides no swap function");
        std::swap(first.m_Capacity, second.m_Capacity);
        Swap(static_cast<BasicVector<T>&>(first), static_cast<BasicVector<T>&>(second));
        Swap(static_cast<Alloc&>(first), static_cast<Alloc&>(second));
    }

    constexpr FORCEINLINE friend void swap(Vector<T, Alloc>& first, Vector<T, Alloc>& second) noexcept
    {
        Swap(first, second);
    }

private:
    constexpr FORCEINLINE void ReserveInternal(usize sz);

    constexpr FORCEINLINE void ReserveHelper(usize nCap);

    constexpr FORCEINLINE void Free(T* data, usize sz);

    constexpr FORCEINLINE static usize GetAllocCapIfSizeIsLessThan(usize sz)
    {
        return sz > STATIC_ELEMENTS_COUNT ? sz : STATIC_ELEMENTS_COUNT;
    }

    constexpr FORCEINLINE T* GetInitialData()
    {
        if constexpr (STATIC_ELEMENTS_COUNT != 0) {
            return this->template Allocate<T>(STATIC_ELEMENTS_COUNT);
        }else{
            return nullptr;
        }
    }

    constexpr FORCEINLINE void HandleMoveAfterRealloc(T* oldData, usize size)
    {
        auto HandleRealloc = [this](T* oldData, usize size) {
            Utils::MoveConstruct(this->m_Data, oldData, size);
            this->FreeMemory(oldData);
        };

        if constexpr (Alloc::Traits::HAVE_REALLOC) {
            if (oldData != this->m_Data) {
                HandleRealloc(oldData, size);
            }
        }else{
            HandleRealloc(oldData, size);
        }
    }

private:
    usize m_Capacity;
};

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector() noexcept
    : Base(GetInitialData(), 0), m_Capacity(STATIC_ELEMENTS_COUNT)
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz)
    : Base(this->template Allocate<T>(sz), 0),
      m_Capacity(GetAllocCapIfSizeIsLessThan(sz))
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz, const T& obj)
    : Vector(sz)
{
    this->Fill(m_Capacity, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const T* data, usize size)
    : Base(this->template Allocate<T>(size), size),
      m_Capacity(GetAllocCapIfSizeIsLessThan(size))
{
    this->Base::CopyFrom(data, size);
}

template<typename T, AllocConcept Alloc>
template<usize S>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const T(&arr)[S])
    : Vector(arr, S)
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::~Vector()
{
    auto data = this->m_Data;

    if (data != NULL) {
        this->Free(data, this->m_Length);
        this->m_Data = NULL;
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Fill(usize length, const T& obj) noexcept
{
    this->Reserve(length);
    this->Base::Fill(length, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::EmplaceBack(Args&&... args)
{
    auto newLen = this->m_Length + 1;
    this->Reserve(newLen);
    return this->Base::EmplaceBack(std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::PushBack(const T& obj)
{
    return this->EmplaceBack(obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE bool Vector<T, Alloc>::Reserve(usize sz)
{
    if (sz < m_Capacity)
        return false;
    this->ReserveInternal(sz);
    return true;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::ReserveInternal(usize sz)
{
    // sz = sz ? sz * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
    sz = std::max(sz, (m_Capacity << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY);
    this->ReserveHelper(sz);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::ReserveHelper(usize nCap)
{
    // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
    T* newData = this->template Reallocate<T>(this->m_Data, nCap);
    std::swap(this->m_Data, newData);
    this->HandleMoveAfterRealloc(newData, this->m_Length);
    m_Capacity = nCap;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Free(T* data, usize sz)
{
    Utils::Destroy(data, sz);
    this->FreeMemory(data);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Resize(usize newSize)
{
    auto len = this->m_Length;

    if (newSize < len) {
        usize offset = len - newSize;
        Utils::Destroy(this->m_Data + newSize, offset);
        this->m_Length = newSize;
    } else if (newSize > len) {
        this->Reserve(newSize);
        this->m_Length = newSize;
    }
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::Emplace(usize i, Args&&... args)
{
    auto len = this->m_Length;
    TRE_ASSERTF(i <= len, "Given index is out of bound please choose from [0..%" SZu "].", len);
    auto cap = m_Capacity;
    auto data = this->m_Data;

    if (len + 1 >= cap) {
        // usize nCap = m_Capacity ? m_Capacity * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
        usize nCap = (cap << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
        this->m_Data = this->template Reallocate<T>(data, nCap);
        T& res = this->Base::Emplace(data, i, std::forward<Args>(args)...);
        this->HandleMoveAfterRealloc(data, i);
        m_Capacity = nCap;
        return res;
    } else {
        T& res = this->Base::Emplace(i, std::forward<Args>(args)...);
        return res;
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::Insert(usize i, const T& obj)
{
    return this->Emplace(i, obj);
}

template<typename T, AllocConcept Alloc>
template<typename ...Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::EmplaceFront(Args&&... args)
{
    return this->Emplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::PushFront(const T& obj)
{
    return this->Insert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastEmplaceFront(Args&&... args)
{
    return this->FastEmplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastPushFront(const T& obj)
{
    return this->FastInsert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastEmplace(usize i, Args&&... args)
{
    auto len = this->m_Length;
    TRE_ASSERTF(i <= len, "Given index is out of bound please choose from [0..%" SZu "].", len);
    if (len + 1 >= m_Capacity) // The default way might be faster as we have to reallocate the memory and copy the objects anyways
        return this->Emplace(i, std::forward<Args>(args)...);
    return this->Base::FastEmplace(i, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastInsert(usize i, const T& obj)
{
    return this->FastEmplace(i, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Append(const Vector<T, Alloc>& other)
{
    auto len = this->m_Length;
    auto newLen = len + other.m_Length;
    this->Reserve(newLen);
    Utils::Copy(this->m_Data + len, other.m_Data, other.m_Length);
    this->m_Length = newLen;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Append(Vector<T, Alloc>&& other)
{
    auto len = this->m_Length;
    auto newLen = len + other.m_Length;
    this->Reserve(newLen);
    Utils::Move(this->m_Data + len, other.m_Data, other.m_Length);
    this->m_Length = newLen;
    other.m_Length = 0;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(const Vector<T, Alloc>& other)
{
    this->Append(other);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(Vector<T, Alloc>&& other)
{
    this->Append(std::forward<Vector<T, Alloc>>(other));
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T* Vector<T, Alloc>::StealPtr() noexcept
{
    T* data = this->m_Data;
    m_Capacity = 0;
    this->m_Length = 0;
    this->m_Data = NULL;
    return data;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE usize Vector<T, Alloc>::Capacity() const noexcept
{
    return m_Capacity;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const Vector<T, Alloc>& other) :
    Alloc(other), Base(nullptr, other.m_Length), m_Capacity(other.m_Capacity)
{
    static_assert(CopyableConcept<Alloc>, "Can't implement Vector copy ctor because the underlying allocator provides no copy ctor");
    if (m_Capacity) {
        this->m_Data = this->template Allocate<T>(m_Capacity);
        this->Base::CopyFrom(other.m_Data, other.m_Length);
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& other)
{
    static_assert(CopyableConcept<Alloc>, "Can't implement Vector copy assignement because the underlying allocator provides no copy assignement");
    Vector<T, Alloc> tmp(other);
    Swap(*this, tmp);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(Vector<T, Alloc>&& other) noexcept
    : Alloc(std::move(other)), Base(std::move(other)), m_Capacity(other.m_Capacity)
{
    static_assert(MoveableConcept<Alloc>, "Can't implement Vector move ctor because the underlying allocator provides no move ctor");
    other.m_Data = NULL;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector<T, Alloc>&& other) noexcept
{
    static_assert(MoveableConcept<Alloc>, "Can't implement Vector move assignement because the underlying allocator provides no move assignement");
    Vector<T, Alloc> tmp(std::move(other));
    Swap(*this, tmp);
    return *this;
}

TRE_NS_END
