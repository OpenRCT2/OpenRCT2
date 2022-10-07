/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

    WIDX_HIDE_VEGETATION,
    WIDX_HIDE_SCENERY,
    WIDX_HIDE_PATHS,
    WIDX_HIDE_RIDES,
    WIDX_HIDE_VEHICLES,
    WIDX_HIDE_SUPPORTS,
    WIDX_HIDE_GUESTS,
    WIDX_HIDE_STAFF,
    WIDX_INVISIBLE_VEGETATION,
    WIDX_INVISIBLE_SCENERY,
    WIDX_INVISIBLE_PATHS,
    WIDX_INVISIBLE_RIDES,
    WIDX_INVISIBLE_VEHICLES,
    WIDX_INVISIBLE_SUPPORTS,
};

#pragma region MEASUREMENTS

static constexpr const StringId WINDOW_TITLE = STR_TRANSPARENCY_OPTIONS_TITLE;
static constexpr const int32_t WW = 204;
static constexpr const int32_t WH = 57;

static constexpr ScreenSize HIDE_SIZE = {24, 24};
static constexpr ScreenSize INVISIBLE_SIZE = {24, 12};

#pragma endregion

static rct_widget window_transparency_main_widgets[] =
{
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  2, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_HIDE_VEGETATION,  STR_SEE_THROUGH_VEGETATION),
    MakeWidget({ 27, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_HIDE_SCENERY,     STR_SEE_THROUGH_SCENERY),
    MakeWidget({ 52, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_FOOTPATH,         STR_SEE_THROUGH_PATHS),
    MakeWidget({ 77, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_RIDE,                       STR_SEE_THROUGH_RIDES),
    MakeWidget({102, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_HIDE_VEHICLES,    STR_SEE_THROUGH_VEHICLES),
    MakeWidget({127, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_G2_BUTTON_HIDE_SUPPORTS,    STR_SEE_THROUGH_SUPPORTS),
    MakeWidget({152, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_GUESTS,                     STR_SEE_THROUGH_GUESTS),
    MakeWidget({177, 17}, HIDE_SIZE,      WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_SEE_THROUGH_STAFF),

    MakeWidget({  2, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_VEGETATION),
    MakeWidget({ 27, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SCENERY),
    MakeWidget({ 52, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_PATHS),
    MakeWidget({ 77, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_RIDES),
    MakeWidget({102, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_VEHICLES),
    MakeWidget({127, 42}, INVISIBLE_SIZE, WindowWidgetType::FlatBtn, WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SUPPORTS),

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
        window_push_others_below(*this);

        auto* w = window_get_main();
        if (w != nullptr)
            windowPos.x = ((w->width / 2) - (width / 2));
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
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

        SetWidgetPressed(WIDX_HIDE_VEGETATION, (wflags & VIEWPORT_FLAG_HIDE_VEGETATION));
        SetWidgetPressed(WIDX_HIDE_SCENERY, (wflags & VIEWPORT_FLAG_HIDE_SCENERY));
        SetWidgetPressed(WIDX_HIDE_PATHS, (wflags & VIEWPORT_FLAG_HIDE_PATHS));
        SetWidgetPressed(WIDX_HIDE_RIDES, (wflags & VIEWPORT_FLAG_HIDE_RIDES));
        SetWidgetPressed(WIDX_HIDE_VEHICLES, (wflags & VIEWPORT_FLAG_HIDE_VEHICLES));
        SetWidgetPressed(WIDX_HIDE_SUPPORTS, (wflags & VIEWPORT_FLAG_HIDE_SUPPORTS));
        SetWidgetPressed(WIDX_HIDE_GUESTS, (wflags & VIEWPORT_FLAG_HIDE_GUESTS));
        SetWidgetPressed(WIDX_HIDE_STAFF, (wflags & VIEWPORT_FLAG_HIDE_STAFF));
        SetWidgetPressed(WIDX_INVISIBLE_VEGETATION, (wflags & VIEWPORT_FLAG_INVISIBLE_VEGETATION));
        SetWidgetPressed(WIDX_INVISIBLE_SCENERY, (wflags & VIEWPORT_FLAG_INVISIBLE_SCENERY));
        SetWidgetPressed(WIDX_INVISIBLE_PATHS, (wflags & VIEWPORT_FLAG_INVISIBLE_PATHS));
        SetWidgetPressed(WIDX_INVISIBLE_RIDES, (wflags & VIEWPORT_FLAG_INVISIBLE_RIDES));
        SetWidgetPressed(WIDX_INVISIBLE_VEHICLES, (wflags & VIEWPORT_FLAG_INVISIBLE_VEHICLES));
        SetWidgetPressed(WIDX_INVISIBLE_SUPPORTS, (wflags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS));

        for (WidgetIndex i = WIDX_INVISIBLE_VEGETATION; i <= WIDX_INVISIBLE_SUPPORTS; i++)
        {
            widgets[i].image = IsWidgetPressed(i) ? SPR_G2_BUTTON_HIDE_FULL : SPR_G2_BUTTON_HIDE_PARTIAL;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        // Locate mechanic button image
        const auto& widget = widgets[WIDX_HIDE_STAFF];
        auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };
        auto image = ImageId(SPR_MECHANIC, COLOUR_BLACK, gStaffMechanicColour);
        gfx_draw_sprite(&dpi, image, screenCoords);
    }

private:
    uint32_t ToggleTransparency(uint32_t wflags, uint32_t transparencyFlag, uint32_t seeThroughFlag)
    {
        wflags ^= transparencyFlag;
        if (wflags & transparencyFlag)
        {
            wflags |= seeThroughFlag;
        }
        SaveInConfig(wflags);
        return wflags;
    }

    void ToggleViewportFlag(WidgetIndex widgetIndex)
    {
        uint32_t wflags = 0;
        rct_window* w = window_get_main();

        if (w == nullptr)
            return;

        wflags = w->viewport->flags;

        switch (widgetIndex)
        {
            case WIDX_HIDE_RIDES:
                wflags ^= VIEWPORT_FLAG_HIDE_RIDES;
                break;
            case WIDX_HIDE_VEHICLES:
                wflags ^= VIEWPORT_FLAG_HIDE_VEHICLES;
                break;
            case WIDX_HIDE_SCENERY:
                wflags ^= VIEWPORT_FLAG_HIDE_SCENERY;
                break;
            case WIDX_HIDE_VEGETATION:
                wflags ^= VIEWPORT_FLAG_HIDE_VEGETATION;
                break;
            case WIDX_HIDE_PATHS:
                wflags ^= VIEWPORT_FLAG_HIDE_PATHS;
                break;
            case WIDX_HIDE_SUPPORTS:
                wflags ^= VIEWPORT_FLAG_HIDE_SUPPORTS;
                break;
            case WIDX_INVISIBLE_RIDES:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_RIDES, VIEWPORT_FLAG_HIDE_RIDES);
                break;
            case WIDX_INVISIBLE_VEHICLES:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_VEHICLES, VIEWPORT_FLAG_HIDE_VEHICLES);
                break;
            case WIDX_INVISIBLE_SCENERY:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_SCENERY, VIEWPORT_FLAG_HIDE_SCENERY);
                break;
            case WIDX_INVISIBLE_VEGETATION:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_VEGETATION, VIEWPORT_FLAG_HIDE_VEGETATION);
                break;
            case WIDX_INVISIBLE_PATHS:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_PATHS, VIEWPORT_FLAG_HIDE_PATHS);
                break;
            case WIDX_INVISIBLE_SUPPORTS:
                wflags = ToggleTransparency(wflags, VIEWPORT_FLAG_INVISIBLE_SUPPORTS, VIEWPORT_FLAG_HIDE_SUPPORTS);
                break;
            case WIDX_HIDE_GUESTS:
                wflags ^= VIEWPORT_FLAG_HIDE_GUESTS;
                break;
            case WIDX_HIDE_STAFF:
                wflags ^= VIEWPORT_FLAG_HIDE_STAFF;
                break;
            default:
                return;
        }

        if (w->viewport->flags == wflags)
            return;

        w->viewport->flags = wflags;
        w->Invalidate();
    }

    void SaveInConfig(uint32_t wflags)
    {
        gConfigGeneral.invisible_rides = wflags & VIEWPORT_FLAG_INVISIBLE_RIDES;
        gConfigGeneral.invisible_vehicles = wflags & VIEWPORT_FLAG_INVISIBLE_VEHICLES;
        gConfigGeneral.invisible_scenery = wflags & VIEWPORT_FLAG_INVISIBLE_SCENERY;
        gConfigGeneral.invisible_trees = wflags & VIEWPORT_FLAG_INVISIBLE_VEGETATION;
        gConfigGeneral.invisible_paths = wflags & VIEWPORT_FLAG_INVISIBLE_PATHS;
        gConfigGeneral.invisible_supports = wflags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
        config_save_default();
    }
};

rct_window* WindowTransparencyOpen()
{
    auto* window = window_bring_to_front_by_class(WindowClass::Transparency);
    if (window == nullptr)
        window = WindowCreate<TransparencyWindow>(WindowClass::Transparency, ScreenCoordsXY(32, 32), WW, WH);

    return window;
}
