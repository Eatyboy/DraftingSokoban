#include <ui.h>
#include <cmath>

void UIContext::BeginUI(Vec2 currentScreenSize, UI::MouseState currentFrameMouseState, UI::FlexDir rootFlexDir) {
    elementArena.clear();
    elementStack.clear();

    screenSize = currentScreenSize;
    mouseState = currentFrameMouseState;

    isContextActive = true;

    OpenElement(UI::Style{.sizing = {UI::Fixed(screenSize.x), UI::Fixed(screenSize.y)}, .flexDir = rootFlexDir, .backgroundColor = BLANK});
    root = elementStack.back();
}

void UIContext::EndUI() {
    CloseElement();
    isContextActive = false;

    GrowWidths();
    WrapText();
    FitHeights(root);
    GrowHeights();
    LayoutElements();
    ResolveCallbacks();
}

void UIContext::OpenElement(const UI::Style& style) {
    if (!isContextActive) return;

    UI::Element* element = elementArena.alloc<UI::Element>();
    if (element == nullptr) return;

    if (!elementStack.empty()) {
		element->parent = elementStack.back();
        elementStack.back()->children.push_back(element);
    }

    element->style = style;
    if (style.positioning.mode == UI::PositionMode::ABSOLUTE) element->position = style.positioning.position;
    if (style.sizing.width.mode == UI::SizingMode::FIXED) element->size.x = style.sizing.width.value;
    if (style.sizing.height.mode == UI::SizingMode::FIXED) element->size.y = style.sizing.height.value;

    elementStack.push_back(element);
}

void UIContext::CloseElement() {
    if (!isContextActive) return;

    UI::Element* element = elementStack.back();
    elementStack.pop_back();
    UI::Element* parent = element->parent;

    if (parent == nullptr) return;

    if (element->isText()) {
        element->size = MeasureTextEx(*element->style.font, element->label.c_str(), element->style.fontSize, element->style.fontSpacing);
    }

    float totalChildGap = fmaxf(static_cast<float>(element->children.size()) - 1, 0) * element->style.childGap;


    if (element->style.sizing.width.mode == UI::SizingMode::FIT) {
		Texture image = element->style.image;
        if (image.id != 0) {
            element->size.x = image.width;
        } else {
			element->size.x += element->style.padding.left + element->style.padding.right;
			if (element->style.flexDir == UI::FlexDir::ROW) element->size.x += totalChildGap;
        }
    }

    if (parent->style.sizing.width.mode == UI::SizingMode::FIT) {
        float effectiveWidth = element->size.x + element->style.margin.left + element->style.margin.right;
        parent->size.x = parent->style.flexDir == UI::FlexDir::ROW
            ? parent->size.x + effectiveWidth
            : fmaxf(effectiveWidth, parent->size.x);
    }
}

void UIContext::FitHeights(UI::Element* element) {
    if (element == nullptr) return;
    UI::Element* parent = element->parent;

    for (auto* child : element->children) FitHeights(child);

    float totalChildGap = fmaxf(static_cast<float>(element->children.size()) - 1, 0) * element->style.childGap;

    if (element->style.sizing.height.mode == UI::SizingMode::FIT) {
		Texture image = element->style.image;
        if (image.id != 0) {
            element->size.y = image.height;
        } else {
            element->size.y += element->style.padding.top + element->style.padding.bottom;
            if (element->style.flexDir == UI::FlexDir::COLUMN) element->size.y += totalChildGap;
        }
    }

    if (parent == nullptr) return;

	if (parent->style.sizing.height.mode == UI::SizingMode::FIT) {
        float effectiveHeight = element->size.y + element->style.margin.top + element->style.margin.bottom;
        parent->size.y = parent->style.flexDir == UI::FlexDir::ROW
            ? fmaxf(effectiveHeight, parent->size.y) 
            : parent->size.y + effectiveHeight;
	}
}

void UIContext::WrapText() {
    elementStack.clear();
    elementStack.push_back(root);
    while (!elementStack.empty()) {
        UI::Element* element = elementStack.back();
        elementStack.pop_back();
        for (UI::Element* child : element->children) elementStack.push_back(child);

        if (!element->isText() || !element->style.isWrap) continue;
    }
}

