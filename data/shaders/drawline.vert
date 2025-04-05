#version 150

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
    // Explicitly cast ivec4 to vec4 for position computation
    vec2 pos = vVertMat * vec4(float(vBounds.x), float(vBounds.y), float(vBounds.z), float(vBounds.w));

    // Explicitly cast ivec2 to vec2 for screen size
    vec2 screenSizeFloat = vec2(float(uScreenSize.x), float(uScreenSize.y));
    pos = (pos * (2.0 / screenSizeFloat)) - 1.0;
    pos.y *= -1.0;

    float depth = 1.0 - (float(vDepth) + 1.0) * DEPTH_INCREMENT;
    depth = clamp(depth, 0.0, 1.0);

    fColour = vColour;

    gl_Position = vec4(pos, depth, 1.0);
}
