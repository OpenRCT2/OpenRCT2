/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
        GLuint uScreenSize;
        GLuint uTexture;
        GLuint uPaletteTex;

        GLuint uPeelingTex;
        GLuint uPeeling;

        GLuint vVertMat;
        GLuint vVertVec;

        GLuint vClip;
        GLuint vTexColourAtlas;
        GLuint vTexColourBounds;
        GLuint vTexMaskAtlas;
        GLuint vTexMaskBounds;
        GLuint vPalettes;
        GLuint vFlags;
        GLuint vColour;
        GLuint vBounds;
        GLuint vDepth;

        GLuint _vbo;
        GLuint _vboInstances;
        GLuint _vao;

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
