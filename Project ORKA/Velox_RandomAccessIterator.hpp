#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include <iterator>

template <typename DataType>
class RandomAccessIterator : public std::iterator<
		std::random_access_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&> {
public:
	RandomAccessIterator() noexcept : m_Current(nullptr) { }
	RandomAccessIterator(DataType* node) noexcept : m_Current(node) { }

	RandomAccessIterator(const RandomAccessIterator<DataType>& rawIterator) noexcept = default;
	RandomAccessIterator& operator=(const RandomAccessIterator<DataType>& rawIterator) noexcept = default;

	RandomAccessIterator(RandomAccessIterator<DataType>&& rawIterator) noexcept = default;
	RandomAccessIterator& operator=(RandomAccessIterator<DataType>&& rawIterator) noexcept = default;

	~RandomAccessIterator() {}

	bool operator!=(const RandomAccessIterator& iterator) noexcept { return m_Current != iterator.m_Current; }

	DataType& operator*() noexcept { return *m_Current; }
	const DataType& operator*() const noexcept { return (*m_Current); }

	DataType* operator->() noexcept { return m_Current; }
	const DataType* operator->() const noexcept { return m_Current; }

	DataType* getPointer() const noexcept { return m_Current; }
	const DataType* getConstPtr() const noexcept { return m_Current; }

	RandomAccessIterator<DataType>& operator+=(const ptrdiff_t& movement) noexcept {
		m_Current += movement;
		return (*this);
	}

	RandomAccessIterator<DataType>& operator-=(const ptrdiff_t& movement) noexcept {
		m_Current -= movement;
		return (*this);
	}

	RandomAccessIterator<DataType>& operator++() noexcept {
		++m_Current;
		return (*this);
	}

	RandomAccessIterator<DataType>& operator--() noexcept {
		--m_Current;
		return (*this);
	}

	RandomAccessIterator<DataType> operator++(int) noexcept {
		auto temp(*this);
		++m_Current;
		return temp;
	}

	RandomAccessIterator<DataType> operator--(int) noexcept {
		auto temp(*this);
		--m_Current;
		return temp;
	}

	RandomAccessIterator<DataType> operator+(const ptrdiff_t& movement) noexcept {
		auto oldPtr = m_Current;
		m_Current += movement;
		auto temp(*this);
		m_Current = oldPtr;
		return temp;
	}

	RandomAccessIterator<DataType> operator-(const ptrdiff_t& movement) noexcept {
		auto oldPtr = m_Current;
		m_Current -= movement;
		auto temp(*this);
		m_Current = oldPtr;
		return temp;
	}

	operator DataType*() { return m_Current; }

private:
	DataType* m_Current;
};
