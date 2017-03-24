#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifdef DISABLE_OPENGL

#include "../../IDrawingEngine.h"

IDrawingEngine * DrawingEngineFactory::CreateOpenGL()
{
    return nullptr;
}

#else

#include <unordered_map>
#include <vector>
#include <SDL_platform.h>

#include "GLSLTypes.h"
#include "OpenGLAPI.h"
#include "OpenGLFramebuffer.h"
#include "CopyFramebufferShader.h"
#include "DrawImageShader.h"
#include "DrawLineShader.h"
#include "FillRectShader.h"
#include "SwapFramebuffer.h"
#include "TextureCache.h"
#include "DrawCommands.h"

#include "../../../core/Console.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/Math.hpp"
#include "../../../core/Memory.hpp"
#include "../../IDrawingContext.h"
#include "../../IDrawingEngine.h"
#include "../../Rain.h"
#include "../../../config/Config.h"

extern "C"
{
    #include "../../../config/Config.h"
    #include "../../../interface/screenshot.h"
    #include "../../../interface/window.h"
    #include "../../../intro.h"
    #include "../../drawing.h"
}

struct OpenGLVersion
{
    GLint Major;
    GLint Minor;
};

constexpr OpenGLVersion OPENGL_MINIMUM_REQUIRED_VERSION = { 3, 3 };

