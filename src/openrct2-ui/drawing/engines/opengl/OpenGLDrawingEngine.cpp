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
    #include "ApplyDropShadowShader.h"
    #include "ApplyPaletteShader.h"
    #include "ApplyTransparencyShader.h"
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

    #include <SDL_video.h>
    #include <algorithm>
    #include <array>
    #include <cassert>
    #include <cmath>
    #include <cstdlib>
    #include <cstring>
    #include <openrct2-ui/interface/Window.h>
    #include <openrct2/Context.h>
    #include <openrct2/OpenRCT2.h>
    #include <openrct2/PlatformEnvironment.h>
    #include <openrct2/config/Config.h>
    #include <openrct2/core/Console.hpp>
    #include <openrct2/core/File.h>
    #include <openrct2/core/Imaging.h>
    #include <openrct2/core/Path.hpp>
    #include <openrct2/drawing/Drawing.Sprite.h>
    #include <openrct2/drawing/Drawing.String.h>
    #include <openrct2/drawing/Drawing.h>
    #include <openrct2/drawing/IDrawingContext.h>
    #include <openrct2/drawing/IDrawingEngine.h>
    #include <openrct2/drawing/InvalidationGrid.h>
    #include <openrct2/drawing/LightFX.h>
    #include <openrct2/drawing/RenderTarget.h>
    #include <openrct2/drawing/TTF.h>
    #include <openrct2/drawing/WeatherDrawer.h>
    #include <openrct2/interface/Screenshot.h>
    #include <openrct2/interface/Viewport.h>
    #include <openrct2/interface/Window.h>
    #include <openrct2/interface/WindowBase.h>
    #include <openrct2/paint/Paint.h>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/world/Map.h>
    #include <openrct2/world/Weather.h>
    #include <openrct2/world/tile_element/SurfaceElement.h>
    #include <openrct2/world/tile_element/TileElement.h>
    #include <optional>
    #include <unordered_map>

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

/** View-space units per destination pixel for a zoom level (0.5 zoomed in, 2 zoomed out, ...). */
static float ZoomScaleOf(ZoomLevel zoom)
{
    const int8_t level = static_cast<int8_t>(zoom);
    return level >= 0 ? static_cast<float>(1 << level) : 1.0f / static_cast<float>(1 << -level);
}

/** Shadows are drawn this frame: switched on, and not under rain or snow - except for the one
    frame a lightning flash lights the storm, by which time UpdatePaletteEffects has moved
    gLightningFlash to 2. */
static bool ShadowsActiveThisFrame()
{
    return Config::Get().general.dropShadows && (!Weather::isPrecipitating() || Weather::gLightningFlash == 2);
}

/** Writes GL-order (bottom-up) RGBA8 rows to a PNG. */
static void WriteBottomUpRgbaPng(const std::string& path, int32_t width, int32_t height, const std::vector<uint8_t>& rgba)
{
    const size_t stride = static_cast<size_t>(width) * 4u;
    Image image;
    image.Width = width;
    image.Height = height;
    image.Depth = 32;
    image.Stride = static_cast<uint32_t>(stride);
    image.Pixels.resize(rgba.size());
    for (int32_t y = 0; y < height; y++)
    {
        std::memcpy(
            image.Pixels.data() + static_cast<size_t>(y) * stride, rgba.data() + static_cast<size_t>(height - 1 - y) * stride,
            stride);
    }
    try
    {
        Imaging::WriteToFile(path, image, ImageFormat::png);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Unable to write %s: %s", path.c_str(), e.what());
    }
}

class OpenGLDrawingEngine;

class OpenGLDrawingContext final : public IDrawingContext
{
private:
    OpenGLDrawingEngine& _engine;
    std::unique_ptr<ApplyTransparencyShader> _applyTransparencyShader;
    std::unique_ptr<ApplyDropShadowShader> _applyDropShadowShader;
    std::unique_ptr<DrawLineShader> _drawLineShader;
    std::unique_ptr<DrawRectShader> _drawRectShader;
    std::unique_ptr<SwapFramebuffer> _swapFramebuffer;
    std::unique_ptr<OpenGLFramebuffer> _shadowFramebuffer;
    OpenGLFramebuffer* _flushTarget = nullptr;

    std::unique_ptr<TextureCache> _textureCache;

    int32_t _drawCount = 0;
    SpriteWorldDepthHint _spriteWorldDepth{};
    /** Zoom of the destination while DrawSprite draws a sprite's zoomed-out variant through
        an RT one level further in. */
    std::optional<ZoomLevel> _depthZoomOverride;

    // Canvas (camera) size, so the sun pass can restore the shaders' screen size.
    int32_t _canvasWidth = 0;
    int32_t _canvasHeight = 0;

    /** Every visible viewport needs its own sun capture: a mini-viewport (ride camera, tile
        preview, ...) can sit at a different zoom, rotation or position than the main view, so
        one shared sun buffer is only ever correct for whichever viewport captured last. Keyed
        by the camera Viewport's address, which is stable for that viewport's lifetime. */
    struct SunCapture
    {
        /** Sized to this viewport's world footprint under the sun's 90-degree yaw, so it is
            deliberately not canvas-sized. */
        std::unique_ptr<OpenGLFramebuffer> framebuffer;
        /** The sun pass paints through the normal software-RT bookkeeping, which locates an RT
            by pointer-diffing against getRT()->bits, so it needs a bits block of its own. */
        std::unique_ptr<PaletteIndex[]> bits;
        RenderTarget rt{};
        /** Sun-space projection the buffer contents were painted with. pos stays {0,0}, so
            projecting through it lands directly in the buffer's own pixel space. */
        Viewport sunViewport{};
        /** View-units per sun-buffer texel. */
        float zoomScale = 1.0f;
        /** False until the first full paint, and whenever the buffer is reallocated. */
        bool contentsValid = false;

        /** Dirty blocks in sun-buffer space since the last capture, fed by
            RegisterShadowInvalidationTarget: every world invalidation that reaches a real
            viewport is also projected through sunViewport and marked here. */
        Drawing::InvalidationGrid invalidationGrid;

        /** The grid is only as good as the invalidations feeding it; anything that animates
            without issuing one would cast a stale shadow for as long as the camera holds
            still. Each capture therefore also repaints one column band, sweeping the whole
            buffer every kSunRefreshSweepCaptures captures. A column costs its share of a full
            repaint and no more, unlike a row band, which pays the paint traversal's whole
            vertical margin, and unlike a periodic full repaint, which hitches. */
        int32_t refreshSweepIndex = 0;

        /** Cleared and re-marked each ApplyDropShadows call; captures whose viewport was not
            visible this frame are unregistered and freed. */
        bool usedThisFrame = false;
    };
    std::unordered_map<const Viewport*, SunCapture> _sunCaptures;
    /** Lightning: the sun stands on the camera's left instead of its right for this frame. */
    bool _sunFromLeft = false;
    bool _inLightningFlash = false;
    uint32_t _lightningFlashCount = 0;
    static constexpr int32_t kSunRefreshSweepCaptures = 30;
    /** Sun-grid block size. Measured on a busy park: 256 repaints a whole block for one peep,
        while 64 and below split the work into so many rects that the paint traversal's
        per-rect margin dominates, even after merging. */
    static constexpr uint32_t kSunGridBlockSize = 128;

    /** Scratch target for scrolling a sun buffer: GL leaves an overlapping blit within one
        framebuffer undefined, so the shift goes through here. Shared by every capture and
        grown to the largest one. */
    std::unique_ptr<OpenGLFramebuffer> _sunScrollScratch;

    /** The mask in _shadowFramebuffer stays correct for as long as neither the world, nor any
        sun buffer, nor any viewport's projection changed, so it is cached against those. */
    bool _shadowMaskValid = false;
    bool _worldRedrawn = true;
    uint64_t _shadowViewportKey = 0;

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
    void ApplyWorldDepthToCommand(DrawRectCommand& command, const RenderTarget& rt, const ScreenRect& clip);

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

    GLuint GetWorldDepthTexture() const
    {
        return _swapFramebuffer->GetWorldDepthTexture();
    }

    GLuint GetShadowTexture() const
    {
        return _shadowFramebuffer != nullptr ? _shadowFramebuffer->GetTexture() : 0;
    }

