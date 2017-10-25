#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#ifndef DISABLE_OPENGL

#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <SDL.h>

#include <openrct2/config/Config.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/Exception.hpp>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/Memory.hpp>
#include <openrct2/drawing/IDrawingContext.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/Rain.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/ui/UiContext.h>

#include <openrct2/interface/window.h>
#include <openrct2/intro.h>
#include <openrct2/drawing/drawing.h>
#include <openrct2/drawing/lightfx.h>

#include "../DrawingEngines.h"
#include "GLSLTypes.h"
#include "OpenGLAPI.h"
#include "OpenGLFramebuffer.h"
#include "CopyFramebufferShader.h"
#include "DrawLineShader.h"
#include "DrawRectShader.h"
#include "SwapFramebuffer.h"
#include "TextureCache.h"
#include "DrawCommands.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

struct OpenGLVersion
{
    GLint Major;
    GLint Minor;
};

constexpr OpenGLVersion OPENGL_MINIMUM_REQUIRED_VERSION = { 3, 3 };

class OpenGLDrawingEngine;

class OpenGLDrawingContext final : public IDrawingContext
{
private:
    OpenGLDrawingEngine *   _engine = nullptr;
    rct_drawpixelinfo *     _dpi    = nullptr;

    ApplyTransparencyShader *   _applyTransparencyShader    = nullptr;
    DrawLineShader *            _drawLineShader             = nullptr;
    DrawRectShader *            _drawRectShader             = nullptr;
    SwapFramebuffer *           _swapFramebuffer            = nullptr;

    TextureCache * _textureCache = nullptr;

    sint32 _offsetX    = 0;
    sint32 _offsetY    = 0;
    sint32 _clipLeft   = 0;
    sint32 _clipTop    = 0;
    sint32 _clipRight  = 0;
    sint32 _clipBottom = 0;

    sint32 _drawCount  = 0;

    struct
    {
        LineCommandBatch lines;
        RectCommandBatch rects;
        RectCommandBatch transparent;
    }
    _commandBuffers;

public:
    explicit OpenGLDrawingContext(OpenGLDrawingEngine * engine);
    ~OpenGLDrawingContext() override;

    IDrawingEngine * GetEngine() override;
    TextureCache * GetTextureCache() const { return _textureCache; }
    const OpenGLFramebuffer & GetFinalFramebuffer() const { return _swapFramebuffer->GetFinalFramebuffer(); }

    void Initialise();
    void Resize(sint32 width, sint32 height);
    void ResetPalette();
    void StartNewDraw();

    void Clear(uint8 paletteIndex) override;
    void FillRect(uint32 colour, sint32 x, sint32 y, sint32 w, sint32 h) override;
    void FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom) override;
    void DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2) override;
    void DrawSprite(uint32 image, sint32 x, sint32 y, uint32 tertiaryColour) override;
    void DrawSpriteRawMasked(sint32 x, sint32 y, uint32 maskImage, uint32 colourImage) override;
    void DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour) override;
    void DrawGlyph(uint32 image, sint32 x, sint32 y, uint8 * palette) override;

    void FlushCommandBuffers();

    void FlushLines();
    void FlushRectangles();
    void HandleTransparency();

    void SetDPI(rct_drawpixelinfo * dpi);
    sint32 MaxTransparencyDepth();
};

class OpenGLDrawingEngine : public IDrawingEngine
{
private:
    IUiContext * const  _uiContext  = nullptr;
    SDL_Window *        _window     = nullptr;
    SDL_GLContext       _context    = nullptr;

    uint32  _width      = 0;
    uint32  _height     = 0;
    uint32  _pitch      = 0;
    size_t  _bitsSize   = 0;
    uint8 * _bits       = nullptr;

    rct_drawpixelinfo _bitsDPI  = { 0 };

    OpenGLDrawingContext *    _drawingContext;

    CopyFramebufferShader * _copyFramebufferShader  = nullptr;
    OpenGLFramebuffer *     _screenFramebuffer      = nullptr;
    OpenGLFramebuffer *     _scaleFramebuffer       = nullptr;

public:
    SDL_Color Palette[256];
    vec4      GLPalette[256];

