#pragma once

#include <vector>
#include <raylib.h>
#include <memory>
#include <stdint.h>
#include <utils.h>
#include <raymath.h>

namespace UI {
    enum class FlexDir {
        ROW,
        COLUMN,
    };

    enum class AlignX {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum class AlignY {
        TOP,
        CENTER,
        BOTTOM,
    };

    enum class SizingMode {
        FIT,
        GROW,
        PERCENT,
        FIXED,
    };

    struct Sizing {
        SizingMode mode = SizingMode::FIT;
        Vector2 size = { 0, 0 };
    };

    static inline Sizing Fit(Vector2 size) { return {SizingMode::FIT, size}; }
    static inline Sizing Grow(Vector2 size) { return {SizingMode::GROW, size}; }
    static inline Sizing Percent(Vector2 size) { return { SizingMode::PERCENT, Vector2ClampValue(size, 0.0f, 1.0f) }; }
    static inline Sizing Fixed(Vector2 size) { return { SizingMode::FIXED, size }; }
    static inline Sizing Fit(float width, float height) { return Fit({ width, height }); }
    static inline Sizing Grow(float width, float height) { return Grow({ width, height }); }
    static inline Sizing Percent(float width, float height) { return Percent({ width, height }); }
    static inline Sizing Fixed(float width, float height) { return Fixed({ width, height }); }
    static inline Sizing Fit() { return Fit(0, 0); }
    static inline Sizing Grow() { return Grow(0, 0); }

    struct Spacing {
        float left = 0;
        float top = 0;
        float right = 0;
        float bottom = 0;

        Spacing() = default;
        Spacing(float value) : left(value), top(value), right(value), bottom(value) {}
        Spacing(int value) : left((float)value), top((float)value), right((float)value), bottom((float)value) {}
    };

    struct Style {
        FlexDir flexDir = FlexDir::ROW;
        AlignX alignX = AlignX::LEFT;
        AlignY alignY = AlignY::TOP;
        Sizing sizing = Fit({ 0, 0 });
        Color backgroundColor = GRAY;
        float childGap = 0;
        Spacing padding;
        Spacing margin;
        float roundness = 0;
        float borderWidth = 0;
        Color borderColor = DARKGRAY;
    };

    struct Element {
        Element* parent = nullptr;
        std::vector<Element*> children;

        Vector2 position = { 0, 0 };
        Vector2 size = { 0, 0 };
        Style style;
        const char* label = nullptr;
        bool isLeaf = false;

        inline Rectangle GetRect() const {
            return Rectangle{
                position.x,
                position.y,
                size.x,
                size.y
            };
        }
    };

    inline uint64_t HashID(uint64_t seed, const char* str) {
        uint64_t hash = seed ? seed : 1469598103934665603ull;
        while (*str) {
            hash ^= (unsigned char)(*str++);
            hash *= 1099511628211ull;
        }
        return hash;
    }
}

struct MouseState {
    Vector2 mousePos = { 0, 0 };
    bool mouseLeftDown = false;
    bool mouseLeftUp = false;
};

class UIContext {
public:
    UI::Element* root = nullptr;
    UI::Element* current = nullptr;
    uint64_t hotID = 0;
    uint64_t activeID = 0;

    UIContext(size_t arenaSize = sizeof(UI::Element) * 256) : elementArena(arenaSize) {}

    uint64_t PushID(const char* str);
    uint64_t PopID();
    uint64_t GetID(const char* str);

    void BeginUI(MouseState currentFrameMouseState);
    void EndUI();

    void ComputeLayout(UI::Element* element);
    void Render(UI::Element* element);
    void Render();

    template <typename Fn>
    void Panel(Vector2 position, const UI::Style& style, Fn children) {
        OpenElement(position, style);
        children();
        CloseElement();
    }
    bool Button(const char* label, Vector2 position, const UI::Style& style);
private:
    Arena elementArena;
    std::vector<UI::Element*> stack;
    MouseState mouseState;
    bool isContextActive = false;
    std::vector<uint64_t> idStack;

    void OpenElement(Vector2 position, const UI::Style& style = {});
    void CloseElement();
};
