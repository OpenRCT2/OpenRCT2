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
#include "../core/Numerics.hpp"
#include "../interface/Screenshot.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../scenes/intro/IntroScene.h"
#include "../ui/UiContext.h"
#include "Drawing.h"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"
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
    DrawPixelInfo& dpi, int32_t x, int32_t y, int32_t width, int32_t height, int32_t xStart, int32_t yStart,
    const uint8_t* weatherpattern)
{
    const uint8_t* pattern = weatherpattern;
    auto patternXSpace = *pattern++;
    auto patternYSpace = *pattern++;

    uint8_t patternStartXOffset = xStart % patternXSpace;
    uint8_t patternStartYOffset = yStart % patternYSpace;

    uint32_t pixelOffset = dpi.LineStride() * y + x;
    uint8_t patternYPos = patternStartYOffset % patternYSpace;

    uint8_t* screenBits = dpi.bits;

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

        pixelOffset += dpi.LineStride();
        patternYPos++;
        patternYPos %= patternYSpace;
    }
}

void X8WeatherDrawer::Restore(DrawPixelInfo& dpi)
{
    if (_weatherPixelsCount > 0)
    {
        uint32_t numPixels = dpi.LineStride() * dpi.height;
        uint8_t* bits = dpi.bits;
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

X8DrawingEngine::X8DrawingEngine([[maybe_unused]] const std::shared_ptr<Ui::IUiContext>& uiContext)
{
    _drawingContext = new X8DrawingContext(this);
    _bitsDPI.DrawingEngine = this;
    LightFx::SetAvailable(true);
    _lastLightFXenabled = Config::Get().general.EnableLightFx;
}

X8DrawingEngine::~X8DrawingEngine()
{
    delete _drawingContext;
    delete[] _dirtyGrid.Blocks;
    delete[] _bits;
}

void X8DrawingEngine::Initialise()
{
}

void X8DrawingEngine::Resize(uint32_t width, uint32_t height)
{
    uint32_t pitch = width;
    ConfigureBits(width, height, pitch);
    _drawingContext->Clear(_bitsDPI, PALETTE_INDEX_10);
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
    left = std::max(left, 0);
    top = std::max(top, 0);
    right = std::min(right, static_cast<int32_t>(_width));
    bottom = std::min(bottom, static_cast<int32_t>(_height));

    if (left >= right)
        return;
    if (top >= bottom)
        return;

    right--;
    bottom--;

    left >>= _dirtyGrid.BlockShiftX;
    right >>= _dirtyGrid.BlockShiftX;
    top >>= _dirtyGrid.BlockShiftY;
    bottom >>= _dirtyGrid.BlockShiftY;

    uint32_t dirtyBlockColumns = _dirtyGrid.BlockColumns;
    uint8_t* screenDirtyBlocks = _dirtyGrid.Blocks;
    for (int16_t y = top; y <= bottom; y++)
    {
        uint32_t yOffset = y * dirtyBlockColumns;
        for (int16_t x = left; x <= right; x++)
        {
            screenDirtyBlocks[yOffset + x] = 0xFF;
        }
    }
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
        _weatherDrawer.Restore(_bitsDPI);
    }
}

void X8DrawingEngine::EndDraw()
{
}

void X8DrawingEngine::PaintWindows()
{
    WindowResetVisibilities();

    // Redraw dirty regions before updating the viewports, otherwise
    // when viewports get panned, they copy dirty pixels
    DrawAllDirtyBlocks();
    WindowUpdateAllViewports();
    DrawAllDirtyBlocks();
}

void X8DrawingEngine::PaintWeather()
{
    DrawWeather(_bitsDPI, &_weatherDrawer);
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

    int32_t stride = _bitsDPI.LineStride();
    uint8_t* to = _bitsDPI.bits + y * stride + x;
    uint8_t* from = _bitsDPI.bits + (y - dy) * stride + x - dx;

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
    return ScreenshotDumpPNG(_bitsDPI);
}

IDrawingContext* X8DrawingEngine::GetDrawingContext()
{
    return _drawingContext;
}

DrawPixelInfo* X8DrawingEngine::GetDrawingPixelInfo()
{
    return &_bitsDPI;
}

DRAWING_ENGINE_FLAGS X8DrawingEngine::GetFlags()
{
    return static_cast<DRAWING_ENGINE_FLAGS>(DEF_DIRTY_OPTIMISATIONS | DEF_PARALLEL_DRAWING);
}

void X8DrawingEngine::InvalidateImage([[maybe_unused]] uint32_t image)
{
    // Not applicable for this engine
}

DrawPixelInfo* X8DrawingEngine::GetDPI()
{
    return &_bitsDPI;
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

    DrawPixelInfo* dpi = &_bitsDPI;
    dpi->bits = _bits;
    dpi->x = 0;
    dpi->y = 0;
    dpi->width = width;
    dpi->height = height;
    dpi->pitch = _pitch - width;

    ConfigureDirtyGrid();

    if (LightFx::IsAvailable())
    {
        LightFx::UpdateBuffers(*dpi);
    }
}

void X8DrawingEngine::OnDrawDirtyBlock(
    [[maybe_unused]] uint32_t x, [[maybe_unused]] uint32_t y, [[maybe_unused]] uint32_t columns, [[maybe_unused]] uint32_t rows)
{
}

void X8DrawingEngine::ConfigureDirtyGrid()
{
    _dirtyGrid.BlockShiftX = 7;
    _dirtyGrid.BlockShiftY = 5; // Keep column at 32 (1 << 5)
    _dirtyGrid.BlockWidth = 1 << _dirtyGrid.BlockShiftX;
    _dirtyGrid.BlockHeight = 1 << _dirtyGrid.BlockShiftY;
    _dirtyGrid.BlockColumns = (_width >> _dirtyGrid.BlockShiftX) + 1;
    _dirtyGrid.BlockRows = (_height >> _dirtyGrid.BlockShiftY) + 1;

    delete[] _dirtyGrid.Blocks;
    _dirtyGrid.Blocks = new uint8_t[_dirtyGrid.BlockColumns * _dirtyGrid.BlockRows];
}

void X8DrawingEngine::DrawAllDirtyBlocks()
{
    // TODO: For optimal performance it is currently limited to a single column.
    // The optimal approach would be to extract all dirty regions as rectangles not including
    // parts that are not marked dirty and have the grid more fine grained.
    // A situation like following:
    //
    //   0 1 2 3 4 5 6 7 8 9
    //   1 - - - - - - - - -
    //   2 - x x x x - - - -
    //   3 - x x - - - - - -
    //   4 - - - - - - - - -
    //   5 - - - - - - - - -
    //   6 - - - - - - - - -
    //   7 - - - - - - - - -
    //   8 - - - - - - - - -
    //   9 - - - - - - - - -
    //
    // Would currently redraw {2,2} to {3,5} where {3,4} and {3,5} are not dirty. Choosing to do this
    // per column eliminates this issue but limits it to rendering just a single column at a time.

    for (uint32_t x = 0; x < _dirtyGrid.BlockColumns; x++)
    {
        for (uint32_t y = 0; y < _dirtyGrid.BlockRows; y++)
        {
            uint32_t yOffset = y * _dirtyGrid.BlockColumns;
            if (_dirtyGrid.Blocks[yOffset + x] == 0)
            {
                continue;
            }

            // See comment above as to why this is 1.
            const uint32_t columns = 1;

            // Check rows
            auto rows = GetNumDirtyRows(x, y, columns);
            DrawDirtyBlocks(x, y, columns, rows);
        }
    }
}

uint32_t X8DrawingEngine::GetNumDirtyRows(const uint32_t x, const uint32_t y, const uint32_t columns)
{
    uint32_t yy = y;

    for (yy = y; yy < _dirtyGrid.BlockRows; yy++)
    {
        uint32_t yyOffset = yy * _dirtyGrid.BlockColumns;
        for (uint32_t xx = x; xx < x + columns; xx++)
        {
            if (_dirtyGrid.Blocks[yyOffset + xx] == 0)
            {
                return yy - y;
            }
        }
    }
    return yy - y;
}

void X8DrawingEngine::DrawDirtyBlocks(uint32_t x, uint32_t y, uint32_t columns, uint32_t rows)
{
    uint32_t dirtyBlockColumns = _dirtyGrid.BlockColumns;
    uint8_t* screenDirtyBlocks = _dirtyGrid.Blocks;

    // Unset dirty blocks
    for (uint32_t top = y; top < y + rows; top++)
    {
        uint32_t topOffset = top * dirtyBlockColumns;
        for (uint32_t left = x; left < x + columns; left++)
        {
            screenDirtyBlocks[topOffset + left] = 0;
        }
    }

    // Determine region in pixels
    uint32_t left = std::max<uint32_t>(0, x * _dirtyGrid.BlockWidth);
    uint32_t top = std::max<uint32_t>(0, y * _dirtyGrid.BlockHeight);
    uint32_t right = std::min(_width, left + (columns * _dirtyGrid.BlockWidth));
    uint32_t bottom = std::min(_height, top + (rows * _dirtyGrid.BlockHeight));
    if (right <= left || bottom <= top)
    {
        return;
    }

    // Draw region
    OnDrawDirtyBlock(x, y, columns, rows);
    WindowDrawAll(_bitsDPI, left, top, right, bottom);
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

X8DrawingContext::X8DrawingContext(X8DrawingEngine* engine)
{
    _engine = engine;
}

void X8DrawingContext::Clear(DrawPixelInfo& dpi, uint8_t paletteIndex)
{
    int32_t w = dpi.width;
    int32_t h = dpi.height;
    uint8_t* ptr = dpi.bits;

    for (int32_t y = 0; y < h; y++)
    {
        std::fill_n(ptr, w, paletteIndex);
        ptr += w + dpi.pitch;
    }
}

/** rct2: 0x0097FF04 */
// clang-format off
static constexpr uint16_t Pattern[] = {
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
static constexpr uint16_t PatternInverse[] = {
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
static constexpr const uint16_t* Patterns[] = {
    Pattern,
    PatternInverse,
};
// clang-format on

void X8DrawingContext::FillRect(DrawPixelInfo& dpi, uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    assert(dpi.zoom_level == ZoomLevel{ 0 });
    if (left > right)
        return;
    if (top > bottom)
        return;
    if (dpi.x > right)
        return;
    if (left >= dpi.x + dpi.width)
        return;
    if (bottom < dpi.y)
        return;
    if (top >= dpi.y + dpi.height)
        return;

    uint16_t crossPattern = 0;

    int32_t startX = left - dpi.x;
    if (startX < 0)
    {
        crossPattern ^= startX;
        startX = 0;
    }

    int32_t endX = right - dpi.x + 1;
    if (endX > dpi.width)
    {
        endX = dpi.width;
    }

    int32_t startY = top - dpi.y;
    if (startY < 0)
    {
        crossPattern ^= startY;
        startY = 0;
    }

    int32_t endY = bottom - dpi.y + 1;
    if (endY > dpi.height)
    {
        endY = dpi.height;
    }

    int32_t width = endX - startX;
    int32_t height = endY - startY;

    if (colour & 0x1000000)
    {
        // Cross hatching
        uint8_t* dst = startY * dpi.LineStride() + startX + dpi.bits;
        for (int32_t i = 0; i < height; i++)
        {
            uint8_t* nextdst = dst + dpi.LineStride();
            uint32_t p = Numerics::ror32(crossPattern, 1);
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
            crossPattern ^= 1;
            dst = nextdst;
        }
    }
    else if (colour & 0x2000000)
    {
        assert(false);
    }
    else if (colour & 0x4000000)
    {
        uint8_t* dst = startY * dpi.LineStride() + startX + dpi.bits;

        // The pattern loops every 15 lines this is which
        // part the pattern is on.
        int32_t patternY = (startY + dpi.y) % 16;

        // The pattern loops every 15 pixels this is which
        // part the pattern is on.
        int32_t startPatternX = (startX + dpi.x) % 16;
        int32_t patternX = startPatternX;

        const uint16_t* patternsrc = Patterns[colour >> 28]; // or possibly uint8_t)[esi*4] ?

        for (int32_t numLines = height; numLines > 0; numLines--)
        {
            uint8_t* nextdst = dst + dpi.LineStride();
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
            patternX = startPatternX;
            patternY = (patternY + 1) % 16;
            dst = nextdst;
        }
    }
    else
    {
        uint8_t* dst = startY * dpi.LineStride() + startX + dpi.bits;
        for (int32_t i = 0; i < height; i++)
        {
            std::fill_n(dst, width, colour & 0xFF);
            dst += dpi.LineStride();
        }
    }
}

void X8DrawingContext::FilterRect(
    DrawPixelInfo& dpi, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if (left > right)
        return;
    if (top > bottom)
        return;
    if (dpi.x > right)
        return;
    if (left >= dpi.x + dpi.width)
        return;
    if (bottom < dpi.y)
        return;
    if (top >= dpi.y + dpi.height)
        return;

    int32_t startX = left - dpi.x;
    if (startX < 0)
    {
        startX = 0;
    }

    int32_t endX = right - dpi.x + 1;
    if (endX > dpi.width)
    {
        endX = dpi.width;
    }

    int32_t startY = top - dpi.y;
    if (startY < 0)
    {
        startY = 0;
    }

    int32_t endY = bottom - dpi.y + 1;
    if (endY > dpi.height)
    {
        endY = dpi.height;
    }

    int32_t width = endX - startX;
    int32_t height = endY - startY;

    uint8_t* dst = dpi.bits + (startY * dpi.LineStride() + startX);

    // Find colour in colour table?
    auto paletteMap = GetPaletteMapForColour(EnumValue(palette));
    if (paletteMap.has_value())
    {
        const auto& paletteEntries = paletteMap.value();
        const int32_t scaled_width = width;
        const int32_t step = dpi.LineStride();

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

void X8DrawingContext::DrawLine(DrawPixelInfo& dpi, uint32_t colour, const ScreenLine& line)
{
    GfxDrawLineSoftware(dpi, line, colour);
}

void X8DrawingContext::DrawSprite(DrawPixelInfo& dpi, const ImageId imageId, int32_t x, int32_t y)
{
    GfxDrawSpriteSoftware(dpi, imageId, { x, y });
}

void X8DrawingContext::DrawSpriteRawMasked(
    DrawPixelInfo& dpi, int32_t x, int32_t y, const ImageId maskImage, const ImageId colourImage)
{
    GfxDrawSpriteRawMaskedSoftware(dpi, { x, y }, maskImage, colourImage);
}

void X8DrawingContext::DrawSpriteSolid(DrawPixelInfo& dpi, const ImageId image, int32_t x, int32_t y, uint8_t colour)
{
    uint8_t palette[256];
    std::fill_n(palette, sizeof(palette), colour);
    palette[0] = 0;

    const auto spriteCoords = ScreenCoordsXY{ x, y };
    GfxDrawSpritePaletteSetSoftware(dpi, ImageId(image.GetIndex(), 0), spriteCoords, PaletteMap(palette));
}

void X8DrawingContext::DrawGlyph(DrawPixelInfo& dpi, const ImageId image, int32_t x, int32_t y, const PaletteMap& paletteMap)
{
    GfxDrawSpritePaletteSetSoftware(dpi, image, { x, y }, paletteMap);
}

#ifndef NO_TTF
template<bool TUseHinting>
static void DrawTTFBitmapInternal(
    DrawPixelInfo& dpi, uint8_t colour, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
{
    assert(dpi.zoom_level == ZoomLevel{ 0 });
    const int32_t surfaceWidth = surface->w;
    int32_t width = surfaceWidth;
    int32_t height = surface->h;

    const int32_t overflowX = (dpi.x + dpi.width) - (x + width);
    const int32_t overflowY = (dpi.y + dpi.height) - (y + height);
    if (overflowX < 0)
        width += overflowX;
    if (overflowY < 0)
        height += overflowY;
    int32_t skipX = x - dpi.x;
    int32_t skipY = y - dpi.y;

    auto src = static_cast<const uint8_t*>(surface->pixels);
    uint8_t* dst = dpi.bits;

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
    dst += skipY * dpi.LineStride();

    const int32_t srcScanSkip = surfaceWidth - width;
    const int32_t dstScanSkip = dpi.LineStride() - width;
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
#endif // NO_TTF

void X8DrawingContext::DrawTTFBitmap(
    DrawPixelInfo& dpi, TextDrawInfo* info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
{
#ifndef NO_TTF
    const uint8_t fgColor = info->palette[1];
    const uint8_t bgColor = info->palette[3];

    if (info->flags & TEXT_DRAW_FLAG_OUTLINE)
    {
        DrawTTFBitmapInternal<false>(dpi, bgColor, surface, x + 1, y, 0);
        DrawTTFBitmapInternal<false>(dpi, bgColor, surface, x - 1, y, 0);
        DrawTTFBitmapInternal<false>(dpi, bgColor, surface, x, y + 1, 0);
        DrawTTFBitmapInternal<false>(dpi, bgColor, surface, x, y - 1, 0);
    }
    if (info->flags & TEXT_DRAW_FLAG_INSET)
    {
        DrawTTFBitmapInternal<false>(dpi, bgColor, surface, x + 1, y + 1, 0);
    }

    if (hintingThreshold > 0)
        DrawTTFBitmapInternal<true>(dpi, fgColor, surface, x, y, hintingThreshold);
    else
        DrawTTFBitmapInternal<false>(dpi, fgColor, surface, x, y, 0);
#endif // NO_TTF
}
