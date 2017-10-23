#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

flat in ivec4   vClip[1];
flat in int     vFlags[1];
flat in uint    vColour[1];
flat in int     vTexColourAtlas[1];
flat in int     vTexMaskAtlas[1];
flat in int     vTexPaletteAtlas[1];
flat in vec4    vTexPaletteBounds[1];
flat in int     vMask[1];

in vec4         vPosition[1];
in vec4         vScreenCoords[1];
in vec4         vTexColourCoords[1];
in vec4         vTexMaskCoords[1];


flat out ivec4  fClip;
flat out int    fFlags;
flat out uint   fColour;
flat out int    fTexColourAtlas;
flat out int    fTexMaskAtlas;
flat out int    fTexPaletteAtlas;
flat out vec4   fTexPaletteBounds;
flat out int    fMask;

out vec2        fPosition;
out vec2        fTexColourCoords;
out vec2        fTexMaskCoords;

void main()
{
    fClip = vClip[0];
    fFlags = vFlags[0];
    fColour = vColour[0];
    fTexColourAtlas = vTexColourAtlas[0];
    fTexMaskAtlas = vTexMaskAtlas[0];
    fTexPaletteAtlas = vTexPaletteAtlas[0];
    fTexPaletteBounds = vTexPaletteBounds[0];
    fMask = vMask[0];

    fPosition = vPosition[0].xy;
    fTexColourCoords = vTexColourCoords[0].xy;
    fTexMaskCoords = vTexMaskCoords[0].xy;
    gl_Position = vec4(vScreenCoords[0].xy, 0.0, 1.0);
    EmitVertex();

    fPosition = vPosition[0].zy;
    fTexColourCoords = vTexColourCoords[0].zy;
    fTexMaskCoords = vTexMaskCoords[0].zy;
    gl_Position = vec4(vScreenCoords[0].zy, 0.0, 1.0);
    EmitVertex();

    fPosition = vPosition[0].xw;
    fTexColourCoords = vTexColourCoords[0].xw;
    fTexMaskCoords = vTexMaskCoords[0].xw;
    gl_Position = vec4(vScreenCoords[0].xw, 0.0, 1.0);
    EmitVertex();

    fPosition = vPosition[0].zw;
    fTexColourCoords = vTexColourCoords[0].zw;
    fTexMaskCoords = vTexMaskCoords[0].zw;
    gl_Position = vec4(vScreenCoords[0].zw, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
