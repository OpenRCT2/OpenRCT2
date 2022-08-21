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
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/TrackDesignRepository.h>

static constexpr const StringId WINDOW_TITLE = STR_STRING;
static constexpr const int32_t WH = 44;
static constexpr const int32_t WW = 250;
static constexpr const int32_t WH_DELETE_PROMPT = 74;
static constexpr const int32_t WW_DELETE_PROMPT = 250;
static constexpr const int32_t TrackDesignNameMaxLength = 127;

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
    WIDGETS_END,
};

static rct_widget window_track_delete_prompt_widgets[] = {
    WINDOW_SHIM(STR_DELETE_FILE, WW_DELETE_PROMPT, WH_DELETE_PROMPT),
    MakeWidget({ 10, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_TRACK_MANAGE_DELETE),
    MakeWidget({130, 54}, {110, 12}, WindowWidgetType::Button, WindowColour::Primary, STR_CANCEL             ),
    WIDGETS_END,
};

#pragma endregion

#pragma region Events

static void WindowTrackManageClose(rct_window *w);
static void WindowTrackManageMouseup(rct_window *w, WidgetIndex widgetIndex);
static void WindowTrackManageTextinput(rct_window *w, WidgetIndex widgetIndex, char *text);
static void WindowTrackManagePaint(rct_window *w, rct_drawpixelinfo *dpi);

static void WindowTrackDeletePromptMouseup(rct_window *w, WidgetIndex widgetIndex);
static void WindowTrackDeletePromptPaint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x009940EC
static rct_window_event_list window_track_manage_events([](auto& events)
{
    events.close = &WindowTrackManageClose;
    events.mouse_up = &WindowTrackManageMouseup;
    events.text_input = &WindowTrackManageTextinput;
    events.paint = &WindowTrackManagePaint;
});

// 0x0099415C
static rct_window_event_list window_track_delete_prompt_events([](auto& events)
{
    events.mouse_up = &WindowTrackDeletePromptMouseup;
    events.paint = &WindowTrackDeletePromptPaint;
});
// clang-format on

#pragma endregion

static TrackDesignFileRef* _trackDesignFileReference;

static void WindowTrackDeletePromptOpen();
static void WindowTrackDesignListReloadTracks();

/**
 *
 *  rct2: 0x006D348F
 */
rct_window* WindowTrackManageOpen(TrackDesignFileRef* tdFileRef)
{
    window_close_by_class(WindowClass::ManageTrackDesign);

    rct_window* w = WindowCreateCentred(
        WW, WH, &window_track_manage_events, WindowClass::ManageTrackDesign, WF_STICK_TO_FRONT | WF_TRANSPARENT);
    w->widgets = window_track_manage_widgets;
    WindowInitScrollWidgets(*w);

    rct_window* trackDesignListWindow = window_find_by_class(WindowClass::TrackDesignList);
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
static void WindowTrackManageClose(rct_window* w)
{
    rct_window* trackDesignListWindow = window_find_by_class(WindowClass::TrackDesignList);
    if (trackDesignListWindow != nullptr)
    {
        trackDesignListWindow->track_list.track_list_being_updated = false;
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void WindowTrackManageMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close_by_class(WindowClass::TrackDeletePrompt);
            window_close(*w);
            break;
        case WIDX_RENAME:
            WindowTextInputRawOpen(
                w, widgetIndex, STR_TRACK_DESIGN_RENAME_TITLE, STR_TRACK_DESIGN_RENAME_DESC, {},
                _trackDesignFileReference->name.c_str(), TrackDesignNameMaxLength);
            break;
        case WIDX_DELETE:
            WindowTrackDeletePromptOpen();
            break;
    }
}

/**
 *
 *  rct2: 0x006D3523
 */
static void WindowTrackManageTextinput(rct_window* w, WidgetIndex widgetIndex, char* text)
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

    if (!Platform::IsFilenameValid(text))
    {
        context_show_error(STR_CANT_RENAME_TRACK_DESIGN, STR_NEW_NAME_CONTAINS_INVALID_CHARACTERS, {});
        return;
    }

    if (track_repository_rename(_trackDesignFileReference->path, text))
    {
        window_close_by_class(WindowClass::TrackDeletePrompt);
        window_close(*w);
        WindowTrackDesignListReloadTracks();
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
static void WindowTrackManagePaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Formatter::Common().Add<const utf8*>(_trackDesignFileReference->name.c_str());
    WindowDrawWidgets(*w, dpi);
}

/**
 *
 *  rct2: 0x006D35CD
 */
static void WindowTrackDeletePromptOpen()
{
    window_close_by_class(WindowClass::TrackDeletePrompt);

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    rct_window* w = WindowCreate(
        ScreenCoordsXY(
            std::max(TOP_TOOLBAR_HEIGHT + 1, (screenWidth - WW_DELETE_PROMPT) / 2), (screenHeight - WH_DELETE_PROMPT) / 2),
        WW_DELETE_PROMPT, WH_DELETE_PROMPT, &window_track_delete_prompt_events, WindowClass::TrackDeletePrompt,
        WF_STICK_TO_FRONT);
    w->widgets = window_track_delete_prompt_widgets;
    WindowInitScrollWidgets(*w);
    w->flags |= WF_TRANSPARENT;
}

/**
 *
 *  rct2: 0x006D3823
 */
static void WindowTrackDeletePromptMouseup(rct_window* w, WidgetIndex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
        case WIDX_PROMPT_CANCEL:
            window_close(*w);
            break;
        case WIDX_PROMPT_DELETE:
            window_close(*w);
            if (track_repository_delete(_trackDesignFileReference->path))
            {
                window_close_by_class(WindowClass::ManageTrackDesign);
                WindowTrackDesignListReloadTracks();
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
static void WindowTrackDeletePromptPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(*w, dpi);

    auto ft = Formatter();
    ft.Add<const utf8*>(_trackDesignFileReference->name.c_str());
    DrawTextWrapped(
        dpi, { w->windowPos.x + (WW_DELETE_PROMPT / 2), w->windowPos.y + ((WH_DELETE_PROMPT / 2) - 9) }, (WW_DELETE_PROMPT - 4),
        STR_ARE_YOU_SURE_YOU_WANT_TO_PERMANENTLY_DELETE_TRACK, ft, { TextAlignment::CENTRE });
}

static void WindowTrackDesignListReloadTracks()
{
    rct_window* trackListWindow = window_find_by_class(WindowClass::TrackDesignList);
    if (trackListWindow != nullptr)
    {
        trackListWindow->track_list.reload_track_designs = true;
    }
}
