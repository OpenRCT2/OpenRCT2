/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_OPENGL

    #include "../DrawingEngineFactory.hpp"
    #include "ApplyPaletteShader.h"
    #include "CopyRectShader.h"
    #include "DrawCommands.h"
    #include "DrawLineShader.h"
    #include "DrawRectShader.h"
    #include "GLSLTypes.h"
    #include "OpenGLAPI.h"
    #include "OpenGLFramebuffer.h"
    #include "SwapFramebuffer.h"
    #include "TextureCache.h"
    #include "TransparencyDepth.h"

    #include <SDL.h>
    #include <algorithm>
    #include <cassert>
    #include <cmath>
    #include <openrct2-ui/interface/Window.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/core/Console.hpp>
    #include <openrct2/drawing/Drawing.String.h>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/drawing/IDrawingContext.h>
    #include <openrct2/drawing/IDrawingEngine.h>
    #include <openrct2/drawing/InvalidationGrid.h>
    #include <openrct2/drawing/LightFX.h>
    #include <openrct2/drawing/Weather.h>
    #include <openrct2/interface/Screenshot.h>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/world/Climate.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

struct OpenGLVersion
{
    GLint Major;
    GLint Minor;
};

constexpr OpenGLVersion kOpenGLMinimumRequiredVersion = { 3, 3 };

constexpr uint8_t kCSInside = 0b0000;
constexpr uint8_t kCSLeft = 0b0001;
constexpr uint8_t kCSRight = 0b0010;
constexpr uint8_t kCSTop = 0b0100;
constexpr uint8_t kCSBottom = 0b1000;

class OpenGLDrawingEngine;

class OpenGLDrawingContext final : public IDrawingContext
{
private:
    OpenGLDrawingEngine& _engine;
    std::unique_ptr<ApplyTransparencyShader> _applyTransparencyShader;
    std::unique_ptr<DrawLineShader> _drawLineShader;
    std::unique_ptr<DrawRectShader> _drawRectShader;
    std::unique_ptr<SwapFramebuffer> _swapFramebuffer;

    std::unique_ptr<TextureCache> _textureCache;

    int32_t _drawCount = 0;

    #ifndef DISABLE_TTF
    uint32_t _ttfGlId = 0;
    #endif

    bool _inDraw = false;

    struct
    {
        LineCommandBatch lines;
        RectCommandBatch rects;
        RectCommandBatch transparent;
    } _commandBuffers;

    static uint8_t ComputeOutCode(ScreenCoordsXY, ScreenCoordsXY, ScreenCoordsXY);
    static bool CohenSutherlandLineClip(ScreenLine&, const RenderTarget&);
    [[nodiscard]] ScreenRect CalculateClipping(const RenderTarget& rt) const;

public:
    explicit OpenGLDrawingContext(OpenGLDrawingEngine& engine);
    ~OpenGLDrawingContext() override;

    TextureCache* GetTextureCache() const
    {
        return _textureCache.get();
    }

    const OpenGLFramebuffer& GetFinalFramebuffer() const
    {
        return _swapFramebuffer->GetFinalFramebuffer();
    }

    OpenGLFramebuffer& GetFinalFramebuffer()
    {
        return _swapFramebuffer->GetFinalFramebuffer();
    }

    void Initialise();
    void Resize(int32_t width, int32_t height);
    void ResetPalette();
    void StartNewDraw();
    void FinishDraw();

    void Clear(RenderTarget& rt, PaletteIndex paletteIndex) override;
    void FillRect(
        RenderTarget& rt, PaletteIndex paletteIndex, int32_t x, int32_t y, int32_t w, int32_t h,
        bool crossHatch = false) override;
    void FilterRect(
        RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) override;
    void DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line) override;
    void DrawSprite(RenderTarget& rt, ImageId imageId, int32_t x, int32_t y) override;
    void DrawSpriteRawMasked(RenderTarget& rt, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage) override;
    void DrawSpriteSolid(RenderTarget& rt, ImageId image, int32_t x, int32_t y, PaletteIndex colour) override;
    void DrawGlyph(RenderTarget& rt, ImageId image, int32_t x, int32_t y, const PaletteMap& palette) override;
    void DrawTTFBitmap(
        RenderTarget& rt, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold) override;

    void FlushCommandBuffers();

    bool IsActive() const
    {
        return _inDraw;
    }

private:
    void FlushLines();
    void FlushRectangles();
    void HandleTransparency();
};

class OpenGLWeatherDrawer final : public IWeatherDrawer
{
    OpenGLDrawingContext* _drawingContext;

public:
    explicit OpenGLWeatherDrawer(OpenGLDrawingContext* drawingContext)
        : _drawingContext(drawingContext)
    {
    }

