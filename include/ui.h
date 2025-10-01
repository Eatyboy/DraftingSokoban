#pragma once

#include <vector>
#include <memory>
#include <stdint.h>
#include <utils.h>
#include <functional>
#include <string>
#include <renderer.h>
#include <algorithm>
#include <text.h>

namespace UI {
	struct MouseState {
		Vec2 mousePos = { 0, 0 };
        bool leftDown = false;
        bool leftPressed = false;
        bool leftUp = false;
        bool leftReleased = false;
        bool rightDown = false;
        bool rightPressed = false;
        bool rightUp = false;
        bool rightReleased = false;
	};

    enum class PositionMode {
        RELATIVE,
        ABSOLUTE,
    };

    struct Position {
        Vec2 position = { 0, 0 };
        PositionMode mode = PositionMode::RELATIVE;
    };

    static inline Position Absolute(Vec2 position) { return Position{ position, PositionMode::ABSOLUTE}; }

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
        float value;
    };

    struct Dim {
        Sizing width;
        Sizing height;
    };

    static inline Sizing Fit(float size) { return {SizingMode::FIT, size}; }
    static inline Sizing Grow(float size) { return {SizingMode::GROW, size}; }
    static inline Sizing Percent(float size) { return { SizingMode::PERCENT, std::clamp(size, 0.0f, 1.0f) }; }
    static inline Sizing Fixed(float size) { return { SizingMode::FIXED, size }; }
    static inline Sizing Fit() { return Fit(0); }
    static inline Sizing Grow() { return Grow(0); }

    struct Spacing {
        float left = 0;
        float top = 0;
        float right = 0;
        float bottom = 0;
    };

    static inline Spacing Margin(float margin) { return Spacing{ margin, margin, margin, margin }; }
    static inline Spacing Padding(float padding) { return Spacing{ padding, padding, padding, padding }; }

    struct Style {
        Texture image;

        Dim sizing = { Fit(), Fit() };
        Spacing padding;
        Spacing margin;

        Position positioning;
        FlexDir flexDir = FlexDir::ROW;
        AlignX alignX = AlignX::LEFT;
        AlignY alignY = AlignY::TOP;
        float childGap = 0;

        float roundness = 0;
        Color backgroundColor = GRAY;

        float borderWidth = 0;
        Color borderColor = DARKGRAY;

        Font* font = nullptr;
        float fontSize = 24;
        Color textColor = BLACK;
        float fontSpacing = 2;
        bool isWrap = false;
    };

    constexpr float rounded_sm = 2.0f;
    constexpr float rounded = 4.0f;
	constexpr float rounded_md = 6.0f;
	constexpr float rounded_lg = 8.0f;
	constexpr float rounded_xl = 12.0f;
	constexpr float rounded_2xl = 16.0f;
	constexpr float rounded_3xl = 24.0f;
    constexpr float rounded_full = -1.0f;

    struct Element {
        Element* parent = nullptr;
        std::vector<Element*> children;

        Vec2 position = Vec2::zero;
        Vec2 size = Vec2::zero;
        Style style;
        std::string label;
        uint64_t id = 0;

        std::function<void(Element&)> onHover = nullptr;
        std::function<void(Element&)> onActive = nullptr;
        std::function<void(Element&)> onClick = nullptr;

        inline float width() const { return size.x; }
        inline float height() const { return size.y; }

        inline Rect GetRect() const {
            return Rect{
                position.x,
                position.y,
                size.x,
                size.y
            };
        }

        inline bool isText() const {
            return style.font != nullptr;
        }
    };

	struct Callbacks {
		const char* label = nullptr;
		std::function<void(Element&)> onHover = nullptr;
		std::function<void(Element&)> onActive = nullptr;
		std::function<void(Element&)> onClick = nullptr;
	};

    struct PanelStyle {
        Texture image;

        Dim sizing = { Fit(), Fit() };
        Spacing padding;
        Spacing margin;

        Position positioning;
        FlexDir flexDir = FlexDir::ROW;
        AlignX alignX = AlignX::LEFT;
        AlignY alignY = AlignY::TOP;
        float childGap = 0;

        float roundness = 0;
        Color backgroundColor = WHITE;

        float borderWidth = 0;
        Color borderColor = DARKGRAY;

        bool isActive = true;

        inline Style toStyle() const {
            return Style{
                .image = image,
                .sizing = sizing,
                .padding = padding,
                .margin = margin,
                .positioning = positioning,
                .flexDir = flexDir,
                .alignX = alignX,
                .alignY = alignY,
                .childGap = childGap,
                .roundness = roundness,
                .backgroundColor = backgroundColor,
                .borderWidth = borderWidth,
                .borderColor = borderColor,
            };
        }
    };

    struct TextStyle {
        Font* font = nullptr;
        float fontSize = 24;
        Color textColor = BLACK;
        float fontSpacing = 2;
        bool isWrap = true;

        Position positioning;
        AlignX alignX = AlignX::LEFT;
        AlignY alignY = AlignY::TOP;
        Spacing margin;

        bool isActive = true;

        inline Style toStyle() const {
            return Style{
                .sizing = { Grow(), Fixed(0) },
                .margin = margin,
                .positioning = positioning,
                .alignX = alignX,
                .alignY = alignY,
                .font = font,
                .fontSize = fontSize,
                .textColor = textColor,
                .isWrap = isWrap,
            };
        }
    };

    inline uint64_t GetElementID(const char* label, Element* parent) {
        uint64_t hash = 1469598103934665603ull;
        while (*label) {
            hash ^= (unsigned char)(*label++);
            hash *= 1099511628211ull;
        }
        return hash ^ reinterpret_cast<uintptr_t>(parent);
    }

    inline void AddCallbacks(Element* element, Callbacks callbacks) {
        if (element == nullptr) return;
        if (callbacks.label == nullptr) return;

		element->id = GetElementID(callbacks.label, element->parent);
		element->onHover = callbacks.onHover;
        element->onActive = callbacks.onActive;
		element->onClick = callbacks.onClick;
    }
}

class UIContext {
public:
    Vec2 screenSize = Vec2::zero;
    UI::Element* root = nullptr;
    uint64_t hotID = 0;
    uint64_t activeID = 0;

    UIContext(size_t arenaSize = sizeof(UI::Element) * 256) : elementArena(arenaSize) {
        static Font defaultfont = GetDefaultFont();
		this->defaultFont = defaultFont; 
    }

    void BeginUI(Vec2 currentScreenSize, UI::MouseState currentFrameMouseState, UI::FlexDir rootFlexDir = UI::FlexDir::ROW);
    void EndUI();

    void Render(UI::Element* element);
    void Render();

    void Panel(const UI::PanelStyle& style);
    void Panel(const UI::PanelStyle& style, const std::function<void()> children);
    void Panel(const UI::PanelStyle& style, UI::Callbacks callbacks);
    void Panel(const UI::PanelStyle& style, UI::Callbacks callbacks, const std::function<void()> children);
    void Text(std::string text, const UI::TextStyle& style);
private:
    Arena elementArena;
    UI::MouseState mouseState;
    bool isContextActive = false;
    Font* defaultFont = nullptr;
    std::vector<uint64_t> idStack;
    std::vector<UI::Element*> elementStack;

    void OpenElement(const UI::Style& style = {});
    void CloseElement();

    void FitHeights(UI::Element* element);
    void WrapText();
    void GrowWidths();
    void GrowHeights();
    void LayoutElements();
    void ResolveCallbacks();
};
