#version 150

in int gl_VertexID;

in vec4 vPosition;
in vec2 vTextureCoordinate;

out vec2 fTextureCoordinate;

void main()
{
    fTextureCoordinate = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(fTextureCoordinate * 2.0 + -1.0, 0.0, 1.0);
}
