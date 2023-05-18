#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_String.hpp"
#include "Velox_Utils.hpp"
#include "Velox_RandomAccessIterator.hpp"
#include "Velox_ArrayView.hpp"

template <typename T>
class BasicStringView : public ArrayView<T> {
	using Base = ArrayView<T>;

public:
	constexpr FORCEINLINE BasicStringView() noexcept = default;

	constexpr FORCEINLINE BasicStringView(T* data, usize size) noexcept;

	constexpr FORCEINLINE BasicStringView(const BasicString<T>& str) noexcept;

	template <usize S>
	constexpr FORCEINLINE BasicStringView(const T (&str)[S]) noexcept;

	constexpr FORCEINLINE ~BasicStringView() = default;

	constexpr FORCEINLINE BasicStringView(const BasicStringView& other) noexcept = default;

	constexpr FORCEINLINE BasicStringView(BasicStringView&& other) noexcept = default;

	constexpr FORCEINLINE BasicStringView& operator=(const BasicStringView& other) noexcept = default;

	constexpr FORCEINLINE BasicStringView& operator=(BasicStringView&& other) noexcept = default;

	constexpr FORCEINLINE BasicStringView subString(usize start, usize end) const noexcept;

	constexpr FORCEINLINE bool startsWith(const T* str, usize sz = 0) const noexcept;

	constexpr FORCEINLINE bool startsWith(const BasicStringView& str) const noexcept;

	constexpr FORCEINLINE bool endsWith(const T* str, usize sz = 0) const noexcept;

	constexpr FORCEINLINE bool endsWith(const BasicStringView& str) const noexcept;

	constexpr FORCEINLINE usize find(const T* str, usize sz = 0) const noexcept;

	constexpr FORCEINLINE usize find(const BasicStringView& str) const noexcept;
};

template <typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(T* data, usize size) noexcept
	: Base(data, size) {}

template <typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const BasicString<T>& str) noexcept
	: BasicStringView(str.data(), str.size()) {}

template <typename T>
template <usize S>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const T (&str)[S]) noexcept
	: BasicStringView(str, S - 1) {}

template <typename T>
constexpr FORCEINLINE BasicStringView<T> BasicStringView<T>::subString(usize index, usize count) const noexcept {
	return this->subArray(index, count);
}

template <typename T>
constexpr FORCEINLINE bool BasicStringView<T>::startsWith(const T* str, usize sz) const noexcept {
	if (str != nullptr && sz == 0)
		sz = Utils::strlen(str);

	if (sz > this->m_Size)
		return false;

	return Utils::memCmp(this->m_Data, str, sz);
}

template <typename T>
constexpr FORCEINLINE bool BasicStringView<T>::startsWith(const BasicStringView& str) const noexcept {
	return this->startsWith(str.m_Data, str.m_Size);
}

template <typename T>
constexpr FORCEINLINE bool BasicStringView<T>::endsWith(const T* str, usize sz) const noexcept {
	if (str != nullptr && sz == 0)
		sz = Utils::strlen(str);

	if (sz > this->m_Size)
		return false;

	return Utils::memCmp(this->m_Data + this->m_Size - sz, str, sz);
}

template <typename T>
constexpr FORCEINLINE bool BasicStringView<T>::endsWith(const BasicStringView& str) const noexcept {
	return this->endsWith(str.m_Data, str.m_Size);
}

template <typename T>
constexpr FORCEINLINE usize BasicStringView<T>::find(const T* str, usize sz) const noexcept {
	// TODO : Implement this
	return std::numeric_limits<usize>::max();
}

template <typename T>
constexpr FORCEINLINE usize BasicStringView<T>::find(const BasicStringView& str) const noexcept {
	return this->find(str.m_Data, str.m_Size);
}

using StringView = BasicStringView<char>;
