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

#include <fstream>
#include <vector>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/platform/platform.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/util/Util.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/drawing/Drawing.h>

using namespace OpenRCT2;

enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CONTENT_PANEL,
    WIDX_SCROLL
};

#define WW 500
#define WH 400
#define MIN_WW 300
#define MIN_WH 200

static rct_widget window_changelog_widgets[] = {
    { WWT_FRAME,            0,  0,          WW - 1, 0,      WH - 1,     0xFFFFFFFF,                     STR_NONE },             // panel / background
    { WWT_CAPTION,          0,  1,          WW - 2, 1,      14,         STR_CHANGELOG_TITLE,            STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,         0,  WW - 13,    WW - 3, 2,      13,         STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_RESIZE,           1,  0,          WW - 1, 14,     WH - 1,     0xFFFFFFFF,                     STR_NONE },             // content panel
    { WWT_SCROLL,           1,  3,          WW - 3, 16,     WH - 15,    SCROLL_BOTH,                    STR_NONE },             // scroll area
    { WIDGETS_END },
};

static void window_changelog_close(rct_window *w);
static void window_changelog_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_changelog_resize(rct_window *w);
static void window_changelog_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_changelog_invalidate(rct_window *w);
static void window_changelog_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_changelog_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_changelog_events = {
    window_changelog_close,
    window_changelog_mouseup,
    window_changelog_resize,
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
    window_changelog_scrollgetsize,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_changelog_invalidate,
    window_changelog_paint,
    window_changelog_scrollpaint
};

static bool window_changelog_read_file();
static void window_changelog_dispose_file();

static std::string _changelogText;
static std::vector<const char *> _changelogLines;
static sint32 _changelogLongestLineWidth = 0;

rct_window * window_changelog_open()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_CHANGELOG);
    if (window != nullptr)
        return window;

    if (!window_changelog_read_file())
        return nullptr;

    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();

    window = window_create_centred(
        screenWidth * 4 / 5,
        screenHeight * 4 / 5,
        &window_changelog_events,
        WC_CHANGELOG,
        WF_RESIZABLE
    );
    window->widgets = window_changelog_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE);

    window_init_scroll_widgets(window);
    window->min_width = MIN_WW;
    window->min_height = MIN_WH;
    window->max_width = MIN_WW;
    window->max_height = MIN_WH;
    return window;
}

static void window_changelog_close(rct_window *w)
{
    window_changelog_dispose_file();
}

static void window_changelog_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    }
}

static void window_changelog_resize(rct_window *w)
{
    sint32 screenWidth = context_get_width();
    sint32 screenHeight = context_get_height();

    w->max_width = (screenWidth * 4) / 5;
    w->max_height = (screenHeight * 4) / 5;

    w->min_width = MIN_WW;
    w->min_height = MIN_WH;
    if (w->width < w->min_width) {
        window_invalidate(w);
        w->width = w->min_width;
    }
    if (w->height < w->min_height) {
        window_invalidate(w);
        w->height = w->min_height;
    }
}

static void window_changelog_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
    *width = _changelogLongestLineWidth + 4;
    *height = (sint32)(_changelogLines.size() * 11);
}

static void window_changelog_invalidate(rct_window *w)
{
    window_changelog_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_changelog_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_changelog_widgets[WIDX_TITLE].right = w->width - 2;
    window_changelog_widgets[WIDX_CLOSE].left = w->width - 13;
    window_changelog_widgets[WIDX_CLOSE].right = w->width - 3;
    window_changelog_widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
    window_changelog_widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
    window_changelog_widgets[WIDX_SCROLL].right = w->width - 3;
    window_changelog_widgets[WIDX_SCROLL].bottom = w->height - 15;
}

static void window_changelog_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
}

static void window_changelog_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
    gCurrentFontFlags = 0;
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    sint32 x = 3;
    sint32 y = 3;
    for (auto line : _changelogLines)
    {
        gfx_draw_string(dpi, (char *)line, w->colours[0], x, y);
        y += 11;
    }
}

static std::string GetChangelogPath()
{
    auto env = GetContext()->GetPlatformEnvironment();
    return env->GetFilePath(PATHID::CHANGELOG);
}

static std::string GetChangelogText()
{
    auto path = GetChangelogPath();
#if defined(_WIN32) && !defined(__MINGW32__)
    auto pathW = String::ToUtf16(path);
    auto fs = std::ifstream(pathW, std::ios::in);
#else
    auto fs = std::ifstream(path, std::ios::in);
#endif
    if (!fs.is_open())
    {
        throw std::runtime_error("Unable to open " + path);
    }
    return std::string((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
}

static bool window_changelog_read_file()
{
    try
    {
        _changelogText = GetChangelogText();
    }
    catch (const std::bad_alloc &)
    {
        log_error("Unable to allocate memory for changelog.txt");
        return false;
    }
    catch (const std::exception &)
    {
        log_error("Unable to read changelog.txt");
        return false;
    }

    // Non-const cast required until C++17 is enabled
    auto * start = (char *)_changelogText.data();
    if (_changelogText.size() >= 3 && utf8_is_bom(start))
    {
        start += 3;
    }

    _changelogLines.clear();
    _changelogLines.push_back(start);
    auto ch = start;
    while (*ch != '\0')
    {
        uint8 c = *ch;
        if (c == '\n')
        {
            *ch++ = 0;
            _changelogLines.push_back(ch);
        }
        else if (utf8_is_format_code(c))
        {
            *ch++ = FORMAT_OUTLINE_OFF;
        }
        else
        {
            ch++;
        }
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    _changelogLongestLineWidth = 0;
    for (auto line : _changelogLines)
    {
        auto width = gfx_get_string_width(line);
        _changelogLongestLineWidth = Math::Max(width, _changelogLongestLineWidth);
    }
    return true;
}

static void window_changelog_dispose_file()
{
    _changelogText = std::string();
    _changelogLines.clear();
    _changelogLines.shrink_to_fit();
}
