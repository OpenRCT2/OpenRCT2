#version 150

uniform sampler2D uTexture;
in vec2 fTexCoord;
out vec4 oColour;
uniform float uZoom;
uniform float uTicks;

void main()
{
    oColour = texture(uTexture, fTexCoord) * 1;
}