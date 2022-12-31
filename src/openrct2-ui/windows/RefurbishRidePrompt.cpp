/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

static constexpr const int32_t WW = 200;
static constexpr const int32_t WH = 100;

// clang-format off
enum WindowRideRefurbishWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_REFURBISH,
    WIDX_CANCEL
};

static Widget window_ride_refurbish_widgets[] =
{
    WINDOW_SHIM_WHITE(STR_REFURBISH_RIDE, WW, WH),
    MakeWidget({ 10, WH - 22 }, { 85, 14 }, WindowWidgetType::Button, WindowColour::Primary, STR_REFURBISH),
    MakeWidget({ WW - 95, WH - 22 }, { 85, 14 }, WindowWidgetType::Button, WindowColour::Primary, STR_SAVE_PROMPT_CANCEL),
    WIDGETS_END,
};
// clang-format on

class RefurbishRidePromptWindow final : public Window
{
    money32 _demolishRideCost;

public:
    void SetRide(Ride* currentRide)
    {
        rideId = currentRide->id;
        _demolishRideCost = -ride_get_refund_price(currentRide);
    }

    void OnOpen() override
    {
        widgets = window_ride_refurbish_widgets;
        WindowInitScrollWidgets(*this);
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_REFURBISH:
            {
                auto gameAction = RideDemolishAction(rideId, RIDE_MODIFY_RENEW);
                GameActions::Execute(&gameAction);
                break;
            }
            case WIDX_CANCEL:
            case WIDX_CLOSE:
                Close();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        WindowDrawWidgets(*this, &dpi);

        auto currentRide = get_ride(rideId);
        if (currentRide != nullptr)
        {
            auto stringId = (gParkFlags & PARK_FLAGS_NO_MONEY) ? STR_REFURBISH_RIDE_ID_NO_MONEY : STR_REFURBISH_RIDE_ID_MONEY;
            auto ft = Formatter();
            currentRide->FormatNameTo(ft);
            ft.Add<money64>(_demolishRideCost / 2);

            ScreenCoordsXY stringCoords(windowPos.x + WW / 2, windowPos.y + (WH / 2) - 3);
            DrawTextWrapped(&dpi, stringCoords, WW - 4, stringId, ft, { TextAlignment::CENTRE });
        }
    }
};

rct_window* WindowRideRefurbishPromptOpen(Ride* ride)
{
    rct_window* w;
    RefurbishRidePromptWindow* newWindow;

    w = window_find_by_class(WindowClass::DemolishRidePrompt);
    if (w != nullptr)
    {
        auto windowPos = w->windowPos;
        window_close(*w);
        newWindow = WindowCreate<RefurbishRidePromptWindow>(WindowClass::DemolishRidePrompt, windowPos, WW, WH, WF_TRANSPARENT);
    }
    else
    {
        newWindow = WindowCreate<RefurbishRidePromptWindow>(
            WindowClass::DemolishRidePrompt, WW, WH, WF_CENTRE_SCREEN | WF_TRANSPARENT);
    }

    newWindow->SetRide(ride);

    return newWindow;
}
