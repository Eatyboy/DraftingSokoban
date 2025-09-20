#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <string>

#include <tiles.h>
#include <int2.h>

class Tilemap;

enum class ObjectType {
    Box,
    Unknown
};

struct ObjectData {
    Int2 position = Int2::zero;
    Int2 size = Int2::zero;
    Vector2 offset = Vector2{ 0, 0 };
    ObjectType type;
    uint32_t tileGID = 0;
    float rotation = 0.0f;
    bool visible = false;
};

struct Pushable {
    Int2 moveDelta = Int2::zero;
    float move_t = 0.0f;
    bool isMoving = false;
};

struct Box {
    uint32_t ID = UINT32_MAX;
    Box(uint32_t initID) : ID(initID) {}

    Pushable pushData = {};
};

struct GameObjects {
    std::vector<Box> boxes;
};

ObjectType stringToObjectType(const std::string& str);
void Push(Pushable& pushable, Int2 direction);
void UpdatePushable(Pushable& pushable, ObjectData& object, float dt);
static inline bool isPushable(ObjectType objType) {
    switch (objType) {
    case ObjectType::Box: return true;
    default: return false;
    }
}
