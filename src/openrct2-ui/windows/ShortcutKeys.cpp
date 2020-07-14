/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../input/ShortcutManager.h"
#include "Window.h"

#include <openrct2-ui/input/KeyboardShortcuts.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>

using namespace OpenRCT2::Ui;

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
    MakeWidget({4,    18}, {412, 245}, WindowWidgetType::Scroll, WindowColour::Primary, SCROLL_VERTICAL,           STR_SHORTCUT_LIST_TIP        ),
    MakeWidget({4, WH-15}, {150,  12}, WindowWidgetType::Button, WindowColour::Primary, STR_SHORTCUT_ACTION_RESET, STR_SHORTCUT_ACTION_RESET_TIP),
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

static rct_window_event_list window_shortcut_events([](auto& events)
{
    events.mouse_up = &window_shortcut_mouseup;
    events.resize = &window_shortcut_resize;
    events.get_scroll_size = &window_shortcut_scrollgetsize;
    events.scroll_mousedown = &window_shortcut_scrollmousedown;
    events.scroll_mouseover = &window_shortcut_scrollmouseover;
    events.invalidate = &window_shortcut_invalidate;
    events.paint = &window_shortcut_paint;
    events.scroll_paint = &window_shortcut_scrollpaint;
});

struct ShortcutStringPair
{
    size_t ShortcutIndex;
    std::string ShortcutId;
    rct_string_id StringId = STR_NONE;
};

static std::vector<ShortcutStringPair> _shortcutList;

static void InitialiseShortcutList()
{
    _shortcutList.clear();

    size_t index = 0;
    std::string group;
    auto& shortcutManager = GetShortcutManager();
    for (auto& shortcut : shortcutManager.Shortcuts)
    {
        if (group.empty())
        {
            group = shortcut.GetGroup();
        }
        else
        {
            auto groupName = shortcut.GetGroup();
            if (group != groupName)
            {
                // Add separator
                group = groupName;
                _shortcutList.emplace_back();
            }
        }

        ShortcutStringPair ssp;
        ssp.ShortcutIndex = index;
        ssp.ShortcutId = shortcut.Id;
        ssp.StringId = shortcut.LocalisedName;
        _shortcutList.push_back(std::move(ssp));
        index++;
    }
}

static void FormatKeyChordsString(size_t index, char* dst, size_t dstLen)
{
    if (dstLen == 0)
        return;

    auto& shortcutManager = GetShortcutManager();
    auto shortcutIndex = _shortcutList[index].ShortcutIndex;
    const auto& shortcut = shortcutManager.Shortcuts[shortcutIndex];
    auto numChords = shortcut.Current.size();
    *dst = '\0';
    for (size_t i = 0; i < numChords; i++)
    {
        const auto &kc = shortcut.Current[i];
        auto szkc = kc.ToString();
        String::Append(dst, dstLen, szkc.c_str());
        if (i < numChords - 1)
        {
            String::Append(dst, dstLen, " or ");
        }
    }
}

/**
 *
 *  rct2: 0x006E3884
 */
rct_window* window_shortcut_keys_open()
{
    rct_window* w = window_bring_to_front_by_class(WC_KEYBOARD_SHORTCUT_LIST);
    if (w == nullptr)
    {
        InitialiseShortcutList();

        w = WindowCreateAutoPos(WW, WH, &window_shortcut_events, WC_KEYBOARD_SHORTCUT_LIST, WF_RESIZABLE);

        w->widgets = window_shortcut_widgets;
        w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_RESET);
        WindowInitScrollWidgets(w);

        w->no_list_items = static_cast<uint16_t>(_shortcutList.size());
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
            KeyboardShortcutsReset();
            KeyboardShortcutsSave();
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
    WindowDrawWidgets(w, dpi);
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
    if (_shortcutList[selected_item].ShortcutId.empty())
        return;

    auto& shortcut = _shortcutList[selected_item];
    window_shortcut_change_open(shortcut.ShortcutId);
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
        if (_shortcutList[i].ShortcutId.empty())
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
            gfx_filter_rect(dpi, 0, y - 1, scrollWidth, y + (SCROLLABLE_ROW_HEIGHT - 2), FilterPaletteID::PaletteDarken1);
        }

        const int32_t bindingOffset = scrollWidth - 150;
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_SHORTCUT_ENTRY_FORMAT);
        ft.Add<rct_string_id>(_shortcutList[i].StringId);
        DrawTextEllipsised(dpi, { 0, y - 1 }, bindingOffset, format, ft, COLOUR_BLACK);

        char keybinding[128];
        FormatKeyChordsString(i, keybinding, sizeof(keybinding));

        if (strlen(keybinding) > 0)
        {
            const int32_t maxWidth = 150;
            ft = Formatter();
            ft.Add<rct_string_id>(STR_STRING);
            ft.Add<char*>(keybinding);
            DrawTextEllipsised(dpi, { bindingOffset, y - 1 }, maxWidth, format, ft, COLOUR_BLACK);
        }
    }
}
