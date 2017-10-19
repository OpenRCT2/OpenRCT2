#version 150

uniform ivec2 uScreenSize;
uniform ivec4 uBounds;
uniform ivec4 uClip;

in mat4x2 vVertMat;

out vec2 fPosition;

void main()
{
    vec2 pos = clamp(vVertMat * uBounds, uClip.xy, uClip.zw);
    fPosition = pos;

    // Transform screen coordinates to viewport
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    gl_Position = vec4(pos, 0.0, 1.0);
}
