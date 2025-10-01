#pragma once

#include <cmath>
#include <cstddef>
#include <new>
#include <utility>
#include <cassert>
#include <limits>

#include <tmxlite/Types.hpp>
#include <glm/glm.hpp>

constexpr float EPSILON = 0.000001f;

inline bool Approximately(float a, float b) {
	return std::fabsf(a - b) < EPSILON;
}

struct Vec2;

struct Int2 {
    int x = 0;
    int y = 0;

	Int2() = default;
    Int2(int x, int y) : x(x), y(y) {}
    Int2(float x, float y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}
	Int2(Vec2 v);
    template <typename T>
	Int2(tmx::Vector2<T> v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)) {}

    struct Hash {
        size_t operator()(const Int2& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };

	Int2 operator+(const Int2& other) const {
		return Int2(x + other.x, y + other.y);
	}

	Int2 operator-(const Int2& other) const {
		return Int2(x - other.x, y + other.y);
	}

	Int2 operator*(const Int2& other) const {
		return Int2(x * other.x, y * other.y);
	}

	Int2 operator/(const Int2& other) const {
		return Int2(x / other.x, y / other.y);
	}

	Int2 operator*(int scalar) const {
		return Int2(x * scalar, y * scalar);
	}

	Int2 operator/(int scalar) const {
		return Int2(x / scalar, y / scalar);
	}

	friend Int2 operator*(int scalar, const Int2& i) {
		return Int2(i.x * scalar, i.y * scalar);
	}

	Int2 operator-() const {
		return Int2(-x, -y);
	}

	Int2 operator%(const Int2& other) const {
		return Int2(x % other.x, y % other.y);
	}

	Int2& operator+=(const Int2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Int2& operator-=(const Int2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Int2& operator*=(const Int2& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Int2& operator/=(const Int2& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Int2& operator*=(int scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Int2& operator/=(int scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}

	bool operator==(const Int2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const Int2& other) const {
		return x != other.x || y != other.y;
	}

	operator glm::vec2() const {
		return glm::vec2(static_cast<float>(x), static_cast<float>(y));
	}

	int dot(Int2 a, Int2 b) {
		return a.x * b.x + a.y * b.y;
	}

	float distance(Int2 a, Int2 b) {
		return std::sqrtf(static_cast<float>(distanceSq(a, b)));
	}

	int distanceSq(Int2 a, Int2 b) {
		return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	}

	float magnitude() const {
		return std::sqrtf(static_cast<float>(this->magnitudeSq()));
	}

	int magnitudeSq() const {
		return x * x + y * y;
	}

	static const Int2 zero;
	static const Int2 one;
	static const Int2 up;
	static const Int2 down;
	static const Int2 left;
	static const Int2 right;
	static const Int2 min;
	static const Int2 max;
};

const Int2 Int2::zero(0, 0);
const Int2 Int2::one(1, 1);
const Int2 Int2::up(0, -1);
const Int2 Int2::down(0, 1);
const Int2 Int2::left(-1, 0);
const Int2 Int2::right(1, 0);
const Int2 Int2::min(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
const Int2 Int2::max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

	Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
	Vec2(Int2 i) : x(static_cast<float>(i.x)), y(static_cast<float>(i.y)) {}
    template <typename T>
    Vec2(tmx::Vector2<T> v) {
        this->x = v.x;
        this->y = v.y;
    }

    struct Hash {
        size_t operator()(const Vec2& v) const {
            return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1);
        }
    };

	Vec2 operator+(const Vec2& other) const {
		return Vec2(x + other.x, y + other.y);
	}

	Vec2 operator-(const Vec2& other) const {
		return Vec2(x - other.x, y + other.y);
	}

	Vec2 operator*(const Vec2& other) const {
		return Vec2(x * other.x, y * other.y);
	}

	Vec2 operator/(const Vec2& other) const {
		return Vec2(x / other.x, y / other.y);
	}

	Vec2 operator*(int scalar) const {
		return Vec2(x * scalar, y * scalar);
	}

	Vec2 operator/(int scalar) const {
		return Vec2(x / scalar, y / scalar);
	}

	friend Vec2 operator*(int scalar, const Vec2& i) {
		return Vec2(i.x * scalar, i.y * scalar);
	}

	Vec2 operator-() const {
		return Vec2(-x, -y);
	}

	Vec2& operator+=(const Vec2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2& operator-=(const Vec2& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2& operator*=(const Vec2& other) {
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vec2& operator/=(const Vec2& other) {
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Vec2& operator*=(int scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vec2& operator/=(int scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}

	operator glm::vec2() const {
		return glm::vec2(x, y);
	}

	bool operator==(const Vec2& other) const {
		return Approximately(x, other.x) && Approximately(y, other.y);
	}

	bool operator!=(const Vec2& other) const {
		return !Approximately(x, other.x) || !Approximately(y, other.y);
	}

	float dot(Vec2 a, Vec2 b) {
		return a.x * b.x + a.y * b.y;
	}

	float distance(Vec2 a, Vec2 b) {
		return std::sqrtf(static_cast<float>(distanceSq(a, b)));
	}

	float distanceSq(Vec2 a, Vec2 b) {
		return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	}

	float magnitude() const {
		return std::sqrtf(static_cast<float>(this->magnitudeSq()));
	}

	float magnitudeSq() const {
		return x * x + y * y;
	}

	static const Vec2 zero;
	static const Vec2 one;
	static const Vec2 up;
	static const Vec2 down;
	static const Vec2 left;
	static const Vec2 right;
	static const Vec2 min;
	static const Vec2 max;
};

const Vec2 Vec2::zero(0, 0);
const Vec2 Vec2::one(1, 1);
const Vec2 Vec2::up(0, -1);
const Vec2 Vec2::down(0, 1);
const Vec2 Vec2::left(-1, 0);
const Vec2 Vec2::right(1, 0);
const Vec2 Vec2::min(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
const Vec2 Vec2::max(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

inline Int2::Int2(Vec2 v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)) {}

struct Rect {
	float x;
	float y;
	float width;
	float height;

	Rect() = default;
	Rect(float x, float y, float width, float height): x(x), y(y), width(width), height(height) {}
	Rect(Vec2 pos, Vec2 size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}
};

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

    ~Arena() { if (buffer) ::operator delete(buffer); }

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
