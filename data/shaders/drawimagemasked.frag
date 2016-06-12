#version 150

uniform ivec4       uClip;
uniform usampler2D  uTextureMask;
uniform usampler2D  uTextureColour;
uniform vec4        uPalette[256];

in vec2 fPosition;
in vec2 fTextureCoordinate;

out vec4 oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    vec4 mask = uPalette[texture(uTextureMask, fTextureCoordinate).r];
    vec4 colour = uPalette[texture(uTextureColour, fTextureCoordinate).r];
    oColour = colour * mask;
}
