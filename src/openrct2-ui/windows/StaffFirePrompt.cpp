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
#include <openrct2/Game.h>
#include <openrct2/actions/StaffFireAction.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/interface/Colour.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>

static constexpr const StringId WINDOW_TITLE = STR_SACK_STAFF;
static constexpr const int32_t WW = 200;
static constexpr const int32_t WH = 100;

// clang-format off
enum WindowStaffFireWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_YES,
    WIDX_CANCEL
};

// 0x9AFB4C
static Widget window_staff_fire_widgets[] = {
    WINDOW_SHIM_WHITE(WINDOW_TITLE, WW, WH),
    MakeWidget({     10, WH - 20}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_YES               ),
    MakeWidget({WW - 95, WH - 20}, {85, 14}, WindowWidgetType::Button, WindowColour::Primary, STR_SAVE_PROMPT_CANCEL),
    WIDGETS_END,
};

// clang-format on

class StaffFirePromptWindow final : public Window
{
public:
    void SetWindowNumber(rct_windownumber windownumber)
    {
        number = windownumber;
    }

    void OnOpen() override
    {
        widgets = window_staff_fire_widgets;
        WindowInitScrollWidgets(*this);
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_YES:
            {
                auto staffFireAction = StaffFireAction(EntityId::FromUnderlying(number));
                GameActions::Execute(&staffFireAction);
                break;
            }
            case WIDX_CLOSE:
            case WIDX_CANCEL:
                Close();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        Peep* peep = GetEntity<Staff>(EntityId::FromUnderlying(number));
        auto ft = Formatter();
        peep->FormatNameTo(ft);

        ScreenCoordsXY textCoords(windowPos + ScreenCoordsXY{ WW / 2, (WH / 2) - 3 });
        DrawTextWrapped(&dpi, textCoords, WW - 4, STR_FIRE_STAFF_ID, ft, { TextAlignment::CENTRE });
    }
};

rct_window* WindowStaffFirePromptOpen(Peep* peep)
{
    // Check if the confirm window already exists
    auto* window = WindowFocusOrCreate<StaffFirePromptWindow>(
        WindowClass::FirePrompt, WW, WH, WF_CENTRE_SCREEN | WF_TRANSPARENT);
    window->SetWindowNumber(peep->sprite_index.ToUnderlying());
    return window;
}
