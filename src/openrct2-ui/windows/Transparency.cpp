/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ParkSetDateAction.h>
#include <openrct2/actions/SetCheatAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

// clang-format off
enum WINDOW_TRANSPARENCY_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,

    WIDX_SEE_THROUGH_RIDES,
    WIDX_SEE_THROUGH_VEHICLES,
    WIDX_SEE_THROUGH_SCENERY,
    WIDX_SEE_THROUGH_TREES,
    WIDX_SEE_THROUGH_PATHS,
    WIDX_SEE_THROUGH_SUPPORTS,
    WIDX_INVISIBLE_GUESTS,
    WIDX_INVISIBLE_STAFF,
    WIDX_INVISIBLE_RIDES,
    WIDX_INVISIBLE_VEHICLES,
    WIDX_INVISIBLE_SCENERY,
    WIDX_INVISIBLE_TREES,
    WIDX_INVISIBLE_PATHS,
    WIDX_INVISIBLE_SUPPORTS,
};

#pragma region MEASUREMENTS

static constexpr const rct_string_id WINDOW_TITLE = STR_TRANSPARENCY_OPTIONS_TITLE;
static constexpr const int32_t WW = 204;
static constexpr const int32_t WH = 57;

static constexpr ScreenSize ICON_BUTTON = {24, 24};
static constexpr ScreenSize FLAT_BUTTON = {24, 12};

#pragma endregion