    void Initialise();
    void Resize(int32_t width, int32_t height);
    void ResetPalette();
    void StartNewDraw();
    void FinishDraw();
    void ApplyDropShadows();
    /** Returns the (possibly newly created) capture for cameraVp, or nullptr if this frame's
        capture failed (not mid-draw, or the sun target couldn't be allocated). */
    SunCapture* CaptureSunDepth(const Viewport& cameraVp);
    using SunRects = sfl::static_vector<ScreenRect, 128>;
    using ShadowedViewports = sfl::static_vector<const Viewport*, kMaxViewportCount>;
    /** The sun-space projection that covers cameraVp's world footprint, with the capture's
        target allocated to fit. Nothing if the target could not be allocated. */
    std::optional<Viewport> PlaceSunProjection(SunCapture& capture, const Viewport& cameraVp);
    /** What an incremental repaint must redraw, merged by paint cost. Consumes the dirty grid. */
    SunRects CollectSunRepaintRects(SunCapture& capture, const Viewport& sunVp, ScreenCoordsXY shift);
    void BeginSunPass(SunCapture& capture);
    void EndSunPass();
    void RenderSunRect(SunCapture& capture, const Viewport& sunVp, const ScreenRect& rect, uint8_t cameraRotation);
    void UpdateLightning();
    void ReleaseSunCaptures();
    ShadowedViewports SelectShadowedViewports() const;
    /** Changes whenever a shadowed viewport's projection or the sun's side does. */
    uint64_t ShadowViewportKey(const ShadowedViewports& viewports) const;
    /** Frees the captures of viewports no longer shadowed; true if a remaining one has dirt. */
    bool RetireUnusedSunCaptures(const ShadowedViewports& viewports);
    void ResolveViewport(const Viewport& vp, const SunCapture& capture);
    void EnsureSunTarget(SunCapture& capture, const Viewport* ownerKey, int32_t width, int32_t height);
    /** Moves the buffer contents by shift texels (top-down). False if it could not. */
    bool ScrollSunBuffer(SunCapture& capture, ScreenCoordsXY shift);
    void ClearWorldDepth();
    void ClearWorldDepthRect(int32_t left, int32_t top, int32_t right, int32_t bottom);

    /** Any repaint of world pixels invalidates the cached shadow mask. */
    void NotifyWorldRedrawn()
    {
        _worldRedrawn = true;
    }

    /** Force a full resolve on the next call, ignoring the one-frame lag allowance. Used by
        the screenshot path, which must not hand back a mask that trails the frame. */
    void InvalidateShadowMask()
    {
        _shadowMaskValid = false;
        _worldRedrawn = true;
    }

    void SetSpriteWorldDepthHint(const SpriteWorldDepthHint& hint) override
    {
        _spriteWorldDepth = hint;
    }

