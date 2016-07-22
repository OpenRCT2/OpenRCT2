#version 150

uniform ivec2 uScreenSize;
uniform ivec4 uTextureCoordinates;

in ivec4 ivClip;
in vec2  ivTexCoordScale;
in int   ivTexSlot;
in int   ivFlags;
in vec4  ivColour;
in ivec4 ivBounds;

in uint vIndex;

out vec2       fTextureCoordinate;
out vec2       fPosition;
flat out ivec4 fClip;
flat out int   fFlags;
out vec4       fColour;
out vec2       fTexCoordScale;
flat out int   fTexSlot;

void main()
{
    vec2 pos;
    switch (vIndex) {
    case 0u:
        pos = ivBounds.xy;
        fTextureCoordinate = uTextureCoordinates.xy;
        break;
    case 1u:
        pos = ivBounds.zy;
        fTextureCoordinate = uTextureCoordinates.zy;
        break;
    case 2u:
        pos = ivBounds.xw;
        fTextureCoordinate = uTextureCoordinates.xw;
        break;
    case 3u:
        pos = ivBounds.zw;
        fTextureCoordinate = uTextureCoordinates.zw;
        break;
    }

    fPosition = pos;

    // Transform screen coordinates to viewport
    pos.x = (pos.x * (2.0 / uScreenSize.x)) - 1.0;
    pos.y = (pos.y * (2.0 / uScreenSize.y)) - 1.0;
    pos.y *= -1;

    fClip = ivClip;
    fFlags = ivFlags;
    fColour = ivColour;
    fTexCoordScale = ivTexCoordScale;
    fTexSlot = ivTexSlot;

    gl_Position = vec4(pos, 0.0, 1.0);
}
