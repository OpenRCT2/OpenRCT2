#version 150

uniform sampler2D uTexture;

in vec2 fPosition;
in vec2 fTextureCoordinate;

out vec4 oColour;

void main()
{
    oColour = texture(uTexture, fTextureCoordinate);
}