static const vec3f TransparentColourTable[144 - 44] =
{
    { 0.7f, 0.8f, 0.8f }, // 44
    { 0.7f, 0.8f, 0.8f },
    { 0.3f, 0.4f, 0.4f },
    { 0.2f, 0.3f, 0.3f },
    { 0.1f, 0.2f, 0.2f },
    { 0.4f, 0.5f, 0.5f },
    { 0.3f, 0.4f, 0.4f },
    { 0.4f, 0.5f, 0.5f },
    { 0.4f, 0.5f, 0.5f },
    { 0.3f, 0.4f, 0.4f },
    { 0.6f, 0.7f, 0.7f },
    { 0.3f, 0.5f, 0.9f },
    { 0.1f, 0.3f, 0.8f },
    { 0.5f, 0.7f, 0.9f },
    { 0.6f, 0.2f, 0.2f },
    { 0.5f, 0.1f, 0.1f },
    { 0.8f, 0.4f, 0.4f },
    { 0.3f, 0.5f, 0.4f },
    { 0.2f, 0.4f, 0.2f },
    { 0.5f, 0.7f, 0.5f },
    { 0.5f, 0.5f, 0.7f },
    { 0.3f, 0.3f, 0.5f },
    { 0.6f, 0.6f, 0.8f },
    { 0.5f, 0.5f, 0.2f },
    { 0.4f, 0.4f, 0.1f },
    { 0.7f, 0.7f, 0.4f },
    { 0.7f, 0.5f, 0.3f },
    { 0.6f, 0.4f, 0.2f },
    { 0.8f, 0.7f, 0.4f },
    { 0.8f, 0.7f, 0.1f },
    { 0.7f, 0.4f, 0.0f },
    { 1.0f, 0.9f, 0.2f },
    { 0.4f, 0.6f, 0.2f },
    { 0.3f, 0.4f, 0.2f },
    { 0.5f, 0.7f, 0.3f },
    { 0.5f, 0.6f, 0.4f },
    { 0.4f, 0.4f, 0.3f },
    { 0.7f, 0.8f, 0.5f },
    { 0.3f, 0.7f, 0.2f },
    { 0.2f, 0.6f, 0.0f },
    { 0.4f, 0.8f, 0.3f },
    { 0.8f, 0.5f, 0.4f },
    { 0.7f, 0.4f, 0.3f },
    { 0.9f, 0.7f, 0.5f },
    { 0.5f, 0.3f, 0.7f },
    { 0.4f, 0.2f, 0.6f },
    { 0.7f, 0.5f, 0.8f },
    { 0.9f, 0.0f, 0.0f },
    { 0.7f, 0.0f, 0.0f },
    { 1.0f, 0.3f, 0.3f },
    { 1.0f, 0.4f, 0.1f },
    { 0.9f, 0.3f, 0.0f },
    { 1.0f, 0.6f, 0.3f },
    { 0.2f, 0.6f, 0.6f },
    { 0.0f, 0.4f, 0.4f },
    { 0.4f, 0.7f, 0.7f },
    { 0.9f, 0.2f, 0.6f },
    { 0.6f, 0.1f, 0.4f },
    { 1.0f, 0.5f, 0.7f },
    { 0.6f, 0.5f, 0.4f },
    { 0.4f, 0.3f, 0.2f },
    { 0.7f, 0.7f, 0.6f },
    { 0.9f, 0.6f, 0.6f },
    { 0.8f, 0.5f, 0.5f },
    { 1.0f, 0.7f, 0.7f },
    { 0.7f, 0.8f, 0.8f },
    { 0.5f, 0.6f, 0.6f },
    { 0.9f, 1.0f, 1.0f },
    { 0.2f, 0.3f, 0.3f },
    { 0.4f, 0.5f, 0.5f },
    { 0.7f, 0.8f, 0.8f },
    { 0.2f, 0.3f, 0.5f },
    { 0.5f, 0.5f, 0.7f },
    { 0.5f, 0.3f, 0.7f },
    { 0.1f, 0.3f, 0.7f },
    { 0.3f, 0.5f, 0.9f },
    { 0.6f, 0.8f, 1.0f },
    { 0.2f, 0.6f, 0.6f },
    { 0.5f, 0.8f, 0.8f },
    { 0.1f, 0.5f, 0.0f },
    { 0.3f, 0.5f, 0.4f },
    { 0.4f, 0.6f, 0.2f },
    { 0.3f, 0.7f, 0.2f },
    { 0.5f, 0.6f, 0.4f },
    { 0.5f, 0.5f, 0.2f },
    { 1.0f, 0.9f, 0.2f },
    { 0.8f, 0.7f, 0.1f },
    { 0.6f, 0.3f, 0.0f },
    { 1.0f, 0.4f, 0.1f },
    { 0.7f, 0.3f, 0.0f },
    { 0.7f, 0.5f, 0.3f },
    { 0.5f, 0.3f, 0.1f },
    { 0.5f, 0.4f, 0.3f },
    { 0.8f, 0.5f, 0.4f },
    { 0.6f, 0.2f, 0.2f },
    { 0.6f, 0.0f, 0.0f },
    { 0.9f, 0.0f, 0.0f },
    { 0.6f, 0.1f, 0.3f },
    { 0.9f, 0.2f, 0.6f },
    { 0.9f, 0.6f, 0.6f },
};

class OpenGLDrawingEngine;

class OpenGLDrawingContext final : public IDrawingContext
{
private:
    OpenGLDrawingEngine *   _engine = nullptr;
    rct_drawpixelinfo *     _dpi    = nullptr;

    DrawImageShader *       _drawImageShader        = nullptr;
    DrawLineShader *        _drawLineShader         = nullptr;
    FillRectShader *        _fillRectShader         = nullptr;

    TextureCache * _textureCache = nullptr;

    sint32 _offsetX    = 0;
    sint32 _offsetY    = 0;
    sint32 _clipLeft   = 0;
    sint32 _clipTop    = 0;
    sint32 _clipRight  = 0;
    sint32 _clipBottom = 0;

    struct {
        std::vector<DrawRectCommand> rectangles;
        std::vector<DrawLineCommand> lines;
        std::vector<DrawImageCommand> images;
    } _commandBuffers;

public:
    explicit OpenGLDrawingContext(OpenGLDrawingEngine * engine);
    ~OpenGLDrawingContext() override;

    IDrawingEngine * GetEngine() override;
    TextureCache * GetTextureCache() const { return _textureCache; }

    void Initialise();
    void Resize(sint32 width, sint32 height);
    void ResetPalette();