    virtual void Draw(
        RenderTarget& rt, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
        const uint8_t* weatherpattern) override
    {
        const uint8_t* pattern = weatherpattern;
        auto patternXSpace = *pattern++;
        auto patternYSpace = *pattern++;

        uint8_t patternStartXOffset = xStart % patternXSpace;
        uint8_t patternStartYOffset = yStart % patternYSpace;

        uint32_t pixelOffset = rt.LineStride() * y + x;
        uint8_t patternYPos = patternStartYOffset % patternYSpace;

        for (; height != 0; height--)
        {
            auto patternX = pattern[patternYPos * 2];
            if (patternX != 0xFF)
            {
                uint32_t finalPixelOffset = width + pixelOffset;

                uint32_t xPixelOffset = pixelOffset;
                xPixelOffset += (static_cast<uint8_t>(patternX - patternStartXOffset)) % patternXSpace;

                auto patternPixel = static_cast<PaletteIndex>(pattern[patternYPos * 2 + 1]);
                for (; xPixelOffset < finalPixelOffset; xPixelOffset += patternXSpace)
                {
                    int32_t pixelX = xPixelOffset % rt.width;
                    int32_t pixelY = (xPixelOffset / rt.width) % rt.height;

                    _drawingContext->DrawLine(rt, patternPixel, { { pixelX, pixelY }, { pixelX + 1, pixelY + 1 } });
                }
            }

            pixelOffset += rt.LineStride();
            patternYPos++;
            patternYPos %= patternYSpace;
        }
    }
};

class OpenGLDrawingEngine final : public IDrawingEngine
{
private:
    IUiContext& _uiContext;
    SDL_Window* _window = nullptr;
    SDL_GLContext _context = nullptr;

    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _pitch = 0;
    size_t _bitsSize = 0;
    std::unique_ptr<uint8_t[]> _bits;

    RenderTarget _mainRT = {};

    std::unique_ptr<OpenGLDrawingContext> _drawingContext;

    std::unique_ptr<ApplyPaletteShader> _applyPaletteShader;
    std::unique_ptr<CopyRectShader> _copyRectShader;

    std::unique_ptr<OpenGLFramebuffer> _screenFramebuffer;
    std::unique_ptr<OpenGLFramebuffer> _scaleFramebuffer;
    std::unique_ptr<OpenGLFramebuffer> _smoothScaleFramebuffer;
    std::unique_ptr<OpenGLFramebuffer> _tempFramebuffer;
    OpenGLWeatherDrawer _weatherDrawer;
    InvalidationGrid _invalidationGrid;

public:
    SDL_Color Palette[256];
    vec4 GLPalette[256];

    explicit OpenGLDrawingEngine(IUiContext& uiContext)
        : _uiContext(uiContext)
        , _drawingContext(std::make_unique<OpenGLDrawingContext>(*this))
        , _weatherDrawer(_drawingContext.get())
    {
        _window = static_cast<SDL_Window*>(_uiContext.GetWindow());
        _mainRT.DrawingEngine = this;
        LightFx::SetAvailable(false);
    }

    ~OpenGLDrawingEngine() override
    {
        SDL_GL_DeleteContext(_context);
    }

    void Initialise() override
    {
        OpenGLVersion requiredVersion = kOpenGLMinimumRequiredVersion;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, requiredVersion.Major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, requiredVersion.Minor);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

