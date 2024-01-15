#pragma once

#include "Basics.hpp"

template <typename T, size_t Size>
class LockFreeQueue {
public:
    LockFreeQueue() : head(0), tail(0) {
        buffer.resize(Size);
    }

    void Enqueue(const T& value) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail;

        do {
            next_tail = (current_tail + 1) % Size;

            // Check if the queue is full
            if (next_tail == head.load(std::memory_order_acquire)) {
                // The queue is full, retry
                continue;
            }

            // Try to update the tail
        } while (!tail.compare_exchange_weak(current_tail, next_tail, std::memory_order_relaxed));

        // The tail is updated, now store the value
        buffer[current_tail] = value;
    }

    bool Dequeue(T& result) {
        size_t current_head = head.load(std::memory_order_relaxed);
        size_t next_head;

        do {
            next_head = (current_head + 1) % Size;

            // Check if the queue is empty
            if (current_head == tail.load(std::memory_order_acquire)) {
                // The queue is empty, retry
                return false;
            }

            // Try to update the head
        } while (!head.compare_exchange_weak(current_head, next_head, std::memory_order_relaxed));

        // The head is updated, now load the value
        result = buffer[current_head];
        return true;
    }

private:
    std::vector<T> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};