    void Clear(uint8 paletteIndex) override;
    void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
    void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) override;
    void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2) override;
    void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
    void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;
    void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour) override;
    void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette) override;

    void FlushCommandBuffers();

    void FlushRectangles();
    void FlushLines();
    void FlushImages();

    void SetDPI(rct_drawpixelinfo * dpi);
};

class OpenGLDrawingEngine : public IDrawingEngine
{
private:
    SDL_Window *    _window         = nullptr;
    SDL_GLContext   _context        = nullptr;

    uint32  _width      = 0;
    uint32  _height     = 0;
    uint32  _pitch      = 0;
    size_t  _bitsSize   = 0;
    uint8 * _bits       = nullptr;

    rct_drawpixelinfo _bitsDPI  = { 0 };

    OpenGLDrawingContext *    _drawingContext;

    CopyFramebufferShader * _copyFramebufferShader  = nullptr;
    OpenGLFramebuffer *     _screenFramebuffer      = nullptr;
    SwapFramebuffer *       _swapFramebuffer        = nullptr;

public:
    SDL_Color Palette[256];
    vec4f     GLPalette[256];

    OpenGLDrawingEngine()
    {
        _drawingContext = new OpenGLDrawingContext(this);
    }

    ~OpenGLDrawingEngine() override
    {
        delete _copyFramebufferShader;
        delete _screenFramebuffer;
        delete _swapFramebuffer;

        delete _drawingContext;
        delete [] _bits;

        SDL_GL_DeleteContext(_context);
    }

    void Initialise(SDL_Window * window) override
    {
        _window = window;

        OpenGLVersion requiredVersion = OPENGL_MINIMUM_REQUIRED_VERSION;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, requiredVersion.Major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, requiredVersion.Minor);

        _context = SDL_GL_CreateContext(_window);
        if (_context == nullptr)
        {
            char szRequiredVersion[32];
            snprintf(szRequiredVersion, 32, "OpenGL %d.%d", requiredVersion.Major, requiredVersion.Minor);
            throw Exception(std::string(szRequiredVersion) + std::string(" not available."));
        }
        SDL_GL_MakeCurrent(_window, _context);

        if (!OpenGLAPI::Initialise())
        {
            throw Exception("Unable to initialise OpenGL.");
        }

        _drawingContext->Initialise();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _copyFramebufferShader = new CopyFramebufferShader();
    }

    void Resize(uint32 width, uint32 height) override
    {
        ConfigureBits(width, height, width);
        ConfigureCanvas();
        _drawingContext->Resize(width, height);
    }

    void SetPalette(SDL_Color * palette) override
    {
        for (sint32 i = 0; i < 256; i++)
        {
            SDL_Color colour = palette[i];
            colour.a = i == 0 ? 0 : 255;

            Palette[i] = colour;
            GLPalette[i] = { colour.r / 255.0f,
                             colour.g / 255.0f,
                             colour.b / 255.0f,
                             colour.a / 255.0f };
        }
        _drawingContext->ResetPalette();
    }

    void SetUncappedFrameRate(bool uncapped) override
    {
        SDL_GL_SetSwapInterval(uncapped ? 0 : 1);
    }

    void Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) override
    {
    }

    void Draw() override
    {
        assert(_screenFramebuffer != nullptr);
        assert(_swapFramebuffer != nullptr);

        _swapFramebuffer->Bind();

        if (gIntroState != INTRO_STATE_NONE) {
            intro_draw(&_bitsDPI);
        } else {
            window_update_all_viewports();
            window_draw_all(&_bitsDPI, 0, 0, _width, _height);
            window_update_all();

            gfx_draw_pickedup_peep(&_bitsDPI);

            _drawingContext->FlushCommandBuffers();
            _swapFramebuffer->SwapCopy();

            rct2_draw(&_bitsDPI);
        }

        _drawingContext->FlushCommandBuffers();

        // Scale up to window
        _screenFramebuffer->Bind();
        _copyFramebufferShader->Use();
        _copyFramebufferShader->SetTexture(_swapFramebuffer->GetTargetFramebuffer()
                                                           ->GetTexture());
        _copyFramebufferShader->Draw();

        CheckGLError();
        Display();
    }

    sint32 Screenshot() override
    {
        const OpenGLFramebuffer * framebuffer = _swapFramebuffer->GetTargetFramebuffer();
        framebuffer->Bind();
        void * pixels = framebuffer->GetPixels();

        sint32 result = screenshot_dump_png_32bpp(_width, _height, pixels);
        Memory::Free(pixels);
        return result;
    }

    void CopyRect(sint32 x, sint32 y, sint32 width, sint32 height, sint32 dx, sint32 dy) override
    {
        // Not applicable for this engine
    }

    IDrawingContext * GetDrawingContext(rct_drawpixelinfo * dpi) override
    {
        _drawingContext->SetDPI(dpi);
        return _drawingContext;
    }

    rct_drawpixelinfo * GetDrawingPixelInfo() override
    {
        return &_bitsDPI;
    }

    DRAWING_ENGINE_FLAGS GetFlags() override
    {
        return DEF_NONE;
    }

    void InvalidateImage(uint32 image) override
    {
        _drawingContext->GetTextureCache()
                       ->InvalidateImage(image);
    }

    rct_drawpixelinfo * GetDPI()
    {
        return &_bitsDPI;
    }

    GLuint SwapCopyReturningSourceTexture()
    {
        _swapFramebuffer->SwapCopy();
        return _swapFramebuffer->GetSourceTexture();
    }

