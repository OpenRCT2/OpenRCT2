/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "ApplyTransparencyShader.h"

ApplyTransparencyShader::ApplyTransparencyShader()
    : OpenGLShaderProgram("applytransparency")
{
    GetLocations();
    Use();
    glUniform1i(uOpaqueTex, 0);
    glUniform1i(uOpaqueDepth, 1);
    glUniform1i(uTransparentTex, 2);
    glUniform1i(uTransparentDepth, 3);
    glUniform1i(uPaletteTex, 4);
    glUniform1i(uBlendPaletteTex, 5);
}

void ApplyTransparencyShader::GetLocations()
{
    uOpaqueTex = GetUniformLocation("uOpaqueTex");
    uOpaqueDepth = GetUniformLocation("uOpaqueDepth");
    uTransparentTex = GetUniformLocation("uTransparentTex");
    uTransparentDepth = GetUniformLocation("uTransparentDepth");
    uPaletteTex = GetUniformLocation("uPaletteTex");
    uBlendPaletteTex = GetUniformLocation("uBlendPaletteTex");
}

void ApplyTransparencyShader::SetTextures(
    GLuint opaqueTex, GLuint opaqueDepth, GLuint transparentTex, GLuint transparentDepth, GLuint paletteTex,
    GLuint blendPaletteTex)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, opaqueTex);
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, opaqueDepth);
    OpenGLAPI::SetTexture(2, GL_TEXTURE_2D, transparentTex);
    OpenGLAPI::SetTexture(3, GL_TEXTURE_2D, transparentDepth);
    OpenGLAPI::SetTexture(4, GL_TEXTURE_2D, paletteTex);
    OpenGLAPI::SetTexture(5, GL_TEXTURE_2D, blendPaletteTex);
}

void ApplyTransparencyShader::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

#endif /* DISABLE_OPENGL */
