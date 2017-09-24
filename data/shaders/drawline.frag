#version 150

uniform ivec4   uClip;
uniform uint    uColour;

in vec2 fPosition;

out uint oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    oColour = uColour;
}
