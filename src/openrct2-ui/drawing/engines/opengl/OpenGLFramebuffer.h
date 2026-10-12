/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "OpenGLAPI.h"

namespace OpenRCT2::Drawing
{
    struct RenderTarget;
}

struct SDL_Window;
namespace OpenRCT2::Ui
{
    class OpenGLFramebuffer
    {
    private:
        GLuint _id;
        GLuint _texture;
        GLuint _worldDepth{};
        GLuint _depth;
        int32_t _width;
        int32_t _height;
        bool _complete = true;

    public:
        explicit OpenGLFramebuffer(SDL_Window* window);
        /** depth16: use a 16-bit depth attachment instead of 24-bit. The shadow sun pass only
            needs to resolve a rank quantised to about 2048 distinct values, which 16 bits holds
            comfortably, and its targets are large enough that halving this attachment is worth
            having. Everything else keeps 24-bit. */
        OpenGLFramebuffer(
            int32_t width, int32_t height, bool depth = true, bool integer = true, bool word = false, bool worldDepth = false,
            bool depth16 = false);
        ~OpenGLFramebuffer();

        OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;
        OpenGLFramebuffer& operator=(const OpenGLFramebuffer&) = delete;

        /** False when the GL framebuffer never became complete - in practice a GPU-side
            allocation failure on a very large target. glTexImage2D does not throw, so this is
            the only signal; using such a framebuffer anyway crashes later, so callers that can
            allocate big targets must check it and drop the result. */
        bool IsComplete() const
        {
            return _complete;
        }

        GLuint GetWidth() const
        {
            return _width;
        }
        GLuint GetHeight() const
        {
            return _height;
        }
        GLuint GetFBO() const
        {
            return _id;
        }
        GLuint GetTexture() const
        {
            return _texture;
        }
        GLuint GetWorldDepthTexture() const
        {
            return _worldDepth;
        }
        GLuint GetDepthTexture() const
        {
            return _depth;
        }

        void Bind() const;
        void BindDraw() const;
        void BindRead() const;
        void GetPixels(Drawing::RenderTarget& rt) const;

        void SwapColourBuffer(OpenGLFramebuffer& other);
        GLuint SwapDepthTexture(GLuint depth);
        void Copy(OpenGLFramebuffer& src, GLenum filter);
        /** Copies a rect of src's world-depth attachment into this one's. GL coordinates
            (bottom-left origin); the two must not be the same framebuffer if the rects overlap. */
        void BlitWorldDepth(
            const OpenGLFramebuffer& src, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t dstX,
            int32_t dstY);
        void SetPixels(const Drawing::RenderTarget& rt);
        void ClearWorldDepth();

        static GLuint CreateDepthTexture(int32_t width, int32_t height, bool halfPrecision);
        static GLuint CreateWorldDepthTexture(int32_t width, int32_t height);
    };
} // namespace OpenRCT2::Ui
