#version 150

const int MASK_REMAP_COUNT          = 3;
const int FLAG_NO_TEXTURE           = (1 << 2);
const int FLAG_MASK                 = (1 << 3);
const int FLAG_CROSS_HATCH          = (1 << 4);

uniform usampler2DArray uTexture;
uniform usampler2DRect  uPaletteTex;

flat in int             fFlags;
flat in uint            fColour;
in vec3                 fTexColour;
in vec3                 fTexMask;
flat in vec3            fPalettes;

in vec2 fPosition;

out uint oColour;

void main()
{
    uint texel;
    if ((fFlags & FLAG_NO_TEXTURE) == 0)
    {
        texel = texture(uTexture, fTexColour).r;
        if (texel == 0u)
        {
            discard;
        }
        texel += fColour;
    }
    else
    {
        texel = fColour;
    }

    int paletteCount = fFlags & MASK_REMAP_COUNT;
    if (paletteCount >= 3 && texel >= 0x2Eu && texel < 0x3Au)
    {
        texel = texture(uPaletteTex, vec2(texel + 0xC5u, fPalettes.z)).r;
    }
    else if (paletteCount >= 2 && texel >= 0xCAu && texel < 0xD6u)
    {
        texel = texture(uPaletteTex, vec2(texel + 0x29u, fPalettes.y)).r;
    }
    else if (paletteCount >= 1)
    {
        texel = texture(uPaletteTex, vec2(texel, fPalettes.x)).r;
    }

    if (texel == 0u)
    {
        discard;
    }

    if ((fFlags & FLAG_CROSS_HATCH) != 0)
    {
        int posSum = int(fPosition.x) + int(fPosition.y);
        if ((posSum % 2) == 0)
        {
            discard;
        }
    }

    if ((fFlags & FLAG_MASK) != 0)
    {
        uint mask = texture(uTexture, fTexMask).r;
        if ( mask == 0u )
        {
            discard;
        }
    }

    oColour = texel;
}
