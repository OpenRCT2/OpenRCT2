#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <openrct2/config/Config.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/core/Memory.hpp>

#include <openrct2/audio/audio.h>
#include <openrct2/localisation/date.h>
#include <openrct2/localisation/localisation.h>
#include <openrct2/sprites.h>
#include <openrct2/interface/widget.h>
#include <openrct2/interface/themes.h>
#include <openrct2/util/util.h>

#define INITIAL_NUM_UNLOCKED_SCENARIOS 5

enum {
    LIST_ITEM_TYPE_HEADING,
    LIST_ITEM_TYPE_SCENARIO,
    LIST_ITEM_TYPE_END,
};

typedef struct sc_list_item {
    uint8 type;
    union {
        struct {
            rct_string_id string_id;
        } heading;
        struct {
            const scenario_index_entry *scenario;
            bool is_locked;
        } scenario;
    };
} sc_list_item;

static sc_list_item *_listItems = nullptr;

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
    { WWT_FRAME,    0,  0,      733,    0,      333,    0xFFFFFFFF,                 STR_NONE },             // panel / background
    { WWT_CAPTION,  0,  1,      732,    1,      14,     STR_SELECT_SCENARIO,        STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX, 0,  721,    731,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_IMGBTN,   1,  0,      733,    50,     333,    0xFFFFFFFF,                 STR_NONE },             // tab content panel
    { WWT_TAB,      1,  3,      93,     17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 1
    { WWT_TAB,      1,  94,     184,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 2
    { WWT_TAB,      1,  185,    275,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 3
    { WWT_TAB,      1,  276,    366,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 4
    { WWT_TAB,      1,  367,    457,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 5
    { WWT_TAB,      1,  458,    593,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 6
    { WWT_TAB,      1,  594,    684,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 7
    { WWT_TAB,      1,  685,    775,    17,     50,     IMAGE_TYPE_REMAP | SPR_TAB_LARGE, STR_NONE },             // tab 8
    { WWT_SCROLL,   1,  3,      555,    54,     329,    SCROLL_VERTICAL,            STR_NONE },             // level list
    { WIDGETS_END },
};

static const rct_string_id ScenarioOriginStringIds[] = {
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
static void window_scenarioselect_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_scenarioselect_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_scenarioselect_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_scenarioselect_invalidate(rct_window *w);
static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_scenarioselect_events = {
    window_scenarioselect_close,
    window_scenarioselect_mouseup,
    nullptr,
    window_scenarioselect_mousedown,
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
    window_scenarioselect_scrollgetsize,
    window_scenarioselect_scrollmousedown,
    nullptr,
    window_scenarioselect_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_scenarioselect_invalidate,
    window_scenarioselect_paint,
    window_scenarioselect_scrollpaint
};

static void draw_category_heading(rct_window *w, rct_drawpixelinfo *dpi, sint32 left, sint32 right, sint32 y, rct_string_id stringId);
static void initialise_list_items(rct_window *w);
static bool is_scenario_visible(rct_window *w, const scenario_index_entry *scenario);
static bool is_locking_enabled(rct_window *w);

static scenarioselect_callback _callback;
static bool _showLockedInformation = false;

/**
 *
 *  rct2: 0x006781B5
 */
static void _window_scenarioselect_open(scenarioselect_callback callback)
{
    rct_window* window;
    sint32 windowWidth;
    sint32 windowHeight = 334;

    _callback = callback;

    if (window_bring_to_front_by_class(WC_SCENARIO_SELECT) != nullptr)
        return;

    // Load scenario list
    scenario_repository_scan();

    // Shrink the window if we're showing scenarios by difficulty level.
    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_DIFFICULTY) {
        windowWidth = 610;
    } else {
        windowWidth = 733;
    }

    window = window_create_centred(
        windowWidth,
        windowHeight,
        &window_scenarioselect_events,
        WC_SCENARIO_SELECT,
        WF_10
    );
    window->widgets = window_scenarioselect_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2)
                            | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
                            | (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8);

    window_scenarioselect_init_tabs(window);
    initialise_list_items(window);

    window_init_scroll_widgets(window);
    window->viewport_focus_coordinates.var_480 = -1;
    window->highlighted_scenario = nullptr;
}

/**
 *
 *  rct2: 0x00677C8A
 */
static void window_scenarioselect_init_tabs(rct_window *w)
{
    sint32 showPages = 0;
    size_t numScenarios = scenario_repository_get_count();
    for (size_t i = 0; i < numScenarios; i++) {
        const scenario_index_entry *scenario = scenario_repository_get_by_index(i);
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
            showPages |= 1 << scenario->source_game;
        } else {
            sint32 category = scenario->category;
            if (category > SCENARIO_CATEGORY_OTHER) {
                category = SCENARIO_CATEGORY_OTHER;
            }
            showPages |= 1 << category;
        }
        }

    sint32 firstPage = bitscanforward(showPages);
    if (firstPage != -1) {
        w->selected_tab = firstPage;
    }

    sint32 x = 3;
    for (sint32 i = 0; i < 8; i++) {
        rct_widget* widget = &w->widgets[i + WIDX_TAB1];
        if (!(showPages & (1 << i))) {
            widget->type = WWT_EMPTY;
            continue;
        }

        widget->type = WWT_TAB;
        widget->left = x;
        widget->right = x + 90;
        x += 91;
    }
}

static void window_scenarioselect_close(rct_window *w)
{
    SafeFree(_listItems);
}

static void window_scenarioselect_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_CLOSE) {
        window_close(w);
    }
}

static void window_scenarioselect_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex >= WIDX_TAB1 && widgetIndex <= WIDX_TAB8) {
        w->selected_tab = widgetIndex - 4;
        w->highlighted_scenario = nullptr;
        initialise_list_items(w);
        window_invalidate(w);
        window_event_resize_call(w);
        window_event_invalidate_call(w);
        window_init_scroll_widgets(w);
        window_invalidate(w);
    }
}

