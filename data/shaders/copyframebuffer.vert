#version 330

uniform ivec2 uScreenSize;
uniform ivec4 uBounds;
uniform ivec4 uTextureCoordinates;

in int vIndex;

out vec2 fPosition;
out vec2 fTextureCoordinate;

void main()
{
    vec2 pos;
    switch (vIndex) {
    case 0:
        pos = uBounds.xy;
        fTextureCoordinate = uTextureCoordinates.xy;
        break;
    case 1:
        pos = uBounds.zy;
        fTextureCoordinate = uTextureCoordinates.zy;
        break;
    case 2:
        pos = uBounds.zw;
        fTextureCoordinate = uTextureCoordinates.zw;
        break;
    case 3:
        pos = uBounds.xw;
        fTextureCoordinate = uTextureCoordinates.xw;
        break;
    }

    fPosition = pos;

    // Transform screen coordinates to viewport
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    gl_Position = vec4(pos, 0.0, 1.0);
}
