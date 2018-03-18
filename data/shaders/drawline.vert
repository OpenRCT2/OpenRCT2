#version 150

// Allows for about 8 million draws per frame
const float DEPTH_INCREMENT = 1.0 / float(1u << 22u);

uniform ivec2 uScreenSize;

in ivec4 vBounds;
in ivec4 vClip;
in uint  vColour;
in int   vDepth;

in mat4x2 vVertMat;

flat out uint fColour;

void main()
{
    vec2 pos = clamp(vVertMat * vBounds, vClip.xy, vClip.zw);

    // Transform screen coordinates to viewport coordinates
    pos = (pos * (2.0 / uScreenSize)) - 1.0;
    pos.y *= -1;
    float depth = 1.0 - (vDepth + 1) * DEPTH_INCREMENT;

    fColour = vColour;

    gl_Position = vec4(pos, depth, 1.0);
}
