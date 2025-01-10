/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ApplyTransparencyShader.h"
#include "OpenGLAPI.h"
#include "OpenGLFramebuffer.h"

namespace OpenRCT2::Ui
{
    /**
     * Class to maintain two different framebuffers where the active framebuffer
     * will swap between the two, copying the other's pixels in the process for
     * performing pre-processing filters.
     *
     * When you need to bind the current frame to a shader, call SwapCopy and
     * then bind the value of GetSourceTexture to your shader.
     */
    class SwapFramebuffer final
    {
    private:
        OpenGLFramebuffer _opaqueFramebuffer;
        OpenGLFramebuffer _transparentFramebuffer;
        OpenGLFramebuffer _mixFramebuffer;
        GLuint _backDepth;

    public:
        SwapFramebuffer(int32_t width, int32_t height);
        ~SwapFramebuffer();

        const OpenGLFramebuffer& GetFinalFramebuffer() const
        {
            return _opaqueFramebuffer;
        }
        GLuint GetBackDepthTexture() const
        {
            return _backDepth;
        }
        void BindOpaque()
        {
            _opaqueFramebuffer.Bind();
        }
        void BindTransparent()
        {
            _transparentFramebuffer.Bind();
        }

        void ApplyTransparency(ApplyTransparencyShader& shader, GLuint paletteTex, GLuint blendPaletteTex);
        void Clear();
    };
} // namespace OpenRCT2::Ui
