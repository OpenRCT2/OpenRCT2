#version 150

uniform ivec2       uScreenSize;
uniform ivec4       uClip;
uniform int         uFlags;
uniform sampler2D   uSourceFramebuffer;
uniform ivec4       uBounds;
uniform vec4        uPalette[256];
uniform int         uPaletteRemap[256];

in vec2 fPosition;

out vec4 oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    if ((uFlags & 1) != 0) // cross-hatch pattern
    {
        int posSum = int(fPosition.x) + int(fPosition.y);
        if ((posSum % 2) == 0)
        {
            discard;
        }
    }

    vec2 textureCoordinates = (fPosition / vec2(uScreenSize)) * vec2(1, -1);
    vec4 sourceColour = texture(uSourceFramebuffer, textureCoordinates);

    // find "best match" palette index of the current 32 bit color
    float nearestDistance = length(uPalette[0] - sourceColour);
    uint bestPaletteIndexMatch = 0u;
    for (uint i = 1u; i < 256u; i++)
    {
        float distance = length(uPalette[i] - sourceColour);
        uint isBetter = -uint(distance < nearestDistance); // unpredictable
        bestPaletteIndexMatch = (bestPaletteIndexMatch & ~isBetter) | (i & isBetter);
        nearestDistance = min(nearestDistance, distance);
    }

    oColour = uPalette[uPaletteRemap[bestPaletteIndexMatch]];
}
