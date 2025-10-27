#include <text.h>
#include <Debug.h>
#include <Windows.h>
#include <cstring>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <renderer.h>
#include <vector>

FT_Library ft;
Int2 dpi;
FT_Face face;

unsigned int VAO, VBO;
Shader textShader;

void InitTextRenderer(Vec2 screenDPI) {
    if (FT_Init_FreeType(&ft)) {
        debugError("Failed to initialize FreeType\n");
        return;
    }
    dpi = screenDPI;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    textShader = Shader("text.vert", "text.frag");
}

constexpr float baseFontSize = 48.0f;

Font* LoadFont(const char* path) {
    auto error = FT_New_Face(ft, path, 0, &face);
    if (error == FT_Err_Unknown_File_Format) {
        debugError("Font from \"%s\" is unsupported\n", path);
        return nullptr;
    } else if (error) {
        debugError("Failed to load font from \"%s\"\n", path);
        return nullptr;
    }

    FT_Set_Char_Size(face, 0, baseFontSize * 64, dpi.x, dpi.y);

    Font* font = new Font();
    font->baseFontSize = baseFontSize;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (int i = ASCII_BEGIN; i < ASCII_END; ++i) {
        error = FT_Load_Char(face, (char)i, FT_LOAD_RENDER);
        if (error) {
            debugError("Failed to character \'%c\' from \"%s\"\n", (char)i, path);
            continue;
        }

        auto& metrics = face->glyph->metrics;
        auto& bmp = face->glyph->bitmap;

        std::vector<unsigned char> flipped(bmp.width * bmp.rows);
        for (unsigned int row = 0; row < bmp.rows; ++row) {
            memcpy(&flipped[row * bmp.width],
                bmp.buffer + (bmp.rows - 1 - row) * bmp.pitch,
                bmp.width);
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            bmp.width,
            bmp.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            flipped.data()
        );

        Character character = {
            texture,
            Int2(metrics.width / 64.0f, metrics.height / 64.0f),
            Int2(metrics.horiBearingX / 64.0f, metrics.horiBearingY / 64.0f),
            metrics.horiAdvance
        };

        font->characters[i] = character;
        font->ascender = face->ascender / 64;
        font->descender = face->descender / 64;
        font->lineHeight = (float)face->height / 64.0f;
    }

    debugLog("Loaded font %s", face->family_name);
    FT_Done_Face(face);
    return font;
}

struct Vec4 { float x, y, z, w; };

void RenderText(const char* text, Font& font, float fontSize, Vec2 position, Color color) {
    textShader.use();
    textShader.setMat4("projection", projection);
    textShader.setVec3("textColor", glm::vec3(color.r, color.g, color.b));

    glActiveTexture(GL_TEXTURE0);
    textShader.setInt("text", 0);
    glBindVertexArray(VAO);

    float scale = fontSize / baseFontSize;
    size_t stringLength = strlen(text);
    Vec2 pen = position;
    pen.y += font.ascender;

    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c == '\n') {
            pen.x = position.x;
            pen.y += font.lineHeight;
            continue;
        }
        if (c < ASCII_BEGIN || c >= ASCII_END) continue;

        Character ch = font.characters[(int)c];

        Vec2 pos = pen;
        pos.x += ch.bearing.x * scale;
        pos.y -= ch.bearing.y * scale;

        Vec2 size = (Vec2)ch.glyphSize * scale;

        float vertices[6][4] = {
            {pos.x,          pos.y + size.y, 0.0f, 0.0f},
            {pos.x,          pos.y,          0.0f, 1.0f},
            {pos.x + size.x, pos.y,          1.0f, 1.0f},
            {pos.x,          pos.y + size.y, 0.0f, 0.0f},
            {pos.x + size.x, pos.y,          1.0f, 1.0f},
            {pos.x + size.x, pos.y + size.y, 1.0f, 0.0f},
        };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        pen.x += (float)(ch.advance >> 6) * scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Vec2 MeasureText(const char* text, Font& font, float fontSize) {
    Vec2 size = Vec2::zero;
    size.y += font.lineHeight;

    float scale = fontSize / baseFontSize;
    size_t stringLength = strlen(text);

    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        if (c == '\n') {
            size.y += font.lineHeight;
            continue;
        }
        if (c < ASCII_BEGIN || c >= ASCII_END) continue;

        Character ch = font.characters[(int)c];
        size.x += (float)(ch.advance >> 6) * scale;
    }

    return size;
}