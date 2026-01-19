/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "NewDrawing.h"

#include "../Context.h"
#include "../drawing/Drawing.h"
#include "../interface/Screenshot.h"
#include "../localisation/StringIds.h"
#include "../paint/Painter.h"
#include "../platform/Platform.h"
#include "../ui/UiContext.h"
#include "../world/Location.hpp"
#include "IDrawingContext.h"
#include "IDrawingEngine.h"

#include <cassert>
#include <cmath>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

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
            auto& uiContext = context->GetUiContext();
            drawingEngine->Resize(uiContext.GetWidth(), uiContext.GetHeight());
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

RenderTarget& DrawingEngineGetRT()
{
    auto context = GetContext();
    auto drawingEngine = context->GetDrawingEngine();
    return *(drawingEngine->getRT());
}

bool DrawingEngineHasDirtyOptimisations()
{
    bool result = false;
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        result = drawingEngine->GetFlags().has(DrawingEngineFlag::dirtyOptimisations);
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

void GfxClear(RenderTarget& rt, PaletteIndex paletteIndex)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->Clear(rt, paletteIndex);
    }
}

void GfxDrawLine(RenderTarget& rt, const ScreenLine& line, PaletteIndex colour)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawLine(rt, colour, line);
    }
}

void GfxDrawDashedLine(
    RenderTarget& rt, const ScreenLine& screenLine, const int32_t dashedLineSegmentLength, const PaletteIndex colour)
{
    assert(dashedLineSegmentLength > 0);

    const auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        constexpr int32_t kPrecisionFactor = 1000;

        const int32_t dashedLineLength = std::hypot(
            screenLine.GetX2() - screenLine.GetX1(), screenLine.GetY2() - screenLine.GetY1());
        const int32_t lineSegmentCount = dashedLineLength / dashedLineSegmentLength / 2;
        if (lineSegmentCount == 0)
        {
            return;
        }

        const int32_t lineXDist = std::abs(screenLine.GetX2() - screenLine.GetX1());
        const int32_t lineYDist = std::abs(screenLine.GetY2() - screenLine.GetY1());
        const int32_t dxPrecise = kPrecisionFactor * lineXDist / lineSegmentCount / 2;
        const int32_t dyPrecise = kPrecisionFactor * lineYDist / lineSegmentCount / 2;
        IDrawingContext* dc = drawingEngine->GetDrawingContext();

        for (int32_t i = 0, x, y; i < lineSegmentCount; ++i)
        {
            x = screenLine.GetX1() + dxPrecise * i * 2 / kPrecisionFactor;
            y = screenLine.GetY1() + dyPrecise * i * 2 / kPrecisionFactor;
            dc->DrawLine(rt, colour, { { x, y }, { x + dxPrecise / kPrecisionFactor, y + dyPrecise / kPrecisionFactor } });
        }
    }
}

void FASTCALL GfxDrawSprite(RenderTarget& rt, const ImageId imageId, const ScreenCoordsXY& coords)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawSprite(rt, imageId, coords.x, coords.y);
    }
}

void FASTCALL GfxDrawGlyph(RenderTarget& rt, const ImageId image, const ScreenCoordsXY& coords, const PaletteMap& paletteMap)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawGlyph(rt, image, coords.x, coords.y, paletteMap);
    }
}

void FASTCALL
    GfxDrawSpriteRawMasked(RenderTarget& rt, const ScreenCoordsXY& coords, const ImageId maskImage, const ImageId colourImage)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawSpriteRawMasked(rt, coords.x, coords.y, maskImage, colourImage);
    }
}

void FASTCALL GfxDrawSpriteSolid(RenderTarget& rt, const ImageId image, const ScreenCoordsXY& coords, PaletteIndex colour)
{
    auto drawingEngine = rt.DrawingEngine;
    if (drawingEngine != nullptr)
    {
        IDrawingContext* dc = drawingEngine->GetDrawingContext();
        dc->DrawSpriteSolid(rt, image, coords.x, coords.y, colour);
    }
}

std::string ScreenshotDump()
{
    auto drawingEngine = GetDrawingEngine();
    if (drawingEngine != nullptr)
    {
        return drawingEngine->Screenshot();
    }
    return {};
}
