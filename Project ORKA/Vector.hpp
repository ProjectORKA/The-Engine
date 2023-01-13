
#pragma once

//written by Omarito
//modified by
//  Blendurian


#include "Velox_GenericAllocator.hpp"
#include "BasicVector.hpp"

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

    constexpr FORCEINLINE Vector(std::initializer_list<T> list);

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

    constexpr FORCEINLINE bool reserve(usize sz);

    template<typename... Args>
    constexpr FORCEINLINE T& emplaceBack(Args&&... args);

    constexpr FORCEINLINE T& pushBack(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& emplaceFront(Args&&... args);

    constexpr FORCEINLINE T& pushFront(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& fastEmplaceFront(Args&&... args);

    constexpr FORCEINLINE T& fastPushFront(const T& obj);

    template<typename... Args>
    constexpr T& emplace(usize i, Args&&... args);

    constexpr FORCEINLINE T& insert(usize i, const T& obj);

    constexpr FORCEINLINE T& fastInsert(usize i, const T& obj);

    template<typename... Args>
    constexpr T& fastEmplace(usize i, Args&&... args);

    constexpr FORCEINLINE void fill(usize length, const T& obj = {}) noexcept;

    constexpr FORCEINLINE void resize(usize newSize);

    constexpr FORCEINLINE void append(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE void append(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE usize capacity() const noexcept;

    constexpr FORCEINLINE Vector& operator+=(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector& operator+=(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE T* stealPtr() noexcept;

    constexpr FORCEINLINE friend void swap(Vector<T, Alloc>& first, Vector<T, Alloc>& second) noexcept
    {
        static_assert(SwappableConcept<Alloc>, "Can't implement Vector::Swap because the underlying allocator provides no swap function");
        std::swap(first.m_Capacity, second.m_Capacity);
        swap(static_cast<BasicVector<T>&>(first), static_cast<BasicVector<T>&>(second));
        swap(static_cast<Alloc&>(first), static_cast<Alloc&>(second));
    }

private:
    constexpr FORCEINLINE void reserveInternal(usize sz);

    constexpr FORCEINLINE void reserveHelper(usize nCap);

    constexpr FORCEINLINE void free(T* data, usize sz);

    constexpr FORCEINLINE static usize getAllocCapIfSizeIsLessThan(usize sz)
    {
        return sz > STATIC_ELEMENTS_COUNT ? sz : STATIC_ELEMENTS_COUNT;
    }

    constexpr FORCEINLINE T* getInitialData()
    {
        if constexpr (STATIC_ELEMENTS_COUNT != 0) {
            return this->template allocate<T>(STATIC_ELEMENTS_COUNT);
        }else{
            return nullptr;
        }
    }

    constexpr FORCEINLINE void handleMoveAfterRealloc(T* oldData, usize size)
    {
        auto HandleRealloc = [this](T* oldData, usize size) {
            Utils::moveConstruct(this->m_Data, oldData, size);
            this->freeMemory(oldData);
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
constexpr FORCEINLINE Vector<T, Alloc>::Vector(std::initializer_list<T> list)
    : Base(this->template allocate<T>(list.size()), list.size()),
    m_Capacity(getAllocCapIfSizeIsLessThan(list.size()))
{
    this->Base::copyFrom(list.begin(), list.size());
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector() noexcept
    : Base(getInitialData(), 0), m_Capacity(STATIC_ELEMENTS_COUNT)
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz)
    : Base(this->template allocate<T>(sz), 0),
      m_Capacity(getAllocCapIfSizeIsLessThan(sz))
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz, const T& obj)
    : Vector(sz)
{
    this->fill(m_Capacity, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const T* data, usize size)
    : Base(this->template allocate<T>(size), size),
      m_Capacity(getAllocCapIfSizeIsLessThan(size))
{
    this->Base::copyFrom(data, size);
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
        this->free(data, this->m_Length);
        this->m_Data = NULL;
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::fill(usize length, const T& obj) noexcept
{
    this->reserve(length);
    this->Base::fill(length, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::emplaceBack(Args&&... args)
{
    auto newLen = this->m_Length + 1;
    this->reserve(newLen);
    return this->Base::emplaceBack(std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::pushBack(const T& obj)
{
    return this->emplaceBack(obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE bool Vector<T, Alloc>::reserve(usize sz)
{
    if (sz < m_Capacity)
        return false;
    this->reserveInternal(sz);
    return true;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::reserveInternal(usize sz)
{
    // sz = sz ? sz * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
    sz = std::max(sz, (m_Capacity << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY);
    this->reserveHelper(sz);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::reserveHelper(usize nCap)
{
    // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
    T* newData = this->template reallocate<T>(this->m_Data, nCap);
    std::swap(this->m_Data, newData);
    this->handleMoveAfterRealloc(newData, this->m_Length);
    m_Capacity = nCap;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::free(T* data, usize sz)
{
    Utils::destroy(data, sz);
    this->freeMemory(data);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::resize(usize newSize)
{
    auto len = this->m_Length;

    if (newSize < len) {
        usize offset = len - newSize;
        Utils::destroy(this->m_Data + newSize, offset);
        this->m_Length = newSize;
    } else if (newSize > len) {
        this->reserve(newSize);
        this->m_Length = newSize;
    }
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::emplace(usize i, Args&&... args)
{
    auto len = this->m_Length;
    TRE_ASSERTF(i <= len, "Given index is out of bound please choose from [0..%" SZu "].", len);
    auto cap = m_Capacity;
    auto data = this->m_Data;

    if (len + 1 >= cap) {
        // usize nCap = m_Capacity ? m_Capacity * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
        usize nCap = (cap << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
        this->m_Data = this->template reallocate<T>(data, nCap);
        T& res = this->Base::emplace(data, i, std::forward<Args>(args)...);
        this->handleMoveAfterRealloc(data, i);
        m_Capacity = nCap;
        return res;
    } else {
        T& res = this->Base::emplace(i, std::forward<Args>(args)...);
        return res;
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::insert(usize i, const T& obj)
{
    return this->emplace(i, obj);
}

template<typename T, AllocConcept Alloc>
template<typename ...Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::emplaceFront(Args&&... args)
{
    return this->emplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::pushFront(const T& obj)
{
    return this->Insert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::fastEmplaceFront(Args&&... args)
{
    return this->fastEmplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::fastPushFront(const T& obj)
{
    return this->fastInsert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::fastEmplace(usize i, Args&&... args)
{
    auto len = this->m_Length;
    TRE_ASSERTF(i <= len, "Given index is out of bound please choose from [0..%" SZu "].", len);
    if (len + 1 >= m_Capacity) // The default way might be faster as we have to reallocate the memory and copy the objects anyways
        return this->emplace(i, std::forward<Args>(args)...);
    return this->Base::fastEmplace(i, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::fastInsert(usize i, const T& obj)
{
    return this->fastEmplace(i, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::append(const Vector<T, Alloc>& other)
{
    auto len = this->m_Length;
    auto newLen = len + other.m_Length;
    this->reserve(newLen);
    Utils::copy(this->m_Data + len, other.m_Data, other.m_Length);
    this->m_Length = newLen;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::append(Vector<T, Alloc>&& other)
{
    auto len = this->m_Length;
    auto newLen = len + other.m_Length;
    this->reserve(newLen);
    Utils::move(this->m_Data + len, other.m_Data, other.m_Length);
    this->m_Length = newLen;
    other.m_Length = 0;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(const Vector<T, Alloc>& other)
{
    this->append(other);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(Vector<T, Alloc>&& other)
{
    this->append(std::forward<Vector<T, Alloc>>(other));
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T* Vector<T, Alloc>::stealPtr() noexcept
{
    T* data = this->m_Data;
    m_Capacity = 0;
    this->m_Length = 0;
    this->m_Data = NULL;
    return data;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE usize Vector<T, Alloc>::capacity() const noexcept
{
    return m_Capacity;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const Vector<T, Alloc>& other) :
    Alloc(other), Base(nullptr, other.m_Length), m_Capacity(other.m_Capacity)
{
    static_assert(CopyableConcept<Alloc>, "Can't implement Vector copy ctor because the underlying allocator provides no copy ctor");
    if (m_Capacity) {
        this->m_Data = this->template allocate<T>(m_Capacity);
        this->Base::copyFrom(other.m_Data, other.m_Length);
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& other)
{
    static_assert(CopyableConcept<Alloc>, "Can't implement Vector copy assignement because the underlying allocator provides no copy assignement");
    Vector<T, Alloc> tmp(other);
    swap(*this, tmp);
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
    swap(*this, tmp);
    return *this;
}