        _applyPaletteShader = std::make_unique<ApplyPaletteShader>();
        _copyRectShader = std::make_unique<CopyRectShader>();
    }

    void Resize(uint32_t width, uint32_t height) override
    {
        ConfigureBits(width, height, width);
        ConfigureCanvas();
        ConfigureDirtyGrid();

        _drawingContext->Resize(width, height);
        _tempFramebuffer = std::make_unique<OpenGLFramebuffer>(width, height);

        _drawingContext->StartNewDraw();
        _drawingContext->Clear(_mainRT, PaletteIndex::pi10);
        _drawingContext->FlushCommandBuffers();
        _drawingContext->FinishDraw();
    }

    void ConfigureDirtyGrid()
    {
        const auto blockWidth = 1u << 8;
        const auto blockHeight = 1u << 8;

        _invalidationGrid.reset(_width, _height, blockWidth, blockHeight);
    }

    void SetPalette(const GamePalette& palette) override
    {
        for (int32_t i = 0; i < 256; i++)
        {
            SDL_Color colour;
            colour.r = palette[i].red;
            colour.g = palette[i].green;
            colour.b = palette[i].blue;
            colour.a = i == 0 ? 0 : 255;

            Palette[i] = colour;
            GLPalette[i] = {
                colour.r / 255.0f,
                colour.g / 255.0f,
                colour.b / 255.0f,
                colour.a / 255.0f,
            };
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
        _invalidationGrid.invalidate(left, top, right, bottom);
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

        _drawingContext->FinishDraw();

        Display();
    }

    void PaintWindows() override
    {
        if (ClimateHasWeatherEffect() || gPaintForceRedraw)
        {
            WindowUpdateAllViewports();
            // OpenGL doesn't support restoring pixels, always redraw.
            // TODO: Render the weather to a texture and use that instead.
            WindowDrawAll(_mainRT, 0, 0, static_cast<int32_t>(_width), static_cast<int32_t>(_height));
        }
        else
        {
            // Redraw dirty regions before updating the viewports, otherwise
            // when viewports get panned, they copy dirty pixels
            DrawAllDirtyBlocks();
            WindowUpdateAllViewports();
            DrawAllDirtyBlocks();
        }
    }

    void DrawAllDirtyBlocks()
    {
        _invalidationGrid.traverseDirtyCells([this](int32_t left, int32_t top, int32_t right, int32_t bottom) {
            // Draw region
            DrawDirtyBlocks(left, top, right, bottom);
        });
    }

    void DrawDirtyBlocks(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        // Draw region
        WindowDrawAll(_mainRT, left, top, right, bottom);
    }

    void PaintWeather() override
    {
        DrawWeather(_mainRT, &_weatherDrawer);
    }

    std::string Screenshot() override
    {
        const OpenGLFramebuffer& framebuffer = _drawingContext->GetFinalFramebuffer();
        framebuffer.Bind();
        framebuffer.GetPixels(_mainRT);
        std::string result = ScreenshotDumpPNG(_mainRT);
        return result;
    }

    void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override
    {
        if (dx == 0 && dy == 0)
            return;

        _drawingContext->FlushCommandBuffers();

        auto& framebuffer = _drawingContext->GetFinalFramebuffer();

        const int32_t texWidth = static_cast<int32_t>(framebuffer.GetWidth());
        const int32_t texHeight = static_cast<int32_t>(framebuffer.GetHeight());

        // Adjust for move off screen
        int32_t lmargin = std::min(x - dx, 0);
        int32_t rmargin = std::min(texWidth - (x - dx + width), 0);
        int32_t tmargin = std::min(y - dy, 0);
        int32_t bmargin = std::min(texHeight - (y - dy + height), 0);
        x -= lmargin;
        y -= tmargin;
        width += lmargin + rmargin;
        height += tmargin + bmargin;

        if (width <= 0 || height <= 0)
            return;

    #ifndef __MACOSX__

        auto& tempBuffer = *_tempFramebuffer;

        const auto flipYAxis = [texHeight](int32_t yPos, int32_t h) {
            // Convert to OpenGL bottom-left origin coords.
            return texHeight - yPos - h;
        };

        int32_t srcX = x - dx;
        int32_t srcY = flipYAxis(y - dy, height);
        int32_t destX = x;
        int32_t destY = flipYAxis(y, height);

        glCall(glDisable, GL_BLEND);
        glCall(glDisable, GL_DEPTH_TEST);

        // First pass: Copy from main to temp
        GLuint mainTexture = framebuffer.GetTexture();
        tempBuffer.BindDraw();
        framebuffer.BindRead();
        glCall(glViewport, 0, 0, width, height);
        _copyRectShader->Use();
        _copyRectShader->SetTexture(mainTexture);
        _copyRectShader->SetSourceRect(srcX, srcY, width, height);
        _copyRectShader->SetTextureSize(texWidth, texHeight);
        _copyRectShader->Draw();

        // Second pass: Copy from temp to main
        GLuint tempTexture = tempBuffer.GetTexture();
        framebuffer.BindDraw();
        tempBuffer.BindRead();
        glCall(glViewport, destX, destY, width, height);
        _copyRectShader->Use();
        _copyRectShader->SetTexture(tempTexture);
        _copyRectShader->SetSourceRect(0, 0, width, height);
        _copyRectShader->SetTextureSize(texWidth, texHeight);
        _copyRectShader->Draw();

    #else

        // Slow but functional path for MacOS, OpenGL is deprecated on MacOS
        // and there seems to be issues that aren't present on other platforms.

        framebuffer.GetPixels(_mainRT);

        int32_t stride = _mainRT.LineStride();
        uint8_t* to = _mainRT.bits + y * stride + x;
        uint8_t* from = _mainRT.bits + (y - dy) * stride + x - dx;

        if (dy > 0)
        {
            // If positive dy, reverse directions
            to += (height - 1) * stride;
            from += (height - 1) * stride;
            stride = -stride;
        }

        // Move bytes
        for (int32_t i = 0; i < height; i++)
        {
            memmove(to, from, width);
            to += stride;
            from += stride;
        }

        framebuffer.SetPixels(_mainRT);

    #endif
    }

    IDrawingContext* GetDrawingContext() override
    {
        if (!_drawingContext->IsActive())
        {
            Guard::Fail("Drawing context is not active.");
            return nullptr;
        }
        return _drawingContext.get();
    }

    RenderTarget* getRT() override
    {
        return &_mainRT;
    }

    DrawingEngineFlags GetFlags() override
    {
        return DrawingEngineFlag::dirtyOptimisations;
    }

    void InvalidateImage(uint32_t image) override
    {
        _drawingContext->GetTextureCache()->InvalidateImage(image);
    }

private:
    static OpenGLVersion GetOpenGLVersion()
    {
        CheckGLError(); // Clear Any Errors
        OpenGLVersion version = { 0, 0 };
        glGetIntegerv(GL_MAJOR_VERSION, &version.Major);
        if (glGetError() != GL_NO_ERROR)
            return { 0, 0 };
        glGetIntegerv(GL_MINOR_VERSION, &version.Minor);
        if (glGetError() != GL_NO_ERROR)
            return { 0, 0 };
        return version;
    }

    void ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch)
    {
        size_t newBitsSize = pitch * height;

        auto newBits = std::make_unique<uint8_t[]>(newBitsSize);
        if (_bits == nullptr)
        {
            std::fill_n(newBits.get(), newBitsSize, 0);
        }
        else
        {
            if (_pitch == pitch)
            {
                std::copy_n(_bits.get(), std::min(_bitsSize, newBitsSize), newBits.get());
            }
            else
            {
                uint8_t* src = _bits.get();
                uint8_t* dst = newBits.get();

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
        }

        _bits = std::move(newBits);
        _bitsSize = newBitsSize;
        _width = width;
        _height = height;
        _pitch = pitch;

        RenderTarget* rt = &_mainRT;
        rt->bits = _bits.get();
        rt->x = 0;
        rt->y = 0;
        rt->width = width;
        rt->height = height;
        rt->pitch = _pitch - width;
    }

    void ConfigureCanvas()
    {
        // Re-create screen framebuffer
        _screenFramebuffer = std::make_unique<OpenGLFramebuffer>(_window);
        _smoothScaleFramebuffer.reset();
        _scaleFramebuffer.reset();
        if (GetContext()->GetUiContext().GetScaleQuality() != ScaleQuality::NearestNeighbour)
        {
            _scaleFramebuffer = std::make_unique<OpenGLFramebuffer>(_width, _height, false, false);
        }
        if (GetContext()->GetUiContext().GetScaleQuality() == ScaleQuality::SmoothNearestNeighbour)
        {
            uint32_t scale = std::ceil(Config::Get().general.windowScale);
            _smoothScaleFramebuffer = std::make_unique<OpenGLFramebuffer>(_width * scale, _height * scale, false, false);
        }
    }

    void Display()
    {
        SDL_GL_SwapWindow(_window);
    }
};

