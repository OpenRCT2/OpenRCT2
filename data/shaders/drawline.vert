#version 330 core

// Allows for about 8 million draws per frame
const float DEPTH_INCREMENT = 1.0 / float(1u << 22u);

uniform ivec2 uScreenSize;

// clang-format off
in ivec4 vBounds;
in uint  vColour;
in int   vDepth;
// clang-format on

in mat4x2 vVertMat;

flat out uint fColour;

void main()
{
    vec2 pos = vVertMat * vec4(vBounds);

    // Transform screen coordinates to viewport coordinates
    pos = (pos * (2.0 / vec2(uScreenSize))) - 1.0;
    pos.y *= -1.0;
    float depth = 1.0 - (float(vDepth) + 1.0) * DEPTH_INCREMENT;

    fColour = vColour;

    gl_Position = vec4(pos, depth, 1.0);
}