    OpenGLDrawingEngine(IUiContext * uiContext)
        : _uiContext(uiContext)
    {
        _window = (SDL_Window *)_uiContext->GetWindow();
        _drawingContext = new OpenGLDrawingContext(this);
#ifdef __ENABLE_LIGHTFX__
        lightfx_set_available(false);
#endif
    }

    ~OpenGLDrawingEngine() override
    {
        delete _copyFramebufferShader;
        delete _screenFramebuffer;

        delete _drawingContext;
        delete [] _bits;

        SDL_GL_DeleteContext(_context);
    }

    void Initialise() override
    {
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

        _copyFramebufferShader = new CopyFramebufferShader();
    }

    void Resize(uint32 width, uint32 height) override
    {
        ConfigureBits(width, height, width);
        ConfigureCanvas();
        _drawingContext->Resize(width, height);
    }

    void SetPalette(const rct_palette_entry * palette) override
    {
        for (sint32 i = 0; i < 256; i++)
        {
            SDL_Color colour;
            colour.r = palette[i].red;
            colour.g = palette[i].green;
            colour.b = palette[i].blue;
            colour.a = i == 0 ? 0 : 255;

            Palette[i] = colour;
            GLPalette[i] = { colour.r / 255.0f,
                             colour.g / 255.0f,
                             colour.b / 255.0f,
                             colour.a / 255.0f };
        }

        _copyFramebufferShader->Use();
        _copyFramebufferShader->SetPalette(GLPalette);
        _drawingContext->ResetPalette();
    }

    void SetUncappedFrameRate(bool uncapped) override
    {
        SDL_GL_SetSwapInterval(uncapped ? 0 : 1);
    }

    void Invalidate(sint32 left, sint32 top, sint32 right, sint32 bottom) override
    {
    }

    void BeginDraw() override
    {
        assert(_screenFramebuffer != nullptr);

        _drawingContext->StartNewDraw();
    }

    void EndDraw() override
    {
        _drawingContext->FlushCommandBuffers();

        glDisable(GL_DEPTH_TEST);
        if (_scaleFramebuffer != nullptr)
        {
            // Render to intermediary RGB buffer for GL_LINEAR
            _scaleFramebuffer->Bind();
        }
        else
        {
            _screenFramebuffer->Bind();
        }

        _copyFramebufferShader->Use();
        _copyFramebufferShader->SetTexture(_drawingContext->GetFinalFramebuffer().GetTexture());
        _copyFramebufferShader->Draw();

        if (_scaleFramebuffer != nullptr)
        {
            _screenFramebuffer->Copy(*_scaleFramebuffer, GL_LINEAR);
        }

        CheckGLError();
        Display();
    }

    void PaintWindows() override
    {
        window_update_all_viewports();
        window_draw_all(&_bitsDPI, 0, 0, _width, _height);

        // TODO move this out from drawing
        window_update_all();
    }

    void PaintRain() override
    {
        // Not implemented
    }

    sint32 Screenshot() override
    {
        const OpenGLFramebuffer & framebuffer = _drawingContext->GetFinalFramebuffer();
        framebuffer.Bind();
        framebuffer.GetPixels(_bitsDPI);
        sint32 result = screenshot_dump_png(&_bitsDPI);
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

        if (_scaleFramebuffer != nullptr)
        {
            delete _scaleFramebuffer;
            _scaleFramebuffer = nullptr;
        }
        if (GetContext()->GetUiContext()->GetScaleQuality() > 0)
        {
            _scaleFramebuffer = new OpenGLFramebuffer(_width, _height, false, false);
        }
    }

    void Display()
    {
        SDL_GL_SwapWindow(_window);
    }
};

IDrawingEngine * OpenRCT2::Ui::CreateOpenGLDrawingEngine(IUiContext * uiContext)
{
    return new OpenGLDrawingEngine(uiContext);
}

OpenGLDrawingContext::OpenGLDrawingContext(OpenGLDrawingEngine * engine)
{
    _engine = engine;
}

OpenGLDrawingContext::~OpenGLDrawingContext()
{
    delete _applyTransparencyShader;
    delete _drawLineShader;
    delete _drawRectShader;
    delete _swapFramebuffer;

    delete _textureCache;
}

IDrawingEngine * OpenGLDrawingContext::GetEngine()
{
    return _engine;
}

