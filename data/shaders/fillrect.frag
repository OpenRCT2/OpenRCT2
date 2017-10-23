#version 150

uniform ivec2       uScreenSize;
uniform ivec4       uClip;
uniform int         uFlags;
uniform usampler2D  uSourceFramebuffer;
uniform uint        uPaletteRemap[256];

in vec2 fPosition;

out uint oColour;

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

    vec2 textureCoordinates = (fPosition / vec2(uScreenSize)) * vec2(1.0, -1.0) + vec2(0.0, 1.0);
    uint sourceColour = texture(uSourceFramebuffer, textureCoordinates).r;
    oColour = uPaletteRemap[sourceColour];
}