    void Clear(RenderTarget& rt, PaletteIndex paletteIndex) override;
    void FillRect(
        RenderTarget& rt, PaletteIndex paletteIndex, int32_t left, int32_t top, int32_t right, int32_t bottom,
        bool crossHatch = false) override;
    void FilterRect(
        RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom) override;
    void DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line) override;
    void DrawSprite(RenderTarget& rt, ImageId imageId, int32_t x, int32_t y) override;
    void DrawSpriteRawMasked(RenderTarget& rt, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage) override;
    void DrawSpriteSolid(RenderTarget& rt, ImageId image, int32_t x, int32_t y, PaletteIndex colour) override;
    void DrawGlyph(RenderTarget& rt, ImageId image, int32_t x, int32_t y, const PaletteMap& palette) override;
    void DrawTTFBitmap(
        RenderTarget& rt, const TextDrawInfo& info, TTFSurface* surface, int32_t x, int32_t y,
        uint8_t hintingThreshold) override;

    void FlushCommandBuffers();
    void DrawRectsMaskingDepthlessRuns(const RectCommandBatch& batch);

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
    std::unique_ptr<PaletteIndex[]> _bits;

    RenderTarget _mainRT = {};
    // While the sun-depth pass runs it paints into its own, larger render target.
    // CalculateClipping locates an RT by pointer-diffing against whatever getRT()
    // returns, so the override has to be swapped in for the whole sun pass.
    RenderTarget* _rtOverride = nullptr;

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
    GLuint _shadeRemapTexture = 0;

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
        _tempFramebuffer = std::make_unique<OpenGLFramebuffer>(width, height, false, true, false, true);

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

        _drawingContext->ApplyDropShadows();

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

        DrawPaletteComposite();

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

    /** Palette lookup and shadow mask, into whatever framebuffer is bound. */
    /** The game's own darkening palettes - darken 1, 2 and 3 - as a 256x3 index lookup, so the
        composite can shade a pixel by stepping down its colour ramp the way the art is shaded,
        rather than multiplying it toward black. Built once the sprite data is loaded. */
    void EnsureShadeRemapTexture()
    {
        if (_shadeRemapTexture != 0)
            return;
        std::array<uint8_t, 256 * 3> table{};
        const FilterPaletteID levels[3] = { FilterPaletteID::paletteDarken1, FilterPaletteID::paletteDarken2,
                                            FilterPaletteID::paletteDarken3 };
        for (int32_t level = 0; level < 3; level++)
        {
            const auto map = GetPaletteMapForColour(levels[level]);
            for (int32_t i = 0; i < 256; i++)
                table[level * 256 + i] = map.has_value() ? static_cast<uint8_t>((*map)[i]) : static_cast<uint8_t>(i);
        }
        glCall(glGenTextures, 1, &_shadeRemapTexture);
        glCall(glBindTexture, GL_TEXTURE_2D, _shadeRemapTexture);
        glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glCall(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
        glCall(glTexImage2D, GL_TEXTURE_2D, 0, GL_R8UI, 256, 3, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, table.data());
    }

    void DrawPaletteComposite()
    {
        EnsureShadeRemapTexture();
        _applyPaletteShader->Use();
        _applyPaletteShader->SetUseShadows(ShadowsActiveThisFrame());
        ApplyPaletteShader::SetTexture(_drawingContext->GetFinalFramebuffer().GetTexture());
        ApplyPaletteShader::SetShadowTexture(_drawingContext->GetShadowTexture());
        ApplyPaletteShader::SetWorldDepthTexture(_drawingContext->GetWorldDepthTexture());
        ApplyPaletteShader::SetShadeRemapTexture(_shadeRemapTexture);
        _applyPaletteShader->Draw();
    }

    void PaintWindows() override
    {
        if (Weather::hasWeatherEffect() || gPaintForceRedraw)
        {
            _drawingContext->NotifyWorldRedrawn();
            _drawingContext->ClearWorldDepth();
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
        _drawingContext->NotifyWorldRedrawn();
        _drawingContext->ClearWorldDepthRect(left, top, right, bottom);
        // Draw region
        WindowDrawAll(_mainRT, left, top, right, bottom);
    }

    void PaintWeather() override
    {
        DrawWeather(_mainRT, &_weatherDrawer);
    }

    std::string Screenshot() override
    {
        // The screen shows a composited RGB frame, not the palette-indexed canvas, so shoot
        // that: shadows and all. A screenshot must not hand back a mask that trails the
        // frame by the one-frame lag the live view allows, so force a full resolve.
        _drawingContext->FlushCommandBuffers();
        _drawingContext->InvalidateShadowMask();
        _drawingContext->ApplyDropShadows();

        const auto width = static_cast<int32_t>(_width);
        const auto height = static_cast<int32_t>(_height);
        std::vector<uint8_t> pixels(static_cast<size_t>(width) * static_cast<size_t>(height) * 4u);
        OpenGLFramebuffer rgbaFramebuffer(width, height, false, false);
        rgbaFramebuffer.Bind();
        glDisable(GL_DEPTH_TEST);
        _applyPaletteShader->Use();
        _applyPaletteShader->SetPalette(GLPalette);
        DrawPaletteComposite();
        glCall(glPixelStorei, GL_PACK_ALIGNMENT, 1);
        glCall(glReadPixels, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        _drawingContext->FinishDraw();

        const auto outPath = ScreenshotGetNextPath();
        if (!outPath.has_value())
        {
            return {};
        }

        WriteBottomUpRgbaPng(*outPath, width, height, pixels);
        return File::Exists(*outPath) ? *outPath : std::string{};
    }

    void CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy) override
    {
        if (dx == 0 && dy == 0)
            return;

    #ifdef __MACOSX__
        // The macOS path below round-trips the palette indices through the CPU and has no
        // way to carry the world-depth attachment along, so redraw the moved region instead.
        _drawingContext->ClearWorldDepth();
        _invalidationGrid.invalidate(0, 0, static_cast<int32_t>(_width), static_cast<int32_t>(_height));
        return;
    #endif

        _drawingContext->FlushCommandBuffers();

        auto& framebuffer = _drawingContext->GetFinalFramebuffer();

        const int32_t texWidth = static_cast<int32_t>(framebuffer.GetWidth());
        const int32_t texHeight = static_cast<int32_t>(framebuffer.GetHeight());

        const ScreenRect shiftedRect{ { x, y }, { x + width, y + height } };

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

        // First pass: Copy from main to temp. The copy shader only writes the colour output, so
        // restrict both targets to that attachment; the world-depth one is blitted separately.
        GLuint mainTexture = framebuffer.GetTexture();
        tempBuffer.BindDraw();
        glCall(glDrawBuffer, GL_COLOR_ATTACHMENT0);
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
        glCall(glDrawBuffer, GL_COLOR_ATTACHMENT0);
        tempBuffer.BindRead();
        glCall(glViewport, destX, destY, width, height);
        _copyRectShader->Use();
        _copyRectShader->SetTexture(tempTexture);
        _copyRectShader->SetSourceRect(0, 0, width, height);
        _copyRectShader->SetTextureSize(texWidth, texHeight);
        _copyRectShader->Draw();

        // World positions are absolute, so the G-buffer scrolls exactly like the colour does.
        tempBuffer.BlitWorldDepth(framebuffer, srcX, srcY, width, height, 0, 0);
        framebuffer.BlitWorldDepth(tempBuffer, 0, 0, width, height, destX, destY);

        // The caller repaints the strips this shift exposed with WindowDrawAll, which writes
        // depth only where it draws something, so void at the map edge would keep whatever
        // was there before. Clear them - after the copy, since they were part of its source.
        const int32_t left = shiftedRect.getLeft();
        const int32_t top = shiftedRect.getTop();
        const int32_t right = shiftedRect.getRight();
        const int32_t bottom = shiftedRect.getBottom();
        if (dx > 0)
            _drawingContext->ClearWorldDepthRect(left, top, left + dx, bottom);
        else if (dx < 0)
            _drawingContext->ClearWorldDepthRect(right + dx, top, right, bottom);
        if (dy > 0)
            _drawingContext->ClearWorldDepthRect(left, top, right, top + dy);
        else if (dy < 0)
            _drawingContext->ClearWorldDepthRect(left, bottom + dy, right, bottom);

        framebuffer.Bind();

    #else

        // Slow but functional path for MacOS, OpenGL is deprecated on MacOS
        // and there seems to be issues that aren't present on other platforms.

        framebuffer.GetPixels(_mainRT);

        int32_t stride = _mainRT.LineStride();
        PaletteIndex* to = _mainRT.bits + y * stride + x;
        PaletteIndex* from = _mainRT.bits + (y - dy) * stride + x - dx;

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
        return _rtOverride != nullptr ? _rtOverride : &_mainRT;
    }

    /** Redirect getRT() for the duration of an off-screen pass (sun depth capture). */
    void SetRTOverride(RenderTarget* rt)
    {
        _rtOverride = rt;
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

        auto newBits = std::make_unique<PaletteIndex[]>(newBitsSize);
        if (_bits == nullptr)
        {
            std::fill_n(newBits.get(), newBitsSize, PaletteIndex::transparent);
        }
        else
        {
            if (_pitch == pitch)
            {
                std::copy_n(_bits.get(), std::min(_bitsSize, newBitsSize), newBits.get());
            }
            else
            {
                PaletteIndex* src = _bits.get();
                PaletteIndex* dst = newBits.get();

                uint32_t minWidth = std::min(_width, width);
                uint32_t minHeight = std::min(_height, height);
                for (uint32_t y = 0; y < minHeight; y++)
                {
                    std::copy_n(src, minWidth, dst);
                    if (pitch - minWidth > 0)
                    {
                        std::fill_n(dst + minWidth, pitch - minWidth, PaletteIndex::transparent);
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
        if (GetContext()->GetUiContext().GetScaleQuality() != ScaleQuality::nearestNeighbour)
        {
            _scaleFramebuffer = std::make_unique<OpenGLFramebuffer>(_width, _height, false, false);
        }
        if (GetContext()->GetUiContext().GetScaleQuality() == ScaleQuality::smoothNearestNeighbour)
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
    _applyDropShadowShader = std::make_unique<ApplyDropShadowShader>();
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

    _canvasWidth = width;
    _canvasHeight = height;

    // Re-create canvas framebuffer
    _swapFramebuffer = std::make_unique<SwapFramebuffer>(width, height);
    _shadowFramebuffer = std::make_unique<OpenGLFramebuffer>(width, height, false, false);
    // Sun framebuffers are allocated lazily by EnsureSunTarget — their size depends on each
    // viewport's world footprint under a 90° yaw, not on the canvas size. Every viewport's
    // captures still get discarded here: a canvas resize usually comes with viewport size
    // changes too, and any that don't will just get a fresh (still correct) full capture next
    // frame - simpler and safer than trying to figure out which ones are still valid.
    for (auto& [owner, capture] : _sunCaptures)
    {
        UnregisterShadowInvalidationTarget(owner);
    }
    _sunCaptures.clear();
    _shadowMaskValid = false;
    _worldRedrawn = true;
}

void OpenGLDrawingContext::EnsureSunTarget(SunCapture& capture, const Viewport* ownerKey, int32_t width, int32_t height)
{
    // Grow to cover both this request and whatever the target already holds, so a viewport
    // that resizes or re-zooms slightly does not reallocate every frame. A larger capture only
    // sees more of the world, so it stays correct - but stop growing once that costs more than
    // double the area actually asked for, and let it shrink back instead.
    if (capture.framebuffer != nullptr)
    {
        const int32_t grownWidth = std::max(width, capture.rt.width);
        const int32_t grownHeight = std::max(height, capture.rt.height);
        if (static_cast<int64_t>(grownWidth) * grownHeight <= 2 * static_cast<int64_t>(width) * height)
        {
            width = grownWidth;
            height = grownHeight;
        }
    }

    if (capture.framebuffer != nullptr && capture.rt.width == width && capture.rt.height == height)
    {
        return;
    }

    // Allocation can genuinely fail: one capture is live per visible viewport, and a tall
    // window drives this hard (the camera's screen HEIGHT sets the sun buffer's WIDTH, doubled).
    // Failing to shadow one viewport is far better than dying, so drop the capture and carry
    // on; a null framebuffer makes CaptureSunDepth skip this viewport for the frame.
    try
    {
        capture.framebuffer = std::make_unique<OpenGLFramebuffer>(width, height, true, true, false, true, /*depth16*/ true);
        // GPU-side failure does not throw - glTexImage2D reports an error and leaves the
        // framebuffer incomplete, which only crashes once something draws into it.
        if (!capture.framebuffer->IsComplete())
        {
            throw std::runtime_error("framebuffer incomplete (GPU allocation likely failed)");
        }

        const size_t bitsSize = static_cast<size_t>(width) * static_cast<size_t>(height);
        capture.bits = std::make_unique<PaletteIndex[]>(bitsSize);
        std::fill_n(capture.bits.get(), bitsSize, PaletteIndex::transparent);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Sun depth target %dx%d could not be allocated (%s); skipping shadows here", width, height, e.what());
        capture.framebuffer.reset();
        capture.bits.reset();
        capture.rt = {};
        capture.contentsValid = false;
        return;
    }

    capture.rt = {};
    capture.rt.bits = capture.bits.get();
    capture.rt.width = width;
    capture.rt.height = height;
    capture.rt.DrawingEngine = _engine.getRT()->DrawingEngine;

    // Nothing in the new buffer has been painted, so the next capture must be a full one, and
    // any blocks accumulated against the old buffer mean nothing.
    capture.contentsValid = false;
    capture.invalidationGrid.reset(width, height, kSunGridBlockSize, kSunGridBlockSize);
    RegisterShadowInvalidationTarget(ownerKey, &capture.sunViewport, &capture.invalidationGrid);

    LOG_VERBOSE(
        "Sun depth target allocated: %dx%d (canvas %dx%d, owner %p)", width, height, _canvasWidth, _canvasHeight,
        static_cast<const void*>(ownerKey));
}

void OpenGLDrawingContext::ApplyWorldDepthToCommand(DrawRectCommand& command, const RenderTarget& rt, const ScreenRect& clip)
{
    if (!_spriteWorldDepth.valid || _spriteWorldDepth.mode == SpriteWorldDepthMode::none)
    {
        command.world0 = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.world1 = { 0.0f, 0.0f, 0.0f, 0.0f };
        return;
    }

    if (_spriteWorldDepth.mode == SpriteWorldDepthMode::worldNone)
    {
        command.world0 = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.world1 = { 0.0f, 0.0f, static_cast<float>(static_cast<int>(SpriteWorldDepthMode::worldNone)), 0.0f };
        return;
    }

    const float yOffset = static_cast<float>(clip.getTop() - rt.y);
    // A destination pixel is one view-space unit only at zoom 0, so each command carries the
    // scale of the RT it is drawn into. Take it from the RT rather than from any viewport: every
    // viewport, and the sun pass, paints at its own zoom. A wrong scale throws the solve off
    // the tile and clamps every pixel of it to one corner - whole tiles shadowed or lit.
    const float zoomScale = ZoomScaleOf(_depthZoomOverride.value_or(rt.zoom_level));

    if (_spriteWorldDepth.mode == SpriteWorldDepthMode::billboard)
    {
        // world0: zMin, zMax, centreX, centreY
        // world1: screenTop, screenBottom, mode, zoomScale
        command.world0 = {
            _spriteWorldDepth.zMin,
            _spriteWorldDepth.zMax,
            _spriteWorldDepth.centreX,
            _spriteWorldDepth.centreY,
        };
        // screenTop/Bottom come out of Translate3DTo2DWithZ in view-space units; the shader
        // compares them against destination pixels, so divide before adding the pixel offset.
        const float invZoom = 1.0f / zoomScale;
        command.world1 = {
            _spriteWorldDepth.screenTop * invZoom + yOffset,
            _spriteWorldDepth.screenBottom * invZoom + yOffset,
            static_cast<float>(static_cast<int>(_spriteWorldDepth.mode) | (static_cast<int>(_spriteWorldDepth.rotation) << 8)),
            zoomScale,
        };
        return;
    }

    if (_spriteWorldDepth.mode == SpriteWorldDepthMode::wall)
    {
        // world0: edge origin X, edge origin Y, zMin, zMax
        // world1: viewOffsetX, viewOffsetY, mode | rotation | edge axis, zoomScale
        // The axis rides in the mode word because world0 is already full; the shader needs
        // it to know which way the 32-unit edge runs.
        command.world0 = {
            _spriteWorldDepth.centreX,
            _spriteWorldDepth.centreY,
            _spriteWorldDepth.zMin,
            _spriteWorldDepth.zMax,
        };
        command.world1 = {
            static_cast<float>(rt.x - clip.getLeft()),
            static_cast<float>(rt.y - clip.getTop()),
            static_cast<float>(
                static_cast<int>(_spriteWorldDepth.mode) | (static_cast<int>(_spriteWorldDepth.rotation) << 8)
                | (static_cast<int>(_spriteWorldDepth.wallAxis) << 10)
                | (static_cast<int>(_spriteWorldDepth.wallTileShift) << 11)
                | (static_cast<int>(_spriteWorldDepth.wallFacesSun) << 13)
                | (static_cast<int>(_spriteWorldDepth.wallIsTerrainSide) << 14)),
            zoomScale,
        };
        return;
    }

    // Flat / water: tile origin + baseZ + slope for TileElementHeight in the shader.
    // world0: originX, originY, baseZ, slope
    // world1: viewOffsetX, viewOffsetY, mode, zoomScale
    command.world0 = {
        _spriteWorldDepth.centreX,
        _spriteWorldDepth.centreY,
        _spriteWorldDepth.baseZ,
        _spriteWorldDepth.slope,
    };
    command.world1 = {
        static_cast<float>(rt.x - clip.getLeft()),
        static_cast<float>(rt.y - clip.getTop()),
        static_cast<float>(static_cast<int>(_spriteWorldDepth.mode) | (static_cast<int>(_spriteWorldDepth.rotation) << 8)),
        zoomScale,
    };
}

/** Floors v to a multiple of m (m > 0), rounding toward negative infinity. */
static int32_t FloorToMultiple(int32_t v, int32_t m)
{
    const int32_t q = v / m;
    return (q * m > v ? q - 1 : q) * m;
}

bool OpenGLDrawingContext::ScrollSunBuffer(SunCapture& capture, ScreenCoordsXY shift)
{
    // shift is how far the contents move, in top-down texels. Blit the part that stays in view
    // out to the scratch target and back in at its new place. Only the world-depth attachment
    // is read by the resolve; the painter depth is cleared under every rect before it is
    // repainted, so neither of the others needs to move.
    const int32_t bufferW = capture.rt.width;
    const int32_t bufferH = capture.rt.height;
    const int32_t keepW = bufferW - std::abs(shift.x);
    const int32_t keepH = bufferH - std::abs(shift.y);
    if (keepW <= 0 || keepH <= 0)
        return false;

    const int32_t scratchW = _sunScrollScratch != nullptr ? static_cast<int32_t>(_sunScrollScratch->GetWidth()) : 0;
    const int32_t scratchH = _sunScrollScratch != nullptr ? static_cast<int32_t>(_sunScrollScratch->GetHeight()) : 0;
    if (scratchW < keepW || scratchH < keepH)
    {
        _sunScrollScratch = std::make_unique<OpenGLFramebuffer>(
            std::max(keepW, scratchW), std::max(keepH, scratchH), false, true, false, true);
        if (!_sunScrollScratch->IsComplete())
        {
            _sunScrollScratch.reset();
            return false;
        }
    }

    const int32_t srcX = std::max(0, -shift.x);
    const int32_t srcTop = std::max(0, -shift.y);
    const int32_t dstX = std::max(0, shift.x);
    const int32_t dstTop = std::max(0, shift.y);
    // GL rows count from the bottom.
    _sunScrollScratch->BlitWorldDepth(*capture.framebuffer, srcX, bufferH - srcTop - keepH, keepW, keepH, 0, 0);
    capture.framebuffer->BlitWorldDepth(*_sunScrollScratch, 0, 0, keepW, keepH, dstX, bufferH - dstTop - keepH);
    return true;
}

OpenGLDrawingContext::SunCapture* OpenGLDrawingContext::CaptureSunDepth(const Viewport& cameraVp)
{
    if (!_inDraw)
    {
        return nullptr;
    }

    // Finish any pending camera draws into the swap FBO first.
    if (!_commandBuffers.rects.empty() || !_commandBuffers.lines.empty() || !_commandBuffers.transparent.empty())
    {
        FlushCommandBuffers();
    }

    SunCapture& capture = _sunCaptures[&cameraVp];
    capture.usedThisFrame = true;

    const auto sunVp = PlaceSunProjection(capture, cameraVp);
    if (!sunVp.has_value())
    {
        return nullptr;
    }

    // A full repaint is needed when nothing valid is there yet or the sun projection itself
    // changed. Otherwise the buffer only needs what moved: the blocks the invalidation grid
    // collected, the strips a pan exposed, and this capture's refresh column.
    const Viewport& prev = capture.sunViewport;
    const int32_t texelSize = sunVp->zoom.ApplyTo(1);
    bool fullRepaint = !capture.contentsValid || prev.rotation != sunVp->rotation || prev.zoom != sunVp->zoom
        || prev.width != sunVp->width || prev.height != sunVp->height;
    const ScreenCoordsXY shift = fullRepaint
        ? ScreenCoordsXY{}
        : ScreenCoordsXY{ (prev.viewPos.x - sunVp->viewPos.x) / texelSize, (prev.viewPos.y - sunVp->viewPos.y) / texelSize };
    if (!fullRepaint && (shift.x != 0 || shift.y != 0) && !ScrollSunBuffer(capture, shift))
    {
        fullRepaint = true;
    }

    BeginSunPass(capture);
    if (fullRepaint)
    {
        RenderSunRect(capture, *sunVp, ScreenRect{ { 0, 0 }, { sunVp->width, sunVp->height } }, cameraVp.rotation);
        capture.invalidationGrid.reset(sunVp->width, sunVp->height, kSunGridBlockSize, kSunGridBlockSize);
        capture.refreshSweepIndex = 0;
    }
    else
    {
        for (const auto& rect : CollectSunRepaintRects(capture, *sunVp, shift))
        {
            RenderSunRect(capture, *sunVp, rect, cameraVp.rotation);
        }
    }
    EndSunPass();

    capture.sunViewport = *sunVp;
    capture.contentsValid = true;
    return &capture;
}

std::optional<Viewport> OpenGLDrawingContext::PlaceSunProjection(SunCapture& capture, const Viewport& cameraVp)
{
    // Sun yaw = camera + 90°, so shadows fall camera-LEFT on screen.
    Viewport sunVp = cameraVp;
    sunVp.rotation = static_cast<uint8_t>((cameraVp.rotation + (_sunFromLeft ? 3 : 1)) & 3);
    sunVp.pos = {};

    // Size the sun buffer to contain the camera's world footprint. With (a,b) the
    // camera-rotated world XY, the camera projects
    //   sx = b - a,  sy = (a + b)/2 - z
    // and the +90° sun view projects
    //   sx' = a + b = 2*(sy + z),  sy' = (a - b)/2 - z = -sx/2 - z
    // so the camera's screen HEIGHT drives the sun's WIDTH (doubled) and its WIDTH drives the
    // sun's HEIGHT (halved). Height moves a point too, relative to the screen-centre terrain the
    // buffer is centred on: 2 units of sx' and 1 of sy' per unit of Z, in either direction. The
    // allowance is how far above or below that terrain anything is covered - ground across the
    // view, and casters standing outside it that throw shadow in. Budgeting only one unit of
    // sx' per unit of Z left half of it uncovered, which lost the shadows along the bottom of
    // any view over high ground.
    constexpr int32_t kZAllowance = 288;
    const int32_t needViewW = 2 * cameraVp.ViewHeight() + 4 * kZAllowance;
    const int32_t needViewH = cameraVp.ViewWidth() / 2 + 2 * kZAllowance;

    // Resolution. Camera X maps onto sun Y at half scale, so a sun buffer at the camera's own
    // zoom has half a row per camera pixel - coarser than the screen, which is what made
    // zoomed-out views blocky. One zoom level finer gives at least one texel per camera pixel
    // on both axes (a 4x1 footprint), which is all the screen can show; going finer still
    // only multiplies the cost of every sun repaint and of the resolve's footprint walk.
    // Zoomed in, stay at 1:1 - magnifying adds no caster detail. The budget is a last resort
    // against allocation failure, one capture per visible viewport.
    constexpr int64_t kMaxSunTexels = 16 * 1024 * 1024;
    const int32_t cameraZoomLevel = static_cast<int8_t>(cameraVp.zoom);
    sunVp.zoom = ZoomLevel{ static_cast<int8_t>(std::max(0, cameraZoomLevel - 1)) };
    int32_t sunPxW = 0;
    int32_t sunPxH = 0;
    for (;;)
    {
        sunPxW = std::clamp(sunVp.zoom.ApplyInversedTo(needViewW), 64, 8192);
        sunPxH = std::clamp(sunVp.zoom.ApplyInversedTo(needViewH), 64, 8192);
        if (static_cast<int64_t>(sunPxW) * sunPxH <= kMaxSunTexels || static_cast<int8_t>(sunVp.zoom) >= 3)
            break;
        sunVp.zoom = sunVp.zoom + 1;
    }

    EnsureSunTarget(capture, &cameraVp, sunPxW, sunPxH);
    if (capture.framebuffer == nullptr)
    {
        return std::nullopt;
    }

    // EnsureSunTarget may hand back a larger target than asked for, and the projection has to
    // describe the buffer actually being drawn into.
    sunVp.width = capture.rt.width;
    sunVp.height = capture.rt.height;
    capture.zoomScale = ZoomScaleOf(sunVp.zoom);

    const ScreenCoordsXY mid = cameraVp.viewPos + ScreenCoordsXY{ cameraVp.ViewWidth() / 2, cameraVp.ViewHeight() / 2 };
    // The terrain actually under the screen centre: a screen point names a different tile at
    // every height, so settle the height and the tile against each other rather than reading the
    // height of whatever tile lies under the centre at Z 0.
    CoordsXY mapGuess = ViewportPosToMapPos(mid, 0, cameraVp.rotation);
    int32_t centreZ = TileElementHeight(mapGuess);
    for (int32_t i = 0; i < 4; i++)
    {
        mapGuess = ViewportPosToMapPos(mid, centreZ, cameraVp.rotation);
        centreZ = TileElementHeight(mapGuess);
    }
    if (const auto centre = centre2dCoordinates({ mapGuess, centreZ }, &sunVp))
    {
        sunVp.viewPos = *centre;
    }
    // Whole texels only, so a camera pan moves the buffer contents by an exact texel offset
    // that can be scrolled instead of repainted.
    const int32_t texelSize = sunVp.zoom.ApplyTo(1);
    sunVp.viewPos = { FloorToMultiple(sunVp.viewPos.x, texelSize), FloorToMultiple(sunVp.viewPos.y, texelSize) };
    return sunVp;
}

OpenGLDrawingContext::SunRects OpenGLDrawingContext::CollectSunRepaintRects(
    SunCapture& capture, const Viewport& sunVp, ScreenCoordsXY shift)
{
    const int32_t sunPxW = sunVp.width;
    const int32_t sunPxH = sunVp.height;
    SunRects rects;
    const auto addRect = [&](int32_t left, int32_t top, int32_t right, int32_t bottom) {
        left = std::max(left, 0);
        top = std::max(top, 0);
        right = std::min(right, sunPxW);
        bottom = std::min(bottom, sunPxH);
        if (left >= right || top >= bottom)
            return;
        if (rects.size() == rects.capacity())
        {
            // Too fragmented to be worth tracking separately: one rect over the lot.
            for (const auto& r : rects)
            {
                left = std::min(left, r.getLeft());
                top = std::min(top, r.getTop());
                right = std::max(right, r.getRight());
                bottom = std::max(bottom, r.getBottom());
            }
            rects.clear();
        }
        rects.push_back({ { left, top }, { right, bottom } });
    };

    // Blocks were marked against the previous projection, so they move with the contents.
    capture.invalidationGrid.traverseDirtyCells([&](int32_t left, int32_t top, int32_t right, int32_t bottom) {
        addRect(left + shift.x, top + shift.y, right + shift.x, bottom + shift.y);
    });
    if (shift.x > 0)
        addRect(0, 0, shift.x, sunPxH);
    else if (shift.x < 0)
        addRect(sunPxW + shift.x, 0, sunPxW, sunPxH);
    if (shift.y > 0)
        addRect(0, 0, sunPxW, shift.y);
    else if (shift.y < 0)
        addRect(0, sunPxH + shift.y, sunPxW, sunPxH);

    // The grid is only as good as the invalidations feeding it, so one column band is repainted
    // regardless; see SunCapture::refreshSweepIndex.
    const int32_t sweepWidth = (sunPxW + kSunRefreshSweepCaptures - 1) / kSunRefreshSweepCaptures;
    const int32_t sweepLeft = capture.refreshSweepIndex * sweepWidth;
    addRect(sweepLeft, 0, sweepLeft + sweepWidth, sunPxH);
    capture.refreshSweepIndex = (capture.refreshSweepIndex + 1) % kSunRefreshSweepCaptures;

    // Every rect is its own ViewportRender, and each one walks the paint traversal's
    // tall-caster margin below itself - several times the height of a typical dirty block.
    // So merge any two rects whose union costs less to paint than the pair, counting cost
    // as paint columns times the rows walked including that margin.
    const int32_t marginTexels = sunVp.zoom.ApplyInversedTo(kPaintVerticalTileMargin);
    const auto paintCost = [marginTexels](const ScreenRect& r) {
        return static_cast<int64_t>((r.getWidth() + 31) / 32) * (r.getHeight() + marginTexels);
    };
    for (bool merged = true; merged;)
    {
        merged = false;
        for (size_t i = 0; i < rects.size() && !merged; i++)
        {
            for (size_t j = i + 1; j < rects.size(); j++)
            {
                const ScreenRect u{
                    { std::min(rects[i].getLeft(), rects[j].getLeft()), std::min(rects[i].getTop(), rects[j].getTop()) },
                    { std::max(rects[i].getRight(), rects[j].getRight()), std::max(rects[i].getBottom(), rects[j].getBottom()) }
                };
                if (paintCost(u) <= paintCost(rects[i]) + paintCost(rects[j]))
                {
                    rects[i] = u;
                    rects.erase(rects.begin() + j);
                    merged = true;
                    break;
                }
            }
        }
    }
    return rects;
}

void OpenGLDrawingContext::BeginSunPass(SunCapture& capture)
{
    // Point the shared draw state at the sun buffer for the duration of the pass: vertex
    // positions map to NDC by destination size, and CalculateClipping pointer-diffs against
    // getRT().
    _drawRectShader->Use();
    _drawRectShader->SetScreenSize(capture.rt.width, capture.rt.height);
    _drawLineShader->Use();
    _drawLineShader->SetScreenSize(capture.rt.width, capture.rt.height);
    _flushTarget = capture.framebuffer.get();
    _engine.SetRTOverride(&capture.rt);
    capture.framebuffer->Bind();
    _drawCount = 0;
}

void OpenGLDrawingContext::EndSunPass()
{
    _engine.SetRTOverride(nullptr);
    _flushTarget = nullptr;

    // FlushCommandBuffers skips HandleTransparency for the sun pass (the depth map only needs
    // opaque geometry), which leaves the sun view's transparent sprites - water above all -
    // queued. Surviving to the next camera flush, they would be composited into the visible
    // frame at sun-view coordinates.
    _commandBuffers.transparent.clear();

    _drawRectShader->Use();
    _drawRectShader->SetScreenSize(_canvasWidth, _canvasHeight);
    _drawLineShader->Use();
    _drawLineShader->SetScreenSize(_canvasWidth, _canvasHeight);
}

void OpenGLDrawingContext::RenderSunRect(
    SunCapture& capture, const Viewport& sunVp, const ScreenRect& rect, uint8_t cameraRotation)
{
    // Scissored, so geometry overhanging the rect cannot bleed into the untouched buffer around
    // it. GL scissor origin is bottom-left.
    const GLuint zero[4] = { 0, 0, 0, 0 };
    const GLfloat depthClear[1] = { 1.0f };
    glCall(glEnable, GL_SCISSOR_TEST);
    glCall(glScissor, rect.getLeft(), sunVp.height - rect.getBottom(), rect.getWidth(), rect.getHeight());
    glCall(glClearBufferuiv, GL_COLOR, 0, zero);
    capture.framebuffer->ClearWorldDepth();
    glCall(glClearBufferfv, GL_DEPTH, 0, depthClear);

    RenderTarget rectRT = capture.rt.Crop({ rect.getLeft(), rect.getTop() }, { rect.getWidth(), rect.getHeight() });
    gPaintShadowCameraRotation = cameraRotation;
    ViewportRender(rectRT, &sunVp);
    gPaintShadowCameraRotation = -1;
    FlushCommandBuffers();
    glCall(glDisable, GL_SCISSOR_TEST);
}

/** A viewport's screen rect as a GL scissor box (x, y from the bottom, width, height), or nothing
    if none of it is on the framebuffer. */
static std::optional<std::array<GLint, 4>> ViewportScissor(const Viewport& vp, int32_t fbWidth, int32_t fbHeight)
{
    const int32_t sx = std::clamp(vp.pos.x, 0, fbWidth);
    const int32_t sw = std::clamp(vp.width, 0, fbWidth - sx);
    const int32_t syTop = std::clamp(vp.pos.y, 0, fbHeight);
    const int32_t sh = std::clamp(vp.height, 0, fbHeight - syTop);
    if (sw <= 0 || sh <= 0)
        return std::nullopt;
    return std::array<GLint, 4>{ sx, fbHeight - syTop - sh, sw, sh };
}

void OpenGLDrawingContext::ApplyDropShadows()
{
    if (_shadowFramebuffer == nullptr || _applyDropShadowShader == nullptr)
    {
        return;
    }

    UpdateLightning();
    if (!Config::Get().general.dropShadows)
    {
        // Free what the shadows were holding - a sun buffer per viewport is not small - and
        // make sure turning them back on resolves from scratch. The composite skips the mask.
        ReleaseSunCaptures();
        GetFinalFramebuffer().Bind();
        return;
    }
    if (!ShadowsActiveThisFrame())
    {
        // Rain or snow: keep the buffers - the weather passes - but skip the work. The composite
        // skips the mask.
        _shadowMaskValid = false;
        GetFinalFramebuffer().Bind();
        return;
    }

    const ShadowedViewports viewports = SelectShadowedViewports();
    const uint64_t viewportKey = ShadowViewportKey(viewports);
    const bool sunBuffersDirty = RetireUnusedSunCaptures(viewports);

    // The mask is a function of the world G-buffer, the sun buffers and every viewport's
    // projection. If none of them moved, last frame's mask is still the right answer. Never let
    // it trail a changed frame, even by one: the pixels a moving peep just left are ground again,
    // and last frame's mask has them as an unshaded sprite.
    if (_shadowMaskValid && viewportKey == _shadowViewportKey && !_worldRedrawn && !sunBuffersDirty)
    {
        GetFinalFramebuffer().Bind();
        return;
    }
    _shadowViewportKey = viewportKey;
    _shadowMaskValid = true;
    _worldRedrawn = false;

    // Start fully lit; each viewport then resolves only its own rectangle.
    _shadowFramebuffer->Bind();
    glCall(glDisable, GL_DEPTH_TEST);
    glCall(glDisable, GL_BLEND);
    const GLfloat lit[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glCall(glClearBufferfv, GL_COLOR, 0, lit);

    for (const Viewport* vp : viewports)
    {
        if (vp == nullptr || vp->width <= 0 || vp->height <= 0)
            continue;
        if (const SunCapture* capture = CaptureSunDepth(*vp); capture != nullptr)
            ResolveViewport(*vp, *capture);
    }

    // A viewport left out (secondary viewports switched off) still sits inside the main view's
    // screen rect, which was just resolved against the main view's sun - so clear it back to lit
    // rather than leave it shaded by a projection that is not its own.
    _shadowFramebuffer->Bind();
    const auto fbWidth = static_cast<int32_t>(_shadowFramebuffer->GetWidth());
    const auto fbHeight = static_cast<int32_t>(_shadowFramebuffer->GetHeight());
    glCall(glEnable, GL_SCISSOR_TEST);
    for (const Viewport* vp : GetVisibleViewports())
    {
        if (std::find(viewports.begin(), viewports.end(), vp) != viewports.end())
            continue;
        if (const auto box = ViewportScissor(*vp, fbWidth, fbHeight))
        {
            glCall(glScissor, (*box)[0], (*box)[1], (*box)[2], (*box)[3]);
            glCall(glClearBufferfv, GL_COLOR, 0, lit);
        }
    }
    glCall(glDisable, GL_SCISSOR_TEST);

    GetFinalFramebuffer().Bind();
}

void OpenGLDrawingContext::UpdateLightning()
{
    // A lightning flash lights the storm for one frame: UpdatePaletteEffects has already
    // brightened the palette for it and moved gLightningFlash to 2 by the time this runs. Shadows
    // are cast for that frame only, harder than daylight's, and from the side opposite the usual
    // sun on every other flash, so the flashes do not all read the same.
    const bool lightningFlash = Config::Get().general.dropShadows && Weather::gLightningFlash == 2;
    if (lightningFlash && !_inLightningFlash)
        _lightningFlashCount++;
    _inLightningFlash = lightningFlash;
    _sunFromLeft = lightningFlash && (_lightningFlashCount & 1) != 0;
}

void OpenGLDrawingContext::ReleaseSunCaptures()
{
    for (const auto& [owner, capture] : _sunCaptures)
    {
        UnregisterShadowInvalidationTarget(owner);
    }
    _sunCaptures.clear();
    _sunScrollScratch.reset();
    _shadowMaskValid = false;
}

OpenGLDrawingContext::ShadowedViewports OpenGLDrawingContext::SelectShadowedViewports() const
{
    // Every viewport needs its OWN sun capture: the projection into sun space depends on that
    // viewport's viewPos, rotation and zoom. Viewports are handled in list order so a window drawn
    // on top of the main view also resolves last, overwriting the shared pixels.
    ShadowedViewports viewports;
    if (Config::Get().general.dropShadowsOnSecondaryViewports)
    {
        for (const Viewport* vp : GetVisibleViewports())
        {
            viewports.push_back(vp);
        }
    }
    else if (const auto* mainWindow = WindowGetMain(); mainWindow != nullptr && mainWindow->viewport != nullptr)
    {
        // Secondary viewports (ride cameras, tile previews, ...) opted out: each one costs
        // roughly what the main view's own shadow pass costs, since it needs an independent
        // sun capture at its own zoom and rotation.
        viewports.push_back(mainWindow->viewport);
    }
    return viewports;
}

uint64_t OpenGLDrawingContext::ShadowViewportKey(const ShadowedViewports& viewports) const
{
    uint64_t key = 1469598103934665603ULL;
    const auto mix = [&key](int64_t value) { key = (key ^ static_cast<uint64_t>(value)) * 1099511628211ULL; };
    for (const Viewport* vp : viewports)
    {
        if (vp == nullptr)
            continue;
        mix(vp->pos.x);
        mix(vp->pos.y);
        mix(vp->width);
        mix(vp->height);
        mix(vp->viewPos.x);
        mix(vp->viewPos.y);
        mix(static_cast<int8_t>(vp->zoom));
        mix(vp->rotation);
        mix(_sunFromLeft ? 3 : 1);
        mix(_inLightningFlash ? 1 : 0);
    }
    return key;
}

bool OpenGLDrawingContext::RetireUnusedSunCaptures(const ShadowedViewports& viewports)
{
    // Drop sun captures for viewports that aren't visible anymore (window closed, scrolled
    // off-screen, ...), so windows opening and closing don't leak a growing pool. Done before
    // the cache-hit check, which skips CaptureSunDepth and so marks nothing used itself.
    //
    // A caster can also move while staying off screen - just outside the view, or behind a
    // window - and throw a moving shadow into it. That dirties its sun buffer without redrawing
    // any camera pixel, so it has to count as a world change in its own right.
    for (auto& [owner, capture] : _sunCaptures)
    {
        capture.usedThisFrame = false;
    }
    bool sunBuffersDirty = false;
    for (const Viewport* vp : viewports)
    {
        if (vp == nullptr)
            continue;
        if (auto it = _sunCaptures.find(vp); it != _sunCaptures.end())
        {
            it->second.usedThisFrame = true;
            sunBuffersDirty |= it->second.invalidationGrid.isDirty();
        }
    }
    std::erase_if(_sunCaptures, [](const auto& entry) {
        if (entry.second.usedThisFrame)
            return false;
        UnregisterShadowInvalidationTarget(entry.first);
        return true;
    });
    return sunBuffersDirty;
}

void OpenGLDrawingContext::ResolveViewport(const Viewport& vp, const SunCapture& capture)
{
    _shadowFramebuffer->Bind();
    glCall(glDisable, GL_DEPTH_TEST);
    glCall(glDisable, GL_BLEND);

    const auto box = ViewportScissor(
        vp, static_cast<int32_t>(_shadowFramebuffer->GetWidth()), static_cast<int32_t>(_shadowFramebuffer->GetHeight()));
    if (!box.has_value())
        return;
    glCall(glEnable, GL_SCISSOR_TEST);
    glCall(glScissor, (*box)[0], (*box)[1], (*box)[2], (*box)[3]);

    _applyDropShadowShader->Use();
    ApplyDropShadowShader::SetCameraDepthTexture(GetWorldDepthTexture());
    ApplyDropShadowShader::SetSunDepthTexture(capture.framebuffer->GetWorldDepthTexture());
    const Viewport& sunVp = capture.sunViewport;
    _applyDropShadowShader->SetParameters(
        static_cast<float>(sunVp.viewPos.x), static_cast<float>(sunVp.viewPos.y), static_cast<float>(sunVp.width),
        static_cast<float>(sunVp.height), sunVp.rotation, capture.zoomScale, ZoomScaleOf(vp.zoom),
        _inLightningFlash ? 0.5f : Config::Get().general.shadowStrength, vp.rotation, _sunFromLeft);
    _applyDropShadowShader->Draw();

    glCall(glDisable, GL_SCISSOR_TEST);
}

void OpenGLDrawingContext::ClearWorldDepth()
{
    if (_swapFramebuffer != nullptr)
    {
        _swapFramebuffer->ClearWorldDepth();
    }
}

void OpenGLDrawingContext::ClearWorldDepthRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if (_swapFramebuffer != nullptr)
    {
        _swapFramebuffer->ClearWorldDepthRect(left, top, right, bottom);
    }
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
    // Full redraws need a clean world-depth buffer.
    if (gPaintForceRedraw)
    {
        ClearWorldDepth();
    }
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

    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_NO_TEXTURE) };
    command.colour = EnumValue(paletteIndex);
    command.bounds = { left, top, right + 1, bottom + 1 };

    command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };

    if (crossHatch)
    {
        // cross-pattern
        command.palettesFlags.w |= DrawRectCommand::FLAG_CROSS_HATCH;
    }
    ApplyWorldDepthToCommand(command, rt, clip);
}

void OpenGLDrawingContext::FilterRect(
    RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    Guard::Assert(_inDraw == true);

    const ScreenRect clip = CalculateClipping(rt);

    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.transparent.allocate();

    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_NO_TEXTURE) };
    command.colour = TextureCache::PaletteToY(palette);
    command.bounds = { left, top, right + 1, bottom + 1 };
    command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };
    ApplyWorldDepthToCommand(command, rt, clip);

    // A translucent window panel is drawn through the transparency pass, which has no
    // world-depth attachment. Mark what it covers (see FLAG_WORLD_DEPTH_ONLY) so the composite
    // can soften the shadow there instead of laying it at full strength over the panel.
    if (!_spriteWorldDepth.valid)
    {
        DrawRectCommand& depthCommand = _commandBuffers.rects.allocate();
        depthCommand = command;
        depthCommand.palettesFlags.w = DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_WORLD_DEPTH_ONLY;
    }
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
    uint8_t outcode1 = ComputeOutCode(line.point1, topLeft, bottomRight);
    uint8_t outcode2 = ComputeOutCode(line.point2, topLeft, bottomRight);

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
            clipped.x = line.point1.x + (line.point2.x - line.point1.x) *
                (bottomRight.y - line.point1.y) / (line.point2.y - line.point1.y);
            clipped.y = bottomRight.y;
        }
        else if (outcodeOut & kCSTop)
        {
            clipped.x = line.point1.x + (line.point2.x - line.point1.x) *
                (topLeft.y - line.point1.y) / (line.point2.y - line.point1.y);
            clipped.y = topLeft.y;
        }
        else if (outcodeOut & kCSRight)
        {
            clipped.y = line.point1.y + (line.point2.y - line.point1.y) *
                (bottomRight.x - line.point1.x) / (line.point2.x - line.point1.x);
            clipped.x = bottomRight.x;
        }
        else if (outcodeOut & kCSLeft)
        {
            clipped.y = line.point1.y + (line.point2.y - line.point1.y) *
                (topLeft.x - line.point1.x) / (line.point2.x - line.point1.x);
            clipped.x = topLeft.x;
        }
        // clang-format on

        if (outcodeOut == outcode1)
        {
            line.point1 = clipped;
            outcode1 = ComputeOutCode(line.point1, topLeft, bottomRight);
        }
        else
        {
            line.point2 = clipped;
            outcode2 = ComputeOutCode(line.point2, topLeft, bottomRight);
        }
    }
}

