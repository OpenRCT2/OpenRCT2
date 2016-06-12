#version 330

uniform ivec2 uScreenSize;
uniform ivec4 uBounds;

in int vIndex;

out vec2 fPosition;

void main()
{
    vec2 pos;
    switch (vIndex) {
    case 0:
        pos = uBounds.xy;
        break;
    case 1:
        pos = uBounds.zy;
        break;
    case 2:
        pos = uBounds.zw;
        break;
    case 3:
        pos = uBounds.xw;
        break;
    }

    fPosition = pos;

    // Transform screen coordinates to viewport
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    gl_Position = vec4(pos, 0.0, 1.0);
}