static void window_scenarioselect_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    sint32 y = 0;
    for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
        switch (listItem->type) {
        case LIST_ITEM_TYPE_HEADING:
            y += 18;
            break;
        case LIST_ITEM_TYPE_SCENARIO:
            y += 24;
            break;
        }
    }
    *height = y;
}

/**
 *
 *  rct2: 0x6780FE
 */
static void window_scenarioselect_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
        switch (listItem->type) {
        case LIST_ITEM_TYPE_HEADING:
            y -= 18;
            break;
        case LIST_ITEM_TYPE_SCENARIO:
            y -= 24;
            if (y < 0 && !listItem->scenario.is_locked) {
                audio_play_sound(SOUND_CLICK_1, 0, w->x + (w->width / 2));
                _callback(listItem->scenario.scenario->path);
            }
            break;
        }
        if (y < 0) {
            break;
        }
    }
}

/**
 *
 *  rct2: 0x678162
 */
static void window_scenarioselect_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
    bool originalShowLockedInformation = _showLockedInformation;
    _showLockedInformation = false;
    const scenario_index_entry *selected = nullptr;
    for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
        switch (listItem->type) {
        case LIST_ITEM_TYPE_HEADING:
            y -= 18;
            break;
        case LIST_ITEM_TYPE_SCENARIO:
            y -= 24;
            if (y < 0) {
                if (listItem->scenario.is_locked) {
                    _showLockedInformation = true;
                } else {
                    selected = listItem->scenario.scenario;
                }
            }
            break;
        }
        if (y < 0) {
            break;
        }
    }

    if (w->highlighted_scenario != selected) {
        w->highlighted_scenario = selected;
        window_invalidate(w);
    } else if (_showLockedInformation != originalShowLockedInformation) {
        window_invalidate(w);
    }
}

