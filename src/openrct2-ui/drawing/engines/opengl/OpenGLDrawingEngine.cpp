/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/IDrawingContext.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/LightFX.h>
#include <openrct2/drawing/Rain.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/Intro.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/ui/UiContext.h>
#include "../DrawingEngineFactory.hpp"
#include "ApplyPaletteShader.h"
#include "DrawCommands.h"
#include "DrawLineShader.h"
#include "DrawRectShader.h"
#include "GLSLTypes.h"
#include "OpenGLAPI.h"
#include "OpenGLFramebuffer.h"
#include "SwapFramebuffer.h"
#include "TextureCache.h"
#include "TransparencyDepth.h"

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

    int32_t _offsetX    = 0;
    int32_t _offsetY    = 0;
    int32_t _clipLeft   = 0;
    int32_t _clipTop    = 0;
    int32_t _clipRight  = 0;
    int32_t _clipBottom = 0;

    int32_t _drawCount  = 0;

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
    void Resize(int32_t width, int32_t height);
    void ResetPalette();
    void StartNewDraw();

    void Clear(uint8_t paletteIndex) override;
    void FillRect(uint32_t colour, int32_t x, int32_t y, int32_t w, int32_t h) override;
    void FilterRect(FILTER_PALETTE_ID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) override;
    void DrawLine(uint32_t colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2) override;
    void DrawSprite(uint32_t image, int32_t x, int32_t y, uint32_t tertiaryColour) override;
    void DrawSpriteRawMasked(int32_t x, int32_t y, uint32_t maskImage, uint32_t colourImage) override;
    void DrawSpriteSolid(uint32_t image, int32_t x, int32_t y, uint8_t colour) override;
    void DrawGlyph(uint32_t image, int32_t x, int32_t y, uint8_t * palette) override;

    void FlushCommandBuffers();

    void FlushLines();
    void FlushRectangles();
    void HandleTransparency();

    void SetDPI(rct_drawpixelinfo * dpi);
};

class OpenGLDrawingEngine : public IDrawingEngine
{
private:
    std::shared_ptr<IUiContext> const _uiContext;
    SDL_Window *        _window     = nullptr;
    SDL_GLContext       _context    = nullptr;

    uint32_t  _width      = 0;
    uint32_t  _height     = 0;
    uint32_t  _pitch      = 0;
    size_t  _bitsSize   = 0;
    uint8_t * _bits       = nullptr;

    rct_drawpixelinfo _bitsDPI  = {};

    OpenGLDrawingContext *    _drawingContext;

    ApplyPaletteShader *    _applyPaletteShader     = nullptr;
    OpenGLFramebuffer *     _screenFramebuffer      = nullptr;
    OpenGLFramebuffer *     _scaleFramebuffer       = nullptr;
    OpenGLFramebuffer *     _smoothScaleFramebuffer = nullptr;

public:
    SDL_Color Palette[256];
    vec4      GLPalette[256];

