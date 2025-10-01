#include <gameObjects.h>
#include <utils.h>

ObjectType stringToObjectType(const std::string& str) {
    static const std::unordered_map<std::string, ObjectType> typeMap = {
        {"Box", ObjectType::Box},
    };

    auto it = typeMap.find(str);
    if (it != typeMap.end()) {
        return it->second;
    }
    return ObjectType::Unknown;
}

void Push(Pushable& pushable, Int2 direction) {
    pushable.moveDelta = direction;
    pushable.move_t = 0.0f;
    pushable.isMoving = true;
}

void UpdatePushable(Pushable& pushable, ObjectData& object, float dt) {
    if (pushable.isMoving) {
        pushable.move_t += dt;
        object.offset = Vec2(pushable.moveDelta * object.size) * Smoothstep(pushable.move_t);
        if (pushable.move_t >= 1.0f) {
            pushable.move_t = 0.0f;
            pushable.isMoving = false;
            object.position += pushable.moveDelta;
            object.offset = Vec2::zero;
        }
    }
}
