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

    vec4 texel = uPalette[texture(uTexture, vec3(fTexColourCoords, float(fTexColourAtlas))).r];
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