std::unique_ptr<IDrawingEngine> Ui::CreateOpenGLDrawingEngine(IUiContext& uiContext)
{
    return std::make_unique<OpenGLDrawingEngine>(uiContext);
}

OpenGLDrawingContext::OpenGLDrawingContext(OpenGLDrawingEngine& engine)
    : _engine(engine)
{
}

OpenGLDrawingContext::~OpenGLDrawingContext()
{
}

void OpenGLDrawingContext::Initialise()
{
    _textureCache = std::make_unique<TextureCache>();
    _applyTransparencyShader = std::make_unique<ApplyTransparencyShader>();
    _drawRectShader = std::make_unique<DrawRectShader>();
    _drawLineShader = std::make_unique<DrawLineShader>();
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
    _swapFramebuffer = std::make_unique<SwapFramebuffer>(width, height);
}

void OpenGLDrawingContext::ResetPalette()
{
    // FlushCommandBuffers();
}

void OpenGLDrawingContext::StartNewDraw()
{
    Guard::Assert(_inDraw == false);

    _drawCount = 0;
    _swapFramebuffer->Clear();
    _inDraw = true;
}

void OpenGLDrawingContext::FinishDraw()
{
    Guard::Assert(_inDraw == true);

    _inDraw = false;
}

void OpenGLDrawingContext::Clear(RenderTarget& rt, PaletteIndex paletteIndex)
{
    Guard::Assert(_inDraw == true);

    FillRect(rt, paletteIndex, rt.x, rt.y, rt.x + rt.width, rt.y + rt.height);
}