private:
    static OpenGLVersion GetOpenGLVersion()
    {
        CheckGLError(); // Clear Any Errors
        OpenGLVersion version = { 0, 0 };
        glGetIntegerv(GL_MAJOR_VERSION, &version.Major);
        if (glGetError() != GL_NO_ERROR) return { 0, 0 };
        glGetIntegerv(GL_MINOR_VERSION, &version.Minor);
        if (glGetError() != GL_NO_ERROR) return { 0, 0 };
        return version;
    }

    void ConfigureBits(uint32 width, uint32 height, uint32 pitch)
    {
        size_t  newBitsSize = pitch * height;
        uint8 * newBits = new uint8[newBitsSize];
        if (_bits == nullptr)
        {
            Memory::Set(newBits, 0, newBitsSize);
        }
        else
        {
            if (_pitch == pitch)
            {
                Memory::Copy(newBits, _bits, Math::Min(_bitsSize, newBitsSize));
            }
            else
            {
                uint8 * src = _bits;
                uint8 * dst = newBits;

                uint32 minWidth = Math::Min(_width, width);
                uint32 minHeight = Math::Min(_height, height);
                for (uint32 y = 0; y < minHeight; y++)
                {
                    Memory::Copy(dst, src, minWidth);
                    if (pitch - minWidth > 0)
                    {
                        Memory::Set(dst + minWidth, 0, pitch - minWidth);
                    }
                    src += _pitch;
                    dst += pitch;
                }
            }
            delete [] _bits;
        }

        _bits = newBits;
        _bitsSize = newBitsSize;
        _width = width;
        _height = height;
        _pitch = pitch;

        rct_drawpixelinfo * dpi = &_bitsDPI;
        dpi->bits = _bits;
        dpi->x = 0;
        dpi->y = 0;
        dpi->width = width;
        dpi->height = height;
        dpi->pitch = _pitch - width;
    }

    void ConfigureCanvas()
    {
        // Re-create screen framebuffer
        delete _screenFramebuffer;
        _screenFramebuffer = new OpenGLFramebuffer(_window);

        // Re-create canvas framebuffer
        delete _swapFramebuffer;
        _swapFramebuffer = new SwapFramebuffer(_width, _height);

        _copyFramebufferShader->Use();
        _copyFramebufferShader->SetScreenSize(_width, _height);
        _copyFramebufferShader->SetBounds(0, 0, _width, _height);
        _copyFramebufferShader->SetTextureCoordinates(0, 1, 1, 0);
    }

    void Display()
    {
        SDL_GL_SwapWindow(_window);
    }
};

