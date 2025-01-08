/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"

namespace OpenRCT2::Ui
{
    class ApplyPaletteShader final : public OpenGLShaderProgram
    {
    private:
        GLuint uTexture;
        GLuint uPalette;

        GLuint vPosition;
        GLuint vTextureCoordinate;

        GLuint _vbo;
        GLuint _vao;

    public:
        ApplyPaletteShader();
        ~ApplyPaletteShader() override;

        static void SetTexture(GLuint texture);
        void SetPalette(const vec4* glPalette);

        void Draw();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
