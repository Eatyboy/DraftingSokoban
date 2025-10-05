#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 FragPos;
out vec2 TexCoords;

uniform mat4 projection;
uniform vec4 rect;
uniform vec4 uvRec;

void main() {
    vec2 pos = aPos * rect.zw + rect.xy;
    gl_Position = projection * vec4(pos, 0.0, 1.0);
    FragPos = pos;
    TexCoords = uvRec.xy + aPos * uvRec.zw;
}