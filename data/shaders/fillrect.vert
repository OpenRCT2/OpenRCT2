#version 330

uniform ivec2 uScreenSize;

in ivec2 vPosition;

flat out ivec2 fPosition;

void main()
{
    fPosition = vPosition;

    // Transform screen coordinates to viewport
    vec2 pos = vec2(vPosition);
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    gl_Position = vec4(pos, 0.0, 1.0);
}