IDrawingEngine * DrawingEngineFactory::CreateOpenGL()
{
    return new OpenGLDrawingEngine();
}

OpenGLDrawingContext::OpenGLDrawingContext(OpenGLDrawingEngine * engine)
{
    _engine = engine;
}

OpenGLDrawingContext::~OpenGLDrawingContext()
{
    delete _drawImageShader;
    delete _drawLineShader;
    delete _fillRectShader;

    delete _textureCache;
}

IDrawingEngine * OpenGLDrawingContext::GetEngine()
{
    return _engine;
}

void OpenGLDrawingContext::Initialise()
{
    _textureCache = new TextureCache();
    _drawImageShader = new DrawImageShader();
    _drawLineShader = new DrawLineShader();
    _fillRectShader = new FillRectShader();
}

void OpenGLDrawingContext::Resize(sint32 width, sint32 height)
{
    FlushCommandBuffers();

    _drawImageShader->Use();
    _drawImageShader->SetScreenSize(width, height);
    _drawLineShader->Use();
    _drawLineShader->SetScreenSize(width, height);
    _fillRectShader->Use();
    _fillRectShader->SetScreenSize(width, height);
}

void OpenGLDrawingContext::ResetPalette()
{
    FlushCommandBuffers();

    _textureCache->SetPalette(_engine->Palette);
    _drawImageShader->Use();
    _drawImageShader->SetPalette(_engine->GLPalette);
}

void OpenGLDrawingContext::Clear(uint8 paletteIndex)
{
    FillRect(paletteIndex, _clipLeft - _offsetX, _clipTop - _offsetY, _clipRight, _clipBottom);
}

void OpenGLDrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawRectCommand command = {};

    command.sourceFramebuffer = _fillRectShader->GetSourceFramebuffer();

    vec4f paletteColour[2];
    paletteColour[0] = _engine->GLPalette[(colour >> 0) & 0xFF];
    paletteColour[1] = paletteColour[0];
    if (colour & 0x1000000)
    {
        paletteColour[1].a = 0;

        command.flags = 0;
    }
    else if (colour & 0x2000000)
    {
        assert(false);
        // Should be FilterRect
    }
    else
    {
        command.flags = 0;
    }

    command.colours[0] = paletteColour[0];
    command.colours[1] = paletteColour[1];

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right + 1;
    command.bounds[3] = bottom + 1;

    _commandBuffers.rectangles.push_back(command);

    // Must be rendered in order, depends on already rendered contents
    FlushCommandBuffers();
}

void OpenGLDrawingContext::FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawRectCommand command = {};

    command.sourceFramebuffer = _fillRectShader->GetSourceFramebuffer();

    vec4f paletteColour[2];
    //paletteColour[0] = _engine->GLPalette[(colour >> 0) & 0xFF];
    //paletteColour[1] = paletteColour[0];

    // START FILTER

    uint8 tableIndex = palette;
    if (tableIndex <   44) return;
    if (tableIndex >= 144) return;
    tableIndex -= 44;

    vec3f transformColour = TransparentColourTable[tableIndex];
    paletteColour[0].r = transformColour.r;
    paletteColour[0].g = transformColour.g;
    paletteColour[0].b = transformColour.b;
    paletteColour[0].a = 1;
    paletteColour[1] = paletteColour[0];

    GLuint srcTexture =  _engine->SwapCopyReturningSourceTexture();
    command.flags = 1;
    command.sourceFramebuffer = srcTexture;

    // END FILTER


    command.colours[0] = paletteColour[0];
    command.colours[1] = paletteColour[1];

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right + 1;
    command.bounds[3] = bottom + 1;

    _commandBuffers.rectangles.push_back(command);

    // Must be rendered in order, depends on already rendered contents
    FlushCommandBuffers();
}