    explicit OpenGLDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
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
        delete _applyPaletteShader;
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
            throw std::runtime_error(std::string(szRequiredVersion) + std::string(" not available."));
        }
        SDL_GL_MakeCurrent(_window, _context);

        if (!OpenGLAPI::Initialise())
        {
            throw std::runtime_error("Unable to initialise OpenGL.");
        }

        _drawingContext->Initialise();

        _applyPaletteShader = new ApplyPaletteShader();
    }

    void Resize(uint32_t width, uint32_t height) override
    {
        ConfigureBits(width, height, width);
        ConfigureCanvas();
        _drawingContext->Resize(width, height);
    }

    void SetPalette(const rct_palette_entry * palette) override
    {
        for (int32_t i = 0; i < 256; i++)
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

        _applyPaletteShader->Use();
        _applyPaletteShader->SetPalette(GLPalette);
        _drawingContext->ResetPalette();
    }

    void SetVSync(bool vsync) override
    {
        SDL_GL_SetSwapInterval(vsync);
    }

    void Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom) override
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

        _applyPaletteShader->Use();
        _applyPaletteShader->SetTexture(_drawingContext->GetFinalFramebuffer().GetTexture());
        _applyPaletteShader->Draw();

        if (_smoothScaleFramebuffer != nullptr)
        {
            _smoothScaleFramebuffer->Copy(*_scaleFramebuffer, GL_NEAREST);
            _screenFramebuffer->Copy(*_smoothScaleFramebuffer, GL_LINEAR);
        }
        else if (_scaleFramebuffer != nullptr)
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

    int32_t Screenshot() override
    {
        const OpenGLFramebuffer & framebuffer = _drawingContext->GetFinalFramebuffer();
        framebuffer.Bind();
        framebuffer.GetPixels(_bitsDPI);
        int32_t result = screenshot_dump_png(&_bitsDPI);
        return result;
    }

    void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override
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

    void InvalidateImage(uint32_t image) override
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

    void ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch)
    {
        size_t  newBitsSize = pitch * height;
        uint8_t * newBits = new uint8_t[newBitsSize];
        if (_bits == nullptr)
        {
            std::fill_n(newBits, newBitsSize, 0);
        }
        else
        {
            if (_pitch == pitch)
            {
                std::copy_n(_bits, std::min(_bitsSize, newBitsSize), newBits);
            }
            else
            {
                uint8_t * src = _bits;
                uint8_t * dst = newBits;

                uint32_t minWidth = std::min(_width, width);
                uint32_t minHeight = std::min(_height, height);
                for (uint32_t y = 0; y < minHeight; y++)
                {
                    std::copy_n(src, minWidth, dst);
                    if (pitch - minWidth > 0)
                    {
                        std::fill_n(dst + minWidth, pitch - minWidth, 0);
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
        if (_smoothScaleFramebuffer != nullptr)
        {
            delete _smoothScaleFramebuffer;
            _smoothScaleFramebuffer = nullptr;
        }
        if (GetContext()->GetUiContext()->GetScaleQuality() > 0)
        {
            _scaleFramebuffer = new OpenGLFramebuffer(_width, _height, false, false);
        }
        if (GetContext()->GetUiContext()->GetScaleQuality() == SCALE_QUALITY_SMOOTH_NN)
        {
            uint32_t scale = std::ceil(gConfigGeneral.window_scale);
            _smoothScaleFramebuffer = new OpenGLFramebuffer(_width * scale, _height * scale, false, false);
        }
    }

    void Display()
    {
        SDL_GL_SwapWindow(_window);
    }
};

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateOpenGLDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
{
    return std::make_unique<OpenGLDrawingEngine>(uiContext);
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

void OpenGLDrawingContext::Resize(int32_t width, int32_t height)
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

void OpenGLDrawingContext::Clear(uint8_t paletteIndex)
{
    FillRect(paletteIndex, _clipLeft - _offsetX, _clipTop - _offsetY, _clipRight - _offsetX, _clipBottom - _offsetY);
}

void OpenGLDrawingContext::FillRect(uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom)
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

void OpenGLDrawingContext::FilterRect(FILTER_PALETTE_ID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
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

void OpenGLDrawingContext::DrawLine(uint32_t colour, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
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

void OpenGLDrawingContext::DrawSprite(uint32_t image, int32_t x, int32_t y, uint32_t tertiaryColour)
{
    int32_t g1Id = image & 0x7FFFF;
    auto g1Element = gfx_get_g1_element(g1Id);
    if (g1Element == nullptr)
    {
        return;
    }

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

    uint8_t zoomLevel = (1 << _dpi->zoom_level);

    int32_t left = x + g1Element->x_offset;
    int32_t top = y + g1Element->y_offset;

    int32_t zoom_mask = 0xFFFFFFFF << _dpi->zoom_level;
    if (_dpi->zoom_level && g1Element->flags & G1_FLAG_RLE_COMPRESSION){
        top -= ~zoom_mask;
    }

    if (!(g1Element->flags & G1_FLAG_RLE_COMPRESSION)) {
        top &= zoom_mask;
        left += ~zoom_mask;
    }

    left &= zoom_mask;

    int32_t right = left + g1Element->width;
    int32_t bottom = top + g1Element->height;

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

    const auto texture = _textureCache->GetOrLoadImageTexture(image);

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
        uint32_t palette = (image >> 19) & 0xFF;
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
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.normalizedBounds;
        command.texMaskAtlas = texture.index;
        command.texMaskBounds = texture.normalizedBounds;
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
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.normalizedBounds;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.palettes = palettes;
        command.colour = 0;
        command.bounds = { left, top, right, bottom };
        command.flags = paletteCount;
        command.depth = _drawCount++;
    }
}

void OpenGLDrawingContext::DrawSpriteRawMasked(int32_t x, int32_t y, uint32_t maskImage, uint32_t colourImage)
{
    auto g1ElementMask = gfx_get_g1_element(maskImage & 0x7FFFF);
    auto g1ElementColour = gfx_get_g1_element(colourImage & 0x7FFFF);
    if (g1ElementMask == nullptr || g1ElementColour == nullptr)
    {
        return;
    }

    const auto textureMask = _textureCache->GetOrLoadImageTexture(maskImage);
    const auto textureColour = _textureCache->GetOrLoadImageTexture(colourImage);

    uint8_t zoomLevel = (1 << _dpi->zoom_level);

    int32_t drawOffsetX = g1ElementMask->x_offset;
    int32_t drawOffsetY = g1ElementMask->y_offset;
    int32_t drawWidth = std::min(g1ElementMask->width, g1ElementColour->width);
    int32_t drawHeight = std::min(g1ElementMask->height, g1ElementColour->height);

    int32_t left = x + drawOffsetX;
    int32_t top = y + drawOffsetY;
    int32_t right = left + drawWidth;
    int32_t bottom = top + drawHeight;

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
    command.texColourAtlas = textureColour.index;
    command.texColourBounds = textureColour.normalizedBounds;
    command.texMaskAtlas = textureMask.index;
    command.texMaskBounds = textureMask.normalizedBounds;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_MASK;
    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
}

void OpenGLDrawingContext::DrawSpriteSolid(uint32_t image, int32_t x, int32_t y, uint8_t colour)
{
    assert((colour & 0xFF) > 0u);

    int32_t g1Id = image & 0x7FFFF;
    auto g1Element = gfx_get_g1_element(g1Id);
    if (g1Element == nullptr)
    {
        return;
    }

    const auto texture = _textureCache->GetOrLoadImageTexture(image);

    int32_t drawOffsetX = g1Element->x_offset;
    int32_t drawOffsetY = g1Element->y_offset;
    int32_t drawWidth = (uint16_t)g1Element->width;
    int32_t drawHeight = (uint16_t)g1Element->height;

    int32_t left = x + drawOffsetX;
    int32_t top = y + drawOffsetY;
    int32_t right = left + drawWidth;
    int32_t bottom = top + drawHeight;

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
    command.texMaskAtlas = texture.index;
    command.texMaskBounds = texture.normalizedBounds;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK;
    command.colour = colour & 0xFF;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
}

void OpenGLDrawingContext::DrawGlyph(uint32_t image, int32_t x, int32_t y, uint8_t * palette)
{
    auto g1Element = gfx_get_g1_element(image & 0x7FFFF);
    if (g1Element == nullptr)
    {
        return;
    }

    const auto texture = _textureCache->GetOrLoadGlyphTexture(image, palette);

    int32_t drawOffsetX = g1Element->x_offset;
    int32_t drawOffsetY = g1Element->y_offset;
    int32_t drawWidth = (uint16_t)g1Element->width;
    int32_t drawHeight = (uint16_t)g1Element->height;

    int32_t left = x + drawOffsetX;
    int32_t top = y + drawOffsetY;
    int32_t right = left + drawWidth;
    int32_t bottom = top + drawHeight;

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

    int32_t max_depth = MaxTransparencyDepth(_commandBuffers.transparent);
    for (int32_t i=0; i < max_depth; ++i)
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

    _clipLeft = (int32_t)(bitsOffset % (screenDPI->width + screenDPI->pitch));
    _clipTop = (int32_t)(bitsOffset / (screenDPI->width + screenDPI->pitch));

    _clipRight = _clipLeft + (dpi->width >> dpi->zoom_level);
    _clipBottom = _clipTop + (dpi->height >> dpi->zoom_level);
    _offsetX = _clipLeft - dpi->x;
    _offsetY = _clipTop - dpi->y;

    _dpi = dpi;
}

#endif /* DISABLE_OPENGL */
