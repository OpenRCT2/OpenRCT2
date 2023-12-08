#version 150

// clang-format off
uniform usampler2D      uOpaqueTex;
uniform sampler2D       uOpaqueDepth;
uniform usampler2D      uTransparentTex;
uniform sampler2D       uTransparentDepth;
uniform usampler2D      uPaletteTex;
uniform usampler2D      uBlendPaletteTex;
// clang-format off

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
    
    uint blendColour = (transparent & 0xff00u) >> 8;
    if(blendColour > 0u)
    {
        if((transparent & 0x00ffu) != 0u)
        {
            oColour = blendColour;
        }
        else 
        {
            oColour = texture(uBlendPaletteTex, vec2(opaque, blendColour) / 256.f).r;
        }
    }
    else
    {
        oColour = texture(uPaletteTex, vec2(opaque, transparent) / 256.f).r;
    }
}