void UIContext::GrowWidths() {
    elementStack.clear();
    elementStack.push_back(root);
    while (!elementStack.empty()) {
        UI::Element* element = elementStack.back();
        elementStack.pop_back();
        for (UI::Element* child : element->children) elementStack.push_back(child);

        bool isFlexRow = element->style.flexDir == UI::FlexDir::ROW;

        float remainingWidth = element->width();
        remainingWidth -= element->style.padding.left + element->style.padding.right;

        std::vector<UI::Element*> growableWidths;
        for (UI::Element* child : element->children) {
            if (isFlexRow) remainingWidth -= child->width() + child->style.margin.left + child->style.margin.right;
            if (child->style.sizing.width.mode == UI::SizingMode::GROW) growableWidths.push_back(child);
        }

        if (growableWidths.empty()) continue;

        if (!isFlexRow) {
            for (auto* child : growableWidths) {
                child->size.x = remainingWidth - child->style.margin.left - child->style.margin.right;
            }
            continue;
        }
        
        remainingWidth -= fmaxf(static_cast<float>(element->children.size()) - 1, 0) * element->style.childGap;

        while (remainingWidth > EPSILON) {
            float smallestWidth = growableWidths.front()->width();
            float secondSmallestWidth = std::numeric_limits<float>::max();
            float widthToAdd = remainingWidth;
            for (auto* child : growableWidths) {
                if (child->width() < smallestWidth) {
                    secondSmallestWidth = smallestWidth;
                    smallestWidth = child->width();
                }
                if (child->width() > smallestWidth) {
                    secondSmallestWidth = fminf(secondSmallestWidth, child->width());
                    widthToAdd = secondSmallestWidth - smallestWidth;
                }
            }

            widthToAdd = fminf(widthToAdd, remainingWidth / growableWidths.size());

            for (auto* child : growableWidths) {
                if (Approximately(child->width(), smallestWidth)) {
                    child->size.x += widthToAdd;
                    remainingWidth -= widthToAdd;
                }
            }
        }
    }
}

void UIContext::GrowHeights() {
    elementStack.clear();
    elementStack.push_back(root);
    while (!elementStack.empty()) {
        UI::Element* element = elementStack.back();
        elementStack.pop_back();
        for (UI::Element* child : element->children) elementStack.push_back(child);

        bool isFlexColumn = element->style.flexDir == UI::FlexDir::COLUMN;

        float remainingHeight = element->height();
        remainingHeight -= element->style.padding.top + element->style.padding.bottom;

        std::vector<UI::Element*> growableHeights;
        for (UI::Element* child : element->children) {
            if (isFlexColumn) remainingHeight -= child->height() + child->style.margin.top + child->style.margin.bottom;
            if (child->style.sizing.height.mode == UI::SizingMode::GROW) growableHeights.push_back(child);
        }

        if (growableHeights.empty()) continue;

        if (!isFlexColumn) {
            for (auto* child : growableHeights) {
				child->size.y = remainingHeight - child->style.margin.top - child->style.margin.bottom;
            }
            continue;
        }

        remainingHeight -= fmaxf(static_cast<float>(element->children.size()) - 1, 0) * element->style.childGap;

        while (remainingHeight > EPSILON) {
            float smallestHeight = growableHeights.front()->height();
            float secondSmallestHeight = std::numeric_limits<float>::max();
            float heightToAdd = remainingHeight;
            for (auto* child : growableHeights) {
                if (child->height() < smallestHeight) {
                    secondSmallestHeight = smallestHeight;
                    smallestHeight = child->height();
                }
                if (child->height() > smallestHeight) {
                    secondSmallestHeight = fminf(secondSmallestHeight, child->height());
                    heightToAdd = secondSmallestHeight - smallestHeight;
                }
            }

            heightToAdd = fminf(heightToAdd, remainingHeight / growableHeights.size());

            for (auto* child : growableHeights) {
                if (Approximately(child->height(), smallestHeight)) {
                    child->size.y += heightToAdd;
                    remainingHeight -= heightToAdd;
                }
            }
        }
    }
}

