/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "X8DrawingEngine.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../core/Guard.hpp"
#include "../core/Numerics.hpp"
#include "../interface/Screenshot.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../scenes/intro/IntroScene.h"
#include "../ui/UiContext.h"
#include "Drawing.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"
#include "InvalidationGrid.h"
#include "LightFX.h"
#include "Weather.h"

#include <algorithm>
#include <cassert>
#include <cstring>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

X8WeatherDrawer::X8WeatherDrawer()
{
    _weatherPixels = new WeatherPixel[_weatherPixelsCapacity];
}

X8WeatherDrawer::~X8WeatherDrawer()
{
    delete[] _weatherPixels;
}

void X8WeatherDrawer::Draw(
    RenderTarget& rt, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
    const uint8_t* weatherpattern)
{
    const uint8_t* pattern = weatherpattern;
    auto patternXSpace = *pattern++;
    auto patternYSpace = *pattern++;

    uint8_t patternStartXOffset = xStart % patternXSpace;
    uint8_t patternStartYOffset = yStart % patternYSpace;

    uint32_t pixelOffset = rt.LineStride() * y + x;
    uint8_t patternYPos = patternStartYOffset % patternYSpace;

    uint8_t* screenBits = rt.bits;

    // Stores the colours of changed pixels
    WeatherPixel* newPixels = &_weatherPixels[_weatherPixelsCount];
    for (; height != 0; height--)
    {
        auto patternX = pattern[patternYPos * 2];
        if (patternX != 0xFF)
        {
            if (_weatherPixelsCount < (_weatherPixelsCapacity - static_cast<uint32_t>(width)))
            {
                uint32_t finalPixelOffset = width + pixelOffset;

                uint32_t xPixelOffset = pixelOffset;
                xPixelOffset += (static_cast<uint8_t>(patternX - patternStartXOffset)) % patternXSpace;

                auto patternPixel = pattern[patternYPos * 2 + 1];
                for (; xPixelOffset < finalPixelOffset; xPixelOffset += patternXSpace)
                {
                    uint8_t current_pixel = screenBits[xPixelOffset];
                    screenBits[xPixelOffset] = patternPixel;
                    _weatherPixelsCount++;

                    // Store colour and position
                    *newPixels++ = { xPixelOffset, current_pixel };
                }
            }
        }

        pixelOffset += rt.LineStride();
        patternYPos++;
        patternYPos %= patternYSpace;
    }
}

