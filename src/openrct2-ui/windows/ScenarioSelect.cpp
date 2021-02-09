/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <vector>

static constexpr const rct_string_id WINDOW_TITLE = STR_SELECT_SCENARIO;
static constexpr const int32_t WW = 734;
static constexpr const int32_t WH = 334;
#define INITIAL_NUM_UNLOCKED_SCENARIOS 5

// clang-format off
enum class LIST_ITEM_TYPE : uint8_t
{
    HEADING,
    SCENARIO,
};

struct sc_list_item
{
    LIST_ITEM_TYPE type;
    union
    {
        struct
        {
            rct_string_id string_id;
        } heading;
        struct
        {
            const scenario_index_entry * scenario;
            bool is_locked;
        } scenario;
    };
};

static std::vector<sc_list_item> _listItems;

enum {
    WIDX_BACKGROUND,
    WIDX_TITLEBAR,
    WIDX_CLOSE,
    WIDX_TABCONTENT,
    WIDX_TAB1,
    WIDX_TAB2,
    WIDX_TAB3,
    WIDX_TAB4,
    WIDX_TAB5,
    WIDX_TAB6,
    WIDX_TAB7,
    WIDX_TAB8,
    WIDX_SCENARIOLIST
};

static rct_widget window_scenarioselect_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({  0, 50}, {734, 284}, WindowWidgetType::ImgBtn, WindowColour::Secondary),                  // tab content panel
    MakeRemapWidget({  3, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 1
    MakeRemapWidget({ 94, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 2
    MakeRemapWidget({185, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 3
    MakeRemapWidget({276, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 4
    MakeRemapWidget({367, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 5
    MakeRemapWidget({458, 17}, {136,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 6
    MakeRemapWidget({594, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 7
    MakeRemapWidget({685, 17}, { 91,  34}, WindowWidgetType::Tab,    WindowColour::Secondary, SPR_TAB_LARGE),   // tab 8
    MakeWidget     ({  3, 54}, {553, 276}, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL), // level list
    { WIDGETS_END },
};

static constexpr const rct_string_id ScenarioOriginStringIds[] = {
    STR_SCENARIO_CATEGORY_RCT1,
    STR_SCENARIO_CATEGORY_RCT1_AA,
    STR_SCENARIO_CATEGORY_RCT1_LL,
    STR_SCENARIO_CATEGORY_RCT2,
    STR_SCENARIO_CATEGORY_RCT2_WW,
    STR_SCENARIO_CATEGORY_RCT2_TT,
    STR_SCENARIO_CATEGORY_REAL_PARKS,
    STR_SCENARIO_CATEGORY_OTHER_PARKS,
};

static void window_scenarioselect_init_tabs(rct_window *w);

static void window_scenarioselect_close(rct_window *w);
static void window_scenarioselect_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_scenarioselect_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_scenarioselect_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_scenarioselect_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_scenarioselect_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_scenarioselect_invalidate(rct_window *w);
static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_scenarioselect_events([](auto& events)
{
    events.close = &window_scenarioselect_close;
    events.mouse_up = &window_scenarioselect_mouseup;
    events.mouse_down = &window_scenarioselect_mousedown;
    events.get_scroll_size = &window_scenarioselect_scrollgetsize;
    events.scroll_mousedown = &window_scenarioselect_scrollmousedown;
    events.scroll_mouseover = &window_scenarioselect_scrollmouseover;
    events.invalidate = &window_scenarioselect_invalidate;
    events.paint = &window_scenarioselect_paint;
    events.scroll_paint = &window_scenarioselect_scrollpaint;
});
// clang-format on

static void draw_category_heading(
    rct_window* w, rct_drawpixelinfo* dpi, int32_t left, int32_t right, int32_t y, rct_string_id stringId);
static void initialise_list_items(rct_window* w);
static bool is_scenario_visible(rct_window* w, const scenario_index_entry* scenario);
static bool is_locking_enabled(rct_window* w);

static std::function<void(std::string_view)> _callback;
static bool _showLockedInformation = false;
static bool _titleEditor = false;
static bool _disableLocking{};

rct_window* window_scenarioselect_open(scenarioselect_callback callback, bool titleEditor)
{
    if (_titleEditor != titleEditor)
    {
        _titleEditor = titleEditor;
        window_close_by_class(WC_SCENARIO_SELECT);
    }

    auto window = window_bring_to_front_by_class(WC_SCENARIO_SELECT);
    if (window != nullptr)
        return window;

    return window_scenarioselect_open(
        [callback](std::string_view scenario) { callback(std::string(scenario).c_str()); }, titleEditor, titleEditor);
}

/**
 *
 *  rct2: 0x006781B5
 */
rct_window* window_scenarioselect_open(std::function<void(std::string_view)> callback, bool titleEditor, bool disableLocking)
{
    rct_window* window;
    int32_t windowWidth;
    int32_t windowHeight = 334;

    _callback = callback;
    _disableLocking = disableLocking;

    // Load scenario list
    scenario_repository_scan();

    // Shrink the window if we're showing scenarios by difficulty level.
    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY && !_titleEditor)
        windowWidth = 610;
    else
        windowWidth = 733;

    window = WindowCreateCentred(
        windowWidth, windowHeight, &window_scenarioselect_events, WC_SCENARIO_SELECT,
        WF_10 | (titleEditor ? WF_STICK_TO_FRONT : 0));
    window->widgets = window_scenarioselect_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4)
        | (1 << WIDX_TAB5) | (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8);

    window_scenarioselect_init_tabs(window);
    initialise_list_items(window);

    WindowInitScrollWidgets(window);
    window->viewport_focus_coordinates.var_480 = -1;
    window->highlighted_scenario = nullptr;

    return window;
}

/**
 *
 *  rct2: 0x00677C8A
 */
static void window_scenarioselect_init_tabs(rct_window* w)
{
    int32_t showPages = 0;
    size_t numScenarios = scenario_repository_get_count();
    for (size_t i = 0; i < numScenarios; i++)
    {
        const scenario_index_entry* scenario = scenario_repository_get_by_index(i);
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN || _titleEditor)
        {
            if (_titleEditor && scenario->source_game == ScenarioSource::Other)
                continue;
            showPages |= 1 << static_cast<uint8_t>(scenario->source_game);
        }
        else
        {
            int32_t category = scenario->category;
            if (category > SCENARIO_CATEGORY_OTHER)
            {
                category = SCENARIO_CATEGORY_OTHER;
            }
            showPages |= 1 << category;
        }
    }

    if (showPages & (1 << gConfigInterface.scenarioselect_last_tab))
    {
        w->selected_tab = gConfigInterface.scenarioselect_last_tab;
    }
    else
    {
        int32_t firstPage = bitscanforward(showPages);
        if (firstPage != -1)
        {
            w->selected_tab = firstPage;
        }
    }

    int32_t x = 3;
    for (int32_t i = 0; i < 8; i++)
    {
        rct_widget* widget = &w->widgets[i + WIDX_TAB1];
        if (!(showPages & (1 << i)))
        {
            widget->type = WindowWidgetType::Empty;
            continue;
        }

        widget->type = WindowWidgetType::Tab;
        widget->left = x;
        widget->right = x + 90;
        x += 91;
    }
}

static void window_scenarioselect_close(rct_window* w)
{
    _listItems.clear();
    _listItems.shrink_to_fit();
}

static void window_scenarioselect_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE)
    {
        window_close(w);
    }
}

static void window_scenarioselect_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex >= WIDX_TAB1 && widgetIndex <= WIDX_TAB8)
    {
        w->selected_tab = widgetIndex - 4;
        w->highlighted_scenario = nullptr;
        gConfigInterface.scenarioselect_last_tab = w->selected_tab;
        config_save_default();
        initialise_list_items(w);
        w->Invalidate();
        window_event_resize_call(w);
        window_event_invalidate_call(w);
        WindowInitScrollWidgets(w);
        w->Invalidate();
    }
}

static int32_t get_scenario_list_item_size()
{
    if (!LocalisationService_UseTrueTypeFont())
        return 24;

    // Scenario title
    int32_t lineHeight = font_get_line_height(FONT_SPRITE_BASE_MEDIUM);

    // 'Completed by' line
    lineHeight += font_get_line_height(FONT_SPRITE_BASE_SMALL);

    return lineHeight;
}

static void window_scenarioselect_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    const int32_t scenarioItemHeight = get_scenario_list_item_size();

    int32_t y = 0;
    for (const auto& listItem : _listItems)
    {
        switch (listItem.type)
        {
            case LIST_ITEM_TYPE::HEADING:
                y += 18;
                break;
            case LIST_ITEM_TYPE::SCENARIO:
                y += scenarioItemHeight;
                break;
        }
    }
    *height = y;
}

/**
 *
 *  rct2: 0x6780FE
 */
static void window_scenarioselect_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    const int32_t scenarioItemHeight = get_scenario_list_item_size();

    auto mutableScreenCoords = screenCoords;
    for (const auto& listItem : _listItems)
    {
        switch (listItem.type)
        {
            case LIST_ITEM_TYPE::HEADING:
                mutableScreenCoords.y -= 18;
                break;
            case LIST_ITEM_TYPE::SCENARIO:
                mutableScreenCoords.y -= scenarioItemHeight;
                if (mutableScreenCoords.y < 0 && !listItem.scenario.is_locked)
                {
                    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, w->windowPos.x + (w->width / 2));
                    gFirstTimeSaving = true;
                    _callback(listItem.scenario.scenario->path);
                    window_close(w);
                }
                break;
        }
        if (mutableScreenCoords.y < 0)
        {
            break;
        }
    }
}

/**
 *
 *  rct2: 0x678162
 */
static void window_scenarioselect_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    const int32_t scenarioItemHeight = get_scenario_list_item_size();

    bool originalShowLockedInformation = _showLockedInformation;
    _showLockedInformation = false;
    const scenario_index_entry* selected = nullptr;
    auto mutableScreenCoords = screenCoords;
    for (const auto& listItem : _listItems)
    {
        switch (listItem.type)
        {
            case LIST_ITEM_TYPE::HEADING:
                mutableScreenCoords.y -= 18;
                break;
            case LIST_ITEM_TYPE::SCENARIO:
                mutableScreenCoords.y -= scenarioItemHeight;
                if (mutableScreenCoords.y < 0)
                {
                    if (listItem.scenario.is_locked)
                    {
                        _showLockedInformation = true;
                    }
                    else
                    {
                        selected = listItem.scenario.scenario;
                    }
                }
                break;
        }
        if (mutableScreenCoords.y < 0)
        {
            break;
        }
    }

    if (w->highlighted_scenario != selected)
    {
        w->highlighted_scenario = selected;
        w->Invalidate();
    }
    else if (_showLockedInformation != originalShowLockedInformation)
    {
        w->Invalidate();
    }
}

static void window_scenarioselect_invalidate(rct_window* w)
{
    w->pressed_widgets &= ~(
        (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2) | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
        | (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8));

    w->pressed_widgets |= 1LL << (w->selected_tab + WIDX_TAB1);

    int32_t windowWidth = w->width;
    window_scenarioselect_widgets[WIDX_BACKGROUND].right = windowWidth - 1;
    window_scenarioselect_widgets[WIDX_TITLEBAR].right = windowWidth - 2;
    window_scenarioselect_widgets[WIDX_CLOSE].left = windowWidth - 13;
    window_scenarioselect_widgets[WIDX_CLOSE].right = windowWidth - 3;
    window_scenarioselect_widgets[WIDX_TABCONTENT].right = windowWidth - 1;
    window_scenarioselect_widgets[WIDX_SCENARIOLIST].right = windowWidth - 179;

    int32_t windowHeight = w->height;
    window_scenarioselect_widgets[WIDX_BACKGROUND].bottom = windowHeight - 1;
    window_scenarioselect_widgets[WIDX_TABCONTENT].bottom = windowHeight - 1;

    const int32_t bottomMargin = gConfigGeneral.debugging_tools ? 17 : 5;
    window_scenarioselect_widgets[WIDX_SCENARIOLIST].bottom = windowHeight - bottomMargin;
}

static void window_scenarioselect_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t format;
    const scenario_index_entry* scenario;

    WindowDrawWidgets(w, dpi);

    format = (ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT) ? STR_SMALL_WINDOW_COLOUR_2_STRINGID
                                                                                   : STR_WINDOW_COLOUR_2_STRINGID;

    // Text for each tab
    for (uint32_t i = 0; i < std::size(ScenarioOriginStringIds); i++)
    {
        rct_widget* widget = &window_scenarioselect_widgets[WIDX_TAB1 + i];
        if (widget->type == WindowWidgetType::Empty)
            continue;

        auto ft = Formatter();
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN || _titleEditor)
        {
            ft.Add<rct_string_id>(ScenarioOriginStringIds[i]);
        }
        else
        { // old-style
            ft.Add<rct_string_id>(ScenarioCategoryStringIds[i]);
        }

        ScreenCoordsXY stringCoords(widget->midX() + w->windowPos.x, widget->midY() + w->windowPos.y - 3);
        gfx_draw_string_centred_wrapped(dpi, ft.Data(), stringCoords, 87, format, COLOUR_AQUAMARINE);
    }

    // Return if no scenario highlighted
    scenario = w->highlighted_scenario;
    if (scenario == nullptr)
    {
        if (_showLockedInformation)
        {
            // Show locked information
            auto screenPos = w->windowPos
                + ScreenCoordsXY{ window_scenarioselect_widgets[WIDX_SCENARIOLIST].right + 4,
                                  window_scenarioselect_widgets[WIDX_TABCONTENT].top + 5 };
            DrawTextEllipsised(
                dpi, screenPos + ScreenCoordsXY{ 85, 0 }, 170, STR_SCENARIO_LOCKED, {}, COLOUR_BLACK, TextAlignment::CENTRE);
            gfx_draw_string_left_wrapped(
                dpi, nullptr, screenPos + ScreenCoordsXY{ 0, 15 }, 170, STR_SCENARIO_LOCKED_DESC, COLOUR_BLACK);
        }
        return;
    }

    // Scenario path
    if (gConfigGeneral.debugging_tools)
    {
        utf8 path[MAX_PATH];

        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        shorten_path(path, sizeof(path), scenario->path, w->width - 6);

        const utf8* pathPtr = path;
        gfx_draw_string_left(
            dpi, STR_STRING, static_cast<void*>(&pathPtr), w->colours[1],
            w->windowPos + ScreenCoordsXY{ 3, w->height - 3 - 11 });
    }

    // Scenario name
    auto screenPos = w->windowPos
        + ScreenCoordsXY{ window_scenarioselect_widgets[WIDX_SCENARIOLIST].right + 4,
                          window_scenarioselect_widgets[WIDX_TABCONTENT].top + 5 };
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(scenario->name);
    DrawTextEllipsised(
        dpi, screenPos + ScreenCoordsXY{ 85, 0 }, 170, STR_WINDOW_COLOUR_2_STRINGID, ft, COLOUR_BLACK, TextAlignment::CENTRE);
    screenPos.y += 15;

    // Scenario details
    ft = Formatter();
    ft.Add<rct_string_id>(STR_STRING);
    ft.Add<const char*>(scenario->details);
    screenPos.y += gfx_draw_string_left_wrapped(dpi, ft.Data(), screenPos, 170, STR_BLACK_STRING, COLOUR_BLACK) + 5;

    // Scenario objective
    ft = Formatter();
    ft.Add<rct_string_id>(ObjectiveNames[scenario->objective_type]);
    if (scenario->objective_type == OBJECTIVE_BUILD_THE_BEST)
    {
        rct_string_id rideTypeString = STR_NONE;
        auto rideTypeId = scenario->objective_arg_3;
        if (rideTypeId != RIDE_TYPE_NULL && rideTypeId < RIDE_TYPE_COUNT)
        {
            rideTypeString = RideTypeDescriptors[rideTypeId].Naming.Name;
        }
        ft.Add<rct_string_id>(rideTypeString);
    }
    else
    {
        ft.Add<int16_t>(scenario->objective_arg_3);
        ft.Add<int16_t>(date_get_total_months(MONTH_OCTOBER, scenario->objective_arg_1));
        if (scenario->objective_type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
            ft.Add<uint16_t>(scenario->objective_arg_2);
        else
            ft.Add<money32>(scenario->objective_arg_2);
    }
    screenPos.y += gfx_draw_string_left_wrapped(dpi, ft.Data(), screenPos, 170, STR_OBJECTIVE, COLOUR_BLACK) + 5;

    // Scenario score
    if (scenario->highscore != nullptr)
    {
        // TODO: Should probably be translatable
        const utf8* completedByName = "???";
        if (!str_is_null_or_empty(scenario->highscore->name))
        {
            completedByName = scenario->highscore->name;
        }
        ft = Formatter();
        ft.Add<rct_string_id>(STR_STRING);
        ft.Add<const char*>(completedByName);
        ft.Add<money32>(scenario->highscore->company_value);
        screenPos.y += gfx_draw_string_left_wrapped(
            dpi, ft.Data(), screenPos, 170, STR_COMPLETED_BY_WITH_COMPANY_VALUE, COLOUR_BLACK);
    }
}

static void window_scenarioselect_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    int32_t colour;

    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    rct_string_id highlighted_format = (ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT)
        ? STR_WHITE_STRING
        : STR_WINDOW_COLOUR_2_STRINGID;
    rct_string_id unhighlighted_format = (ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT)
        ? STR_WHITE_STRING
        : STR_BLACK_STRING;

    bool wide = gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN || _titleEditor;

    rct_widget* listWidget = &w->widgets[WIDX_SCENARIOLIST];
    int32_t listWidth = listWidget->width() - 12;

    const int32_t scenarioItemHeight = get_scenario_list_item_size();

    // Scenario title
    int32_t scenarioTitleHeight = font_get_line_height(FONT_SPRITE_BASE_MEDIUM);

    int32_t y = 0;
    for (const auto& listItem : _listItems)
    {
        if (y > dpi->y + dpi->height)
        {
            continue;
        }

        switch (listItem.type)
        {
            case LIST_ITEM_TYPE::HEADING:
            {
                const int32_t horizontalRuleMargin = 4;
                draw_category_heading(
                    w, dpi, horizontalRuleMargin, listWidth - horizontalRuleMargin, y + 2, listItem.heading.string_id);
                y += 18;
                break;
            }
            case LIST_ITEM_TYPE::SCENARIO:
            {
                // Draw hover highlight
                const scenario_index_entry* scenario = listItem.scenario.scenario;
                bool isHighlighted = w->highlighted_scenario == scenario;
                if (isHighlighted)
                {
                    gfx_filter_rect(dpi, 0, y, w->width, y + scenarioItemHeight - 1, FilterPaletteID::PaletteDarken1);
                }

                bool isCompleted = scenario->highscore != nullptr;
                bool isDisabled = listItem.scenario.is_locked;

                // Draw scenario name
                char buffer[64];
                safe_strcpy(buffer, scenario->name, sizeof(buffer));
                rct_string_id format = isDisabled ? static_cast<rct_string_id>(STR_STRINGID)
                                                  : (isHighlighted ? highlighted_format : unhighlighted_format);
                auto ft = Formatter();
                ft.Add<rct_string_id>(STR_STRING);
                ft.Add<char*>(buffer);
                colour = isDisabled ? w->colours[1] | COLOUR_FLAG_INSET : COLOUR_BLACK;
                if (isDisabled)
                {
                    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_DARK;
                }
                gfx_draw_string_centred(dpi, format, { wide ? 270 : 210, y + 1 }, colour, ft.Data());

                // Check if scenario is completed
                if (isCompleted)
                {
                    // Draw completion tick
                    gfx_draw_sprite(dpi, SPR_MENU_CHECKMARK, { wide ? 500 : 395, y + 1 }, 0);

                    // Draw completion score
                    const utf8* completedByName = "???";
                    if (!str_is_null_or_empty(scenario->highscore->name))
                    {
                        completedByName = scenario->highscore->name;
                    }
                    safe_strcpy(buffer, completedByName, 64);
                    ft = Formatter();
                    ft.Add<rct_string_id>(STR_COMPLETED_BY);
                    ft.Add<rct_string_id>(STR_STRING);
                    ft.Add<char*>(buffer);
                    gfx_draw_string_centred(
                        dpi, format, { wide ? 270 : 210, y + scenarioTitleHeight + 1 }, COLOUR_BLACK, ft.Data());
                }

                y += scenarioItemHeight;
                break;
            }
        }
    }
}

