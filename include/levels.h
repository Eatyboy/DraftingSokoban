#pragma once

#include <tiles.h>
#include <utils.h>
#include <gameObjects.h>

#include <unordered_set>
#include <vector>

enum class LevelExits {
    None = 0,
    Top = 1 << 0,
    Bottom = 1 << 1,
    Left = 1 << 2,
    Right = 1 << 3,
};

inline LevelExits operator|(LevelExits a, LevelExits b) {
    return static_cast<LevelExits>(static_cast<int>(a) | static_cast<int>(b));
}

struct Level {
    std::vector<ChunkLayer> layers;
    std::unordered_set<Int2, Int2::Hash> collisionMap;
    std::vector<ObjectData> objects;

    Int2 size = Int2::zero;

    LevelExits exits = LevelExits::None;
};

Level* LoadLevel(const char* filename);
