#version 150

uniform ivec2   uScreenSize;
uniform ivec4   uClip;
uniform vec4    uColour;

in vec2 fPosition;

out vec4 oColour;

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    oColour = uColour;
}