void OpenGLDrawingContext::DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line)
{
    Guard::Assert(_inDraw == true);

    const ZoomLevel zoom = rt.zoom_level;
    ScreenLine trimmedLine = { { zoom.ApplyInversedTo(line.getX1()), zoom.ApplyInversedTo(line.getY1()) },
                               { zoom.ApplyInversedTo(line.getX2()), zoom.ApplyInversedTo(line.getY2()) } };
    if (!CohenSutherlandLineClip(trimmedLine, rt))
        return;

    DrawLineCommand& command = _commandBuffers.lines.allocate();

    const ScreenRect clip = CalculateClipping(rt);
    const int32_t x1 = trimmedLine.getX1() - rt.x + clip.getLeft();
    const int32_t y1 = trimmedLine.getY1() - rt.y + clip.getTop();
    const int32_t x2 = trimmedLine.getX2() - rt.x + clip.getLeft();
    const int32_t y2 = trimmedLine.getY2() - rt.y + clip.getTop();

    command.bounds = { x1, y1, x2, y2 };
    command.colour = static_cast<GLuint>(colour);
    command.depth = _drawCount++;
}

static auto EuclideanRemainder(const auto a, const auto b)
{
    const auto r = a % b;
    return r >= 0 ? r : r + b;
};

void OpenGLDrawingContext::DrawSprite(RenderTarget& rt, const ImageId imageId, int32_t x, int32_t y)
{
    Guard::Assert(_inDraw == true);

    // A shadow-pass sprite drawn where its root matches the camera's (see drawOffsetX). Once,
    // at the outermost call: the zoomed-variant recursion below already carries the shift.
    if (_spriteWorldDepth.valid && !_depthZoomOverride.has_value())
    {
        x += _spriteWorldDepth.drawOffsetX;
        y += _spriteWorldDepth.drawOffsetY;
    }

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
            // The zoomed variant is drawn through an RT one level further in, but it lands in
            // the same destination pixels, so the world-depth solve must keep this RT's scale.
            const bool outermost = !_depthZoomOverride.has_value();
            if (outermost)
                _depthZoomOverride = rt.zoom_level;
            DrawSprite(zoomedRT, imageId.WithIndex(imageId.GetIndex() - g1Element->zoomedOffset), x >> 1, y >> 1);
            if (outermost)
                _depthZoomOverride.reset();
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
    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

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

        command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = texture.index;
        command.texMaskBounds = texture.coords;
        command.palettesFlags = { palettes.x, palettes.y, palettes.z,
                                  special ? 0 : DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK };
        command.colour = palettes.x - (special ? 1 : 0);
        command.bounds = { left, top, right, bottom };
        command.depthZoom = { static_cast<GLfloat>(_drawCount++), zoom };
        ApplyWorldDepthToCommand(command, rt, clip);
    }
    else
    {
        DrawRectCommand& command = _commandBuffers.rects.allocate();

        command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, texture.coords.z, texture.coords.w };
        command.palettesFlags = { palettes.x, palettes.y, palettes.z, paletteCount };
        command.colour = 0;
        command.bounds = { left, top, right, bottom };
        command.depthZoom = { static_cast<GLfloat>(_drawCount++), zoom };
        ApplyWorldDepthToCommand(command, rt, clip);
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
    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    const float zoom = rt.zoom_level >= ZoomLevel{ 0 } ? static_cast<float>(rt.zoom_level.ApplyTo(1))
                                                       : 1.0f / static_cast<float>(rt.zoom_level.ApplyInversedTo(1));

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = textureColour.index;
    command.texColourBounds = textureColour.coords;
    command.texMaskAtlas = textureMask.index;
    command.texMaskBounds = textureMask.coords;
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_MASK) };

    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depthZoom = { static_cast<GLfloat>(_drawCount++), zoom };
    ApplyWorldDepthToCommand(command, rt, clip);
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
    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = 0;
    command.texColourBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.texMaskAtlas = texture.index;
    command.texMaskBounds = texture.coords;
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_NO_TEXTURE | DrawRectCommand::FLAG_MASK) };

    command.colour = static_cast<GLuint>(colour);
    command.bounds = { left, top, right, bottom };
    command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };
    ApplyWorldDepthToCommand(command, rt, clip);
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
    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    const float zoom = rt.zoom_level >= ZoomLevel{ 0 } ? static_cast<float>(rt.zoom_level.ApplyTo(1))
                                                       : 1.0f / static_cast<float>(rt.zoom_level.ApplyInversedTo(1));

    DrawRectCommand& command = _commandBuffers.rects.allocate();

    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = texture.index;
    command.texColourBounds = texture.coords;
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(0) };

    command.colour = 0;
    command.bounds = { left, top, right, bottom };
    command.depthZoom = { static_cast<GLfloat>(_drawCount++), zoom };
    ApplyWorldDepthToCommand(command, rt, clip);
}

