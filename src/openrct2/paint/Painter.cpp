/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Painter.h"

#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ReplayManager.h"
#include "../config/Config.h"
#include "../drawing/IDrawingEngine.h"
#include "../drawing/Text.h"
#include "../localisation/Formatting.h"
#include "../localisation/Language.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../scenes/intro/IntroScene.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/TileInspector.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

Painter::Painter(const std::shared_ptr<IUiContext>& uiContext)
    : _uiContext(uiContext)
{
}

void Painter::Paint(IDrawingEngine& de)
{
    PROFILED_FUNCTION();

    auto dpi = de.GetDrawingPixelInfo();

    if (IntroIsPlaying())
    {
        IntroDraw(*dpi);
    }
    else
    {
        de.PaintWindows();

        UpdatePaletteEffects();
        _uiContext->Draw(*dpi);

        GfxDrawPickedUpPeep(*dpi);
        GfxInvalidatePickedUpPeep();

        de.PaintWeather();
    }

    auto* replayManager = GetContext()->GetReplayManager();
    const char* text = nullptr;

    if (replayManager->IsReplaying() && !gSilentReplays)
        text = "Replaying...";
    else if (replayManager->ShouldDisplayNotice())
        text = "Recording...";
    else if (replayManager->IsNormalising())
        text = "Normalising...";

    if (text != nullptr)
        PaintReplayNotice(*dpi, text);

    if (Config::Get().general.ShowFPS)
    {
        PaintFPS(*dpi);
    }
    gCurrentDrawCount++;
}

void Painter::PaintReplayNotice(DrawPixelInfo& dpi, const char* text)
{
    ScreenCoordsXY screenCoords(_uiContext->GetWidth() / 2, _uiContext->GetHeight() - 44);

    char buffer[64]{};
    FormatStringToBuffer(buffer, sizeof(buffer), "{OUTLINE}{RED}{STRING}", text);

    auto stringWidth = GfxGetStringWidth(buffer, FontStyle::Medium);
    screenCoords.x = screenCoords.x - stringWidth;

    if (((GetGameState().CurrentTicks >> 1) & 0xF) > 4)
        DrawText(dpi, screenCoords, { COLOUR_SATURATED_RED }, buffer);

    // Make area dirty so the text doesn't get drawn over the last
    GfxSetDirtyBlocks({ screenCoords, screenCoords + ScreenCoordsXY{ stringWidth, 16 } });
}

static bool ShouldShowFPS()
{
    if (gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)
        return true;

    auto* windowMgr = Ui::GetWindowManager();
    return windowMgr->FindByClass(WindowClass::TopToolbar);
}

void Painter::PaintFPS(DrawPixelInfo& dpi)
{
    if (!ShouldShowFPS())
        return;

    MeasureFPS();

    char buffer[64]{};
    FormatStringToBuffer(buffer, sizeof(buffer), "{OUTLINE}{WHITE}{INT32}", _currentFPS);
    const int32_t stringWidth = GfxGetStringWidth(buffer, FontStyle::Medium);

    // Figure out where counter should be rendered
    ScreenCoordsXY screenCoords(_uiContext->GetWidth() / 2, 2);
    screenCoords.x = screenCoords.x - (stringWidth / 2);

    // Move counter below toolbar if buttons are centred
    const bool isTitle = gScreenFlags == SCREEN_FLAGS_TITLE_DEMO;
    if (!isTitle && Config::Get().interface.ToolbarButtonsCentred)
    {
        screenCoords.y = kTopToolbarHeight + 3;
    }

    DrawText(dpi, screenCoords, { COLOUR_WHITE }, buffer);

    // Make area dirty so the text doesn't get drawn over the last
    GfxSetDirtyBlocks({ { screenCoords - ScreenCoordsXY{ 16, 4 } }, { dpi.lastStringPos.x + 16, screenCoords.y + 16 } });
}

void Painter::MeasureFPS()
{
    _frames++;

    auto currentTime = time(nullptr);
    if (currentTime != _lastSecond)
    {
        _currentFPS = _frames;
        _frames = 0;
    }
    _lastSecond = currentTime;
}

PaintSession* Painter::CreateSession(DrawPixelInfo& dpi, uint32_t viewFlags, uint8_t rotation)
{
    PROFILED_FUNCTION();

    PaintSession* session = nullptr;

    if (_freePaintSessions.empty() == false)
    {
        // Re-use.
        session = _freePaintSessions.back();

        // Shrink by one.
        _freePaintSessions.pop_back();
    }
    else
    {
        // Create new one in pool.
        session = &_paintSessionPool.emplace_back();
    }

    session->DPI = dpi;
    session->ViewFlags = viewFlags;
    session->QuadrantBackIndex = std::numeric_limits<uint32_t>::max();
    session->QuadrantFrontIndex = 0;
    session->Flags = 0;
    session->CurrentRotation = rotation;

    std::fill(std::begin(session->Quadrants), std::end(session->Quadrants), nullptr);
    session->PaintHead = nullptr;
    session->LastPS = nullptr;
    session->LastAttachedPS = nullptr;
    session->PSStringHead = nullptr;
    session->LastPSString = nullptr;
    session->WoodenSupportsPrependTo = nullptr;
    session->CurrentlyDrawnEntity = nullptr;
    session->CurrentlyDrawnTileElement = nullptr;
    session->Surface = nullptr;
    session->SelectedElement = OpenRCT2::TileInspector::GetSelectedElement();

    return session;
}

void Painter::ReleaseSession(PaintSession* session)
{
    PROFILED_FUNCTION();

    session->paintEntries.clear();

    _freePaintSessions.push_back(session);
}

Painter::~Painter()
{
    _paintSessionPool.clear();
}
