#version 330 core

in vec2 vPosition;
in vec2 vTextureCoordinate;

out vec2 fTextureCoordinate;

uniform vec4 uSourceRect;
uniform vec2 uTextureSize;

void main()
{
    gl_Position = vec4(vPosition, 0.0, 1.0);
    vec2 srcPos = vec2(uSourceRect.xy);
    vec2 srcSize = vec2(uSourceRect.zw);
    fTextureCoordinate = (srcPos + vTextureCoordinate * srcSize) / uTextureSize;
}