void X8WeatherDrawer::Restore(RenderTarget& rt)
{
    if (_weatherPixelsCount > 0)
    {
        uint32_t numPixels = rt.LineStride() * rt.height;
        uint8_t* bits = rt.bits;
        for (uint32_t i = 0; i < _weatherPixelsCount; i++)
        {
            WeatherPixel weatherPixel = _weatherPixels[i];
            if (weatherPixel.Position >= numPixels)
            {
                // Pixel out of bounds, bail
                break;
            }

            bits[weatherPixel.Position] = weatherPixel.Colour;
        }
        _weatherPixelsCount = 0;
    }
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif

X8DrawingEngine::X8DrawingEngine([[maybe_unused]] Ui::IUiContext& uiContext)
{
    _drawingContext = new X8DrawingContext(this);
    _mainRT.DrawingEngine = this;
    LightFx::SetAvailable(true);
    _lastLightFXenabled = Config::Get().general.EnableLightFx;
}

X8DrawingEngine::~X8DrawingEngine()
{
    delete _drawingContext;
    delete[] _bits;
}

void X8DrawingEngine::Initialise()
{
}

void X8DrawingEngine::Resize(uint32_t width, uint32_t height)
{
    uint32_t pitch = width;
    ConfigureBits(width, height, pitch);

    _drawingContext->BeginDraw();
    _drawingContext->Clear(_mainRT, PaletteIndex::pi10);
    _drawingContext->EndDraw();
}

void X8DrawingEngine::SetPalette([[maybe_unused]] const GamePalette& palette)
{
}

void X8DrawingEngine::SetVSync([[maybe_unused]] bool vsync)
{
    // Not applicable for this engine
}

void X8DrawingEngine::Invalidate(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    _invalidationGrid.invalidate(left, top, right, bottom);
}

void X8DrawingEngine::BeginDraw()
{
    if (!IntroIsPlaying())
    {
        // HACK we need to re-configure the bits if light fx has been enabled / disabled
        if (_lastLightFXenabled != Config::Get().general.EnableLightFx)
        {
            Resize(_width, _height);
            GfxInvalidateScreen();
            _lastLightFXenabled = Config::Get().general.EnableLightFx;
        }
        _weatherDrawer.Restore(_mainRT);
    }

    _drawingContext->BeginDraw();
}

void X8DrawingEngine::EndDraw()
{
    _drawingContext->EndDraw();
}

void X8DrawingEngine::PaintWindows()
{
    // Redraw dirty regions before updating the viewports, otherwise
    // when viewports get panned, they copy dirty pixels
    DrawAllDirtyBlocks();
    WindowUpdateAllViewports();
    DrawAllDirtyBlocks();
}

void X8DrawingEngine::PaintWeather()
{
    DrawWeather(_mainRT, &_weatherDrawer);
}

void X8DrawingEngine::CopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy)
{
    if (dx == 0 && dy == 0)
        return;

    // Originally 0x00683359
    // Adjust for move off screen
    // NOTE: when zooming, there can be x, y, dx, dy combinations that go off the
    // screen; hence the checks. This code should ultimately not be called when
    // zooming because this function is specific to updating the screen on move
    int32_t lmargin = std::min(x - dx, 0);
    int32_t rmargin = std::min(static_cast<int32_t>(_width) - (x - dx + width), 0);
    int32_t tmargin = std::min(y - dy, 0);
    int32_t bmargin = std::min(static_cast<int32_t>(_height) - (y - dy + height), 0);
    x -= lmargin;
    y -= tmargin;
    width += lmargin + rmargin;
    height += tmargin + bmargin;

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
}

std::string X8DrawingEngine::Screenshot()
{
    return ScreenshotDumpPNG(_mainRT);
}

IDrawingContext* X8DrawingEngine::GetDrawingContext()
{
    if (!_drawingContext->IsActive())
    {
        Guard::Fail("Drawing context is not active.");
        return nullptr;
    }
    return _drawingContext;
}

RenderTarget* X8DrawingEngine::GetDrawingPixelInfo()
{
    return &_mainRT;
}

DrawingEngineFlags X8DrawingEngine::GetFlags()
{
    return { DrawingEngineFlag::dirtyOptimisations, DrawingEngineFlag::parallelDrawing };
}

void X8DrawingEngine::InvalidateImage([[maybe_unused]] uint32_t image)
{
    // Not applicable for this engine
}

RenderTarget* X8DrawingEngine::GetDPI()
{
    return &_mainRT;
}

void X8DrawingEngine::ConfigureBits(uint32_t width, uint32_t height, uint32_t pitch)
{
    size_t newBitsSize = pitch * height;
    uint8_t* newBits = new uint8_t[newBitsSize];
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
            uint8_t* src = _bits;
            uint8_t* dst = newBits;

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
        delete[] _bits;
    }

    _bits = newBits;
    _bitsSize = newBitsSize;
    _width = width;
    _height = height;
    _pitch = pitch;

    RenderTarget* rt = &_mainRT;
    rt->bits = _bits;
    rt->x = 0;
    rt->y = 0;
    rt->width = width;
    rt->height = height;
    rt->pitch = _pitch - width;

    ConfigureDirtyGrid();

    if (LightFx::IsAvailable())
    {
        LightFx::UpdateBuffers(*rt);
    }
}

void X8DrawingEngine::OnDrawDirtyBlock(int32_t, int32_t, int32_t, int32_t)
{
}

void X8DrawingEngine::ConfigureDirtyGrid()
{
    const auto blockWidth = 1u << 7;
    const auto blockHeight = 1u << 7;

    _invalidationGrid.reset(_width, _height, blockWidth, blockHeight);
}

