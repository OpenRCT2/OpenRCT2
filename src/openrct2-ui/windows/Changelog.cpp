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

static rct_widget window_changelog_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({0,  14}, {500, 382}, WindowWidgetType::Resize,      WindowColour::Secondary                               ), // content panel
    MakeWidget({3,  16}, {495, 366}, WindowWidgetType::Scroll,      WindowColour::Secondary, SCROLL_BOTH                  ), // scroll area
    MakeWidget({3, 473}, {300,  14}, WindowWidgetType::Placeholder, WindowColour::Secondary, STR_NEW_RELEASE_DOWNLOAD_PAGE), // changelog button
    { WIDGETS_END },
};

static void window_changelog_close(rct_window *w);
static void window_changelog_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_changelog_resize(rct_window *w);
static void window_changelog_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_changelog_invalidate(rct_window *w);
static void window_changelog_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_changelog_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_changelog_events([](auto& events)
{
    events.close = &window_changelog_close;
    events.mouse_up = &window_changelog_mouseup;
    events.resize = &window_changelog_resize;
    events.get_scroll_size = &window_changelog_scrollgetsize;
    events.invalidate = &window_changelog_invalidate;
    events.paint = &window_changelog_paint;
    events.scroll_paint = &window_changelog_scrollpaint;
});
// clang-format on

static void window_new_version_process_info();
static void window_changelog_dispose_data();
static bool window_changelog_read_file();

static const NewVersionInfo* _newVersionInfo;
static std::vector<std::string> _changelogLines;
static int32_t _changelogLongestLineWidth = 0;
static int _persnality = 0;

rct_window* window_changelog_open(int personality)
{
    rct_window* window;

    window = window_bring_to_front_by_class(WC_CHANGELOG);
    if (window != nullptr)
    {
        return window;
    }

    uint64_t enabled_widgets{};

    window_changelog_widgets[WIDX_OPEN_URL].type = WindowWidgetType::Placeholder;
    switch (personality)
    {
        case WV_NEW_VERSION_INFO:
            if (!GetContext()->HasNewVersionInfo())
            {
                return nullptr;
            }

            _persnality = WV_NEW_VERSION_INFO;
            window_new_version_process_info();
            enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_OPEN_URL);
            window_changelog_widgets[WIDX_OPEN_URL].type = WindowWidgetType::Button;
            break;

        case WV_CHANGELOG:
            if (!window_changelog_read_file())
            {
                return nullptr;
            }

            _persnality = WV_CHANGELOG;
            enabled_widgets = (1 << WIDX_CLOSE);
            break;

        default:
            log_error("Invalid personality for changelog window: %d", personality);
            return nullptr;
    }

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();

    window = WindowCreateCentred(
        screenWidth * 4 / 5, screenHeight * 4 / 5, &window_changelog_events, WC_CHANGELOG, WF_RESIZABLE);
    window->widgets = window_changelog_widgets;
    window->enabled_widgets = enabled_widgets;

    WindowInitScrollWidgets(window);
    window->min_width = MIN_WW;
    window->min_height = MIN_WH;
    window->max_width = MIN_WW;
    window->max_height = MIN_WH;

    return window;
}

static void window_changelog_close([[maybe_unused]] rct_window* w)
{
    window_changelog_dispose_data();
}

static void window_changelog_mouseup(rct_window* w, rct_widgetindex widgetIndex)
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

static void window_changelog_resize(rct_window* w)
{
    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();

    w->max_width = (screenWidth * 4) / 5;
    w->max_height = (screenHeight * 4) / 5;

    w->min_width = MIN_WW;
    w->min_height = MIN_WH;

    auto download_button_width = window_changelog_widgets[WIDX_OPEN_URL].width();
    window_changelog_widgets[WIDX_OPEN_URL].left = (w->width - download_button_width) / 2;
    window_changelog_widgets[WIDX_OPEN_URL].right = window_changelog_widgets[WIDX_OPEN_URL].left + download_button_width;

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

static void window_changelog_scrollgetsize(
    [[maybe_unused]] rct_window* w, [[maybe_unused]] int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *width = _changelogLongestLineWidth + 4;

    const int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);
    *height = static_cast<int32_t>(_changelogLines.size() * lineHeight);
}

static void window_changelog_invalidate(rct_window* w)
{
    window_changelog_widgets[WIDX_BACKGROUND].right = w->width - 1;
    window_changelog_widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    window_changelog_widgets[WIDX_TITLE].right = w->width - 2;
    window_changelog_widgets[WIDX_CLOSE].left = w->width - 13;
    window_changelog_widgets[WIDX_CLOSE].right = w->width - 3;
    window_changelog_widgets[WIDX_CONTENT_PANEL].right = w->width - 1;
    window_changelog_widgets[WIDX_CONTENT_PANEL].bottom = w->height - 1;
    window_changelog_widgets[WIDX_SCROLL].right = w->width - 3;
    window_changelog_widgets[WIDX_SCROLL].bottom = w->height - 22;
    window_changelog_widgets[WIDX_OPEN_URL].bottom = w->height - 5;
    window_changelog_widgets[WIDX_OPEN_URL].top = w->height - 19;
}

static void window_changelog_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
}

static void window_changelog_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, [[maybe_unused]] int32_t scrollIndex)
{
    const int32_t lineHeight = font_get_line_height(FontSpriteBase::MEDIUM);

    ScreenCoordsXY screenCoords(3, 3 - lineHeight);
    for (const auto& line : _changelogLines)
    {
        screenCoords.y += lineHeight;
        if (screenCoords.y + lineHeight < dpi->y || screenCoords.y >= dpi->y + dpi->height)
            continue;

        gfx_draw_string(dpi, screenCoords, line.c_str(), { w->colours[0] });
    }
}

static void window_changelog_process_changelog_text(const std::string& text)
{
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = text.find("\n", prev)) != std::string::npos)
    {
        _changelogLines.push_back(text.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    _changelogLines.push_back(text.substr(prev));

    _changelogLongestLineWidth = 0;
    for (const auto& line : _changelogLines)
    {
        auto width = gfx_get_string_width(line.c_str(), FontSpriteBase::MEDIUM);
        _changelogLongestLineWidth = std::max(width, _changelogLongestLineWidth);
    }
}

static void window_new_version_process_info()
{
    _newVersionInfo = GetContext()->GetNewVersionInfo();

    char version_info[256];

    const char* version_info_ptr = _newVersionInfo->name.c_str();
    format_string(version_info, 256, STR_NEW_RELEASE_VERSION_INFO, &version_info_ptr);

    _changelogLines.emplace_back(version_info);
    _changelogLines.emplace_back("");

    window_changelog_process_changelog_text(_newVersionInfo->changelog);
}

static void window_changelog_dispose_data()
{
    _changelogLines.clear();
    _changelogLines.shrink_to_fit();
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
    auto pathW = String::ToWideChar(path);
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
    std::string _changelogText;
    try
    {
        _changelogText = GetChangelogText();
    }
    catch (const std::bad_alloc&)
    {
        log_error("Unable to allocate memory for changelog.txt");
        return false;
    }
    catch (const std::exception&)
    {
        log_error("Unable to read changelog.txt");
        return false;
    }

    window_changelog_process_changelog_text(_changelogText);
    return true;
}
