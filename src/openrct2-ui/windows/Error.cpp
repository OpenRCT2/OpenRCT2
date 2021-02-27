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
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
enum {
    WIDX_BACKGROUND
};

static rct_widget window_error_widgets[] = {
    MakeWidget({0, 0}, {200, 42}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    { WIDGETS_END }
};

static void window_error_unknown5(rct_window *w);
static void window_error_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_error_events([](auto& events)
{
    events.unknown_05 = &window_error_unknown5;
    events.paint = &window_error_paint;
});
// clang-format on

static std::string _window_error_text;
static uint16_t _window_error_num_lines;

/**
 *
 *  rct2: 0x0066792F
 *
 * bx: title
 * dx: message
 */
rct_window* window_error_open(rct_string_id title, rct_string_id message, const Formatter& args)
{
    auto titlez = format_string(title, args.Data());
    auto messagez = format_string(message, args.Data());
    return window_error_open(titlez, messagez);
}

rct_window* window_error_open(std::string_view title, std::string_view message)
{
    int32_t numLines, width, height, maxY;
    rct_window* w;

    window_close_by_class(WC_ERROR);
    auto& buffer = _window_error_text;
    buffer.assign("{BLACK}");
    buffer.append(title);

    // Format the message
    if (!message.empty())
    {
        buffer.push_back('\n');
        buffer.append(message);
    }

    log_verbose("show error, %s", buffer.c_str() + 1);

    // Don't do unnecessary work in headless. Also saves checking if cursor state is null.
    if (gOpenRCT2Headless)
    {
        return nullptr;
    }

    // Check if there is any text to display
    if (buffer.size() <= 1)
        return nullptr;

    width = gfx_get_string_width_new_lined(buffer.data(), FontSpriteBase::MEDIUM);
    width = std::clamp(width, 64, 196);

    gfx_wrap_string(buffer.data(), width + 1, FontSpriteBase::MEDIUM, &numLines);

    _window_error_num_lines = numLines;
    width = width + 3;
    height = (numLines + 1) * font_get_line_height(FontSpriteBase::MEDIUM) + 4;

    window_error_widgets[WIDX_BACKGROUND].right = width;
    window_error_widgets[WIDX_BACKGROUND].bottom = height;

    int32_t screenWidth = context_get_width();
    int32_t screenHeight = context_get_height();
    const CursorState* state = context_get_cursor_state();
    ScreenCoordsXY windowPosition = state->position - ScreenCoordsXY(width / 2, -26);
    windowPosition.x = std::clamp(windowPosition.x, 0, screenWidth);
    windowPosition.y = std::max(22, windowPosition.y);
    maxY = screenHeight - height;
    if (windowPosition.y > maxY)
    {
        windowPosition.y = std::min(windowPosition.y - height - 40, maxY);
    }

    w = WindowCreate(
        windowPosition, width, height, &window_error_events, WC_ERROR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_RESIZABLE);
    w->widgets = window_error_widgets;
    w->error.var_480 = 0;
    if (!gDisableErrorWindowSound)
    {
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, w->windowPos.x + (w->width / 2));
    }

    return w;
}

/**
 *
 *  rct2: 0x00667BFE
 */
static void window_error_unknown5(rct_window* w)
{
    w->error.var_480++;
    if (w->error.var_480 >= 8)
        window_close(w);
}

/**
 *
 *  rct2: 0x00667AA3
 */
static void window_error_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t t, l, r, b;

    l = w->windowPos.x;
    t = w->windowPos.y;
    r = w->windowPos.x + w->width - 1;
    b = w->windowPos.y + w->height - 1;

    gfx_filter_rect(dpi, l + 1, t + 1, r - 1, b - 1, FilterPaletteID::Palette45);
    gfx_filter_rect(dpi, l, t, r, b, FilterPaletteID::PaletteGlassSaturatedRed);

    gfx_filter_rect(dpi, l, t + 2, l, b - 2, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, r, t + 2, r, b - 2, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, l + 2, b, r - 2, b, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, l + 2, t, r - 2, t, FilterPaletteID::PaletteDarken3);

    gfx_filter_rect(dpi, r + 1, t + 1, r + 1, t + 1, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, r - 1, t + 1, r - 1, t + 1, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, l + 1, b - 1, l + 1, b - 1, FilterPaletteID::PaletteDarken3);
    gfx_filter_rect(dpi, r - 1, b - 1, r - 1, b - 1, FilterPaletteID::PaletteDarken3);

    l = w->windowPos.x + (w->width + 1) / 2 - 1;
    t = w->windowPos.y + 1;
    draw_string_centred_raw(dpi, { l, t }, _window_error_num_lines, _window_error_text.data(), FontSpriteBase::MEDIUM);
}