static void window_scenarioselect_invalidate(rct_window *w)
{
    w->pressed_widgets &= ~( (1 << WIDX_CLOSE) | (1 << WIDX_TAB1) | (1 << WIDX_TAB2)
                           | (1 << WIDX_TAB3) | (1 << WIDX_TAB4) | (1 << WIDX_TAB5)
                           | (1 << WIDX_TAB6) | (1 << WIDX_TAB7) | (1 << WIDX_TAB8) );

    w->pressed_widgets |= 1LL << (w->selected_tab + WIDX_TAB1);

    sint32 windowWidth = w->width;
    window_scenarioselect_widgets[WIDX_BACKGROUND].right = windowWidth - 1;
    window_scenarioselect_widgets[WIDX_TITLEBAR].right = windowWidth - 2;
    window_scenarioselect_widgets[WIDX_CLOSE].left  = windowWidth - 13;
    window_scenarioselect_widgets[WIDX_CLOSE].right = windowWidth - 3;
    window_scenarioselect_widgets[WIDX_TABCONTENT].right = windowWidth - 1;
    window_scenarioselect_widgets[WIDX_SCENARIOLIST].right = windowWidth - 179;

    sint32 windowHeight = w->height;
    window_scenarioselect_widgets[WIDX_BACKGROUND].bottom = windowHeight - 1;
    window_scenarioselect_widgets[WIDX_TABCONTENT].bottom = windowHeight - 1;

    const sint32 bottomMargin = gConfigGeneral.debugging_tools ? 17 : 5;
    window_scenarioselect_widgets[WIDX_SCENARIOLIST].bottom = windowHeight - bottomMargin;
}

static void window_scenarioselect_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    sint32 format;
    const scenario_index_entry *scenario;

    window_draw_widgets(w, dpi);

    format = (theme_get_flags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT) ? STR_SMALL_WINDOW_COLOUR_2_STRINGID : STR_WINDOW_COLOUR_2_STRINGID;

    // Text for each tab
    for (sint32 i = 0; i < 8; i++) {
        rct_widget *widget = &window_scenarioselect_widgets[WIDX_TAB1 + i];
        if (widget->type == WWT_EMPTY)
            continue;

        sint32 x = (widget->left + widget->right) / 2 + w->x;
        sint32 y = (widget->top + widget->bottom) / 2 + w->y - 3;

        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
            set_format_arg(0, rct_string_id, ScenarioOriginStringIds[i]);
        } else { // old-style
            set_format_arg(0, rct_string_id, ScenarioCategoryStringIds[i]);
        }
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, 87, format, COLOUR_AQUAMARINE);
    }

    // Return if no scenario highlighted
    scenario = w->highlighted_scenario;
    if (scenario == nullptr) {
        if (_showLockedInformation) {
            // Show locked information
            sint32 x = w->x + window_scenarioselect_widgets[WIDX_SCENARIOLIST].right + 4;
            sint32 y = w->y + window_scenarioselect_widgets[WIDX_TABCONTENT].top + 5;
            gfx_draw_string_centred_clipped(dpi, STR_SCENARIO_LOCKED, nullptr, COLOUR_BLACK, x + 85, y, 170);
            y += 15;
            y += gfx_draw_string_left_wrapped(dpi, nullptr, x, y, 170, STR_SCENARIO_LOCKED_DESC, COLOUR_BLACK) + 5;
        }
        return;
    }

    // Scenario path
    if (gConfigGeneral.debugging_tools) {
        utf8 path[MAX_PATH];

        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        shorten_path(path, sizeof(path), scenario->path, w->width - 6);

        const utf8 *pathPtr = path;
        gfx_draw_string_left(dpi, STR_STRING, (void*)&pathPtr, w->colours[1], w->x + 3, w->y + w->height - 3 - 11);
    }

    // Scenario name
    sint32 x = w->x + window_scenarioselect_widgets[WIDX_SCENARIOLIST].right + 4;
    sint32 y = w->y + window_scenarioselect_widgets[WIDX_TABCONTENT].top + 5;
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, scenario->name);
    gfx_draw_string_centred_clipped(dpi, STR_WINDOW_COLOUR_2_STRINGID, gCommonFormatArgs, COLOUR_BLACK, x + 85, y, 170);
    y += 15;

    // Scenario details
    set_format_arg(0, rct_string_id, STR_STRING);
    set_format_arg(2, const char *, scenario->details);
    y += gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, 170, STR_BLACK_STRING, COLOUR_BLACK) + 5;

    // Scenario objective
    set_format_arg(0, rct_string_id, ObjectiveNames[scenario->objective_type]);
    set_format_arg(2, sint16, scenario->objective_arg_3);
    set_format_arg(4, sint16, date_get_total_months(MONTH_OCTOBER, scenario->objective_arg_1));
    set_format_arg(6, sint32, scenario->objective_arg_2);
    y += gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, 170, STR_OBJECTIVE, COLOUR_BLACK) + 5;

    // Scenario score
    if (scenario->highscore != nullptr) {
        // TODO: Should probably be translatable
        const utf8 *completedByName = "???";
        if (!str_is_null_or_empty(scenario->highscore->name)) {
            completedByName = scenario->highscore->name;
        }
        set_format_arg(0, rct_string_id, STR_STRING);
        set_format_arg(2, const char *, completedByName);
        set_format_arg(2 + sizeof(const char *), money32, scenario->highscore->company_value);
        y += gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x, y, 170, STR_COMPLETED_BY_WITH_COMPANY_VALUE, COLOUR_BLACK);
    }
}

