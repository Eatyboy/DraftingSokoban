#include <renderer.h>
#include <Debug.h>
#include <stb_image/stb_image.h>
#include <glad.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

unsigned int rectVAO = 0;
unsigned int rectVBO = 0;
unsigned int rectEBO = 0;

Shader rectShader;

extern Int2 screenSize;

glm::vec4 vColor(Color color) {
    return glm::vec4(color.r, color.g, color.b, color.a) / 255.0f;
}

void InitRenderer() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float rectVertices[] = {
        1.0f, 0.0f, // top right
        1.0f, 1.0f, // bottom right
        0.0f, 1.0f, // bottom left
        0.0f, 0.0f, // top left
    };

    unsigned int rectIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glGenBuffers(1, &rectEBO);

    glBindVertexArray(rectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    rectShader = Shader("rect.vert", "rect.frag");
}

void ClearColor(Color color) {
    glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

Texture LoadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    GLenum format = GL_INVALID_ENUM;
    if (data != NULL) {
        switch (nrChannels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: debugError("Invalid number of channels: %d\n", nrChannels); break;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        debugError("Failed to load texture from \"%s\"\n", path);
    }
    stbi_image_free(data);

    return Texture{ textureID, width, height, format };
}

void DrawRect(Vec2 position, Vec2 size, Color backgroundColor, 
    float roundRadius, float borderWidth, Color borderColor) {
    rectShader.use();

    rectShader.setVec4("rect", glm::vec4(position.x, position.y, size.x, size.y));
    rectShader.setFloat("radius", roundRadius);
    rectShader.setFloat("border", borderWidth);
    rectShader.setVec4("bgColor", vColor(backgroundColor));
    rectShader.setVec4("borderColor", vColor(borderColor));
    rectShader.setMat4("projection", projection);
    rectShader.setBool("useTexture", false);

    glBindVertexArray(rectVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void DrawTexturedRect(Vec2 position, Vec2 size, Texture texture, Vec2 baseUV, Vec2 uvOffset, Color tint) {
    rectShader.use();
    rectShader.setVec4("rect", glm::vec4(position.x, position.y, size.x, size.y));
    rectShader.setFloat("radius", 0.0f);
    rectShader.setFloat("border", 0.0f);
    rectShader.setVec4("bgColor", vColor(tint));
    rectShader.setMat4("projection", projection);
    rectShader.setBool("useTexture", true);
    rectShader.setInt("texture0", 0);
    rectShader.setVec4("uvRec", glm::vec4(baseUV.x, baseUV.y, uvOffset.x, uvOffset.y));

    glBindVertexArray(rectVAO);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

