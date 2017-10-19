#version 150

uniform uint    uColour;

in vec2 fPosition;

out uint oColour;

void main()
{
    oColour = uColour;
}
