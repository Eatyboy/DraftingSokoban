#pragma once

#include <raylib.h>
#include <tmxlite/Types.hpp>
#include <utility>

struct Int2 {
    int x;
    int y;

    Int2() = default;
    Int2(int x, int y);
    Int2(float x, float y);
    Int2(Vector2 v);
    template <typename T>
    Int2(tmx::Vector2<T> v) {
        this->x = (int)v.x;
        this->y = (int)v.y;
    }

    struct Hash {
        size_t operator()(const Int2& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };

    Int2 operator+(const Int2& other) const;
    Int2 operator-(const Int2& other) const;
    Int2 operator*(const Int2& other) const;
    Int2 operator/(const Int2& other) const;

    Int2 operator*(int scalar) const;
    Int2 operator/(int scalar) const;
    friend Int2 operator*(int scalar, const Int2& i);
    Int2 operator-() const;
    Int2 operator%(const Int2& other) const;

    Int2& operator+=(const Int2& other);
    Int2& operator-=(const Int2& other);
    Int2& operator*=(const Int2& other);
    Int2& operator/=(const Int2& other);
    Int2& operator*=(int scalar);
    Int2& operator/=(int scalar);

    bool operator==(const Int2& other) const;
    bool operator!=(const Int2& other) const;

    static int dot(Int2 a, Int2 b);
    static float distance(Int2 a, Int2 b);
    static int distanceSq(Int2 a, Int2 b);
    Vector2 toVector2() const;
    float magnitude() const;
    int magnitudeSq() const;

    static const Int2 zero;
    static const Int2 one;
    static const Int2 up;
    static const Int2 down;
    static const Int2 left;
    static const Int2 right;
    static const Int2 min;
    static const Int2 max;
};
