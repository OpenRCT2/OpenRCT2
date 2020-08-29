/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Window.h"

#include <openrct2-ui/input/KeyboardShortcuts.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_SHORTCUTS_TITLE;
static constexpr const int32_t WW = 420;
static constexpr const int32_t WH = 280;

static constexpr const int32_t WW_SC_MAX = 1200;
static constexpr const int32_t WH_SC_MAX = 800;

using namespace OpenRCT2;

// clang-format off
enum WINDOW_SHORTCUT_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SCROLL,
    WIDX_RESET
};

// 0x9DE48C
static rct_widget window_shortcut_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({4,    18}, {412, 245}, WWT_SCROLL, WindowColour::Primary, SCROLL_VERTICAL,           STR_SHORTCUT_LIST_TIP        ),
    MakeWidget({4, WH-15}, {150,  12}, WWT_BUTTON, WindowColour::Primary, STR_SHORTCUT_ACTION_RESET, STR_SHORTCUT_ACTION_RESET_TIP),
    { WIDGETS_END }
};

static void window_shortcut_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_shortcut_resize(rct_window *w);
static void window_shortcut_invalidate(rct_window *w);
static void window_shortcut_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_shortcut_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_shortcut_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_shortcut_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_shortcut_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_shortcut_events = {
    nullptr,
    window_shortcut_mouseup,
    window_shortcut_resize,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_shortcut_scrollgetsize,
    window_shortcut_scrollmousedown,
    nullptr,
    window_shortcut_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_shortcut_invalidate,
    window_shortcut_paint,
    window_shortcut_scrollpaint
};

struct ShortcutStringPair
{
    Input::Shortcut ShortcutId;
    rct_string_id StringId;
};