void OpenGLDrawingContext::FillRect(
    RenderTarget& rt, PaletteIndex paletteIndex, int32_t left, int32_t top, int32_t right, int32_t bottom, bool crossHatch)
{
    Guard::Assert(_inDraw == true);

    const ScreenRect clip = CalculateClipping(rt);

    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.colour = EnumValue(paletteIndex);
    command.bounds = { left, top, right + 1, bottom + 1 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE;
    command.depth = _drawCount++;
    command.zoom = 1.0f;

    if (crossHatch)
    {
        // cross-pattern
        command.flags |= DrawRectCommand::FLAG_CROSS_HATCH;
    }
}

void OpenGLDrawingContext::FilterRect(
    RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    Guard::Assert(_inDraw == true);

    const ScreenRect clip = CalculateClipping(rt);

    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.transparent.allocate();

    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.colour = TextureCache::PaletteToY(palette);
    command.bounds = { left, top, right + 1, bottom + 1 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE;
    command.depth = _drawCount++;
    command.zoom = 1.0f;
}

// Compute the bit code for a point p relative to the clip rectangle defined by topLeft and bottomRight
uint8_t OpenGLDrawingContext::ComputeOutCode(
    const ScreenCoordsXY p, const ScreenCoordsXY topLeft, const ScreenCoordsXY bottomRight)
{
    uint8_t code = kCSInside;

    if (p.x < topLeft.x)
        code |= kCSLeft;
    else if (p.x > bottomRight.x)
        code |= kCSRight;
    if (p.y < topLeft.y)
        code |= kCSTop;
    else if (p.y > bottomRight.y)
        code |= kCSBottom;

    return code;
}

// Trims the line to be within the bounds of the render target.
// based on: https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
bool OpenGLDrawingContext::CohenSutherlandLineClip(ScreenLine& line, const RenderTarget& rt)
{
    ScreenCoordsXY topLeft = { rt.x, rt.y };
    ScreenCoordsXY bottomRight = { rt.x + rt.width - 1, rt.y + rt.height - 1 };
    uint8_t outcode1 = ComputeOutCode(line.Point1, topLeft, bottomRight);
    uint8_t outcode2 = ComputeOutCode(line.Point2, topLeft, bottomRight);

    while (true)
    {
        if (outcode1 == kCSInside && outcode2 == kCSInside)
        {
            // both points inside render target
            return true;
        }
        if (outcode1 & outcode2)
        {
            // both points share an outside zone so both must be outside render target
            return false;
        }

        // At least one endpoint is outside the clip rectangle; pick it.
        uint8_t outcodeOut = outcode2 > outcode1 ? outcode2 : outcode1;
        ScreenCoordsXY clipped;

        // clang-format off
        if (outcodeOut & kCSBottom)
        {
            clipped.x = line.Point1.x + (line.Point2.x - line.Point1.x) *
                (bottomRight.y - line.Point1.y) / (line.Point2.y - line.Point1.y);
            clipped.y = bottomRight.y;
        }
        else if (outcodeOut & kCSTop)
        {
            clipped.x = line.Point1.x + (line.Point2.x - line.Point1.x) *
                (topLeft.y - line.Point1.y) / (line.Point2.y - line.Point1.y);
            clipped.y = topLeft.y;
        }
        else if (outcodeOut & kCSRight)
        {
            clipped.y = line.Point1.y + (line.Point2.y - line.Point1.y) *
                (bottomRight.x - line.Point1.x) / (line.Point2.x - line.Point1.x);
            clipped.x = bottomRight.x;
        }
        else if (outcodeOut & kCSLeft)
        {
            clipped.y = line.Point1.y + (line.Point2.y - line.Point1.y) *
                (topLeft.x - line.Point1.x) / (line.Point2.x - line.Point1.x);
            clipped.x = topLeft.x;
        }
        // clang-format on

        if (outcodeOut == outcode1)
        {
            line.Point1 = clipped;
            outcode1 = ComputeOutCode(line.Point1, topLeft, bottomRight);
        }
        else
        {
            line.Point2 = clipped;
            outcode2 = ComputeOutCode(line.Point2, topLeft, bottomRight);
        }
    }
}

void OpenGLDrawingContext::DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line)
{
    Guard::Assert(_inDraw == true);

    const ZoomLevel zoom = rt.zoom_level;
    ScreenLine trimmedLine = { { zoom.ApplyInversedTo(line.GetX1()), zoom.ApplyInversedTo(line.GetY1()) },
                               { zoom.ApplyInversedTo(line.GetX2()), zoom.ApplyInversedTo(line.GetY2()) } };
    if (!CohenSutherlandLineClip(trimmedLine, rt))
        return;

    DrawLineCommand& command = _commandBuffers.lines.allocate();

    const ScreenRect clip = CalculateClipping(rt);
    const int32_t x1 = trimmedLine.GetX1() - rt.x + clip.GetLeft();
    const int32_t y1 = trimmedLine.GetY1() - rt.y + clip.GetTop();
    const int32_t x2 = trimmedLine.GetX2() - rt.x + clip.GetLeft();
    const int32_t y2 = trimmedLine.GetY2() - rt.y + clip.GetTop();

    command.bounds = { x1, y1, x2, y2 };
    command.colour = static_cast<GLuint>(colour);
    command.depth = _drawCount++;
}

static auto EuclideanRemainder(const auto a, const auto b)
{
    const auto r = a % b;
    return r >= 0 ? r : r + b;
};

void OpenGLDrawingContext::DrawSprite(RenderTarget& rt, const ImageId imageId, const int32_t x, const int32_t y)
{
    Guard::Assert(_inDraw == true);

    auto g1Element = GfxGetG1Element(imageId);
    if (g1Element == nullptr)
    {
        return;
    }

    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        if (g1Element->flags.has(G1Flag::hasZoomSprite))
        {
            RenderTarget zoomedRT;
            zoomedRT.bits = rt.bits;
            zoomedRT.x = rt.x;
            zoomedRT.y = rt.y;
            zoomedRT.height = rt.height;
            zoomedRT.width = rt.width;
            zoomedRT.pitch = rt.pitch;
            zoomedRT.zoom_level = rt.zoom_level - 1;
            DrawSprite(zoomedRT, imageId.WithIndex(imageId.GetIndex() - g1Element->zoomedOffset), x >> 1, y >> 1);
            return;
        }
        if (g1Element->flags.has(G1Flag::noZoomDraw))
        {
            return;
        }
    }

    auto texture = _textureCache->GetOrLoadImageTexture(imageId);

    int32_t left = x + g1Element->xOffset;
    int32_t top = y + g1Element->yOffset;

    int32_t xModifier = 0;
    int32_t yModifier = 0;
    int32_t widthModifier = 0;

    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        const int32_t interval = rt.zoom_level.ApplyTo(1);

        xModifier = EuclideanRemainder(left, interval);
        xModifier = xModifier ? interval - xModifier : 0;
        yModifier = EuclideanRemainder(top, interval);
        widthModifier = EuclideanRemainder(left + g1Element->width, interval);
        widthModifier = widthModifier ? interval - widthModifier : 0;

        texture.coords.x += xModifier;
        texture.coords.y += (interval - 1) - yModifier;
    }

    left = rt.zoom_level.ApplyInversedTo(left + xModifier);
    top = rt.zoom_level.ApplyInversedTo(top);
    int32_t right = left + rt.zoom_level.ApplyInversedTo(g1Element->width + widthModifier);
    int32_t bottom = top + rt.zoom_level.ApplyInversedTo(g1Element->height + yModifier);

    const ScreenRect clip = CalculateClipping(rt);
    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    const float zoom = rt.zoom_level >= ZoomLevel{ 0 } ? static_cast<float>(rt.zoom_level.ApplyTo(1))
                                                       : 1.0f / static_cast<float>(rt.zoom_level.ApplyInversedTo(1));

    int paletteCount;
    ivec3 palettes{};
    bool special = false;
    if (imageId.HasSecondary())
    {
        palettes.x = TextureCache::PaletteToY(static_cast<FilterPaletteID>(imageId.GetPrimary()));
        palettes.y = TextureCache::PaletteToY(static_cast<FilterPaletteID>(imageId.GetSecondary()));
        if (!imageId.HasTertiary())
        {
            paletteCount = 2;
        }
        else
        {
            paletteCount = 3;
            palettes.z = TextureCache::PaletteToY(static_cast<FilterPaletteID>(imageId.GetTertiary()));
        }
    }
    else if (imageId.IsRemap() || imageId.IsBlended())
    {
        paletteCount = 1;
        FilterPaletteID palette = static_cast<FilterPaletteID>(imageId.GetRemap());
        palettes.x = TextureCache::PaletteToY(palette);
        if (palette == FilterPaletteID::paletteWater)
        {
            special = true;
        }
    }
    else
    {
        paletteCount = 0;
    }

    if (special || imageId.IsBlended())
    {
        DrawRectCommand& command = _commandBuffers.transparent.allocate();

        command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = texture.index;
        command.texMaskBounds = texture.coords;
        command.palettes = palettes;
        command.colour = palettes.x - (special ? 1 : 0);
        command.bounds = { left, top, right, bottom };
        command.flags = special ? 0 : DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK;
        command.depth = _drawCount++;
        command.zoom = zoom;
    }
    else
    {
        DrawRectCommand& command = _commandBuffers.rects.allocate();

        command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, texture.coords.z, texture.coords.w };
        command.palettes = palettes;
        command.colour = 0;
        command.bounds = { left, top, right, bottom };
        command.flags = paletteCount;
        command.depth = _drawCount++;
        command.zoom = zoom;
    }
}

