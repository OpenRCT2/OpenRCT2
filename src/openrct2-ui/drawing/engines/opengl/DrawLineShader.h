/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "DrawCommands.h"
#include "GLSLTypes.h"
#include "OpenGLShaderProgram.h"
namespace OpenRCT2::Ui
{
    class DrawLineShader final : public OpenGLShaderProgram
    {
    private:
        GLint uScreenSize{ -1 };

        GLint vBounds{ -1 };
        GLint vColour{ -1 };
        GLint vDepth{ -1 };

        GLint vVertMat{ -1 };

        GLuint _vbo{};
        GLuint _vboInstances{};
        GLuint _vao{};

    public:
        DrawLineShader();
        ~DrawLineShader() override;

        void SetScreenSize(int32_t width, int32_t height);
        void DrawInstances(const LineCommandBatch& instances);

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
