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
#include "../core/Guard.hpp"
#include "../drawing/IDrawingEngine.h"
#include "../drawing/Text.h"
#include "../localisation/Formatting.h"
#include "../paint/Paint.h"
#include "../paint/VirtualFloor.h"
#include "../profiling/Profiling.h"
#include "../scenes/intro/IntroScene.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/TileInspector.h"

#include <string>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Paint;
using namespace OpenRCT2::Ui;

Painter::Painter(IUiContext& uiContext)
    : _uiContext(uiContext)
{
}

void Painter::Paint(IDrawingEngine& de)
{
    PROFILED_FUNCTION();

    auto rt = de.GetDrawingPixelInfo();

    if (IntroIsPlaying())
    {
        IntroDraw(*rt);
    }
    else
    {
        VirtualFloorInvalidate(false);

        de.PaintWindows();

        UpdatePaletteEffects();
        _uiContext.Draw(*rt);

        GfxDrawPickedUpPeep(*rt);
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
        PaintReplayNotice(*rt, text);

    if (Config::Get().general.ShowFPS)
    {
        PaintFPS(*rt);
    }
    gCurrentDrawCount++;
}

void Painter::PaintReplayNotice(RenderTarget& rt, const char* text)
{
    ScreenCoordsXY screenCoords(_uiContext.GetWidth() / 2, _uiContext.GetHeight() - 44);

    char buffer[64]{};
    FormatStringToBuffer(buffer, sizeof(buffer), "{OUTLINE}{RED}{STRING}", text);

    auto stringWidth = GfxGetStringWidth(buffer, FontStyle::Medium);
    screenCoords.x = screenCoords.x - stringWidth;

    if (((getGameState().currentTicks >> 1) & 0xF) > 4)
        DrawText(rt, screenCoords, { COLOUR_SATURATED_RED }, buffer);

    // Make area dirty so the text doesn't get drawn over the last
    GfxSetDirtyBlocks({ screenCoords, screenCoords + ScreenCoordsXY{ stringWidth, 16 } });
}

void Painter::PaintFPS(RenderTarget& rt)
{
    MeasureFPS();
    using namespace std::string_literals;
    ContextSetWindowTitle("OpenRCT2 FPS: "s + std::to_string(_currentFPS));
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

PaintSession* Painter::CreateSession(RenderTarget& rt, uint32_t viewFlags, uint8_t rotation)
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

    session->DPI = rt;
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
