/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    class ApplyTransparencyShader final : public OpenGLShaderProgram
    {
    private:
        GLint uOpaqueTex{ -1 };
        GLint uOpaqueDepth{ -1 };
        GLint uTransparentTex{ -1 };
        GLint uTransparentDepth{ -1 };
        GLint uPaletteTex{ -1 };
        GLint uBlendPaletteTex{ -1 };

        GLint vPosition{ -1 };
        GLint vTextureCoordinate{ -1 };

        GLuint _vbo{};
        GLuint _vao{};

    public:
        ApplyTransparencyShader();
        ~ApplyTransparencyShader() override;

        static void SetTextures(
            GLuint opaqueTex, GLuint opaqueDepth, GLuint transparentTex, GLuint transparentDepth, GLuint paletteTex,
            GLuint blendPaletteTex);
        void Draw();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
