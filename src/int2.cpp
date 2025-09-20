#include <int2.h>
#include <cmath>

Int2::Int2(int x, int y) {
    this->x = x;
    this->y = y;
}

Int2::Int2(float x, float y) {
    this->x = (int)x;
    this->y = (int)y;
}

Int2::Int2(Vector2 v) {
    this->x = (int)v.x;
    this->y = (int)v.y;
}

const Int2 Int2::zero(0, 0);
const Int2 Int2::one(1, 1);
const Int2 Int2::up(0, -1);
const Int2 Int2::down(0, 1);
const Int2 Int2::left(-1, 0);
const Int2 Int2::right(1, 0);
const Int2 Int2::min(INT32_MIN, INT32_MIN);
const Int2 Int2::max(INT32_MAX, INT32_MAX);

Int2 Int2::operator+(const Int2& other) const {
    return Int2(x + other.x, y + other.y);
}

Int2 Int2::operator-(const Int2& other) const {
    return Int2(x - other.x, y + other.y);
}

Int2 Int2::operator*(const Int2& other) const {
    return Int2(x * other.x, y * other.y);
}

Int2 Int2::operator/(const Int2& other) const {
    return Int2(x / other.x, y / other.y);
}

Int2 Int2::operator*(int scalar) const {
    return Int2(x * scalar, y * scalar);
}

Int2 Int2::operator/(int scalar) const {
    return Int2(x / scalar, y / scalar);
}

Int2 operator*(int scalar, const Int2& i) {
    return Int2(i.x * scalar, i.y * scalar);
}

Int2 Int2::operator-() const {
    return Int2(-x, -y);
}

Int2 Int2::operator%(const Int2& other) const {
    return Int2(x % other.x, y % other.y);
}

Int2& Int2::operator+=(const Int2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Int2& Int2::operator-=(const Int2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Int2& Int2::operator*=(const Int2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

Int2& Int2::operator/=(const Int2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

Int2& Int2::operator*=(int scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Int2& Int2::operator/=(int scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

bool Int2::operator==(const Int2& other) const {
    return x == other.x && y == other.y;
}

bool Int2::operator!=(const Int2& other) const {
    return x != other.x || y != other.y;
}

int Int2::dot(Int2 a, Int2 b) {
    return a.x * b.x + a.y * b.y;
}

float Int2::distance(Int2 a, Int2 b) {
    return std::sqrtf(static_cast<float>(distanceSq(a, b)));
}

int Int2::distanceSq(Int2 a, Int2 b) {
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

Vector2 Int2::toVector2() const {
    return Vector2{ (float)x, (float)y };
}

float Int2::magnitude() const {
    return std::sqrtf(static_cast<float>(this->magnitudeSq()));
}

int Int2::magnitudeSq() const {
    return x * x + y * y;
}

