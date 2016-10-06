#version 150

uniform vec4            uPalette[256];
uniform usampler2DArray uTexture;

flat in ivec4           fClip;
flat in int             fFlags;
in vec4                 fColour;
flat in int             fTexColourAtlas;
in vec2                 fTexColourCoords;
flat in int             fTexMaskAtlas;
in vec2                 fTexMaskCoords;
flat in int             fTexPaletteAtlas;
flat in vec4            fTexPaletteBounds;
flat in int             fMask;

in vec2 fPosition;
in vec2 fTextureCoordinate;

out vec4 oColour;

void main()
{
    if (fPosition.x < fClip.x || fPosition.x > fClip.z ||
        fPosition.y < fClip.y || fPosition.y > fClip.w)
    {
        discard;
    }

    vec4 texel;

    // If remap palette used
    if ((fFlags & (1 << 1)) != 0)
    {
        // z is the size of each x pixel in the atlas
        float x = fTexPaletteBounds.x + texture(uTexture, vec3(fTexColourCoords, float(fTexColourAtlas))).r * fTexPaletteBounds.z;
        texel = uPalette[texture(uTexture, vec3(x, fTexPaletteBounds.y, float(fTexPaletteAtlas))).r];
    } // If transparent or special transparent
    else if ((fFlags & ((1 << 2) | (1 << 3))) != 0)
    {
        float line = texture(uTexture,vec3(fTexColourCoords, float(fTexColourAtlas))).r;
        if (line == 0.0)
        {
            discard;
        }
        float alpha = 0.5;
        if ((fFlags & (1 << 2)) != 0)
        {
            alpha = 0.5 + (line - 1.0) / 10.0;
        }
    
        // z is the size of each x pixel in the atlas
        float x = fTexPaletteBounds.x + fTexPaletteBounds.z * 50.0;
        oColour = vec4(uPalette[texture(uTexture, vec3(x, fTexPaletteBounds.y, float(fTexPaletteAtlas))).r].rgb, alpha);
        
        return;
    }
    else
    {
        texel = uPalette[texture(uTexture, vec3(fTexColourCoords, float(fTexColourAtlas))).r];
    }
    vec4 mask = uPalette[texture(uTexture, vec3(fTexMaskCoords, float(fTexMaskAtlas))).r];

    if (fMask != 0)
    {
        oColour = texel * mask;
    }
    else
    {
        if ((fFlags & 1) != 0)
        {
            oColour = vec4(fColour.rgb, fColour.a * texel.a);
        }
        else
        {
            oColour = texel;
        }
    }
}
