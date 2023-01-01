
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_String.hpp"
#include "Velox_Utils.hpp"
#include "Velox_RandomAccessIterator.hpp"
#include "Velox_ArrayView.hpp"

TRE_NS_START

template<typename T>
class BasicStringView : public ArrayView<T>
{
    using Base = ArrayView<T>;

public:
    constexpr FORCEINLINE BasicStringView() noexcept = default;

    constexpr FORCEINLINE BasicStringView(T* data, usize size) noexcept;

    constexpr FORCEINLINE BasicStringView(const BasicString<T>& str) noexcept;

    template<usize S>
    constexpr FORCEINLINE BasicStringView(const T(&str)[S]) noexcept;

    constexpr FORCEINLINE ~BasicStringView() = default;

    constexpr FORCEINLINE BasicStringView(const BasicStringView& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView(BasicStringView&& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView& operator=(const BasicStringView& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView& operator=(BasicStringView&& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView SubString(usize start, usize end) const noexcept;

    constexpr FORCEINLINE bool StartsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool StartsWith(const BasicStringView& str) const noexcept;

    constexpr FORCEINLINE bool EndsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool EndsWith(const BasicStringView& str) const noexcept;

    constexpr FORCEINLINE usize Find(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE usize Find(const BasicStringView& str) const noexcept;
};

template<typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(T* data, usize size) noexcept
    : Base(data, size)
{

}

template<typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const BasicString<T>& str) noexcept
    : BasicStringView(str.Data(), str.Size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const T(&str)[S]) noexcept
    : BasicStringView(str, S - 1)
{

}

template<typename T>
constexpr FORCEINLINE BasicStringView<T> BasicStringView<T>::SubString(usize index, usize count) const noexcept
{
    return this->SubArray(index, count);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::StartsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

    if (sz > this->m_Size)
        return false;

    return Utils::MemCmp(this->m_Data, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::StartsWith(const BasicStringView& str) const noexcept
{
    return this->StartsWith(str.m_Data, str.m_Size);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::EndsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

    if (sz > this->m_Size)
        return false;

    return Utils::MemCmp(this->m_Data + this->m_Size - sz, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::EndsWith(const BasicStringView& str) const noexcept
{
    return this->EndsWith(str.m_Data, str.m_Size);
}

template<typename T>
constexpr FORCEINLINE usize BasicStringView<T>::Find(const T* str, usize sz) const noexcept
{
    // TODO : Implement this
    return std::numeric_limits<usize>::max();
}

template<typename T>
constexpr FORCEINLINE usize BasicStringView<T>::Find(const BasicStringView& str) const noexcept
{
    return this->Find(str.m_Data, str.m_Size);
}

using StringView = BasicStringView<char>;


TRE_NS_END