static const ShortcutStringPair ShortcutList[] =
{
    { Input::Shortcut::CloseTopMostWindow,                  STR_SHORTCUT_CLOSE_TOP_MOST_WINDOW },
    { Input::Shortcut::CloseAllFloatingWindows,             STR_SHORTCUT_CLOSE_ALL_FLOATING_WINDOWS },
    { Input::Shortcut::CancelConstructionMode,              STR_SHORTCUT_CANCEL_CONSTRUCTION_MODE },
    { Input::Shortcut::RemoveTopBottomToolbarToggle,        STR_SHORTCUT_TOGGLE_VISIBILITY_OF_TOOLBARS },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::PauseGame,                           STR_SHORTCUT_PAUSE_GAME },
    { Input::Shortcut::ReduceGameSpeed,                     STR_SHORTCUT_REDUCE_GAME_SPEED },
    { Input::Shortcut::IncreaseGameSpeed,                   STR_SHORTCUT_INCREASE_GAME_SPEED },
    { Input::Shortcut::LoadGame,                            STR_LOAD_GAME },
    { Input::Shortcut::QuickSaveGame,                       STR_SHORTCUT_QUICK_SAVE_GAME },
    { Input::Shortcut::ShowOptions,                         STR_SHORTCUT_SHOW_OPTIONS },
    { Input::Shortcut::Screenshot,                          STR_SHORTCUT_SCREENSHOT },
    { Input::Shortcut::MuteSound,                           STR_SHORTCUT_MUTE_SOUND },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::OpenCheatWindow,                     STR_SHORTCUT_OPEN_CHEATS_WINDOW },
    { Input::Shortcut::ToggleClearanceChecks,               STR_SHORTCUT_TOGGLE_CLEARANCE_CHECKS },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::ZoomViewOut,                         STR_SHORTCUT_ZOOM_VIEW_OUT },
    { Input::Shortcut::ZoomViewIn,                          STR_SHORTCUT_ZOOM_VIEW_IN },
    { Input::Shortcut::RotateViewClockwise,                 STR_SHORTCUT_ROTATE_VIEW_CLOCKWISE },
    { Input::Shortcut::RotateViewAnticlockwise,             STR_SHORTCUT_ROTATE_VIEW_ANTICLOCKWISE },
    { Input::Shortcut::ShowMap,                             STR_SHORTCUT_SHOW_MAP },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::ClearScenery,                        STR_SHORTCUT_CLEAR_SCENERY },
    { Input::Shortcut::AdjustLand,                          STR_SHORTCUT_ADJUST_LAND },
    { Input::Shortcut::AdjustWater,                         STR_SHORTCUT_ADJUST_WATER },
    { Input::Shortcut::BuildScenery,                        STR_SHORTCUT_BUILD_SCENERY },
    { Input::Shortcut::BuildPaths,                          STR_SHORTCUT_BUILD_PATHS },
    { Input::Shortcut::BuildNewRide,                        STR_SHORTCUT_BUILD_NEW_RIDE },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::ShowFinancialInformation,            STR_SHORTCUT_SHOW_FINANCIAL_INFORMATION },
    { Input::Shortcut::ShowResearchInformation,             STR_SHORTCUT_SHOW_RESEARCH_INFORMATION },
    { Input::Shortcut::ShowRidesList,                       STR_SHORTCUT_SHOW_RIDES_LIST },
    { Input::Shortcut::ShowParkInformation,                 STR_SHORTCUT_SHOW_PARK_INFORMATION },
    { Input::Shortcut::ShowGuestList,                       STR_SHORTCUT_SHOW_GUEST_LIST },
    { Input::Shortcut::ShowStaffList,                       STR_SHORTCUT_SHOW_STAFF_LIST },
    { Input::Shortcut::ShowRecentMessages,                  STR_SHORTCUT_SHOW_RECENT_MESSAGES },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::ShowMultiplayer,                     STR_SHORTCUT_SHOW_MULTIPLAYER },
    { Input::Shortcut::OpenChatWindow,                      STR_SEND_MESSAGE },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::UndergroundViewToggle,               STR_SHORTCUT_UNDERGROUND_VIEW_TOGGLE },
    { Input::Shortcut::RemoveBaseLandToggle,                STR_SHORTCUT_REMOVE_BASE_LAND_TOGGLE },
    { Input::Shortcut::RemoveVerticalLandToggle,            STR_SHORTCUT_REMOVE_VERTICAL_LAND_TOGGLE },
    { Input::Shortcut::SeeThroughRidesToggle,               STR_SHORTCUT_SEE_THROUGH_RIDES_TOGGLE },
    { Input::Shortcut::SeeThroughSceneryToggle,             STR_SHORTCUT_SEE_THROUGH_SCENERY_TOGGLE },
    { Input::Shortcut::SeeThroughPathsToggle,               STR_SHORTCUT_SEE_THROUGH_PATHS_TOGGLE },
    { Input::Shortcut::InvisibleSupportsToggle,             STR_SHORTCUT_INVISIBLE_SUPPORTS_TOGGLE },
    { Input::Shortcut::InvisiblePeopleToggle,               STR_SHORTCUT_INVISIBLE_PEOPLE_TOGGLE },
    { Input::Shortcut::HeightMarksOnLandToggle,             STR_SHORTCUT_HEIGHT_MARKS_ON_LAND_TOGGLE },
    { Input::Shortcut::HeightMarksOnRideTracksToggle,       STR_SHORTCUT_HEIGHT_MARKS_ON_RIDE_TRACKS_TOGGLE },
    { Input::Shortcut::HeightMarksOnPathsToggle,            STR_SHORTCUT_HEIGHT_MARKS_ON_PATHS_TOGGLE },
    { Input::Shortcut::ViewClipping,                        STR_SHORTCUT_VIEW_CLIPPING },
    { Input::Shortcut::HighlightPathIssuesToggle,           STR_SHORTCUT_HIGHLIGHT_PATH_ISSUES_TOGGLE },
    { Input::Shortcut::GridlinesDisplayToggle,              STR_SHORTCUT_GRIDLINES_DISPLAY_TOGGLE },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::SceneryPicker,                       STR_SHORTCUT_OPEN_SCENERY_PICKER },
    { Input::Shortcut::RotateConstructionObject,            STR_SHORTCUT_ROTATE_CONSTRUCTION_OBJECT },
    { Input::Shortcut::RideConstructionTurnLeft,            STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_LEFT },
    { Input::Shortcut::RideConstructionTurnRight,           STR_SHORTCUT_RIDE_CONSTRUCTION_TURN_RIGHT },
    { Input::Shortcut::RideConstructionUseTrackDefault,     STR_SHORTCUT_RIDE_CONSTRUCTION_USE_TRACK_DEFAULT },
    { Input::Shortcut::RideConstructionSlopeDown,           STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_DOWN },
    { Input::Shortcut::RideConstructionSlopeUp,             STR_SHORTCUT_RIDE_CONSTRUCTION_SLOPE_UP },
    { Input::Shortcut::RideConstructionChainLiftToggle,     STR_SHORTCUT_RIDE_CONSTRUCTION_CHAIN_LIFT_TOGGLE },
    { Input::Shortcut::RideConstructionBankLeft,            STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_LEFT },
    { Input::Shortcut::RideConstructionBankRight,           STR_SHORTCUT_RIDE_CONSTRUCTION_BANK_RIGHT },
    { Input::Shortcut::RideConstructionPreviousTrack,       STR_SHORTCUT_RIDE_CONSTRUCTION_PREVIOUS_TRACK },
    { Input::Shortcut::RideConstructionNextTrack,           STR_SHORTCUT_RIDE_CONSTRUCTION_NEXT_TRACK },
    { Input::Shortcut::RideConstructionBuildCurrent,        STR_SHORTCUT_RIDE_CONSTRUCTION_BUILD_CURRENT },
    { Input::Shortcut::RideConstructionDemolishCurrent,     STR_SHORTCUT_RIDE_CONSTRUCTION_DEMOLISH_CURRENT },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::ScrollMapUp,                         STR_SHORTCUT_SCROLL_MAP_UP },
    { Input::Shortcut::ScrollMapLeft,                       STR_SHORTCUT_SCROLL_MAP_LEFT },
    { Input::Shortcut::ScrollMapDown,                       STR_SHORTCUT_SCROLL_MAP_DOWN },
    { Input::Shortcut::ScrollMapRight,                      STR_SHORTCUT_SCROLL_MAP_RIGHT },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::WindowedModeToggle,                  STR_SHORTCUT_WINDOWED_MODE_TOGGLE },
    { Input::Shortcut::ScaleUp,                             STR_SHORTCUT_SCALE_UP },
    { Input::Shortcut::ScaleDown,                           STR_SHORTCUT_SCALE_DOWN },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::TileInspector,                       STR_SHORTCUT_OPEN_TILE_INSPECTOR },
    { Input::Shortcut::InsertCorruptElement,                STR_SHORTCUT_INSERT_CORRPUT_ELEMENT },
    { Input::Shortcut::CopyElement,                         STR_SHORTCUT_COPY_ELEMENT },
    { Input::Shortcut::PasteElement,                        STR_SHORTCUT_PASTE_ELEMENT },
    { Input::Shortcut::RemoveElement,                       STR_SHORTCUT_REMOVE_ELEMENT },
    { Input::Shortcut::MoveElementUp,                       STR_SHORTCUT_MOVE_ELEMENT_UP },
    { Input::Shortcut::MoveElementDown,                     STR_SHORTCUT_MOVE_ELEMENT_DOWN },
    { Input::Shortcut::IncreaseXCoord,                      STR_SHORTCUT_INCREASE_X_COORD },
    { Input::Shortcut::DecreaseXCoord,                      STR_SHORTCUT_DECREASE_X_COORD },
    { Input::Shortcut::IncreaseYCoord,                      STR_SHORTCUT_INCREASE_Y_COORD },
    { Input::Shortcut::DecreaseYCoord,                      STR_SHORTCUT_DECREASE_Y_COORD },
    { Input::Shortcut::IncreaseElementHeight,               STR_SHORTCUT_INCREASE_ELEM_HEIGHT },
    { Input::Shortcut::DecreaseElementHeight,               STR_SHORTCUT_DECREASE_ELEM_HEIGHT },

    { Input::Shortcut::Undefined,                           STR_NONE },

    { Input::Shortcut::AdvanceToNextTick,                   STR_ADVANCE_TO_NEXT_TICK },
    { Input::Shortcut::PaintOriginalToggle,                 STR_SHORTCUT_PAINT_ORIGINAL },
    { Input::Shortcut::DebugPaintToggle,                    STR_SHORTCUT_DEBUG_PAINT_TOGGLE },
};
// clang-format on