void OpenGLDrawingContext::DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    x1 += _offsetX;
    y1 += _offsetY;
    x2 += _offsetX;
    y2 += _offsetY;

    vec4f paletteColour = _engine->GLPalette[colour & 0xFF];

    DrawLineCommand command = {};

    command.colour = paletteColour;

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.pos[0] = x1;
    command.pos[1] = y1;
    command.pos[2] = x2;
    command.pos[3] = y2;

    _commandBuffers.lines.push_back(command);

    // Must be rendered in order right now, because it does not yet use depth
    FlushCommandBuffers();
}

void OpenGLDrawingContext::DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour)
{
    sint32 g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    if (_dpi->zoom_level != 0)
    {
        if (g1Element->flags & (1 << 4))
        {
            rct_drawpixelinfo zoomedDPI;
            zoomedDPI.bits = _dpi->bits;
            zoomedDPI.x = _dpi->x >> 1;
            zoomedDPI.y = _dpi->y >> 1;
            zoomedDPI.height = _dpi->height >> 1;
            zoomedDPI.width = _dpi->width >> 1;
            zoomedDPI.pitch = _dpi->pitch;
            zoomedDPI.zoom_level = _dpi->zoom_level - 1;
            SetDPI(&zoomedDPI);
            DrawSprite((image & 0xE0000000) | (g1Id - g1Element->zoomed_offset), x >> 1, y >> 1, tertiaryColour);
            return;
        }
        if (g1Element->flags & (1 << 5))
        {
            return;
        }
    }

    uint8 zoomLevel = (1 << _dpi->zoom_level);

    sint32 drawOffsetX = g1Element->x_offset;
    sint32 drawOffsetY = g1Element->y_offset;
    sint32 drawWidth = g1Element->width;
    sint32 drawHeight = g1Element->height;

    sint32 left = x + drawOffsetX;
    sint32 top = y + drawOffsetY;

    sint32 zoom_mask = 0xFFFFFFFF << _dpi->zoom_level;
    if (_dpi->zoom_level && g1Element->flags & G1_FLAG_RLE_COMPRESSION){
        top -= ~zoom_mask;
    }

    if (!(g1Element->flags & G1_FLAG_RLE_COMPRESSION)) {
        top &= zoom_mask;
        left += ~zoom_mask;
    }

    left &= zoom_mask;

    sint32 right = left + drawWidth;
    sint32 bottom = top + drawHeight;

    if (_dpi->zoom_level && g1Element->flags & G1_FLAG_RLE_COMPRESSION) {
        bottom += top & ~zoom_mask;
    }

    if (left > right)
    {
        std::swap(left, right);
    }
    if (top > bottom)
    {
        std::swap(top, bottom);
    }

    left -= _dpi->x;
    top -= _dpi->y;
    right -= _dpi->x;
    bottom -= _dpi->y;

    left /= zoomLevel;
    top /= zoomLevel;
    right /= zoomLevel;
    bottom /= zoomLevel;

    left += _clipLeft;
    top += _clipTop;
    right += _clipLeft;
    bottom += _clipTop;

    DrawImageCommand command = {};

    command.flags = 0;
    command.mask = false;

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    auto texture = _textureCache->GetOrLoadImageTexture(image);
    command.texColour = texture;

    bool special = false;
    if (!(image & IMAGE_TYPE_REMAP_2_PLUS) && (image & IMAGE_TYPE_REMAP)) {
        if (((image >> 19) & 0x7F) == 32) {
            special = true;
        }
    }

    if (!((image & IMAGE_TYPE_REMAP_2_PLUS) && !(image & IMAGE_TYPE_REMAP))) {
        tertiaryColour = 0;
    }

    texture = _textureCache->GetOrLoadPaletteTexture(image, tertiaryColour, special);
    command.texPalette = texture;

    if (image & IMAGE_TYPE_TRANSPARENT) {
        command.flags |= (1 << 3);
    }
    else if (image & (IMAGE_TYPE_REMAP_2_PLUS | IMAGE_TYPE_REMAP)){
        command.flags |= (1 << 1);
    }
    command.flags |= special << 2;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right;
    command.bounds[3] = bottom;

    _commandBuffers.images.push_back(command);
}

