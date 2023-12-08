#version 150

// Allows for about 8 million draws per frame
const float DEPTH_INCREMENT = 1.0 / float(1u << 22u);

uniform ivec2 uScreenSize;

// clang-format off
in ivec4 vClip;
in int   vTexColourAtlas;
in vec4  vTexColourBounds;
in int   vTexMaskAtlas;
in vec4  vTexMaskBounds;
in ivec3 vPalettes;
in int   vFlags;
in uint  vColour;
in ivec4 vBounds;
in int   vDepth;

in mat4x2 vVertMat;
in vec2   vVertVec;

out vec2       fPosition;
out vec3       fPeelPos;
flat out int   fFlags;
flat out uint  fColour;
out vec3       fTexColour;
out vec3       fTexMask;
flat out vec3  fPalettes;
// clang-format on

void main()
{
    // Clamp position by vClip, correcting interpolated values for the clipping
    vec2 m = clamp(
        ((vVertMat * vec4(vClip)) - (vVertMat * vec4(vBounds))) / vec2(vBounds.zw - vBounds.xy) + vVertVec, 0.0, 1.0);
    vec2 pos = mix(vec2(vBounds.xy), vec2(vBounds.zw), m);
    fTexColour = vec3(mix(vTexColourBounds.xy, vTexColourBounds.zw, m), vTexColourAtlas);
    fTexMask = vec3(mix(vTexMaskBounds.xy, vTexMaskBounds.zw, m), vTexMaskAtlas);

    fPosition = pos;

    // Transform screen coordinates to texture coordinates
    float depth = 1.0 - (float(vDepth) + 1.0) * DEPTH_INCREMENT;
    pos = pos / vec2(uScreenSize);
    pos.y = pos.y * -1.0 + 1.0;
    fPeelPos = vec3(pos, depth * 0.5 + 0.5);

    fFlags = vFlags;
    fColour = vColour;
    fPalettes = vec3(vPalettes);

    // Transform texture coordinates to viewport coordinates
    pos = pos * 2.0 - 1.0;
    gl_Position = vec4(pos, depth, 1.0);
}
