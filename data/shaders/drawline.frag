#version 330 core

flat in uint fColour;

out uint oColour;

void main()
{
    oColour = fColour;
}
