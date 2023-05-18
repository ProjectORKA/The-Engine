#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

template <typename T>
union PoolChunk {
	PoolChunk<T>* next = nullptr;
	T value;
	PoolChunk() {}
	~PoolChunk() {}
};

template <typename T>
struct PoolAllocator {
	UInt count = 0;
	PoolChunk<T> data = nullptr;
	PoolChunk<T> head = nullptr;

	~PoolAllocator() {
		delete[] data;
		data = nullptr;
		head = nullptr;
	}

	void free(T* data) {
		if (data < this->data || data + sizeof(T) > this->data + count * sizeof(T)) logError(
			"Data can't be freed, its outside pool allocator!");
		data->~T();
		PoolChunk<T>* poolChunk = reinterpret_cast<PoolChunk<T>*>(data);
		poolChunk->next = head;
		head = poolChunk;
	}

	explicit PoolAllocator(UInt count = 128) {
		this->count = count;
		data = new PoolChunk<T>[count];
		head = data;

		for (UInt i = 0; i < count - 1; i++) { data[i] = std::addressof(data[i + 1]); }

		data[count - 1].next = nullptr;
	}

	template <typename... arguments>
	T* allocate(arguments&&... args) {
		if (head == nullptr) {
			logError("Pool allocator is full!");
			return nullptr;
		}

		PoolChunk<T>* poolChunk = head;
		head = head.next;

		return new(std::addressof(poolChunk->value)) T(std::forward<arguments>(args)...);
	}

	PoolAllocator(const PoolAllocator& other) = delete;
	PoolAllocator(const PoolAllocator&& other) = delete;
	PoolAllocator& operator=(const PoolAllocator& other) = delete;
	PoolAllocator& operator=(const PoolAllocator&& other) = delete;
};