void OpenGLDrawingContext::DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage)
{
    rct_g1_element * g1ElementMask = gfx_get_g1_element(maskImage & 0x7FFFF);
    rct_g1_element * g1ElementColour = gfx_get_g1_element(colourImage & 0x7FFFF);

    auto textureMask = _textureCache->GetOrLoadImageTexture(maskImage);
    auto textureColour = _textureCache->GetOrLoadImageTexture(colourImage);

    uint8 zoomLevel = (1 << _dpi->zoom_level);

    sint32 drawOffsetX = g1ElementMask->x_offset;
    sint32 drawOffsetY = g1ElementMask->y_offset;
    sint32 drawWidth = Math::Min(g1ElementMask->width, g1ElementColour->width);
    sint32 drawHeight = Math::Min(g1ElementMask->height, g1ElementColour->height);

    sint32 left = x + drawOffsetX;
    sint32 top = y + drawOffsetY;
    sint32 right = left + drawWidth;
    sint32 bottom = top + drawHeight;

    if (left > right)
    {
        std::swap(left, right);
    }
    if (top > bottom)
    {
        std::swap(top, bottom);
    }

    left -= _dpi->x;
    top -= _dpi->y;
    right -= _dpi->x;
    bottom -= _dpi->y;

    left /= zoomLevel;
    top /= zoomLevel;
    right /= zoomLevel;
    bottom /= zoomLevel;

    left += _clipLeft;
    top += _clipTop;
    right += _clipLeft;
    bottom += _clipTop;

    DrawImageCommand command = {};

    command.mask = true;

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.texMask = textureMask;
    command.texColour = textureColour;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right;
    command.bounds[3] = bottom;

    _commandBuffers.images.push_back(command);
}

void OpenGLDrawingContext::DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)
{
    vec4f paletteColour = _engine->GLPalette[colour & 0xFF];

    sint32 g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    auto texture = _textureCache->GetOrLoadImageTexture(image);

    sint32 drawOffsetX = g1Element->x_offset;
    sint32 drawOffsetY = g1Element->y_offset;
    sint32 drawWidth = (uint16)g1Element->width;
    sint32 drawHeight = (uint16)g1Element->height;

    sint32 left = x + drawOffsetX;
    sint32 top = y + drawOffsetY;
    sint32 right = left + drawWidth;
    sint32 bottom = top + drawHeight;

    if (left > right)
    {
        std::swap(left, right);
    }
    if (top > bottom)
    {
        std::swap(top, bottom);
    }

    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawImageCommand command = {};

    command.flags = 1;
    command.mask = false;
    command.colour = paletteColour;

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.texColour = texture;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right;
    command.bounds[3] = bottom;

    _commandBuffers.images.push_back(command);
}

void OpenGLDrawingContext::DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette)
{
    sint32 g1Id = image & 0x7FFFF;
    rct_g1_element * g1Element = gfx_get_g1_element(g1Id);

    auto texture = _textureCache->GetOrLoadGlyphTexture(image, palette);

    sint32 drawOffsetX = g1Element->x_offset;
    sint32 drawOffsetY = g1Element->y_offset;
    sint32 drawWidth = (uint16)g1Element->width;
    sint32 drawHeight = (uint16)g1Element->height;

    sint32 left = x + drawOffsetX;
    sint32 top = y + drawOffsetY;
    sint32 right = left + drawWidth;
    sint32 bottom = top + drawHeight;

    if (left > right)
    {
        std::swap(left, right);
    }
    if (top > bottom)
    {
        std::swap(top, bottom);
    }

    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawImageCommand command = {};

    command.flags = 0;
    command.mask = false;

    command.clip[0] = _clipLeft;
    command.clip[1] = _clipTop;
    command.clip[2] = _clipRight;
    command.clip[3] = _clipBottom;

    command.texColour = texture;

    command.bounds[0] = left;
    command.bounds[1] = top;
    command.bounds[2] = right;
    command.bounds[3] = bottom;

    _commandBuffers.images.push_back(command);
}

