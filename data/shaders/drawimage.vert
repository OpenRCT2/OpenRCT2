#version 150

uniform ivec2 uScreenSize;

in ivec4 ivClip;
in int   ivTexColourAtlas;
in vec4  ivTexColourBounds;
in int   ivTexMaskAtlas;
in vec4  ivTexMaskBounds;
in int   ivTexPaletteAtlas;
in vec4  ivTexPaletteBounds;
in int   ivFlags;
in uint  ivColour;
in ivec4 ivBounds;
in int   ivMask;

flat out ivec4  vClip;
flat out int    vFlags;
flat out uint   vColour;
flat out int    vTexColourAtlas;
flat out int    vTexMaskAtlas;
flat out int    vTexPaletteAtlas;
flat out vec4   vTexPaletteBounds;
flat out int    vMask;

out vec4        vPosition;
out vec4        vScreenCoords;
out vec4        vTexColourCoords;
out vec4        vTexMaskCoords;

void main()
{
    vClip = ivClip;
    vFlags = ivFlags;
    vColour = ivColour;
    vMask = ivMask;
    vTexColourAtlas = ivTexColourAtlas;
    vTexMaskAtlas = ivTexMaskAtlas;
    vTexPaletteAtlas = ivTexPaletteAtlas;
    vTexPaletteBounds = ivTexPaletteBounds;

    vPosition = ivBounds;
    vTexColourCoords = ivTexColourBounds;
    vTexMaskCoords = ivTexMaskBounds;

    vec4 pos = (vPosition * (2.0 / uScreenSize.xyxy)) - 1.0;
    pos *= vec4(1.0, -1.0, 1.0, -1.0);
    vScreenCoords = pos;

    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
