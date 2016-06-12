#version 330

uniform ivec4       uClip;
uniform int         uFlags;
uniform vec4        uColour;
uniform sampler2D   uTexture;

in vec2 fPosition;
in vec2 fTextureCoordinate;

layout (location = 0) out vec4 oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    vec4 texel = texture(uTexture, fTextureCoordinate);
    if ((uFlags & 1) != 0)
    {
        oColour = vec4(uColour.rgb, uColour.a * texel.a);
    }
    else
    {
        oColour = texel;
    }
}