void OpenGLDrawingContext::Initialise()
{
    _textureCache = new TextureCache();
    _applyTransparencyShader = new ApplyTransparencyShader();
    _drawRectShader = new DrawRectShader();
    _drawLineShader = new DrawLineShader();
}

void OpenGLDrawingContext::Resize(sint32 width, sint32 height)
{
    _commandBuffers.lines.clear();
    _commandBuffers.rects.clear();

    _drawRectShader->Use();
    _drawRectShader->SetScreenSize(width, height);
    _drawLineShader->Use();
    _drawLineShader->SetScreenSize(width, height);

    // Re-create canvas framebuffer
    delete _swapFramebuffer;
    _swapFramebuffer = new SwapFramebuffer(width, height);
}

void OpenGLDrawingContext::ResetPalette()
{
    //FlushCommandBuffers();
}

void OpenGLDrawingContext::StartNewDraw()
{
    _drawCount = 0;
    _swapFramebuffer->Clear();
}

void OpenGLDrawingContext::Clear(uint8 paletteIndex)
{
    FillRect(paletteIndex, _clipLeft - _offsetX, _clipTop - _offsetY, _clipRight - _offsetX, _clipBottom - _offsetY);
}

void OpenGLDrawingContext::FillRect(uint32 colour, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.colour = colour & 0xFF;
    command.bounds = { left, top, right + 1, bottom + 1 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE;
    command.depth = _drawCount++;

    if (colour & 0x1000000)
    {
        // cross-pattern
        command.flags = DrawRectCommand::FLAG_CROSS_HATCH;
    }
    else if (colour & 0x2000000)
    {
        assert(false);
        // Should be FilterRect
    }
}

void OpenGLDrawingContext::FilterRect(FILTER_PALETTE_ID palette, sint32 left, sint32 top, sint32 right, sint32 bottom)
{
    left += _offsetX;
    top += _offsetY;
    right += _offsetX;
    bottom += _offsetY;

    DrawRectCommand& command = _commandBuffers.transparent.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.colour = TextureCache::PaletteToY(palette);
    command.bounds = { left, top, right + 1, bottom + 1 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE;
    command.depth = _drawCount++;
}

void OpenGLDrawingContext::DrawLine(uint32 colour, sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
    x1 += _offsetX;
    y1 += _offsetY;
    x2 += _offsetX;
    y2 += _offsetY;

    DrawLineCommand& command = _commandBuffers.lines.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.bounds = { x1, y1, x2, y2 };
    command.colour = colour & 0xFF;
    command.depth = _drawCount++;
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
            DrawSprite((image & 0xFFF80000) | (g1Id - g1Element->zoomed_offset), x >> 1, y >> 1, tertiaryColour);
            return;
        }
        if (g1Element->flags & (1 << 5))
        {
            return;
        }
    }

    uint8 zoomLevel = (1 << _dpi->zoom_level);

    sint32 left = x + g1Element->x_offset;
    sint32 top = y + g1Element->y_offset;

    sint32 zoom_mask = 0xFFFFFFFF << _dpi->zoom_level;
    if (_dpi->zoom_level && g1Element->flags & G1_FLAG_RLE_COMPRESSION){
        top -= ~zoom_mask;
    }

    if (!(g1Element->flags & G1_FLAG_RLE_COMPRESSION)) {
        top &= zoom_mask;
        left += ~zoom_mask;
    }

    left &= zoom_mask;

    sint32 right = left + g1Element->width;
    sint32 bottom = top + g1Element->height;

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

    auto texture = _textureCache->GetOrLoadImageTexture(image);

    int paletteCount;
    ivec3 palettes{};
    bool special = false;
    if (image & IMAGE_TYPE_REMAP_2_PLUS)
    {
        palettes.x = TextureCache::PaletteToY((image >> 19) & 0x1F);
        palettes.y = TextureCache::PaletteToY((image >> 24) & 0x1F);
        if (image & IMAGE_TYPE_REMAP)
        {
            paletteCount = 2;
        }
        else
        {
            paletteCount = 3;
            palettes.z = TextureCache::PaletteToY(tertiaryColour & 0xFF);
        }
    }
    else if ((image & IMAGE_TYPE_REMAP) || (image & IMAGE_TYPE_TRANSPARENT))
    {
        paletteCount = 1;
        uint32 palette = (image >> 19) & 0xFF;
        palettes.x = TextureCache::PaletteToY(palette);
        if (palette == PALETTE_WATER)
        {
            special = true;
        }
    }
    else
    {
        paletteCount = 0;
    }

    if (special || (image & IMAGE_TYPE_TRANSPARENT))
    {
        DrawRectCommand& command = _commandBuffers.transparent.allocate();

        command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
        command.texColourAtlas = texture->index;
        command.texColourBounds = texture->normalizedBounds;
        command.texMaskAtlas = texture->index;
        command.texMaskBounds = texture->normalizedBounds;
        command.palettes = palettes;
        command.colour = palettes.x - (special ? 1 : 0);
        command.bounds = { left, top, right, bottom };
        command.flags = special ? 0 : DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK;
        command.depth = _drawCount++;
    }
    else
    {
        DrawRectCommand& command = _commandBuffers.rects.allocate();

        command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
        command.texColourAtlas = texture->index;
        command.texColourBounds = texture->normalizedBounds;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.palettes = palettes;
        command.colour = 0;
        command.bounds = { left, top, right, bottom };
        command.flags = paletteCount;
        command.depth = _drawCount++;
    }
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

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.texColourAtlas = textureColour->index;
    command.texColourBounds = textureColour->normalizedBounds;
    command.texMaskAtlas = textureMask->index;
    command.texMaskBounds = textureMask->normalizedBounds;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_MASK;
    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
}

