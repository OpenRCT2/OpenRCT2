#version 150

uniform ivec4           uClip;
uniform int             uFlags;
uniform vec4            uColour;
uniform usampler2DArray uTexture;
uniform vec4            uPalette[256];
uniform vec2            uTexCoordScale;
uniform int             uTexSlot;

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

    vec4 texel = uPalette[texture(uTexture, vec3(fTextureCoordinate * uTexCoordScale, float(uTexSlot))).r];
    if ((uFlags & 1) != 0)
    {
        oColour = vec4(uColour.rgb, uColour.a * texel.a);
    }
    else
    {
        oColour = texel;
    }
}
