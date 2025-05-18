#version 150

in vec2 vPosition;
in vec2 vTextureCoordinate;

out vec2 fTextureCoordinate;

uniform vec4 uSourceRect;
uniform vec2 uTextureSize;
uniform bool uFlipY;

void main()
{
    gl_Position = vec4(vPosition, 0.0, 1.0);
    
    vec2 srcOffset = vec2(uSourceRect.x, uFlipY ? 
        (uTextureSize.y - uSourceRect.y - uSourceRect.w) : uSourceRect.y);
    
    fTextureCoordinate = (srcOffset + vTextureCoordinate * vec2(uSourceRect.z, uSourceRect.w)) / uTextureSize;
}