void OpenGLDrawingContext::DrawTTFBitmap(
    RenderTarget& rt, const TextDrawInfo& info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
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
    left += clip.getLeft() - rt.x;
    top += clip.getTop() - rt.y;
    right += clip.getLeft() - rt.x;
    bottom += clip.getTop() - rt.y;

    if (info.colourFlags.has(ColourFlag::withOutline))
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
            command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
            command.texColourAtlas = texture.index;
            command.texColourBounds = texture.coords;
            command.texMaskAtlas = 0;
            command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
            command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_TTF_TEXT) };

            command.colour = static_cast<GLuint>(info.palette.shadowOutline);
            command.bounds = b;
            command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };
            ApplyWorldDepthToCommand(command, rt, clip);
        }
    }
    if (info.colourFlags.has(ColourFlag::inset))
    {
        DrawRectCommand& command = _commandBuffers.rects.allocate();
        command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
        command.texColourAtlas = texture.index;
        command.texColourBounds = texture.coords;
        command.texMaskAtlas = 0;
        command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
        command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_TTF_TEXT) };

        command.colour = static_cast<GLuint>(info.palette.shadowOutline);
        command.bounds = { left + 1, top + 1, right + 1, bottom + 1 };
        command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };
        ApplyWorldDepthToCommand(command, rt, clip);
    }
    auto& cmdBuf = hintingThreshold > 0 ? _commandBuffers.transparent : _commandBuffers.rects;
    DrawRectCommand& command = cmdBuf.allocate();
    command.clip = { clip.getLeft(), clip.getTop(), clip.getRight(), clip.getBottom() };
    command.texColourAtlas = texture.index;
    command.texColourBounds = texture.coords;
    command.texMaskAtlas = 0;
    command.texMaskBounds = { 0.0f, 0.0f, 0.0f, 0.0f };
    command.palettesFlags = { 0, 0, 0, static_cast<GLint>(DrawRectCommand::FLAG_TTF_TEXT | (hintingThreshold << 8)) };

    command.colour = static_cast<GLuint>(info.palette.fill);
    command.bounds = { left, top, right, bottom };
    command.depthZoom = { static_cast<GLfloat>(_drawCount++), 1.0f };
    ApplyWorldDepthToCommand(command, rt, clip);
    #endif // DISABLE_TTF
}

