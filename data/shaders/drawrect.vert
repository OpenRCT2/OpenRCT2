#version 150

// Allows for about 8 million draws per frame
const float DEPTH_INCREMENT = 1.0 / float(1u << 22u);

uniform ivec2 uScreenSize;

// clang-format off
in ivec4 vClip;
in int   vTexColourAtlas;
in vec4  vTexColourCoords;
in int   vTexMaskAtlas;
in vec4  vTexMaskCoords;
in ivec3 vPalettes;
in int   vFlags;
in uint  vColour;
in ivec4 vBounds;
in int   vDepth;
in float vZoom;

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
// clang-format on

void main()
{
    // Clamp position by vClip, correcting interpolated values for the clipping
    vec2 boundsDiff = vec2(float(vBounds.z - vBounds.x), float(vBounds.w - vBounds.y));
    // Avoid division by zero with a small epsilon
    boundsDiff = max(boundsDiff, vec2(0.0001));
    vec2 m = clamp(
        ((vVertMat * vec4(float(vClip.x), float(vClip.y), float(vClip.z), float(vClip.w))) -
         (vVertMat * vec4(float(vBounds.x), float(vBounds.y), float(vBounds.z), float(vBounds.w)))) /
                boundsDiff + vVertVec,
        0.0, 1.0);
    vec2 pos = mix(vec2(float(vBounds.x), float(vBounds.y)), vec2(float(vBounds.z), float(vBounds.w)), m);

    fTexColour = vTexColourCoords;
    fTexMask = vTexMaskCoords;
    fPosition = vec2(float(vBounds.x), float(vBounds.y));
    fZoom = vZoom;
    fTexColourAtlas = vTexColourAtlas;
    fTexMaskAtlas = vTexMaskAtlas;

    // Transform screen coordinates to texture coordinates
    float depth = 1.0 - (float(vDepth) + 1.0) * DEPTH_INCREMENT;
    pos = pos / vec2(float(uScreenSize.x), float(uScreenSize.y));
    pos.y = pos.y * -1.0 + 1.0;
    fPeelPos = vec3(pos, depth * 0.5 + 0.5);

    fFlags = vFlags;
    fColour = vColour;
    fPalettes = vec3(float(vPalettes.x), float(vPalettes.y), float(vPalettes.z));

    // Transform texture coordinates to viewport coordinates
    pos = pos * 2.0 - 1.0;
    gl_Position = vec4(pos, depth, 1.0);
}
