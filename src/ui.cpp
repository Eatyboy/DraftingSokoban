#include <ui.h>
#include <raymath.h>

void UIContext::BeginUI(MouseState currentFrameMouseState) {
    elementArena.clear();
    root = nullptr;
    current = nullptr;
    stack.clear();

    mouseState = currentFrameMouseState;

    isContextActive = true;
}

void UIContext::EndUI() {
    isContextActive = false;
}

void UIContext::OpenElement(Vector2 position, const UI::Style& style) {
    if (!isContextActive) return;

    UI::Element* element = elementArena.alloc<UI::Element>();
    if (element == nullptr) return;

    element->position = position;
    element->style = style;
    element->size = style.sizing.size;

    if (root == nullptr) root = element;
    stack.push_back(element);
    current = element;
}

void UIContext::CloseElement() {
    if (!isContextActive) return;

    UI::Element* element = stack.back();
}

void UIContext::ComputeLayout(UI::Element* element) {

}

void UIContext::Render(UI::Element* element) {
    if (element == nullptr) return;

    const UI::Style& style = element->style;

    int segments = 8;
    if (style.roundness == 0) {
        DrawRectangleRec(element->GetRect(), style.backgroundColor);
        if (style.borderWidth > 0) DrawRectangleLinesEx(element->GetRect(), style.borderWidth, style.borderColor);
    }
    else {
        DrawRectangleRounded(element->GetRect(), style.roundness, segments, style.backgroundColor);
        if (style.borderWidth > 0) DrawRectangleRoundedLinesEx(element->GetRect(), style.roundness, segments, style.borderWidth, style.borderColor);
    }

    for (UI::Element* element : element->children) {
        Render(element);
    }
}

void UIContext::Render() {
    Render(root);
}

bool UIContext::Button(const char* label, Vector2 position, const UI::Style& style) {
    bool wasClicked = false;

    OpenElement(position, style);
    CloseElement();

    return wasClicked;
}
