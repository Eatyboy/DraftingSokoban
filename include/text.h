#pragma once

#include <glad.h>
#include <utils.h>
#include <renderer.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <array>

constexpr int ASCII_BEGIN = 32;
constexpr int ASCII_END = 127;

struct Character {
    unsigned int textureID;
    Int2 glyphSize;
    Int2 bearing;
    int advance;
};

struct Font {
    std::array<Character, ASCII_END> characters;
    float lineHeight = 0.0f;
    int ascender = 0;
    int descender = 0;
    int linegap = 0;
    int topMargin = 0;
    float baseFontSize = 0.0f;
};

void InitTextRenderer(Vec2 screenDPI);
Font* LoadFont(const char* path);
void RenderText(const char* text, Font& font, float fontSize, Vec2 position, Color color = BLACK);
Vec2 MeasureText(const char* text, Font& font, float fontSize);