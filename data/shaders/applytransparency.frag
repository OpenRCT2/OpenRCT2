#version 150

uniform usampler2D      uOpaqueTex;
uniform sampler2D       uOpaqueDepth;
uniform usampler2D      uTransparentTex;
uniform sampler2D       uTransparentDepth;
uniform usampler2DRect  uPaletteTex;

in vec2 fTextureCoordinate;

out uint oColour;

void main()
{
    uint  opaque = texture(uOpaqueTex, fTextureCoordinate).r;
    float opaqueDepth = texture(uOpaqueDepth, fTextureCoordinate).r;
    uint  transparent = texture(uTransparentTex, fTextureCoordinate).r;
    float transparentDepth = texture(uTransparentDepth, fTextureCoordinate).r;

    if (opaqueDepth <= transparentDepth)
    {
        transparent = 0u;
    }

    oColour = texture(uPaletteTex, vec2(opaque, transparent)).r;
}
