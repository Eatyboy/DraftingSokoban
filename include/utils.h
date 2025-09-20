#pragma once

#include <stdint.h>
#include <cstddef>
#include <new>
#include <utility>
#include <cassert>

static inline float Smoothstep(float value) {
    return 3.0f * value * value - 2.0f * value * value * value;
}

struct Arena {
    std::byte* buffer = nullptr;
    size_t capacity = 0;
    size_t offset = 0;

    Arena(size_t newCapacity) {
        buffer = static_cast<std::byte*>(::operator new(newCapacity));
        capacity = newCapacity;
        offset = 0;
    }

    ~Arena() { ::operator delete(buffer); }

    void clear() noexcept { offset = 0; }

    template<typename T, typename... Args>
    T* alloc(Args&&... args) {
        size_t align = alignof(T);
        size_t alignedOffset = (offset + (align - 1)) & ~(align - 1);

        if (alignedOffset + sizeof(T) > capacity) {
            return nullptr;
        }

        void* ptr = static_cast<void*>(buffer + alignedOffset);

        T* obj = nullptr;
        try {
            obj = new (ptr) T(std::forward<Args>(args)...);
        } catch (...) {
            throw;
        }

        offset = alignedOffset + sizeof(T);
        return obj;
    }
};
