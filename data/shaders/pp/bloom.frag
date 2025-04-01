#version 150

uniform sampler2D uTexture;
in vec2 fTexCoord;
out vec4 oColour;

// Adjust these parameters to control the bloom effect:
const float threshold = 0.2; // Only pixels above this brightness contribute to bloom
const float bloomIntensity = 0.35; // How much bloom affects the final color
const float blurSize = 0.01; // Size of the blur, tweak for performance vs. quality

void main()
{
    // Sample the original color
    vec4 originalColor = texture(uTexture, fTexCoord);

    // Calculate the brightness of the current pixel
    float brightness = dot(originalColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    // If the pixel is bright enough, it will contribute to bloom
    if(brightness > threshold)
    {
        vec4 bloomColor = vec4(0.0);
        int samples = 0;

        // Sample neighboring pixels to create a simple blur for bloom effect
        for (int x = -2; x <= 2; ++x)
        {
            for (int y = -2; y <= 2; ++y)
            {
                vec2 offset = vec2(float(x), float(y)) * blurSize;
                vec4 sample = texture(uTexture, fTexCoord + offset);
                // Only bright pixels contribute to bloom
                if (dot(sample.rgb, vec3(0.2126, 0.7152, 0.0722)) > threshold)
                {
                    bloomColor += sample;
                    samples++;
                }
            }
        }

        // Average the bloom color from samples
        if (samples > 0)
        {
            bloomColor /= float(samples);
        }

        // Combine original color with bloom effect
        oColour = originalColor + bloomIntensity * bloomColor;
    }
    else
    {
        // If not contributing to bloom, just use the original color
        oColour = originalColor;
    }

    // Clamp the color to ensure it stays within [0, 1] range
    oColour = clamp(oColour, 0.0, 1.0);
}