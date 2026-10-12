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

#include <SDL_pixels.h>
namespace OpenRCT2::Ui
{
    class ApplyPaletteShader final : public OpenGLShaderProgram
    {
    private:
        GLint uTexture{ -1 };
        GLint uShadowTex{ -1 };
        GLint uWorldDepthTex{ -1 };
        GLint uShadeRemapTex{ -1 };
        GLint uUseShadows{ -1 };
        GLint uPalette{ -1 };

        GLint vPosition{ -1 };
        GLint vTextureCoordinate{ -1 };

        GLuint _vbo{};
        GLuint _vao{};

    public:
        ApplyPaletteShader();
        ~ApplyPaletteShader() override;

        static void SetTexture(GLuint texture);
        static void SetShadowTexture(GLuint texture);
        static void SetWorldDepthTexture(GLuint texture);
        static void SetShadeRemapTexture(GLuint texture);
        void SetUseShadows(bool enabled);
        void SetPalette(const vec4* glPalette);

        void Draw();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