static void window_scenarioselect_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    sint32 colour;

    uint8 paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    rct_string_id highlighted_format = (theme_get_flags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT) ? STR_WHITE_STRING : STR_WINDOW_COLOUR_2_STRINGID;
    rct_string_id unhighlighted_format = (theme_get_flags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT) ? STR_WHITE_STRING : STR_BLACK_STRING;

    bool wide = gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN;

    rct_widget *listWidget = &w->widgets[WIDX_SCENARIOLIST];
    sint32 listWidth = listWidget->right - listWidget->left - 12;

    sint32 y = 0;
    for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
        if (y > dpi->y + dpi->height) {
            continue;
        }

        switch (listItem->type) {
        case LIST_ITEM_TYPE_HEADING:
        {
            const sint32 horizontalRuleMargin = 4;
            draw_category_heading(w, dpi, horizontalRuleMargin, listWidth - horizontalRuleMargin, y + 2, listItem->heading.string_id);
            y += 18;
            break;
        }
        case LIST_ITEM_TYPE_SCENARIO:
        {
            // Draw hover highlight
            const scenario_index_entry *scenario = listItem->scenario.scenario;
            bool isHighlighted = w->highlighted_scenario == scenario;
            if (isHighlighted) {
                gfx_filter_rect(dpi, 0, y, w->width, y + 23, PALETTE_DARKEN_1);
            }

            bool isCompleted = scenario->highscore != nullptr;
            bool isDisabled = listItem->scenario.is_locked;

            // Draw scenario name
            char buffer[64];
            safe_strcpy(buffer, scenario->name, sizeof(buffer));
            rct_string_id format = isDisabled ? STR_STRINGID : (isHighlighted ? highlighted_format : unhighlighted_format);
            set_format_arg(0, rct_string_id, STR_STRING);
            set_format_arg(2, char *, buffer);
            colour = isDisabled ? w->colours[1] | COLOUR_FLAG_INSET : COLOUR_BLACK;
            if (isDisabled) {
                gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM_DARK;
            }
            gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 1, colour, gCommonFormatArgs);

            // Check if scenario is completed
            if (isCompleted) {
                // Draw completion tick
                gfx_draw_sprite(dpi, SPR_MENU_CHECKMARK, wide ? 500 : 395, y + 1, 0);

                // Draw completion score
                const utf8 *completedByName = "???";
                if (!str_is_null_or_empty(scenario->highscore->name)) {
                    completedByName = scenario->highscore->name;
                }
                safe_strcpy(buffer, completedByName, 64);
                set_format_arg(0, rct_string_id, STR_COMPLETED_BY);
                set_format_arg(2, rct_string_id, STR_STRING);
                set_format_arg(4, char *, buffer);
                gfx_draw_string_centred(dpi, format, wide ? 270 : 210, y + 11, COLOUR_BLACK, gCommonFormatArgs);
            }

            y += 24;
            break;
        }
        }
    }
}

