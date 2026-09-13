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

#include <SDL_pixels.h>
namespace OpenRCT2::Ui
{
    class DrawRectShader final : public OpenGLShaderProgram
    {
    private:
        GLint uScreenSize{ -1 };
        GLint uTexture{ -1 };
        GLint uPaletteTex{ -1 };

        GLint uPeelingTex{ -1 };
        GLint uPeeling{ -1 };

        GLint vVertMat{ -1 };
        GLint vVertVec{ -1 };

        GLint vClip{ -1 };
        GLint vTexColourAtlas{ -1 };
        GLint vTexColourCoords{ -1 };
        GLint vTexMaskAtlas{ -1 };
        GLint vTexMaskCoords{ -1 };
        GLint vPalettes{ -1 };
        GLint vFlags{ -1 };
        GLint vColour{ -1 };
        GLint vBounds{ -1 };
        GLint vDepth{ -1 };
        GLint vZoom{ -1 };

        GLuint _vbo{ 0 };
        GLuint _vboInstances{ 0 };
        GLuint _vao{ 0 };

        GLsizei _instanceCount = 0;
        size_t _maxInstancesBufferSize;

    public:
        DrawRectShader();
        ~DrawRectShader() override;

        void SetScreenSize(int32_t width, int32_t height);
        void EnablePeeling(GLuint peelingTex);
        void DisablePeeling();

        void SetInstances(const RectCommandBatch& instances);
        void DrawInstances();

    private:
        void GetLocations();
    };
} // namespace OpenRCT2::Ui
