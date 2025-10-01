#pragma once

#include <glad.h>
#include <utils.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library ft;
Int2 dpi;

struct Character {
	unsigned int textureID;
	Int2 glyphSize;
	Int2 bearing;
	unsigned int advance;
};

struct Font {

};

void InitFreeType(Vec2 screenDPI);
void LoadFont(const char* path);
void RenderText(const char* text, Vec2 position);