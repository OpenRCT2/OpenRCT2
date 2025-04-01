#version 150

uniform sampler2D uTexture; // Current frame texture
uniform sampler2D uVelocity; // Velocity buffer, typically contains motion vector per pixel
uniform float uBlurAmount = 0.005; // Controls the strength of the motion blur
in vec2 fTexCoord;
out vec4 oColour;

const int numSamples = 9; // Number of samples for motion blur

void main()
{
    vec4 currentColor = texture(uTexture, fTexCoord);
    vec2 velocity = texture(uVelocity, fTexCoord).xy; // Assuming velocity stored in RG channels as [-1, 1]
    vec4 blurColor = vec4(0.0);

    // Sample along the motion vector
    for (int i = 0; i < numSamples; ++i)
    {
        float t = float(i) / float(numSamples - 1);
        vec2 offset = velocity * t * uBlurAmount;
        vec2 sampleCoord = fTexCoord - offset; // Sample backwards along motion vector
        blurColor += texture(uTexture, sampleCoord);
    }

    // Average the samples
    blurColor /= float(numSamples);

    // Blend current color with blur color
    // Here we're using a simple lerp (linear interpolation), but you could adjust this blend
    oColour = mix(currentColor, blurColor, 0.8); // 0.8 is blend factor, adjust for intensity
}