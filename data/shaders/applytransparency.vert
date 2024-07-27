#version 150

in vec4 vPosition;
in vec2 vTextureCoordinate;

out vec2 fTextureCoordinate;

void main()
{
    fTextureCoordinate = vTextureCoordinate;
    gl_Position = vPosition;
}