void OpenGLDrawingContext::DrawSpriteRawMasked(
    RenderTarget& rt, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage)
{
    Guard::Assert(_inDraw == true);

    auto g1ElementMask = GfxGetG1Element(maskImage);
    auto g1ElementColour = GfxGetG1Element(colourImage);
    if (g1ElementMask == nullptr || g1ElementColour == nullptr)
    {
        return;
    }

    const auto textureMask = _textureCache->GetOrLoadImageTexture(maskImage);
    const auto textureColour = _textureCache->GetOrLoadImageTexture(colourImage);

    int32_t drawOffsetX = g1ElementMask->xOffset;
    int32_t drawOffsetY = g1ElementMask->yOffset;
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

    left = rt.zoom_level.ApplyInversedTo(left);
    top = rt.zoom_level.ApplyInversedTo(top);
    right = rt.zoom_level.ApplyInversedTo(right);
    bottom = rt.zoom_level.ApplyInversedTo(bottom);

    const ScreenRect clip = CalculateClipping(rt);
    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    const float zoom = rt.zoom_level >= ZoomLevel{ 0 } ? static_cast<float>(rt.zoom_level.ApplyTo(1))
                                                       : 1.0f / static_cast<float>(rt.zoom_level.ApplyInversedTo(1));

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = textureColour.index;
    command.texColourBounds = textureColour.coords;
    command.texMaskAtlas = textureMask.index;
    command.texMaskBounds = textureMask.coords;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_MASK;
    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
    command.zoom = zoom;
}