void OpenGLDrawingContext::FlushCommandBuffers()
{
    Guard::Assert(_inDraw == true);

    glCall(glEnable, GL_DEPTH_TEST);
    glCall(glDepthFunc, GL_LESS);

    if (_flushTarget != nullptr)
    {
        _flushTarget->Bind();
    }
    else
    {
        _swapFramebuffer->BindOpaque();
    }
    _drawRectShader->Use();
    _drawRectShader->DisablePeeling();
    // A non-null flush target means we are capturing the sun's view rather than the camera's.
    // This had been pinned to false, which left the sun capture ranking its samples by paint
    // order instead of by height, and left terrain occupying texels that its own vertical
    // faces needed - the faces are what actually cast, so their shadows went missing.
    _drawRectShader->SetShadowMapPass(_flushTarget != nullptr);

    FlushLines();
    FlushRectangles();

    // Sun depth capture only needs opaque geometry.
    if (_flushTarget == nullptr)
    {
        HandleTransparency();
    }
}

void OpenGLDrawingContext::FlushLines()
{
    if (_commandBuffers.lines.empty())
        return;

    _drawLineShader->Use();
    _drawLineShader->DrawInstances(_commandBuffers.lines);

    _commandBuffers.lines.clear();
}

/**
 * Draws a rect batch in runs by which attachments each command writes. World sprites that carry
 * no world position of their own - labels, price text, placement ghosts, birds - have the
 * world-depth attachment masked off, and depth-only commands have colour masked off.
 *
 * Those fragments would otherwise write zeros into the world-depth buffer, erasing the
 * ground's record underneath them. The resolve pass then reads "nothing here", returns no
 * shadow, and the sprite punches a bright hole through the shadow it is sitting on. An
 * integer attachment cannot blend, so masking the write is the only way to leave the ground
 * intact. Runs are drawn in their original order, so overlapping sprites still resolve the
 * same way they did before.
 */
