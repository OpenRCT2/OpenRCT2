/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"

class ApplyTransparencyShader final : public OpenGLShaderProgram
{
private:
    GLuint uOpaqueTex;
    GLuint uOpaqueDepth;
    GLuint uTransparentTex;
    GLuint uTransparentDepth;
    GLuint uPaletteTex;

    GLuint vPosition;
    GLuint vTextureCoordinate;

    GLuint _vbo;
    GLuint _vao;

public:
    ApplyTransparencyShader();
    ~ApplyTransparencyShader() override;

    static void SetTextures(
        GLuint opaqueTex, GLuint opaqueDepth, GLuint transparentTex, GLuint transparentDepth, GLuint paletteTex);
    void Draw();

private:
    void GetLocations();
};
