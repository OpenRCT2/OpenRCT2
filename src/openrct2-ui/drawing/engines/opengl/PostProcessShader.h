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

#include <openrct2/core/StringTypes.h>

namespace OpenRCT2::Ui
{
    class PostProcessShader final : public OpenGLShaderProgram
    {
    private:
        GLuint uTexture;
        GLuint uTicks;
        GLuint uZoom;

        GLuint vPosition;
        GLuint vTextureCoordinate;

        GLuint _vbo;
        GLuint _vao;

    public:
        PostProcessShader(u8string_view name);
        ~PostProcessShader() override;

        static void SetTexture(GLuint texture);
        void SetTickCount(uint32_t ticks);
        void SetZoom(float zoom);

        void Draw();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