void OpenGLDrawingContext::DrawSpriteSolid(RenderTarget& rt, const ImageId image, int32_t x, int32_t y, PaletteIndex colour)
{
    Guard::Assert(_inDraw == true);

    auto g1Element = GfxGetG1Element(image);
    if (g1Element == nullptr)
    {
        return;
    }

    const auto texture = _textureCache->GetOrLoadImageTexture(image);

    int32_t drawOffsetX = g1Element->xOffset;
    int32_t drawOffsetY = g1Element->yOffset;
    int32_t drawWidth = static_cast<uint16_t>(g1Element->width);
    int32_t drawHeight = static_cast<uint16_t>(g1Element->height);

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

    const ScreenRect clip = CalculateClipping(rt);
    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = texture.index;
    command.texMaskBounds = texture.coords;
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK;
    command.colour = static_cast<GLuint>(colour);
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
    command.zoom = 1.0f;
}

void OpenGLDrawingContext::DrawGlyph(RenderTarget& rt, const ImageId image, int32_t x, int32_t y, const PaletteMap& palette)
{
    Guard::Assert(_inDraw == true);

    auto g1Element = GfxGetG1Element(image);
    if (g1Element == nullptr)
    {
        return;
    }

    const auto texture = _textureCache->GetOrLoadGlyphTexture(image, palette);

    int32_t left = x + g1Element->xOffset;
    int32_t top = y + g1Element->yOffset;
    int32_t right = left + static_cast<uint16_t>(g1Element->width);
    int32_t bottom = top + static_cast<uint16_t>(g1Element->height);

    if (left > right)
    {
        std::swap(left, right);
    }
    if (top > bottom)
    {
        std::swap(top, bottom);
    }

    left = rt.zoom_level.ApplyInversedTo(left);
    top = rt.zoom_level.ApplyInversedTo(top);
    right = rt.zoom_level.ApplyInversedTo(right);
    bottom = rt.zoom_level.ApplyInversedTo(bottom);

    const ScreenRect clip = CalculateClipping(rt);
    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    const float zoom = rt.zoom_level >= ZoomLevel{ 0 } ? static_cast<float>(rt.zoom_level.ApplyTo(1))
                                                       : 1.0f / static_cast<float>(rt.zoom_level.ApplyInversedTo(1));

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = texture.index;
    command.texColourBounds = texture.coords;
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.flags = 0;
    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
    command.zoom = zoom;
}