static void draw_category_heading(
    rct_window* w, rct_drawpixelinfo* dpi, int32_t left, int32_t right, int32_t y, rct_string_id stringId)
{
    uint8_t baseColour = w->colours[1];
    uint8_t lightColour = ColourMapA[baseColour].lighter;
    uint8_t darkColour = ColourMapA[baseColour].mid_dark;

    // Draw string
    int32_t centreX = (left + right) / 2;
    gfx_draw_string_centred(dpi, stringId, { centreX, y }, baseColour, nullptr);

    // Get string dimensions
    utf8* buffer = gCommonStringFormatBuffer;
    format_string(buffer, 256, stringId, nullptr);
    int32_t categoryStringHalfWidth = (gfx_get_string_width(buffer) / 2) + 4;
    int32_t strLeft = centreX - categoryStringHalfWidth;
    int32_t strRight = centreX + categoryStringHalfWidth;

    // Draw light horizontal rule
    int32_t lineY = y + 4;
    auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY };
    auto lightLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY };
    gfx_draw_line(dpi, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

    auto lightLineLeftTop2 = ScreenCoordsXY{ strRight, lineY };
    auto lightLineRightBottom2 = ScreenCoordsXY{ right, lineY };
    gfx_draw_line(dpi, { lightLineLeftTop2, lightLineRightBottom2 }, lightColour);

    // Draw dark horizontal rule
    lineY++;
    auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY };
    auto darkLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY };
    gfx_draw_line(dpi, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);

    auto darkLineLeftTop2 = ScreenCoordsXY{ strRight, lineY };
    auto darkLineRightBottom2 = ScreenCoordsXY{ right, lineY };
    gfx_draw_line(dpi, { darkLineLeftTop2, darkLineRightBottom2 }, darkColour);
}

