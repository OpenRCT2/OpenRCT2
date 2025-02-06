#version 150

uniform sampler2D uTexture;
in vec2 fTexCoord;
out vec4 oColour;

// Sobel operator kernels for edge detection
const vec2 offset[9] = vec2[](
    vec2(-1.0,-1.0), vec2( 0.0,-1.0), vec2( 1.0,-1.0),
    vec2(-1.0, 0.0), vec2( 0.0, 0.0), vec2( 1.0, 0.0),
    vec2(-1.0, 1.0), vec2( 0.0, 1.0), vec2( 1.0, 1.0)
);

const float kernelX[9] = float[](
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
);

const float kernelY[9] = float[](
    -1, -2, -1,
    0,  0,  0,
    1,  2,  1
);

void main()
{
    vec2 texelSize = 1.0 / textureSize(uTexture, 0);
    vec4 sampleTex[19];

    // Sample the texture
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = texture(uTexture, fTexCoord + offset[i] * texelSize);
    }

    // Compute the gradient in x and y direction using Sobel operators
    float gx = 0.0;
    float gy = 0.0;
    for (int i = 0; i < 9; i++)
    {
        gx += sampleTex[i].r * kernelX[i];
        gy += sampleTex[i].r * kernelY[i];
    }

    // Compute the magnitude of the gradient (edge intensity)
    float edgeIntensity = sqrt(gx * gx + gy * gy);

    // Output the edge intensity. Here we're using it for all channels to create a greyscale edge map
    oColour = vec4(vec3(edgeIntensity), 1.0);
}