static rct_widget window_transparency_main_widgets[] =
{
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 77, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_RIDE,                       STR_SEE_THROUGH_RIDES),
    MakeWidget({102, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_COASTER_TRAIN,    STR_SEE_THROUGH_VEHICLES),
    MakeWidget({ 27, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_LARGE_SCENERY,    STR_SEE_THROUGH_SCENERY),
    MakeWidget({  2, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_TREES,            STR_SEE_THROUGH_TREES),
    MakeWidget({ 52, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_FOOTPATH,         STR_SEE_THROUGH_PATHS),
    MakeWidget({127, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_SUPPORTS,         STR_SEE_THROUGH_SUPPORTS),
    MakeWidget({152, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_GUESTS,                     STR_INVISIBLE_GUESTS),
    MakeWidget({177, 17}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                     STR_INVISIBLE_STAFF),

    MakeWidget({ 77,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_RIDES),
    MakeWidget({102,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_VEHICLES),
    MakeWidget({ 27,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SCENERY),
    MakeWidget({  2,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_TREES),
    MakeWidget({ 52,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_PATHS),
    MakeWidget({127,  42}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SUPPORTS),

    { WIDGETS_END },
};
// clang-format on

class TransparencyWindow final : public Window
{
private:
public:
    void OnOpen() override
    {
        widgets = window_transparency_main_widgets;
        window_push_others_below(this);

        auto* w = window_get_main();
        if (w != nullptr)
            windowPos.x = ((w->width / 2) - (width / 2));
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            default:
                ToggleViewportFlag(widgetIndex);
                break;
        }
    }

    void OnPrepareDraw() override
    {
        uint32_t wflags = 0;
        rct_window* w = window_get_main();

        pressed_widgets = 0;
        disabled_widgets = 0;

        if (w != nullptr)
            wflags = w->viewport->flags;

        SetWidgetPressed(WIDX_SEE_THROUGH_RIDES, (wflags & VIEWPORT_FLAG_HIDE_RIDES));
        SetWidgetPressed(WIDX_SEE_THROUGH_VEHICLES, (wflags & VIEWPORT_FLAG_HIDE_VEHICLES));
        SetWidgetPressed(WIDX_SEE_THROUGH_SCENERY, (wflags & VIEWPORT_FLAG_HIDE_SCENERY));
        SetWidgetPressed(WIDX_SEE_THROUGH_TREES, (wflags & VIEWPORT_FLAG_HIDE_TREES));
        SetWidgetPressed(WIDX_SEE_THROUGH_PATHS, (wflags & VIEWPORT_FLAG_HIDE_PATHS));
        SetWidgetPressed(WIDX_SEE_THROUGH_SUPPORTS, (wflags & VIEWPORT_FLAG_HIDE_SUPPORTS));
        SetWidgetPressed(WIDX_INVISIBLE_RIDES, (wflags & VIEWPORT_FLAG_INVISIBLE_RIDES));
        SetWidgetPressed(WIDX_INVISIBLE_VEHICLES, (wflags & VIEWPORT_FLAG_INVISIBLE_VEHICLES));
        SetWidgetPressed(WIDX_INVISIBLE_SCENERY, (wflags & VIEWPORT_FLAG_INVISIBLE_SCENERY));
        SetWidgetPressed(WIDX_INVISIBLE_TREES, (wflags & VIEWPORT_FLAG_INVISIBLE_TREES));
        SetWidgetPressed(WIDX_INVISIBLE_PATHS, (wflags & VIEWPORT_FLAG_INVISIBLE_PATHS));
        SetWidgetPressed(WIDX_INVISIBLE_SUPPORTS, (wflags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS));
        SetWidgetPressed(WIDX_INVISIBLE_GUESTS, (wflags & VIEWPORT_FLAG_HIDE_GUESTS));
        SetWidgetPressed(WIDX_INVISIBLE_STAFF, (wflags & VIEWPORT_FLAG_HIDE_STAFF));
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        // Locate mechanic button image
        const auto& widget = widgets[WIDX_INVISIBLE_STAFF];
        auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };
        gfx_draw_sprite(
            &dpi, (gStaffMechanicColour << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS | SPR_MECHANIC, screenCoords, 0);
    }

private:
    void ToggleViewportFlag(rct_widgetindex widgetIndex)
    {
        uint32_t wflags = 0;
        rct_window* w = window_get_main();

        if (w == nullptr)
            return;

        wflags = w->viewport->flags;

        switch (widgetIndex)
        {
            case WIDX_SEE_THROUGH_RIDES:
                wflags ^= VIEWPORT_FLAG_HIDE_RIDES;
                break;
            case WIDX_SEE_THROUGH_VEHICLES:
                wflags ^= VIEWPORT_FLAG_HIDE_VEHICLES;
                break;
            case WIDX_SEE_THROUGH_SCENERY:
                wflags ^= VIEWPORT_FLAG_HIDE_SCENERY;
                break;
            case WIDX_SEE_THROUGH_TREES:
                wflags ^= VIEWPORT_FLAG_HIDE_TREES;
                break;
            case WIDX_SEE_THROUGH_PATHS:
                wflags ^= VIEWPORT_FLAG_HIDE_PATHS;
                break;
            case WIDX_SEE_THROUGH_SUPPORTS:
                wflags ^= VIEWPORT_FLAG_HIDE_SUPPORTS;
                break;
            case WIDX_INVISIBLE_RIDES:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_RIDES;
                break;
            case WIDX_INVISIBLE_VEHICLES:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_VEHICLES;
                break;
            case WIDX_INVISIBLE_SCENERY:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_SCENERY;
                break;
            case WIDX_INVISIBLE_TREES:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_TREES;
                break;
            case WIDX_INVISIBLE_PATHS:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_PATHS;
                break;
            case WIDX_INVISIBLE_SUPPORTS:
                wflags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
                break;
            case WIDX_INVISIBLE_GUESTS:
                wflags ^= VIEWPORT_FLAG_HIDE_GUESTS;
                break;
            case WIDX_INVISIBLE_STAFF:
                wflags ^= VIEWPORT_FLAG_HIDE_STAFF;
                break;
            default:
                return;
                break;
        }

        if (w->viewport->flags == wflags)
            return;

        w->viewport->flags = wflags;
        w->Invalidate();
    }
};

rct_window* WindowTransparencyOpen()
{
    auto* window = window_bring_to_front_by_class(WC_TRANSPARENCY);
    if (window == nullptr)
        window = WindowCreate<TransparencyWindow>(WC_TRANSPARENCY, ScreenCoordsXY(32, 32), WW, WH);

    return window;
}
