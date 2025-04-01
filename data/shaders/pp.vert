#version 150

in vec2 vPosition;
in vec2 vTextureCoordinate;
out vec2 fTexCoord;

void main()
{
    gl_Position = vec4(vPosition, 0.0, 1.0);
    fTexCoord = vTextureCoordinate;
}