void X8DrawingEngine::DrawAllDirtyBlocks()
{
    _invalidationGrid.traverseDirtyCells([this](int32_t left, int32_t top, int32_t right, int32_t bottom) {
        // Draw region
        DrawDirtyBlocks(left, top, right, bottom);
    });
}

void X8DrawingEngine::DrawDirtyBlocks(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    // Draw region
    OnDrawDirtyBlock(left, top, right, bottom);
    WindowDrawAll(_mainRT, left, top, right, bottom);
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

X8DrawingContext::X8DrawingContext(X8DrawingEngine* engine)
{
    _engine = engine;
}

void X8DrawingContext::Clear(RenderTarget& rt, uint8_t paletteIndex)
{
    Guard::Assert(_isDrawing == true);

    int32_t w = rt.width;
    int32_t h = rt.height;
    uint8_t* ptr = rt.bits;

    for (int32_t y = 0; y < h; y++)
    {
        std::fill_n(ptr, w, paletteIndex);
        ptr += w + rt.pitch;
    }
}

/** rct2: 0x0097FF04 */
// clang-format off
static constexpr uint16_t kPattern[] = {
    0b0111111110000000,
    0b0011111111000000,
    0b0001111111100000,
    0b0000111111110000,
    0b0000011111111000,
    0b0000001111111100,
    0b0000000111111110,
    0b0000000011111111,
    0b1000000001111111,
    0b1100000000111111,
    0b1110000000011111,
    0b1111000000001111,
    0b1111100000000111,
    0b1111110000000011,
    0b1111111000000001,
    0b1111111100000000,
};

/** rct2: 0x0097FF14 */
static constexpr uint16_t kPatternInverse[] = {
    0b1000000001111111,
    0b1100000000111111,
    0b1110000000011111,
    0b1111000000001111,
    0b1111100000000111,
    0b1111110000000011,
    0b1111111000000001,
    0b1111111100000000,
    0b0111111110000000,
    0b0011111111000000,
    0b0001111111100000,
    0b0000111111110000,
    0b0000011111111000,
    0b0000001111111100,
    0b0000000111111110,
    0b0000000011111111,
};

/** rct2: 0x0097FEFC */
static constexpr const uint16_t* kPatterns[] = {
    kPattern,
    kPatternInverse,
};
// clang-format on

void X8DrawingContext::FillRect(RenderTarget& rt, uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    Guard::Assert(_isDrawing == true);

    assert(rt.zoom_level == ZoomLevel{ 0 });
    if (left > right)
        return;
    if (top > bottom)
        return;
    if (rt.x > right)
        return;
    if (left >= rt.x + rt.width)
        return;
    if (bottom < rt.y)
        return;
    if (top >= rt.y + rt.height)
        return;

    uint16_t crosskPattern = 0;

    int32_t startX = left - rt.x;
    if (startX < 0)
    {
        crosskPattern ^= startX;
        startX = 0;
    }

    int32_t endX = right - rt.x + 1;
    if (endX > rt.width)
    {
        endX = rt.width;
    }

    int32_t startY = top - rt.y;
    if (startY < 0)
    {
        crosskPattern ^= startY;
        startY = 0;
    }

    int32_t endY = bottom - rt.y + 1;
    if (endY > rt.height)
    {
        endY = rt.height;
    }

    int32_t width = endX - startX;
    int32_t height = endY - startY;

    if (colour & 0x1000000)
    {
        // Cross hatching
        uint8_t* dst = startY * rt.LineStride() + startX + rt.bits;
        for (int32_t i = 0; i < height; i++)
        {
            uint8_t* nextdst = dst + rt.LineStride();
            uint32_t p = Numerics::ror32(crosskPattern, 1);
            p = (p & 0xFFFF0000) | width;

            // Fill every other pixel with the colour
            for (; (p & 0xFFFF) != 0; p--)
            {
                p = p ^ 0x80000000;
                if (p & 0x80000000)
                {
                    *dst = colour & 0xFF;
                }
                dst++;
            }
            crosskPattern ^= 1;
            dst = nextdst;
        }
    }
    else if (colour & 0x2000000)
    {
        assert(false);
    }
    else if (colour & 0x4000000)
    {
        uint8_t* dst = startY * rt.LineStride() + startX + rt.bits;

        // The pattern loops every 15 lines this is which
        // part the pattern is on.
        int32_t patternY = (startY + rt.y) % 16;

        // The pattern loops every 15 pixels this is which
        // part the pattern is on.
        int32_t startkPatternX = (startX + rt.x) % 16;
        int32_t patternX = startkPatternX;

        const uint16_t* patternsrc = kPatterns[colour >> 28]; // or possibly uint8_t)[esi*4] ?

        for (int32_t numLines = height; numLines > 0; numLines--)
        {
            uint8_t* nextdst = dst + rt.LineStride();
            uint16_t pattern = patternsrc[patternY];

            for (int32_t numPixels = width; numPixels > 0; numPixels--)
            {
                if (pattern & (1 << patternX))
                {
                    *dst = colour & 0xFF;
                }
                patternX = (patternX + 1) % 16;
                dst++;
            }
            patternX = startkPatternX;
            patternY = (patternY + 1) % 16;
            dst = nextdst;
        }
    }
    else
    {
        uint8_t* dst = startY * rt.LineStride() + startX + rt.bits;
        for (int32_t i = 0; i < height; i++)
        {
            std::fill_n(dst, width, colour & 0xFF);
            dst += rt.LineStride();
        }
    }
}

void X8DrawingContext::FilterRect(
    RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    Guard::Assert(_isDrawing == true);

    if (left > right)
        return;
    if (top > bottom)
        return;
    if (rt.x > right)
        return;
    if (left >= rt.x + rt.width)
        return;
    if (bottom < rt.y)
        return;
    if (top >= rt.y + rt.height)
        return;

    int32_t startX = left - rt.x;
    if (startX < 0)
    {
        startX = 0;
    }

    int32_t endX = right - rt.x + 1;
    if (endX > rt.width)
    {
        endX = rt.width;
    }

    int32_t startY = top - rt.y;
    if (startY < 0)
    {
        startY = 0;
    }

    int32_t endY = bottom - rt.y + 1;
    if (endY > rt.height)
    {
        endY = rt.height;
    }

    int32_t width = endX - startX;
    int32_t height = endY - startY;

    uint8_t* dst = rt.bits + (startY * rt.LineStride() + startX);

    // Find colour in colour table?
    auto paletteMap = GetPaletteMapForColour(EnumValue(palette));
    if (paletteMap.has_value())
    {
        const auto& paletteEntries = paletteMap.value();
        const int32_t scaled_width = width;
        const int32_t step = rt.LineStride();

        // Fill the rectangle with the colours from the colour table
        auto c = height;
        for (int32_t i = 0; i < c; i++)
        {
            uint8_t* nextdst = dst + step * i;
            for (int32_t j = 0; j < scaled_width; j++)
            {
                auto index = *(nextdst + j);
                *(nextdst + j) = paletteEntries[index];
            }
        }
    }
}

void X8DrawingContext::DrawLine(RenderTarget& rt, uint32_t colour, const ScreenLine& line)
{
    Guard::Assert(_isDrawing == true);

    GfxDrawLineSoftware(rt, line, colour);
}

void X8DrawingContext::DrawSprite(RenderTarget& rt, const ImageId imageId, int32_t x, int32_t y)
{
    Guard::Assert(_isDrawing == true);

    GfxDrawSpriteSoftware(rt, imageId, { x, y });
}

void X8DrawingContext::DrawSpriteRawMasked(
    RenderTarget& rt, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage)
{
    Guard::Assert(_isDrawing == true);

    GfxDrawSpriteRawMaskedSoftware(rt, { x, y }, maskImage, colourImage);
}

void X8DrawingContext::DrawSpriteSolid(RenderTarget& rt, const ImageId image, int32_t x, int32_t y, uint8_t colour)
{
    Guard::Assert(_isDrawing == true);

    uint8_t palette[256];
    std::fill_n(palette, sizeof(palette), colour);
    palette[0] = 0;

    const auto spriteCoords = ScreenCoordsXY{ x, y };
    GfxDrawSpritePaletteSetSoftware(rt, ImageId(image.GetIndex(), 0), spriteCoords, PaletteMap(palette));
}

void X8DrawingContext::DrawGlyph(RenderTarget& rt, const ImageId image, int32_t x, int32_t y, const PaletteMap& paletteMap)
{
    Guard::Assert(_isDrawing == true);

    GfxDrawSpritePaletteSetSoftware(rt, image, { x, y }, paletteMap);
}

#ifndef DISABLE_TTF
template<bool TUseHinting>
static void DrawTTFBitmapInternal(
    RenderTarget& rt, uint8_t colour, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
{
    assert(rt.zoom_level == ZoomLevel{ 0 });
    const int32_t surfaceWidth = surface->w;
    int32_t width = surfaceWidth;
    int32_t height = surface->h;

    const int32_t overflowX = (rt.x + rt.width) - (x + width);
    const int32_t overflowY = (rt.y + rt.height) - (y + height);
    if (overflowX < 0)
        width += overflowX;
    if (overflowY < 0)
        height += overflowY;
    int32_t skipX = x - rt.x;
    int32_t skipY = y - rt.y;

    auto src = static_cast<const uint8_t*>(surface->pixels);
    uint8_t* dst = rt.bits;

    if (skipX < 0)
    {
        width += skipX;
        src += -skipX;
        skipX = 0;
    }
    if (skipY < 0)
    {
        height += skipY;
        src += (-skipY * surfaceWidth);
        skipY = 0;
    }

    dst += skipX;
    dst += skipY * rt.LineStride();

    const int32_t srcScanSkip = surfaceWidth - width;
    const int32_t dstScanSkip = rt.LineStride() - width;
    for (int32_t yy = 0; yy < height; yy++)
    {
        for (int32_t xx = 0; xx < width; xx++)
        {
            if (*src != 0)
            {
                if constexpr (TUseHinting)
                {
                    if (*src > 180)
                    {
                        // Centre of the glyph: use full colour.
                        *dst = colour;
                    }
                    else if (*src > hintingThreshold)
                    {
                        *dst = BlendColours(colour, *dst);
                    }
                }
                else
                {
                    *dst = colour;
                }
            }
            src++;
            dst++;
        }
        src += srcScanSkip;
        dst += dstScanSkip;
    }
}
#endif // DISABLE_TTF

void X8DrawingContext::DrawTTFBitmap(
    RenderTarget& rt, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
{
#ifndef DISABLE_TTF
    const uint8_t fgColor = info->palette[1];
    const uint8_t bgColor = info->palette[3];

    if (info->flags & TEXT_DRAW_FLAG_OUTLINE)
    {
        DrawTTFBitmapInternal<false>(rt, bgColor, surface, x + 1, y, 0);
        DrawTTFBitmapInternal<false>(rt, bgColor, surface, x - 1, y, 0);
        DrawTTFBitmapInternal<false>(rt, bgColor, surface, x, y + 1, 0);
        DrawTTFBitmapInternal<false>(rt, bgColor, surface, x, y - 1, 0);
    }
    if (info->flags & TEXT_DRAW_FLAG_INSET)
    {
        DrawTTFBitmapInternal<false>(rt, bgColor, surface, x + 1, y + 1, 0);
    }

    if (hintingThreshold > 0)
        DrawTTFBitmapInternal<true>(rt, fgColor, surface, x, y, hintingThreshold);
    else
        DrawTTFBitmapInternal<false>(rt, fgColor, surface, x, y, 0);
#endif // DISABLE_TTF
}

void X8DrawingContext::BeginDraw()
{
    Guard::Assert(_isDrawing == false);

    _isDrawing = true;
}

void X8DrawingContext::EndDraw()
{
    Guard::Assert(_isDrawing == true);

    _isDrawing = false;
}
