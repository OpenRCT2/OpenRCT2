/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include <openrct2/management/Marketing.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

static constexpr const int32_t WW = 200;
static constexpr const int32_t WH = 100;

// clang-format off
enum WindowRideCancelCampaignPromptWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_REFURBISH,
    WIDX_CANCEL
};

static Widget window_ride_refurbish_widgets[] =
{
    WINDOW_SHIM_WHITE(STR_CANCEL_MARKETING_TITLE, WW, WH),
    MakeWidget({ 10, WH - 22 }, { 85, 14 }, WindowWidgetType::Button, WindowColour::Primary, STR_YES),
    MakeWidget({ WW - 95, WH - 22 }, { 85, 14 }, WindowWidgetType::Button, WindowColour::Primary, STR_NO),
    WIDGETS_END,
};
// clang-format on

class CancelCampaignPromptWindow final : public Window
{
    int32_t _campaignType;

public:
    void SetCampaignType(int32_t id)
    {
        _campaignType = id;
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
                MarketingCancelCampaign(_campaignType);
                Close();
                break;
            }
            case WIDX_CANCEL:
            case WIDX_CLOSE:
                Close();
                break;
        }
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        WindowDrawWidgets(*this, dpi);

        ScreenCoordsXY stringCoords(windowPos.x + WW / 2, windowPos.y + (WH / 2) - 3);
        DrawTextWrapped(dpi, stringCoords, WW - 4, STR_CANCEL_MARKETING_CONFIRM, Formatter{}, { TextAlignment::CENTRE });
    }
};

WindowBase* WindowCancelCampaignPromptOpen(int32_t campaignType)
{
    WindowBase* w;
    CancelCampaignPromptWindow* newWindow;

    w = WindowFindByClass(WindowClass::CancelCampaignPrompt);
    if (w != nullptr)
    {
        auto windowPos = w->windowPos;
        WindowClose(*w);
        newWindow = WindowCreate<CancelCampaignPromptWindow>(
            WindowClass::CancelCampaignPrompt, windowPos, WW, WH, WF_TRANSPARENT);
    }
    else
    {
        newWindow = WindowCreate<CancelCampaignPromptWindow>(
            WindowClass::CancelCampaignPrompt, WW, WH, WF_CENTRE_SCREEN | WF_TRANSPARENT);
    }

    newWindow->SetCampaignType(campaignType);

    return newWindow;
}