void OpenGLDrawingContext::FlushCommandBuffers()
{
    FlushRectangles();
    FlushLines();

    FlushImages();
}

void OpenGLDrawingContext::FlushRectangles()
{
    for (const auto& command : _commandBuffers.rectangles)
    {
        _fillRectShader->Use();
        _fillRectShader->SetFlags(command.flags);
        _fillRectShader->SetSourceFramebuffer(command.sourceFramebuffer);
        _fillRectShader->SetColour(0, command.colours[0]);
        _fillRectShader->SetColour(1, command.colours[1]);
        _fillRectShader->SetClip(command.clip[0], command.clip[1], command.clip[2], command.clip[3]);
        _fillRectShader->Draw(command.bounds[0], command.bounds[1], command.bounds[2], command.bounds[3]);
    }

    _commandBuffers.rectangles.clear();
}

void OpenGLDrawingContext::FlushLines() {
    for (const auto& command : _commandBuffers.lines)
    {
        _drawLineShader->Use();
        _drawLineShader->SetColour(command.colour);
        _drawLineShader->SetClip(command.clip[0], command.clip[1], command.clip[2], command.clip[3]);
        _drawLineShader->Draw(command.pos[0], command.pos[1], command.pos[2], command.pos[3]);
    }

    _commandBuffers.lines.clear();
}

void OpenGLDrawingContext::FlushImages()
{
    if (_commandBuffers.images.size() == 0) return;

    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());

    std::vector<DrawImageInstance> instances;
    instances.reserve(_commandBuffers.images.size());

    for (const auto& command : _commandBuffers.images)
    {
        DrawImageInstance instance;

        instance.clip = {command.clip[0], command.clip[1], command.clip[2], command.clip[3]};
        instance.texColourAtlas = command.texColour.index;
        instance.texColourBounds = command.texColour.normalizedBounds;
        instance.texMaskAtlas = command.texMask.index;
        instance.texMaskBounds = command.texMask.normalizedBounds;
        instance.flags = command.flags;
        instance.colour = command.colour;
        instance.bounds = {command.bounds[0], command.bounds[1], command.bounds[2], command.bounds[3]};
        instance.mask = command.mask;
        instance.texPaletteAtlas = command.texPalette.index;
        if (instance.flags != 0) {
            instance.texPaletteBounds = {
                command.texPalette.normalizedBounds.x,
                command.texPalette.normalizedBounds.y,
                (command.texPalette.normalizedBounds.z - command.texPalette.normalizedBounds.x) / (float)(command.texPalette.bounds.z - command.texPalette.bounds.x),
                (command.texPalette.normalizedBounds.w - command.texPalette.normalizedBounds.y) / (float)(command.texPalette.bounds.w - command.texPalette.bounds.y)
            };
        }

        instances.push_back(instance);
    }

    _drawImageShader->Use();
    _drawImageShader->DrawInstances(instances);

    _commandBuffers.images.clear();
}

void OpenGLDrawingContext::SetDPI(rct_drawpixelinfo * dpi)
{
    rct_drawpixelinfo * screenDPI = _engine->GetDPI();
#ifndef NDEBUG
    size_t bitsSize = (size_t)screenDPI->height * (size_t)(screenDPI->width + screenDPI->pitch);
#endif
    size_t bitsOffset = (size_t)(dpi->bits - screenDPI->bits);

    assert(bitsOffset < bitsSize);

    _clipLeft = (sint32)(bitsOffset % (screenDPI->width + screenDPI->pitch));
    _clipTop = (sint32)(bitsOffset / (screenDPI->width + screenDPI->pitch));

    _clipRight = _clipLeft + dpi->width;
    _clipBottom = _clipTop + dpi->height;
    _offsetX = _clipLeft - dpi->x;
    _offsetY = _clipTop - dpi->y;

    _dpi = dpi;
}

#endif /* DISABLE_OPENGL */