static void initialise_list_items(rct_window* w)
{
    size_t numScenarios = scenario_repository_get_count();
    _listItems.clear();

    // Mega park unlock
    const uint32_t rct1RequiredCompletedScenarios = (1 << SC_MEGA_PARK) - 1;
    uint32_t rct1CompletedScenarios = 0;
    size_t megaParkListItemIndex = SIZE_MAX;

    int32_t numUnlocks = INITIAL_NUM_UNLOCKED_SCENARIOS;
    uint8_t currentHeading = UINT8_MAX;
    for (size_t i = 0; i < numScenarios; i++)
    {
        const scenario_index_entry* scenario = scenario_repository_get_by_index(i);

        if (!is_scenario_visible(w, scenario))
            continue;
        if (_titleEditor && scenario->source_game == ScenarioSource::Other)
            continue;

        // Category heading
        rct_string_id headingStringId = STR_NONE;
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN || _titleEditor)
        {
            if (w->selected_tab != static_cast<uint8_t>(ScenarioSource::Real) && currentHeading != scenario->category)
            {
                currentHeading = scenario->category;
                headingStringId = ScenarioCategoryStringIds[currentHeading];
            }
        }
        else
        {
            if (w->selected_tab <= SCENARIO_CATEGORY_EXPERT)
            {
                if (currentHeading != static_cast<uint8_t>(scenario->source_game))
                {
                    currentHeading = static_cast<uint8_t>(scenario->source_game);
                    headingStringId = ScenarioOriginStringIds[currentHeading];
                }
            }
            else if (w->selected_tab == SCENARIO_CATEGORY_OTHER)
            {
                int32_t category = scenario->category;
                if (category <= SCENARIO_CATEGORY_REAL)
                {
                    category = SCENARIO_CATEGORY_OTHER;
                }
                if (currentHeading != category)
                {
                    currentHeading = category;
                    headingStringId = ScenarioCategoryStringIds[category];
                }
            }
        }

        if (headingStringId != STR_NONE)
        {
            sc_list_item headerItem;
            headerItem.type = LIST_ITEM_TYPE::HEADING;
            headerItem.heading.string_id = headingStringId;
            _listItems.push_back(std::move(headerItem));
        }

        // Scenario
        sc_list_item scenarioItem;
        scenarioItem.type = LIST_ITEM_TYPE::SCENARIO;
        scenarioItem.scenario.scenario = scenario;
        if (is_locking_enabled(w))
        {
            scenarioItem.scenario.is_locked = numUnlocks <= 0;
            if (scenario->highscore == nullptr)
            {
                numUnlocks--;
            }
            else
            {
                // Mark RCT1 scenario as completed
                if (scenario->sc_id < SC_MEGA_PARK)
                {
                    rct1CompletedScenarios |= 1 << scenario->sc_id;
                }
            }

            // If scenario is Mega Park, keep a reference to it
            if (scenario->sc_id == SC_MEGA_PARK)
            {
                megaParkListItemIndex = _listItems.size() - 1;
            }
        }
        else
        {
            scenarioItem.scenario.is_locked = false;
        }
        _listItems.push_back(std::move(scenarioItem));
    }

    // Mega park handling
    if (megaParkListItemIndex != SIZE_MAX)
    {
        bool megaParkLocked = (rct1CompletedScenarios & rct1RequiredCompletedScenarios) != rct1RequiredCompletedScenarios;
        _listItems[megaParkListItemIndex].scenario.is_locked = megaParkLocked;
        if (megaParkLocked && gConfigGeneral.scenario_hide_mega_park)
        {
            // Remove mega park
            _listItems.pop_back();

            // Remove empty headings
            for (auto it = _listItems.begin(); it != _listItems.end(); it++)
            {
                const auto& listItem = *it;
                if (listItem.type == LIST_ITEM_TYPE::HEADING)
                {
                    if ((it + 1) == _listItems.end() || (it + 1)->type == LIST_ITEM_TYPE::HEADING)
                    {
                        it = _listItems.erase(it);
                        it--;
                    }
                }
            }
        }
    }
}

static bool is_scenario_visible(rct_window* w, const scenario_index_entry* scenario)
{
    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN || _titleEditor)
    {
        if (static_cast<uint8_t>(scenario->source_game) != w->selected_tab)
        {
            return false;
        }
    }
    else
    {
        int32_t category = scenario->category;
        if (category > SCENARIO_CATEGORY_OTHER)
        {
            category = SCENARIO_CATEGORY_OTHER;
        }
        if (category != w->selected_tab)
        {
            return false;
        }
    }
    return true;
}

static bool is_locking_enabled(rct_window* w)
{
    if (gConfigGeneral.scenario_select_mode != SCENARIO_SELECT_MODE_ORIGIN)
        return false;
    if (!gConfigGeneral.scenario_unlocking_enabled)
        return false;
    if (w->selected_tab >= 6)
        return false;
    if (_titleEditor)
        return false;

    return true;
}
