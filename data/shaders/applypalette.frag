#version 150

uniform vec4 uPalette[256];
uniform usampler2D uTexture;

in vec2 fTextureCoordinate;

out vec4 oColour;

void main()
{
    oColour = uPalette[texture(uTexture, fTextureCoordinate).r];
}
