#version 150

uniform ivec2 uScreenSize;
uniform ivec4 uBounds;

in uint vIndex;

out vec2 fPosition;
out vec2 fTextureCoordinate;

void main()
{
    vec2 pos;
    switch (vIndex) {
    case 0u:
        pos = uBounds.xy;
        fTextureCoordinate = vec2(0, 0);
        break;
    case 1u:
        pos = uBounds.zy;
        fTextureCoordinate = vec2(1, 0);
        break;
    case 2u:
        pos = uBounds.xw;
        fTextureCoordinate = vec2(0, 1);
        break;
    case 3u:
        pos = uBounds.zw;
        fTextureCoordinate = vec2(1, 1);
        break;
    }

    fPosition = pos;

    // Transform screen coordinates to viewport
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    gl_Position = vec4(pos, 0.0, 1.0);
}
