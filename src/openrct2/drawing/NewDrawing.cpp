/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewDrawing.h"

#include "../Context.h"
#include "../config/Config.h"
#include "../drawing/Drawing.h"
#include "../interface/Screenshot.h"
#include "../localisation/StringIds.h"
#include "../paint/Painter.h"
#include "../platform/Platform.h"
#include "../ui/UiContext.h"
#include "../world/Location.hpp"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"

#include <cmath>
namespace OpenRCT2
{
    using namespace Drawing;
    using namespace Paint;
    using namespace Ui;

    StringId DrawingEngineStringIds[] = {
        STR_DRAWING_ENGINE_SOFTWARE,
        STR_DRAWING_ENGINE_SOFTWARE_WITH_HARDWARE_DISPLAY,
        STR_DRAWING_ENGINE_OPENGL,
    };

    DrawingEngine drawing_engine_get_type()
    {
        auto context = GetContext();
        return context->GetDrawingEngineType();
    }

    static IDrawingEngine* GetDrawingEngine()
    {
        IDrawingEngine* result = nullptr;
        auto context = GetContext();
        if (context != nullptr)
        {
            result = context->GetDrawingEngine();
        }
        return result;
    }

    bool DrawingEngineRequiresNewWindow(DrawingEngine srcEngine, DrawingEngine dstEngine)
    {
        bool openGL = srcEngine == DrawingEngine::OpenGL || dstEngine == DrawingEngine::OpenGL;
        return Platform::RequireNewWindow(openGL);
    }

    void DrawingEngineInit()
    {
        auto context = GetContext();
        if (context != nullptr)
        {
            context->InitialiseDrawingEngine();
        }
    }

    void DrawingEngineResize()
    {
        auto context = GetContext();
        if (context != nullptr)
        {
            auto drawingEngine = context->GetDrawingEngine();
            if (drawingEngine != nullptr)
            {
                auto uiContext = context->GetUiContext();
                drawingEngine->Resize(uiContext->GetWidth(), uiContext->GetHeight());
            }
        }
    }

    void DrawingEngineSetPalette(const GamePalette& colours)
    {
        auto context = GetContext();
        if (context != nullptr)
        {
            auto drawingEngine = context->GetDrawingEngine();
            if (drawingEngine != nullptr)
            {
                drawingEngine->SetPalette(colours);
            }
        }
    }

    void DrawingEngineCopyRect(int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy)
    {
        auto context = GetContext();
        if (context != nullptr)
        {
            auto drawingEngine = context->GetDrawingEngine();
            if (drawingEngine != nullptr)
            {
                drawingEngine->CopyRect(x, y, width, height, dx, dy);
            }
        }
    }

    void DrawingEngineDispose()
    {
        auto context = GetContext();
        if (context != nullptr)
        {
            context->DisposeDrawingEngine();
        }
    }

    DrawPixelInfo* DrawingEngineGetDpi()
    {
        auto context = GetContext();
        auto drawingEngine = context->GetDrawingEngine();
        return drawingEngine->GetDrawingPixelInfo();
    }

    bool DrawingEngineHasDirtyOptimisations()
    {
        bool result = false;
        auto drawingEngine = GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            result = (drawingEngine->GetFlags() & DEF_DIRTY_OPTIMISATIONS);
        }
        return result;
    }

    void DrawingEngineInvalidateImage(uint32_t image)
    {
        auto drawingEngine = GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            drawingEngine->InvalidateImage(image);
        }
    }

    void DrawingEngineSetVSync(bool vsync)
    {
        auto drawingEngine = GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            drawingEngine->SetVSync(vsync);
        }
    }

    void GfxSetDirtyBlocks(const ScreenRect& rect)
    {
        auto drawingEngine = GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            drawingEngine->Invalidate(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        }
    }

    void GfxClear(DrawPixelInfo* dpi, uint8_t paletteIndex)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->Clear(dpi, paletteIndex);
        }
    }

    void GfxFillRect(DrawPixelInfo* dpi, const ScreenRect& rect, int32_t colour)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->FillRect(dpi, colour, rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        }
    }

    void GfxFilterRect(DrawPixelInfo* dpi, const ScreenRect& rect, FilterPaletteID palette)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->FilterRect(dpi, palette, rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        }
    }

    void GfxDrawLine(DrawPixelInfo* dpi, const ScreenLine& line, int32_t colour)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->DrawLine(dpi, colour, line);
        }
    }

    void GfxDrawDashedLine(
        DrawPixelInfo* dpi, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const int32_t color)
    {
        assert(dashedLineSegmentLength > 0);

        const auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            constexpr int32_t precisionFactor = 1000;

            const int32_t dashedLineLength = std::hypot(
                screenLine.GetX2() - screenLine.GetX1(), screenLine.GetY2() - screenLine.GetY1());
            const int32_t lineSegmentCount = dashedLineLength / dashedLineSegmentLength / 2;
            if (lineSegmentCount == 0)
            {
                return;
            }

            const int32_t lineXDist = std::abs(screenLine.GetX2() - screenLine.GetX1());
            const int32_t lineYDist = std::abs(screenLine.GetY2() - screenLine.GetY1());
            const int32_t dxPrecise = precisionFactor * lineXDist / lineSegmentCount / 2;
            const int32_t dyPrecise = precisionFactor * lineYDist / lineSegmentCount / 2;
            IDrawingContext* dc = drawingEngine->GetDrawingContext();

            for (int32_t i = 0, x, y; i < lineSegmentCount; ++i)
            {
                x = screenLine.GetX1() + dxPrecise * i * 2 / precisionFactor;
                y = screenLine.GetY1() + dyPrecise * i * 2 / precisionFactor;
                dc->DrawLine(dpi, color, { { x, y }, { x + dxPrecise / precisionFactor, y + dyPrecise / precisionFactor } });
            }
        }
    }

    void FASTCALL GfxDrawSprite(DrawPixelInfo* dpi, const ImageId imageId, const ScreenCoordsXY& coords)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->DrawSprite(dpi, imageId, coords.x, coords.y);
        }
    }

    void FASTCALL
        GfxDrawGlyph(DrawPixelInfo* dpi, const ImageId image, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->DrawGlyph(dpi, image, coords.x, coords.y, paletteMap);
        }
    }

    void FASTCALL GfxDrawSpriteRawMasked(
        DrawPixelInfo* dpi, const ScreenCoordsXY& coords, const ImageId maskImage, const ImageId colourImage)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->DrawSpriteRawMasked(dpi, coords.x, coords.y, maskImage, colourImage);
        }
    }

    void FASTCALL GfxDrawSpriteSolid(DrawPixelInfo* dpi, const ImageId image, const ScreenCoordsXY& coords, uint8_t colour)
    {
        auto drawingEngine = dpi->DrawingEngine;
        if (drawingEngine != nullptr)
        {
            IDrawingContext* dc = drawingEngine->GetDrawingContext();
            dc->DrawSpriteSolid(dpi, image, coords.x, coords.y, colour);
        }
    }

    std::string ScreenshotDump()
    {
        auto drawingEngine = GetDrawingEngine();
        if (drawingEngine != nullptr)
        {
            return drawingEngine->Screenshot();
        }
        return "";
    }
} // namespace OpenRCT2
