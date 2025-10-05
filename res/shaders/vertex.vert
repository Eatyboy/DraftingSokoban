#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aBaseUV;
layout (location = 2) in vec2 iWorldPos;
layout (location = 3) in int iTileIndex;

out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 uvStep;
uniform int tilesetCols;
uniform int tileSize;

void main() {
    vec2 world = iWorldPos + aPos * tileSize;
    gl_Position = projection * vec4(world, 0.0, 1.0);

    int col = iTileIndex % tilesetCols;
    int row = iTileIndex / tilesetCols;
    vec2 uvOffset = vec2(col, row) * uvStep;
    TexCoords = uvOffset + aBaseUV * uvStep;
}