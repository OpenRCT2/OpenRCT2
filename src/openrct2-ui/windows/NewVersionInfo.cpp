/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <fstream>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/Version.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/util/Util.h>
#include <vector>

using namespace OpenRCT2;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_CONTENT_PANEL,
    WIDX_SCROLL,
    WIDX_OPEN_URL,
};

static constexpr const int32_t WW = 500;
static constexpr const int32_t WH = 400;
static constexpr const rct_string_id WINDOW_TITLE = STR_CHANGELOG_TITLE;
constexpr int32_t MIN_WW = 300;
constexpr int32_t MIN_WH = 250;

static rct_widget window_new_version_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({0, 14}, {500, 382}, WWT_RESIZE, 1             ), // content panel
    MakeWidget({3, 16}, {495, 366}, WWT_SCROLL, 1, SCROLL_BOTH), // scroll area
    MakeWidget({3, 473}, {300, 14}, WWT_BUTTON, 1, STR_NEW_RELEASE_DOWNLOAD_PAGE), // changelog button
    { WIDGETS_END },
};

static void window_new_version_close(rct_window *w);
static void window_new_version_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_new_version_resize(rct_window *w);
static void window_new_version_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_new_version_invalidate(rct_window *w);
static void window_new_version_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_new_version_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_new_version_events = {
    window_new_version_close,
    window_new_version_mouseup,
    window_new_version_resize,
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
    window_new_version_scrollgetsize,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_new_version_invalidate,
    window_new_version_paint,
    window_new_version_scrollpaint
};
// clang-format on

static void window_new_version_process_info();
static void window_new_version_dispose_data();

static const NewVersionInfo* _newVersionInfo;
static std::vector<std::string> _changelogLines;
static int32_t _changelogLongestLineWidth = 0;

rct_window* window_new_version_open()
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_NEW_VERSION);
    if (window != nullptr)
    {
        return window;
    }

    if (!GetContext()->HasNewVersionInfo())
    {
        return nullptr;
    }

    window_new_version_process_info();

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();

    window = window_create_centred(
        screenWidth * 4 / 5, screenHeight * 4 / 5, &window_new_version_events, WC_NEW_VERSION, WF_RESIZABLE);
    window->widgets = window_new_version_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_OPEN_URL);

    window_init_scroll_widgets(window);
    window->min_width = MIN_WW;
    window->min_height = MIN_WH;
    window->max_width = MIN_WW;
    window->max_height = MIN_WH;

    window->colours[0] = COLOUR_GREY;
    window->colours[1] = COLOUR_LIGHT_BLUE;
    window->colours[2] = COLOUR_LIGHT_BLUE;

    return window;
}

static void window_new_version_close([[maybe_unused]] rct_window* w)
{
    window_new_version_dispose_data();
}

static void window_new_version_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_OPEN_URL:
            GetContext()->GetUiContext()->OpenURL(_newVersionInfo->url);
            break;
    }
}

static void window_new_version_resize(rct_window* w)
{
    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();

    w->max_width = (screenWidth * 4) / 5;
    w->max_height = (screenHeight * 4) / 5;

    w->min_width = MIN_WW;
    w->min_height = MIN_WH;

    auto download_button_width = window_new_version_widgets[WIDX_OPEN_URL].width();
    window_new_version_widgets[WIDX_OPEN_URL].left = (w->width - download_button_width) / 2;
    window_new_version_widgets[WIDX_OPEN_URL].right = window_new_version_widgets[WIDX_OPEN_URL].left + download_button_width;

    if (w->width < w->min_width)
    {
        w->Invalidate();
        w->width = w->min_width;
    }
    if (w->height < w->min_height)
    {
        w->Invalidate();
        w->height = w->min_height;
    }
}

static void window_new_version_scrollgetsize(
    [[maybe_unused]] rct_window* w, [[maybe_unused]] int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = _changelogLongestLineWidth + 4;

    const int32_t lineHeight = font_get_line_height(gCurrentFontSpriteBase);
    *height = static_cast<int32_t>(_changelogLines.size() * lineHeight);
}

static void window_new_version_invalidate(rct_window* w)
{
    window_new_version_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_new_version_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_new_version_widgets[WIDX_TITLE].right = w->width - 2;
    window_new_version_widgets[WIDX_CLOSE].left = w->width - 13;
    window_new_version_widgets[WIDX_CLOSE].right = w->width - 3;
    window_new_version_widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
    window_new_version_widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
    window_new_version_widgets[WIDX_SCROLL].right = w->width - 3;
    window_new_version_widgets[WIDX_SCROLL].bottom = w->height - 22;
    window_new_version_widgets[WIDX_OPEN_URL].bottom = w->height - 5;
    window_new_version_widgets[WIDX_OPEN_URL].top = w->height - 19;
}

static void window_new_version_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
}

static void window_new_version_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, [[maybe_unused]] int32_t scrollIndex)
{
    gCurrentFontFlags = 0;
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    const int32_t lineHeight = font_get_line_height(gCurrentFontSpriteBase);

    ScreenCoordsXY screenCoords(3, 3 - lineHeight);
    for (auto line : _changelogLines)
    {
        screenCoords.y += lineHeight;
        if (screenCoords.y + lineHeight < dpi->y || screenCoords.y >= dpi->y + dpi->height)
            continue;

        gfx_draw_string(dpi, line.c_str(), w->colours[0], screenCoords);
    }
}

static void window_new_version_process_info()
{
    _newVersionInfo = GetContext()->GetNewVersionInfo();

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    char version_info[256];

    const char* version_info_ptr = _newVersionInfo->name.c_str();
    format_string(version_info, 256, STR_NEW_RELEASE_VERSION_INFO, &version_info_ptr);

    _changelogLines.push_back(version_info);
    _changelogLines.push_back("");

    while ((pos = _newVersionInfo->changelog.find("\n", prev)) != std::string::npos)
    {
        std::string line = _newVersionInfo->changelog.substr(prev, pos - prev);
        for (char* ch = line.data(); *ch != '\0'; ch++)
        {
            if (utf8_is_format_code(*ch))
            {
                *ch = FORMAT_OUTLINE_OFF;
            }
        }
        _changelogLines.push_back(line);
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    _changelogLines.push_back(_newVersionInfo->changelog.substr(prev));

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
    _changelogLongestLineWidth = 0;
    for (auto line : _changelogLines)
    {
        auto width = gfx_get_string_width(line.c_str());
        _changelogLongestLineWidth = std::max(width, _changelogLongestLineWidth);
    }
}

static void window_new_version_dispose_data()
{
    _changelogLines.clear();
    _changelogLines.shrink_to_fit();
}
