/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#    include "ApplyPaletteShader.h"

ApplyPaletteShader::ApplyPaletteShader()
    : OpenGLShaderProgram("applypalette")
{
    GetLocations();
    Use();
    glUniform1i(uTexture, 0);
}

void ApplyPaletteShader::GetLocations()
{
    uTexture = GetUniformLocation("uTexture");
    uPalette = GetUniformLocation("uPalette");
}

void ApplyPaletteShader::SetTexture(GLuint texture)
{
    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D, texture);
}

void ApplyPaletteShader::SetPalette(const vec4* glPalette)
{
    glUniform4fv(uPalette, 256, reinterpret_cast<const GLfloat*>(glPalette));
}

void ApplyPaletteShader::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

#endif /* DISABLE_OPENGL */