/**
 *
 *  rct2: 0x006E3884
 */
rct_window* window_shortcut_keys_open()
{
    rct_window* w = window_bring_to_front_by_class(WC_KEYBOARD_SHORTCUT_LIST);
    if (w == nullptr)
    {
        w = window_create_auto_pos(WW, WH, &window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, WF_RESIZABLE);

        w->widgets = window_shortcut_widgets;
        w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
        window_init_scroll_widgets(w);

        w->no_list_items = static_cast<uint16_t>(std::size(ShortcutList));
        w->selected_list_item = -1;
        w->min_width = WW;
        w->min_height = WH;
        w->max_width = WW_SC_MAX;
        w->max_height = WH_SC_MAX;
    }
    return w;
}

/**
 *
 *  rct2: 0x006E39E4
 */
static void window_shortcut_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_RESET:
            keyboard_shortcuts_reset();
            keyboard_shortcuts_save();
            w->Invalidate();
            break;
    }
}

static void window_shortcut_resize(rct_window* w)
{
    window_set_resize(w, w->min_width, w->min_height, w->max_width, w->max_height);
}

static void window_shortcut_invalidate(rct_window* w)
{
    window_shortcut_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_shortcut_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_shortcut_widgets[WIDX_TITLE].right = w->width - 2;
    window_shortcut_widgets[WIDX_CLOSE].right = w->width - 3;
    window_shortcut_widgets[WIDX_CLOSE].left = w->width - 13;
    window_shortcut_widgets[WIDX_SCROLL].right = w->width - 5;
    window_shortcut_widgets[WIDX_SCROLL].bottom = w->height - 18;
    window_shortcut_widgets[WIDX_RESET].top = w->height - 15;
    window_shortcut_widgets[WIDX_RESET].bottom = w->height - 4;
}