static void draw_category_heading(rct_window *w, rct_drawpixelinfo *dpi, sint32 left, sint32 right, sint32 y, rct_string_id stringId)
{
    uint8 baseColour = w->colours[1];
    uint8 lightColour = ColourMapA[baseColour].lighter;
    uint8 darkColour = ColourMapA[baseColour].mid_dark;

    // Draw string
    sint32 centreX = (left + right) / 2;
    gfx_draw_string_centred(dpi, stringId, centreX, y, baseColour, nullptr);

    // Get string dimensions
    utf8 *buffer = gCommonStringFormatBuffer;
    format_string(buffer, 256, stringId, nullptr);
    sint32 categoryStringHalfWidth = (gfx_get_string_width(buffer) / 2) + 4;
    sint32 strLeft = centreX - categoryStringHalfWidth;
    sint32 strRight = centreX + categoryStringHalfWidth;

    // Draw light horizontal rule
    sint32 lineY = y + 4;
    gfx_draw_line(dpi, left, lineY, strLeft, lineY, lightColour);
    gfx_draw_line(dpi, strRight, lineY, right, lineY, lightColour);

    // Draw dark horizontal rule
    lineY++;
    gfx_draw_line(dpi, left, lineY, strLeft, lineY, darkColour);
    gfx_draw_line(dpi, strRight, lineY, right, lineY, darkColour);
}

