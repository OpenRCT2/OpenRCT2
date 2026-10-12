#version 330 core

// Allows for about 8 million draws per frame
const float DEPTH_INCREMENT = 1.0 / float(1u << 22u);

uniform ivec2 uScreenSize;

// clang-format off
// Attribute count must stay <= GL_MAX_VERTEX_ATTRIBS (16).
// mat4x2 = 4 slots; remaining 12 slots listed below.
in ivec4 vClip;
in int   vTexColourAtlas;
in vec4  vTexColourCoords;
in int   vTexMaskAtlas;
in vec4  vTexMaskCoords;
in ivec4 vPalettesFlags; // xyz = remaps, w = flags
in uint  vColour;
in ivec4 vBounds;
in vec2  vDepthZoom;     // x = painter depth, y = zoom
// Flat:    originXY, baseZ, slope | viewOffXY, mode, _
// Billboard: zMin/zMax, centreXY | screenTop/Bottom, mode, _
in vec4  vWorld0;
in vec4  vWorld1;

in mat4x2 vVertMat;
in vec2   vVertVec;

flat out vec2  fPosition;
out vec3       fPeelPos;
flat out int   fFlags;
flat out uint  fColour;
flat out vec4  fTexColour;
flat out vec4  fTexMask;
flat out vec3  fPalettes;
flat out float fZoom;
flat out int   fTexColourAtlas;
flat out int   fTexMaskAtlas;
flat out int   fScreenHeight;
flat out vec4  fWorld0;
flat out vec4  fWorld1;
// clang-format on

void main()
{
    // Clamp position by vClip, correcting interpolated values for the clipping
    vec2 m = clamp(
        ((vVertMat * vec4(vClip)) - (vVertMat * vec4(vBounds))) / vec2(vBounds.zw - vBounds.xy) + vVertVec, 0.0, 1.0);
    vec2 pos = mix(vec2(vBounds.xy), vec2(vBounds.zw), m);
    fTexColour = vTexColourCoords;
    fTexMask = vTexMaskCoords;

    fPosition = vBounds.xy;
    fZoom = vDepthZoom.y;
    fTexColourAtlas = vTexColourAtlas;
    fTexMaskAtlas = vTexMaskAtlas;

    // Transform screen coordinates to texture coordinates
    float depth = 1.0 - (vDepthZoom.x + 1.0) * DEPTH_INCREMENT;
    pos = pos / vec2(uScreenSize);
    pos.y = pos.y * -1.0 + 1.0;
    fPeelPos = vec3(pos, depth * 0.5 + 0.5);

    fFlags = vPalettesFlags.w;
    fColour = vColour;
    fPalettes = vec3(vPalettesFlags.xyz);

    fScreenHeight = uScreenSize.y;
    fWorld0 = vWorld0;
    fWorld1 = vWorld1;

    // Transform texture coordinates to viewport coordinates
    pos = pos * 2.0 - 1.0;
    gl_Position = vec4(pos, depth, 1.0);
}