void OpenGLDrawingContext::DrawTTFBitmap(
    RenderTarget& rt, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
{
    Guard::Assert(_inDraw == true);

    #ifndef DISABLE_TTF
    auto baseId = static_cast<uint32_t>(0x7FFFF) - 1024;
    auto imageId = baseId + _ttfGlId;
    _engine.InvalidateImage(imageId);
    const auto texture = _textureCache->GetOrLoadBitmapTexture(imageId, surface->pixels, surface->w, surface->h);
    _ttfGlId++;
    if (_ttfGlId >= 1023)
    {
        _ttfGlId = 0;
    }

    int32_t drawOffsetX = 0;
    int32_t drawOffsetY = 0;
    int32_t drawWidth = static_cast<uint16_t>(surface->w);
    int32_t drawHeight = static_cast<uint16_t>(surface->h);

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

    const ScreenRect clip = CalculateClipping(rt);
    left += clip.GetLeft() - rt.x;
    top += clip.GetTop() - rt.y;
    right += clip.GetLeft() - rt.x;
    bottom += clip.GetTop() - rt.y;

    if (info->colourFlags.has(ColourFlag::withOutline))
    {
        std::array<ivec4, 4> boundsArr = { {
            { left + 1, top, right + 1, bottom },
            { left - 1, top, right - 1, bottom },
            { left, top + 1, right, bottom + 1 },
            { left, top - 1, right, bottom - 1 },
        } };
        for (auto b : boundsArr)
        {
            DrawRectCommand& command = _commandBuffers.rects.allocate();
            command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
            command.texColourAtlas = texture.index;
            command.texColourBounds = texture.coords;
            command.texMaskAtlas = 0;
            command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
            command.palettes = { 0, 0, 0 };
            command.flags = DrawRectCommand::FLAG_TTF_TEXT;
            command.colour = static_cast<GLuint>(info->palette.shadowOutline);
            command.bounds = b;
            command.depth = _drawCount++;
            command.zoom = 1.0f;
        }
    }
    if (info->colourFlags.has(ColourFlag::inset))
    {
        DrawRectCommand& command = _commandBuffers.rects.allocate();
        command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.palettes = { 0, 0, 0 };
        command.flags = DrawRectCommand::FLAG_TTF_TEXT;
        command.colour = static_cast<GLuint>(info->palette.shadowOutline);
        command.bounds = { left + 1, top + 1, right + 1, bottom + 1 };
        command.depth = _drawCount++;
        command.zoom = 1.0f;
    }
    auto& cmdBuf = hintingThreshold > 0 ? _commandBuffers.transparent : _commandBuffers.rects;
    DrawRectCommand& command = cmdBuf.allocate();
    command.clip = { clip.GetLeft(), clip.GetTop(), clip.GetRight(), clip.GetBottom() };
    command.texColourAtlas = texture.index;
    command.texColourBounds = texture.coords;
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettes = { 0, 0, 0 };
    command.flags = DrawRectCommand::FLAG_TTF_TEXT | (hintingThreshold << 8);
    command.colour = static_cast<GLuint>(info->palette.fill);
    command.bounds = { left, top, right, bottom };
    command.depth = _drawCount++;
    command.zoom = 1.0f;
    #endif // DISABLE_TTF
}

void OpenGLDrawingContext::FlushCommandBuffers()
{
    Guard::Assert(_inDraw == true);

    glCall(glEnable, GL_DEPTH_TEST);
    glCall(glDepthFunc, GL_LESS);

    _swapFramebuffer->BindOpaque();
    _drawRectShader->Use();
    _drawRectShader->DisablePeeling();

    FlushLines();
    FlushRectangles();

    HandleTransparency();
}

void OpenGLDrawingContext::FlushLines()
{
    if (_commandBuffers.lines.empty())
        return;

    _drawLineShader->Use();
    _drawLineShader->DrawInstances(_commandBuffers.lines);

    _commandBuffers.lines.clear();
}

void OpenGLDrawingContext::FlushRectangles()
{
    if (_commandBuffers.rects.empty())
        return;

    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, _textureCache->GetPaletteTexture());

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
    for (int32_t i = 0; i < max_depth; ++i)
    {
        _swapFramebuffer->BindTransparent();

        glCall(glEnable, GL_DEPTH_TEST);
        glCall(glDepthFunc, GL_GREATER);
        _drawRectShader->Use();

        if (i > 0)
        {
            _drawRectShader->EnablePeeling(_swapFramebuffer->GetBackDepthTexture());
        }

        OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());
        OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, _textureCache->GetPaletteTexture());

        _drawRectShader->Use();
        _drawRectShader->DrawInstances();
        _swapFramebuffer->ApplyTransparency(
            *_applyTransparencyShader, _textureCache->GetPaletteTexture(), _textureCache->GetBlendPaletteTexture());
    }

    _commandBuffers.transparent.clear();
}

ScreenRect OpenGLDrawingContext::CalculateClipping(const RenderTarget& rt) const
{
    // mber: Calculating the screen coordinates by dividing the difference between pointers like this is a dirty hack.
    //       It's also quite slow. In future the drawing code needs to be refactored to avoid this somehow.
    const RenderTarget* mainRT = _engine.getRT();
    const int32_t bytesPerRow = mainRT->LineStride();
    const int32_t bitsOffset = static_cast<int32_t>(rt.bits - mainRT->bits);
    #ifndef NDEBUG
    const ptrdiff_t bitsSize = static_cast<ptrdiff_t>(mainRT->height) * static_cast<ptrdiff_t>(bytesPerRow);
    assert(static_cast<ptrdiff_t>(bitsOffset) < bitsSize && static_cast<ptrdiff_t>(bitsOffset) >= 0);
    #endif

    const int32_t left = bitsOffset % bytesPerRow;
    const int32_t top = bitsOffset / bytesPerRow;
    const int32_t right = left + rt.width;
    const int32_t bottom = top + rt.height;

    return { { left, top }, { right, bottom } };
}

#endif /* DISABLE_OPENGL */