/**
 *
 *  rct2: 0x006E38E0
 */
static void window_shortcut_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
}

/**
 *
 *  rct2: 0x006E3A07
 */
static void window_shortcut_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * SCROLLABLE_ROW_HEIGHT;
}

/**
 *
 *  rct2: 0x006E3A3E
 */
static void window_shortcut_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t selected_item = (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT;
    if (selected_item >= w->no_list_items)
        return;

    // Is this a separator?
    if (ShortcutList[selected_item].ShortcutId == Input::Shortcut::Undefined)
        return;

    auto& shortcut = ShortcutList[selected_item];
    window_shortcut_change_open(shortcut.ShortcutId, shortcut.StringId);
}

/**
 *
 *  rct2: 0x006E3A16
 */
static void window_shortcut_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t selected_item = (screenCoords.y - 1) / SCROLLABLE_ROW_HEIGHT;
    if (selected_item >= w->no_list_items)
        return;

    w->selected_list_item = selected_item;

    w->Invalidate();
}

/**
 *
 *  rct2: 0x006E38E6
 */
static void window_shortcut_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto dpiCoords = ScreenCoordsXY{ dpi->x, dpi->y };
    gfx_fill_rect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi->width - 1, dpi->height - 1 } }, ColourMapA[w->colours[1]].mid_light);

    // TODO: the line below is a workaround for what is presumably a bug with dpi->width
    //       see https://github.com/OpenRCT2/OpenRCT2/issues/11238 for details
    const auto scrollWidth = w->width - SCROLLBAR_WIDTH - 10;

    for (int32_t i = 0; i < w->no_list_items; ++i)
    {
        int32_t y = 1 + i * SCROLLABLE_ROW_HEIGHT;
        if (y > dpi->y + dpi->height)
        {
            break;
        }

        if (y + SCROLLABLE_ROW_HEIGHT < dpi->y)
        {
            continue;
        }

        // Is this a separator?
        if (ShortcutList[i].ShortcutId == Input::Shortcut::Undefined)
        {
            const int32_t top = y + (SCROLLABLE_ROW_HEIGHT / 2) - 1;
            gfx_fill_rect(dpi, { { 0, top }, { scrollWidth, top } }, ColourMapA[w->colours[0]].mid_dark);
            gfx_fill_rect(dpi, { { 0, top + 1 }, { scrollWidth, top + 1 } }, ColourMapA[w->colours[0]].lightest);
            continue;
        }

        int32_t format = STR_BLACK_STRING;
        if (i == w->selected_list_item)
        {
            format = STR_WINDOW_COLOUR_2_STRINGID;
            gfx_filter_rect(dpi, 0, y - 1, scrollWidth, y + (SCROLLABLE_ROW_HEIGHT - 2), PALETTE_DARKEN_1);
        }

        const int32_t bindingOffset = scrollWidth - 150;
        auto ft = Formatter::Common();
        ft.Add<rct_string_id>(STR_SHORTCUT_ENTRY_FORMAT);
        ft.Add<rct_string_id>(ShortcutList[i].StringId);
        gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 0, y - 1 }, bindingOffset);

        char keybinding[128];
        keyboard_shortcuts_format_string(keybinding, 128, static_cast<int32_t>(ShortcutList[i].ShortcutId));

        if (strlen(keybinding) > 0)
        {
            const int32_t maxWidth = 150;
            ft = Formatter::Common();
            ft.Add<rct_string_id>(STR_STRING);
            ft.Add<char*>(keybinding);
            gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { bindingOffset, y - 1 }, maxWidth);
        }
    }
}