void OpenGLDrawingContext::DrawSpriteSolid(uint32 image, sint32 x, sint32 y, uint8 colour)
{
    assert((colour & 0xFF) > 0u);

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

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = texture->index;
    command.texMaskBounds = texture->normalizedBounds;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK;
    command.colour = colour & 0xFF;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
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

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { _clipLeft, _clipTop, _clipRight, _clipBottom };
    command.texColourAtlas = texture.index;
    command.texColourBounds = texture.normalizedBounds;
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.flags = 0;
    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
}

void OpenGLDrawingContext::FlushCommandBuffers()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    _swapFramebuffer->BindOpaque();
    _drawRectShader->Use();
    _drawRectShader->DisablePeeling();

    FlushLines();
    FlushRectangles();

    HandleTransparency();
}

void OpenGLDrawingContext::FlushLines()
{
    if (_commandBuffers.lines.size() == 0) return;

    _drawLineShader->Use();
    _drawLineShader->DrawInstances(_commandBuffers.lines);

    _commandBuffers.lines.clear();
}

void OpenGLDrawingContext::FlushRectangles()
{
    if (_commandBuffers.rects.size() == 0) return;

    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());
    OpenGLAPI::SetTexture(1, GL_TEXTURE_RECTANGLE, _textureCache->GetPaletteTexture());

    _drawRectShader->Use();
    _drawRectShader->SetInstances(_commandBuffers.rects);
    _drawRectShader->DrawInstances();

    _commandBuffers.rects.clear();
}

void OpenGLDrawingContext::HandleTransparency()
{
    if (_commandBuffers.transparent.empty())
    {
        return;
    }

    _drawRectShader->Use();
    _drawRectShader->SetInstances(_commandBuffers.transparent);

    sint32 max_depth = MaxTransparencyDepth();
    for (sint32 i=0; i < max_depth; ++i)
    {
        _swapFramebuffer->BindTransparent();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        _drawRectShader->Use();

        if (i > 0)
        {
            _drawRectShader->EnablePeeling(_swapFramebuffer->GetBackDepthTexture());
        }

        OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());
        OpenGLAPI::SetTexture(1, GL_TEXTURE_RECTANGLE, _textureCache->GetPaletteTexture());

        _drawRectShader->Use();
        _drawRectShader->DrawInstances();
        _swapFramebuffer->ApplyTransparency(*_applyTransparencyShader, _textureCache->GetPaletteTexture());
    }

    _commandBuffers.transparent.clear();
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

    _clipRight = _clipLeft + (dpi->width >> dpi->zoom_level);
    _clipBottom = _clipTop + (dpi->height >> dpi->zoom_level);
    _offsetX = _clipLeft - dpi->x;
    _offsetY = _clipTop - dpi->y;

    _dpi = dpi;
}

