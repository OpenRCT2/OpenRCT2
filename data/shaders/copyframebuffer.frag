#version 330

uniform sampler2D uTexture;

in vec2 fPosition;
in vec2 fTextureCoordinate;

layout (location = 0) out vec4 oColour;

void main()
{
    oColour = texture(uTexture, fTextureCoordinate);
}
