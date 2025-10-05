#version 330 core

in vec2 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 rect;
uniform float radius;
uniform float border;
uniform vec4 bgColor;
uniform vec4 borderColor;

uniform bool useTexture;
uniform sampler2D texture0;

float sdRec(vec2 p, vec4 rec, float rad) {
    vec2 halfSize = 0.5 * rec.zw;
    vec2 center = rec.xy + halfSize;
    vec2 q = abs(p - center) - halfSize + vec2(rad);
    return length(max(q, 0.0)) + min(max(q.x,q.y),0.0) - rad;
}

void main() {
    float d = sdRec(FragPos, rect, radius);
    vec4 col = (d < 0.0)
        ? (d < -border) 
            ? bgColor 
            : borderColor
        : vec4(0.0);
    FragColor = useTexture ? texture(texture0, TexCoords) * bgColor : col;
}