void OpenGLDrawingContext::DrawRectsMaskingDepthlessRuns(const RectCommandBatch& batch)
{
    // Only world sprites that carry no depth of their own get the attachment masked off. A UI
    // draw has no hint at all and lands here as mode 0, which must still write its zeros -
    // otherwise windows inherit the terrain record beneath them and get shaded through.
    // Bit 0: writes colour. Bit 1: writes world depth. A depth-only command writes neither
    // colour nor painter depth, and ORs a flag into the world-depth record it covers.
    const auto writeMask = [](const DrawRectCommand& command) {
        const bool writesColour = (command.palettesFlags.w & DrawRectCommand::FLAG_WORLD_DEPTH_ONLY) == 0;
        const bool writesDepth = (static_cast<int>(command.world1.z) & 0xFF)
            != static_cast<int>(SpriteWorldDepthMode::worldNone);
        return (writesColour ? 1 : 0) | (writesDepth ? 2 : 0);
    };

    size_t start = 0;
    while (start < batch.size())
    {
        const int runMask = writeMask(batch[start]);
        size_t end = start + 1;
        while (end < batch.size() && writeMask(batch[end]) == runMask)
            end++;

        const GLboolean colour = (runMask & 1) ? GL_TRUE : GL_FALSE;
        const GLboolean depth = (runMask & 2) ? GL_TRUE : GL_FALSE;
        glCall(glColorMaski, 0, colour, colour, colour, colour);
        glCall(glColorMaski, 1, depth, depth, depth, depth);
        // A depth-only command must not claim the painter depth either: the transparency
        // composite drops any translucent pixel not strictly in front of the opaque depth, and
        // this command shares its depth with the translucent fill it accompanies.
        glCall(glDepthMask, colour);
        if (!colour)
        {
            glCall(glEnable, GL_COLOR_LOGIC_OP);
            glCall(glLogicOp, GL_OR);
        }
        _drawRectShader->SetInstances(batch.data() + start, end - start);
        _drawRectShader->DrawInstances();
        if (!colour)
        {
            glCall(glDisable, GL_COLOR_LOGIC_OP);
        }
        start = end;
    }
    glCall(glColorMaski, 0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glCall(glColorMaski, 1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glCall(glDepthMask, GL_TRUE);
}

void OpenGLDrawingContext::FlushRectangles()
{
    if (_commandBuffers.rects.empty())
        return;

    OpenGLAPI::SetTexture(0, GL_TEXTURE_2D_ARRAY, _textureCache->GetAtlasesTexture());
    OpenGLAPI::SetTexture(1, GL_TEXTURE_2D, _textureCache->GetPaletteTexture());

    _drawRectShader->Use();
    DrawRectsMaskingDepthlessRuns(_commandBuffers.rects);

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
