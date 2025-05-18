#version 150

in vec2 fTextureCoordinate;
layout(location = 0) out lowp vec4 fragColor;

uniform sampler2D uTexture;

void main()
{
    fragColor = texture(uTexture, fTextureCoordinate);
}