static void initialise_list_items(rct_window *w)
{
    SafeFree(_listItems);

    size_t numScenarios = scenario_repository_get_count();
    size_t capacity = numScenarios + 16;
    size_t length = 0;
    _listItems = Memory::AllocateArray<sc_list_item>(capacity);

    // Mega park unlock
    const uint32 rct1RequiredCompletedScenarios = (1 << SC_MEGA_PARK) - 1;
    uint32 rct1CompletedScenarios = 0;
    size_t megaParkListItemIndex = SIZE_MAX;

    sint32 numUnlocks = INITIAL_NUM_UNLOCKED_SCENARIOS;
    uint8 currentHeading = UINT8_MAX;
    for (size_t i = 0; i < numScenarios; i++) {
        const scenario_index_entry *scenario = scenario_repository_get_by_index(i);
        if (!is_scenario_visible(w, scenario)) {
            continue;
        }

        sc_list_item *listItem;

        // Category heading
        rct_string_id headingStringId = STR_NONE;
        if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
            if (w->selected_tab != SCENARIO_SOURCE_REAL && currentHeading != scenario->category) {
                currentHeading = scenario->category;
                headingStringId = ScenarioCategoryStringIds[currentHeading];
            }
        } else {
            if (w->selected_tab <= SCENARIO_CATEGORY_EXPERT) {
                if (currentHeading != scenario->source_game) {
                    currentHeading = scenario->source_game;
                    headingStringId = ScenarioOriginStringIds[currentHeading];
                }
            } else if (w->selected_tab == SCENARIO_CATEGORY_OTHER) {
                sint32 category = scenario->category;
                if (category <= SCENARIO_CATEGORY_REAL) {
                    category = SCENARIO_CATEGORY_OTHER;
                }
                if (currentHeading != category) {
                    currentHeading = category;
                    headingStringId = ScenarioCategoryStringIds[category];
                }
            }
        }

        if (headingStringId != STR_NONE) {
            // Ensure list capacity
            if (length == capacity) {
                capacity += 32;
                _listItems = Memory::ReallocateArray(_listItems, capacity);
            }
            listItem = &_listItems[length++];

            listItem->type = LIST_ITEM_TYPE_HEADING;
            listItem->heading.string_id = headingStringId;
        }

        // Ensure list capacity
        if (length == capacity) {
            capacity += 32;
            _listItems = Memory::ReallocateArray(_listItems, capacity);
        }
        listItem = &_listItems[length++];

        // Scenario
        listItem->type = LIST_ITEM_TYPE_SCENARIO;
        listItem->scenario.scenario = scenario;
        if (is_locking_enabled(w)) {
            listItem->scenario.is_locked = numUnlocks <= 0;
            if (scenario->highscore == nullptr) {
                numUnlocks--;
            } else {
                // Mark RCT1 scenario as completed
                if (scenario->sc_id < SC_MEGA_PARK) {
                    rct1CompletedScenarios |= 1 << scenario->sc_id;
                }
            }

            // If scenario is Mega Park, keep a reference to it
            if (scenario->sc_id == SC_MEGA_PARK) {
                megaParkListItemIndex = length - 1;
            }
        } else {
            listItem->scenario.is_locked = false;
        }
    }

    length++;
    _listItems = Memory::ReallocateArray(_listItems, capacity);
    _listItems[length - 1].type = LIST_ITEM_TYPE_END;

    // Mega park handling
    if (megaParkListItemIndex != SIZE_MAX) {
        bool megaParkLocked = (rct1CompletedScenarios & rct1RequiredCompletedScenarios) != rct1RequiredCompletedScenarios;
        _listItems[megaParkListItemIndex].scenario.is_locked = megaParkLocked;
        if (megaParkLocked && gConfigGeneral.scenario_hide_mega_park) {
            // Remove mega park
            size_t remainingItems = length - megaParkListItemIndex - 1;
            memmove(&_listItems[megaParkListItemIndex], &_listItems[megaParkListItemIndex + 1], remainingItems);

            // Remove empty headings
            sint32 i = 0;
            for (sc_list_item *listItem = _listItems; listItem->type != LIST_ITEM_TYPE_END; listItem++) {
                if (listItem->type == LIST_ITEM_TYPE_HEADING && (listItem + 1)->type != LIST_ITEM_TYPE_SCENARIO) {
                    remainingItems = length - i - 1;
                    memmove(&_listItems[i], &_listItems[i + 1], remainingItems);
                    listItem--;
                } else {
                    i++;
                }
            }
        }
    }
}

static bool is_scenario_visible(rct_window *w, const scenario_index_entry *scenario)
{
    if (gConfigGeneral.scenario_select_mode == SCENARIO_SELECT_MODE_ORIGIN) {
        if (scenario->source_game != w->selected_tab) {
            return false;
        }
    } else {
        sint32 category = scenario->category;
        if (category > SCENARIO_CATEGORY_OTHER) {
            category = SCENARIO_CATEGORY_OTHER;
        }
        if (category != w->selected_tab) {
            return false;
        }
    }
    return true;
}

static bool is_locking_enabled(rct_window *w)
{
    if (gConfigGeneral.scenario_select_mode != SCENARIO_SELECT_MODE_ORIGIN) {
        return false;
    }
    if (!gConfigGeneral.scenario_unlocking_enabled) {
        return false;
    }
    if (w->selected_tab >= 6) {
        return false;
    }
    return true;
}

extern "C"
{
    void window_scenarioselect_open(scenarioselect_callback callback)
    {
        _window_scenarioselect_open(callback);
    }
}
