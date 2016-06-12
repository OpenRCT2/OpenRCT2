#version 330

uniform ivec4       uClip;
uniform sampler2D   uTextureMask;
uniform sampler2D   uTextureColour;

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

    vec4 mask = texture(uTextureMask, fTextureCoordinate);
    vec4 colour = texture(uTextureColour, fTextureCoordinate);
    oColour = colour * mask;
}
