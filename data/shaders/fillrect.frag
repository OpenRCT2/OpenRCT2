#version 150

uniform ivec2       uScreenSize;
uniform ivec4       uClip;
uniform int         uFlags;
uniform vec4        uColour[2];
uniform sampler2D   uSourceFramebuffer;
uniform ivec4       uBounds;

in vec2 fPosition;

out vec4 oColour;

float getluma(vec3 colour)
{
    return (colour.r * 0.2126) +
           (colour.g * 0.7152) +
           (colour.b * 0.0722);
}

void main()
{
    if (fPosition.x < uClip.x || fPosition.x > uClip.z ||
        fPosition.y < uClip.y || fPosition.y > uClip.w)
    {
        discard;
    }

    vec4 targetColour;
    int posSum = int(fPosition.x) + int(fPosition.y);
    if ((posSum % 2) == 0)
    {
        targetColour = uColour[0];
    }
    else
    {
        targetColour = uColour[1];
    }

    if ((uFlags & 1) != 0)
    {
        vec2 textureCoordinates = (fPosition / vec2(uScreenSize)) * vec2(1, -1);
        vec4 sourceColour = texture(uSourceFramebuffer, textureCoordinates);

        float luma = getluma(sourceColour.rgb);
        sourceColour = vec4(vec3(luma), 1);

        if (luma < 0.5)
        {
            oColour = 2.0 * sourceColour * targetColour;
        }
        else
        {
            oColour = 1.0 - 2.0 * (1.0 - sourceColour) * (1.0 - targetColour);
        }
    }
    else
    {
        oColour = targetColour;
    }
}