sint32 OpenGLDrawingContext::MaxTransparencyDepth()
{
    sint32 max_depth = 1;

    struct xdata
    {
        sint32 xposition;
        bool begin;
        sint32 top, bottom;
    };
    std::vector<xdata> x_sweep;
    x_sweep.reserve(_commandBuffers.transparent.size() * 2);
    for (DrawRectCommand &command : _commandBuffers.transparent)
    {
        sint32 left = std::min(std::max(command.bounds.x, command.clip.x), command.clip.z);
        sint32 top = std::min(std::max(command.bounds.y, command.clip.y), command.clip.w);
        sint32 right = std::min(std::max(command.bounds.z, command.clip.x), command.clip.z);
        sint32 bottom = std::min(std::max(command.bounds.w, command.clip.y), command.clip.w);

        assert(left <= right);
        assert(top <= bottom);
        if (left == right) continue;
        if (top == bottom) continue;

        x_sweep.push_back({left, true, top, bottom});
        x_sweep.push_back({right, false, top, bottom});
    }
    std::sort(x_sweep.begin(), x_sweep.end(), [](const xdata &a, const xdata &b) -> bool {
        if (a.xposition != b.xposition) return a.xposition < b.xposition;
        else return !a.begin && b.begin;
    });

    struct ydata
    {
        sint32 count, depth;
    };
    std::map<sint32, ydata> y_intersect;
    for (const xdata &x : x_sweep)
    {
        assert(y_intersect.size() == 0 || y_intersect.begin()->second.depth == 0);
        if (x.begin)
        {
            auto top_in = y_intersect.insert({x.top, {1, 0}});
            auto top_it = top_in.first;
            if (top_in.second)
            {
                auto top_next = std::next(top_it);
                if (top_next != y_intersect.end())
                {
                    top_it->second.depth = top_next->second.depth;
                }
            }
            else
            {
                assert(top_it->second.count > 0);
                ++top_it->second.count;
            }

            auto bottom_in = y_intersect.insert({x.bottom, {1, 1}});
            auto bottom_it = bottom_in.first;
            if (bottom_in.second)
            {
                auto bottom_next = std::next(bottom_it);
                if (bottom_next != y_intersect.end())
                {
                    bottom_it->second.depth = bottom_next->second.depth + 1;
                }
            }
            else
            {
                assert(bottom_it->second.count > 0);
                ++bottom_it->second.count;
                max_depth = std::max(max_depth, ++bottom_it->second.depth);
            }

            for (auto it = std::next(top_it); it != bottom_it; ++it)
            {
                max_depth = std::max(max_depth, ++it->second.depth);
            }
        }
        else
        {
            auto top_it = y_intersect.find(x.top);
            assert(top_it != y_intersect.end());
            assert(top_it->second.count > 0);
            auto bottom_it = y_intersect.find(x.bottom);
            assert(bottom_it != y_intersect.end());
            assert(bottom_it->second.count > 0);

#ifndef NDEBUG
            if (top_it->second.count == 1)
            {
                auto top_next = std::next(top_it);
                assert(top_next != y_intersect.end() &&
                       top_it->second.depth == top_next->second.depth - 1);
            }

            if (bottom_it->second.count == 1)
            {
                auto bottom_next = std::next(bottom_it);
                assert(bottom_next == y_intersect.end() ?
                       bottom_it->second.depth == 1 :
                       bottom_it->second.depth == bottom_next->second.depth + 1);
            }
#endif /* NDEBUG */

            for (auto it = std::next(top_it); it != bottom_it; ++it)
            {
                assert(it->second.depth > 0);
                --it->second.depth;
            }

            if (top_it->second.count == 1)
            {
                y_intersect.erase(top_it);
            }
            else
            {
                --top_it->second.count;
            }

            if (bottom_it->second.count == 1)
            {
                y_intersect.erase(bottom_it);
            }
            else
            {
                assert(bottom_it->second.depth > 0);
                --bottom_it->second.count;
                --bottom_it->second.depth;
            }
        }
    }

    return max_depth;
}

#endif /* DISABLE_OPENGL */
