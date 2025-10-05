#include <utils.h>

const Int2 Int2::zero(0, 0);
const Int2 Int2::one(1, 1);
const Int2 Int2::up(0, -1);
const Int2 Int2::down(0, 1);
const Int2 Int2::left(-1, 0);
const Int2 Int2::right(1, 0);
const Int2 Int2::min(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
const Int2 Int2::max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

const Vec2 Vec2::zero(0, 0);
const Vec2 Vec2::one(1, 1);
const Vec2 Vec2::up(0, -1);
const Vec2 Vec2::down(0, 1);
const Vec2 Vec2::left(-1, 0);
const Vec2 Vec2::right(1, 0);
const Vec2 Vec2::min(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
const Vec2 Vec2::max(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());