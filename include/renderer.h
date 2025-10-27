#pragma once

#include <string>
#include <glad.h>
#include <utils.h>
#include <glm/glm.hpp>

struct Color { float r, g, b, a; };

glm::vec4 vColor(Color color);

// I took these colors from Raylib
constexpr Color LIGHTGRAY  { 200, 200, 200, 255 };   // Light Gray
constexpr Color GRAY       { 130, 130, 130, 255 };   // Gray
constexpr Color DARKGRAY   { 80, 80, 80, 255 };      // Dark Gray
constexpr Color YELLOW     { 253, 249, 0, 255 };     // Yellow
constexpr Color GOLD       { 255, 203, 0, 255 };     // Gold
constexpr Color ORANGE     { 255, 161, 0, 255 };     // Orange
constexpr Color PINK       { 255, 109, 194, 255 };   // Pink
constexpr Color RED        { 230, 41, 55, 255 };     // Red
constexpr Color MAROON     { 190, 33, 55, 255 };     // Maroon
constexpr Color GREEN      { 0, 228, 48, 255 };      // Green
constexpr Color LIME       { 0, 158, 47, 255 };      // Lime
constexpr Color DARKGREEN  { 0, 117, 44, 255 };      // Dark Green
constexpr Color SKYBLUE    { 102, 191, 255, 255 };   // Sky Blue
constexpr Color BLUE       { 0, 121, 241, 255 };     // Blue
constexpr Color DARKBLUE   { 0, 82, 172, 255 };      // Dark Blue
constexpr Color PURPLE     { 200, 122, 255, 255 };   // Purple
constexpr Color VIOLET     { 135, 60, 190, 255 };    // Violet
constexpr Color DARKPURPLE { 112, 31, 126, 255 };    // Dark Purple
constexpr Color BEIGE      { 211, 176, 131, 255 };   // Beige
constexpr Color BROWN      { 127, 106, 79, 255 };    // Brown
constexpr Color DARKBROWN  { 76, 63, 47, 255 };      // Dark Brown

constexpr Color WHITE      { 255, 255, 255, 255 };   // White
constexpr Color BLACK      { 0, 0, 0, 255 };         // Black
constexpr Color BLANK      { 0, 0, 0, 0 };           // Blank (Transparent)
constexpr Color MAGENTA    { 255, 0, 255, 255 };     // Magenta

struct Texture {
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    GLenum pixelFormat = GL_RGBA8;
};

extern glm::mat4 projection;

void InitRenderer();
Texture LoadTexture(const char* path);
void ClearColor(Color color);
void DrawRect(Vec2 position, Vec2 size, Color backgroundColor,
    float roundRadius = 0.0f, float borderWidth = 0.0f, Color borderColor = BLANK);
void DrawTexturedRect(Vec2 position, Vec2 size, Texture texture, Vec2 baseUV = Vec2::zero, Vec2 uvOffset = Vec2::one, Color tint = WHITE);