void UIContext::LayoutElements() {
    elementStack.clear();
    elementStack.push_back(root);
    while (!elementStack.empty()) {
        UI::Element* element = elementStack.back();
        elementStack.pop_back();

        element->position.x += element->style.margin.left;
        element->position.y += element->style.margin.top;

        if (element->children.empty()) continue;
        for (UI::Element* child : element->children) elementStack.push_back(child);

        if (element->style.positioning.mode == UI::PositionMode::ABSOLUTE) {
			switch (element->style.alignX) {
			case UI::AlignX::CENTER:
				element->position.x += (element->parent->size.x - element->size.x) / 2.0f;
				break;
			case UI::AlignX::RIGHT:
                element->position.x += element->parent->size.x - element->size.x;
				break;
			}
			switch (element->style.alignY) {
			case UI::AlignY::CENTER:
				element->position.y += (element->parent->size.y - element->size.y) / 2.0f;
				break;
			case UI::AlignY::BOTTOM:
                element->position.y += element->parent->size.y - element->size.y;
				break;
			}
        }

        bool isFlexRow = element->style.flexDir == UI::FlexDir::ROW;

        float totalAlongSize = 0.0f;
        for (UI::Element* child : element->children) {
            totalAlongSize += isFlexRow ? child->size.x : child->size.y;
        }

        float currAlongSize = isFlexRow ? element->size.x : element->size.y;
        float freeSpace = currAlongSize - totalAlongSize;

        float alongOffset = 0.0f;
        float acrossOffset = 0.0f;
        float cursor = isFlexRow ? element->style.padding.left : element->style.padding.top;

        for (UI::Element* child : element->children) {
            if (child->style.positioning.mode == UI::PositionMode::ABSOLUTE) continue;

            child->position += element->position;
            if (isFlexRow) child->position.y += element->style.padding.top;
            else child->position.x += element->style.padding.left;

            if (isFlexRow) {
                child->position.x += cursor;
				cursor += child->size.x + element->style.childGap;
            } else {
                child->position.y += cursor;
				cursor += child->size.y + element->style.childGap;
            }
        }
    }
}

void UIContext::ResolveCallbacks() {
    elementStack.clear();
    elementStack.push_back(root);
    while (!elementStack.empty()) {
        UI::Element* curr = elementStack.back();
        elementStack.pop_back();
        for (UI::Element* child : curr->children) elementStack.push_back(child);

        if (CheckCollisionPointRec(mouseState.mousePos, curr->GetRect())) {
			hotID = curr->id;
			if (curr->onHover != nullptr) curr->onHover(*curr);
			if (mouseState.leftDown) {
				activeID = curr->id;
				if (curr->onActive != nullptr) curr->onActive(*curr);
			}
            if (curr->onClick != nullptr && mouseState.leftReleased && activeID == curr->id) {
                curr->onClick(*curr);
            }
        }
    }
    if (mouseState.leftReleased) activeID = 0;
}

void UIContext::Render(UI::Element* element) {
    if (element == nullptr) return;

    const UI::Style& style = element->style;
    Rectangle rect = element->GetRect();

    if (element->isText()) {
        DrawTextPro(*element->style.font, element->label.c_str(), element->position,
            Vec2::zero, 0.0f, element->style.fontSize, element->style.fontSpacing, element->style.textColor);
    }
    else {
        Texture image = element->style.image;
        if (image.id != 0) {
            DrawTextureRec(*image, Rectangle{ 0, 0, rect.width, rect.height }, element->position, element->style.backgroundColor);
        } else if (style.roundness == 0) {
			DrawRectangleRec(rect, style.backgroundColor);
			if (style.borderWidth > 0) DrawRectangleLinesEx(element->GetRect(), style.borderWidth, style.borderColor);
		} else {
			int segments = 8;
			float roundness = style.roundness == UI::rounded_full 
				? 1.0f
				: 2.0f * style.roundness / fminf(rect.width, rect.height);
			DrawRectangleRounded(rect, roundness, segments, style.backgroundColor);
			if (style.borderWidth > 0) DrawRectangleRoundedLinesEx(rect, roundness, segments, style.borderWidth, style.borderColor);
		}
    }

    for (UI::Element* element : element->children) {
        Render(element);
    }
}

void UIContext::Render() {
    Render(root);
}

void UIContext::Panel(const UI::PanelStyle& style) {
    OpenElement(style.toStyle());
    CloseElement();
}

void UIContext::Panel(const UI::PanelStyle& style, const std::function<void()> children) {

    OpenElement(style.toStyle());
    children();
    CloseElement();
}

void UIContext::Panel(const UI::PanelStyle& style, UI::Callbacks callbacks) {
    OpenElement(style.toStyle());
    UI::AddCallbacks(elementStack.back(), callbacks);
    CloseElement();
}

void UIContext::Panel(const UI::PanelStyle& style, UI::Callbacks callbacks, const std::function<void()> children) {
    OpenElement(style.toStyle());
    UI::AddCallbacks(elementStack.back(), callbacks);
    children();
    CloseElement();
}

void UIContext::Text(std::string text, const UI::TextStyle& style) {
    OpenElement(style.toStyle());
    UI::Element* element = elementStack.back();
    element->label = text;
    if (style.font == nullptr) element->style.font = defaultFont;
    CloseElement();
}
