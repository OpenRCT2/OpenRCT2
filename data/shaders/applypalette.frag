#version 330 core

uniform vec4 uPalette[256];
uniform usampler2D uTexture;
uniform sampler2D uShadowTex;
uniform usampler2D uWorldDepthTex;
uniform bool uUseShadows;
/** The game's darkening palettes, darken 1 to 3, one row each: index -> darker index. */
uniform usampler2D uShadeRemapTex;

#include "worlddepth.glsl"

in vec2 fTextureCoordinate;

out vec4 oColour;

// Mean brightness each darkening palette keeps, measured over the colour ramps.
const float DARKEN_LEVEL_BRIGHTNESS[4] = float[4](1.0, 0.853, 0.739, 0.517);

vec3 darkenedColour(uint index, int level)
{
    if (level == 0)
        return uPalette[index].rgb;
    uint darker = texelFetch(uShadeRemapTex, ivec2(int(index), level - 1), 0).r;
    return uPalette[darker].rgb;
}

// Shades a palette colour to keep `brightness` of its light the way the art shades: by stepping
// down its own colour ramp - the game's darkening palettes - rather than scaling it toward black,
// which greys the colour out. Between two darkening levels the two ramp colours are mixed; past
// the darkest the darkest is scaled further.
vec3 shadeColour(uint index, float brightness)
{
    if (brightness >= 1.0)
        return uPalette[index].rgb;
    for (int level = 1; level < 4; level++)
    {
        if (brightness >= DARKEN_LEVEL_BRIGHTNESS[level])
        {
            float t = (DARKEN_LEVEL_BRIGHTNESS[level - 1] - brightness)
                / (DARKEN_LEVEL_BRIGHTNESS[level - 1] - DARKEN_LEVEL_BRIGHTNESS[level]);
            return mix(darkenedColour(index, level - 1), darkenedColour(index, level), t);
        }
    }
    return darkenedColour(index, 3) * (brightness / DARKEN_LEVEL_BRIGHTNESS[3]);
}

void main()
{
    uint index = texture(uTexture, fTextureCoordinate).r;
    vec4 colour = uPalette[index];
    uvec4 depth = texture(uWorldDepthTex, fTextureCoordinate);
    uint mode = depthMode(depth);
    float lit = uUseShadows ? texture(uShadowTex, fTextureCoordinate).r : 1.0;

    // Only receivers take the mask; a sprite or window pixel is never shaded. Under a translucent
    // panel the shadow can only be applied after the panel's tint (the blend happened in palette
    // space already), so it is softened rather than drawn at full strength over the panel.
    if (isReceiverMode(mode))
    {
        colour.rgb = shadeColour(index, (depth.a & UNDER_TRANSLUCENT_UI) != 0u ? mix(1.0, lit, 0.5) : lit);
    }

    oColour = colour;
}
