#version 330

uniform ivec4 uClip;

uniform int   uFlags;
uniform vec4  uColour[2];

in vec2 fPosition;

layout (location = 0) out vec4 oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    int posSum = int(fPosition.x) + int(fPosition.y);
    if ((posSum % 2) == 0)
    {
        oColour = uColour[0];
    }
    else
    {
        oColour = uColour[1];
    }
}
