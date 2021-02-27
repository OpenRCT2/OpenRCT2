/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/util/Util.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_STRING;
static constexpr const int32_t WH = 44;
static constexpr const int32_t WW = 250;

#pragma region Widgets

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RENAME,
    WIDX_DELETE,

    WIDX_PROMPT_DELETE = 3,
    WIDX_PROMPT_CANCEL = 4,
};

static rct_widget window_track_manage_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 10, 24}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_RENAME),
    MakeWidget({130, 24}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_DELETE),
    { WIDGETS_END }
};

static rct_widget window_track_delete_prompt_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 10, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_DELETE),
    MakeWidget({130, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_CANCEL             ),
    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_track_manage_close(rct_window *w);
static void window_track_manage_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_track_manage_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_track_manage_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_track_delete_prompt_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_track_delete_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x009940EC
static rct_window_event_list window_track_manage_events([](auto& events)
{
    events.close = &window_track_manage_close;
    events.mouse_up = &window_track_manage_mouseup;
    events.text_input = &window_track_manage_textinput;
    events.paint = &window_track_manage_paint;
});

// 0x0099415C
static rct_window_event_list window_track_delete_prompt_events([](auto& events)
{
    events.mouse_up = &window_track_delete_prompt_mouseup;
    events.paint = &window_track_delete_prompt_paint;
});
// clang-format on

#pragma endregion

static track_design_file_ref* _trackDesignFileReference;

static void window_track_delete_prompt_open();
static void window_track_design_list_reload_tracks();

/**
 *
 *  rct2: 0x006D348F
 */
rct_window* window_track_manage_open(track_design_file_ref* tdFileRef)
{
    window_close_by_class(WC_MANAGE_TRACK_DESIGN);

    rct_window* w = WindowCreateCentred(
        250, 44, &window_track_manage_events, WC_MANAGE_TRACK_DESIGN, WF_STICK_TO_FRONT | WF_TRANSPARENT);
    w->widgets = window_track_manage_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RENAME) | (1 << WIDX_DELETE);
    WindowInitScrollWidgets(w);

    rct_window* trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackDesignListWindow != nullptr)
    {
        trackDesignListWindow->track_list.track_list_being_updated = true;
    }

    _trackDesignFileReference = tdFileRef;

    return w;
}

/**
 *
 *  rct2: 0x006D364C
 */
static void window_track_manage_close(rct_window* w)
{
    rct_window* trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackDesignListWindow != nullptr)
    {
        trackDesignListWindow->track_list.track_list_being_updated = false;
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_RENAME:
            window_text_input_raw_open(
                w, widgetIndex, STR_TRACK_DESIGN_RENAME_TITLE, STR_TRACK_DESIGN_RENAME_DESC, _trackDesignFileReference->name,
                127);
            break;
        case WIDX_DELETE:
            window_track_delete_prompt_open();
            break;
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_RENAME || str_is_null_or_empty(text))
    {
        return;
    }

    if (str_is_null_or_empty(text))
    {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_NONE, {});
        return;
    }

    if (!filename_valid_characters(text))
    {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_NEW_NAME_CONTAINS_INVALID_CHARACTERS, {});
        return;
    }

    if (track_repository_rename(_trackDesignFileReference->path, text))
    {
        window_close_by_class(WC_TRACK_DELETE_PROMPT);
        window_close(w);
        window_track_design_list_reload_tracks();
    }
    else
    {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_ANOTHER_FILE_EXISTS_WITH_NAME_OR_FILE_IS_WRITE_PROTECTED, {});
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Formatter::Common().Add<char*>(_trackDesignFileReference->name);
    WindowDrawWidgets(w, dpi);
}

/**
 *
 *  rct2: 0x006D35CD
 */
static void window_track_delete_prompt_open()
{
    window_close_by_class(WC_TRACK_DELETE_PROMPT);

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    rct_window* w = WindowCreate(
        ScreenCoordsXY(std::max(TOP_TOOLBAR_HEIGHT + 1, (screenWidth - 250) / 2), (screenHeight - 44) / 2), 250, 74,
        &window_track_delete_prompt_events, WC_TRACK_DELETE_PROMPT, WF_STICK_TO_FRONT);
    w->widgets = window_track_delete_prompt_widgets;
    w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RENAME) | (1 << WIDX_DELETE);
    WindowInitScrollWidgets(w);
    w->flags |= WF_TRANSPARENT;
}

/**
 *
 *  rct2: 0x006D3823
 */
static void window_track_delete_prompt_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
        case WIDX_PROMPT_CANCEL:
            window_close(w);
            break;
        case WIDX_PROMPT_DELETE:
            window_close(w);
            if (track_repository_delete(_trackDesignFileReference->path))
            {
                window_close_by_class(WC_MANAGE_TRACK_DESIGN);
                window_track_design_list_reload_tracks();
            }
            else
            {
                context_show_error(STR_CANT_DELETE_TRACK_DESIGN, STR_FILE_IS_WRITE_PROTECTED_OR_LOCKED, {});
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006D37EE
 */
static void window_track_delete_prompt_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    DrawTextWrapped(
        dpi, { w->windowPos.x + 125, w->windowPos.y + 28 }, 246, STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK,
        &_trackDesignFileReference->name, { TextAlignment::CENTRE });
}

static void window_track_design_list_reload_tracks()
{
    rct_window* trackListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackListWindow != nullptr)
    {
        trackListWindow->track_list.reload_track_designs = true;
    }
}
