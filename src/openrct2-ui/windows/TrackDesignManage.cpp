/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2/Context.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/core/Math.hpp>
#include <openrct2-ui/windows/Window.h>

#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/util/Util.h>
#include <openrct2/drawing/Drawing.h>

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
    { WWT_FRAME,            0,  0,      249,    0,      43,     STR_NONE,                   STR_NONE                },
    { WWT_CAPTION,          0,  1,      248,    1,      14,     STR_STRING,                     STR_WINDOW_TITLE_TIP    },
    { WWT_CLOSEBOX,         0,  237,    247,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP    },
    { WWT_BUTTON,           0,  10,     119,    24,     35,     STR_TRACK_MANAGE_RENAME,    STR_NONE                },
    { WWT_BUTTON,           0,  130,    239,    24,     35,     STR_TRACK_MANAGE_DELETE,    STR_NONE                },
    { WIDGETS_END }
};

static rct_widget window_track_delete_prompt_widgets[] = {
    { WWT_FRAME,            0,  0,      249,    0,      73,     STR_NONE,                   STR_NONE                },
    { WWT_CAPTION,          0,  1,      248,    1,      14,     STR_DELETE_FILE,            STR_WINDOW_TITLE_TIP    },
    { WWT_CLOSEBOX,         0,  237,    247,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP    },
    { WWT_BUTTON,           0,  10,     119,    54,     65,     STR_TRACK_MANAGE_DELETE,    STR_NONE                },
    { WWT_BUTTON,           0,  130,    239,    54,     65,     STR_CANCEL,                 STR_NONE                },
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
static rct_window_event_list window_track_manage_events = {
    window_track_manage_close,
    window_track_manage_mouseup,
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
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_track_manage_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_track_manage_paint,
    nullptr
};

// 0x0099415C
static rct_window_event_list window_track_delete_prompt_events = {
    nullptr,
    window_track_delete_prompt_mouseup,
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
    window_track_delete_prompt_paint,
    nullptr
};
// clang-format on

#pragma endregion

static track_design_file_ref *_trackDesignFileReference;

static void window_track_delete_prompt_open();
static void window_track_design_list_reload_tracks();

/**
 *
 *  rct2: 0x006D348F
 */
rct_window * window_track_manage_open(track_design_file_ref *tdFileRef)
{
    window_close_by_class(WC_MANAGE_TRACK_DESIGN);

    rct_window *w = window_create_centred(
        250,
        44,
        &window_track_manage_events,
        WC_MANAGE_TRACK_DESIGN,
        WF_STICK_TO_FRONT | WF_TRANSPARENT
    );
    w->widgets = window_track_manage_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_RENAME) |
        (1 << WIDX_DELETE);
    window_init_scroll_widgets(w);

    rct_window *trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackDesignListWindow != nullptr) {
        trackDesignListWindow->track_list.track_list_being_updated = true;
    }

    _trackDesignFileReference = tdFileRef;

    return w;
}

/**
 *
 *  rct2: 0x006D364C
 */
static void window_track_manage_close(rct_window *w)
{
    rct_window *trackDesignListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackDesignListWindow != nullptr) {
        trackDesignListWindow->track_list.track_list_being_updated = false;
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_RENAME:
        window_text_input_raw_open(
            w,
            widgetIndex,
            STR_TRACK_DESIGN_RENAME_TITLE,
            STR_TRACK_DESIGN_RENAME_DESC,
            _trackDesignFileReference->name,
            127
        );
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
static void window_track_manage_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_RENAME || str_is_null_or_empty(text)) {
        return;
    }

    if (str_is_null_or_empty(text)) {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_NONE);
        return;
    }

    if (!filename_valid_characters(text)) {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_NEW_NAME_CONTAINS_INVALID_CHARACTERS);
        return;
    }

    if (track_repository_rename(_trackDesignFileReference->path, text)) {
        window_close_by_class(WC_TRACK_DELETE_PROMPT);
        window_close(w);
        window_track_design_list_reload_tracks();
    } else {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_ANOTHER_FILE_EXISTS_WITH_NAME_OR_FILE_IS_WRITE_PROTECTED);
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void window_track_manage_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    set_format_arg(0, char *, _trackDesignFileReference->name);
    window_draw_widgets(w, dpi);
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
    rct_window *w = window_create(
        std::max(TOP_TOOLBAR_HEIGHT + 1, (screenWidth - 250) / 2),
        (screenHeight - 44) / 2,
        250,
        74,
        &window_track_delete_prompt_events,
        WC_TRACK_DELETE_PROMPT,
        WF_STICK_TO_FRONT
    );
    w->widgets = window_track_delete_prompt_widgets;
    w->enabled_widgets =
        (1 << WIDX_CLOSE) |
        (1 << WIDX_RENAME) |
        (1 << WIDX_DELETE);
    window_init_scroll_widgets(w);
    w->flags |= WF_TRANSPARENT;
}

/**
 *
 *  rct2: 0x006D3823
 */
static void window_track_delete_prompt_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
    case WIDX_PROMPT_CANCEL:
        window_close(w);
        break;
    case WIDX_PROMPT_DELETE:
        window_close(w);
        if (track_repository_delete(_trackDesignFileReference->path)) {
            window_close_by_class(WC_MANAGE_TRACK_DESIGN);
            window_track_design_list_reload_tracks();
        } else {
            context_show_error(STR_CANT_DELETE_TRACK_DESIGN, STR_FILE_IS_WRITE_PROTECTED_OR_LOCKED);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006D37EE
 */
static void window_track_delete_prompt_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    gfx_draw_string_centred_wrapped(
        dpi,
        &_trackDesignFileReference->name,
        w->x + 125,
        w->y + 28,
        246,
        STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK,
        COLOUR_BLACK
    );
}

static void window_track_design_list_reload_tracks()
{
    rct_window * trackListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
    if (trackListWindow != nullptr) {
        trackListWindow->track_list.reload_track_designs = true;
    }
}
