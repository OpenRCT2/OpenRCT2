/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>

// clang-format off
enum {
    WIDX_BACKGROUND
};

static rct_widget window_error_widgets[] = {
    MakeWidget({0, 0}, {200, 42}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};
// clang-format on

static std::string _window_error_text;
static uint16_t _window_error_num_lines;

class ErrorWindow final : public Window
{
public:
    void OnOpen() override
    {
        window_error_widgets[WIDX_BACKGROUND].right = width;
        window_error_widgets[WIDX_BACKGROUND].bottom = height;

        widgets = window_error_widgets;
        error.var_480 = 0;
        if (!gDisableErrorWindowSound)
        {
            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, windowPos.x + (width / 2));
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        ScreenCoordsXY leftTop{ windowPos };
        ScreenCoordsXY rightBottom{ windowPos + ScreenCoordsXY{ width - 1, height - 1 } };
        ScreenCoordsXY leftBottom{ leftTop.x, rightBottom.y };
        ScreenCoordsXY rightTop{ rightBottom.x, leftTop.y };

        gfx_filter_rect(
            &dpi, ScreenRect{ leftTop + ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } },
            FilterPaletteID::Palette45);
        gfx_filter_rect(&dpi, ScreenRect{ leftTop, rightBottom }, FilterPaletteID::PaletteGlassSaturatedRed);

        gfx_filter_rect(
            &dpi, ScreenRect{ leftTop + ScreenCoordsXY{ 0, 2 }, leftBottom - ScreenCoordsXY{ 0, 2 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ rightTop + ScreenCoordsXY{ 0, 2 }, rightBottom - ScreenCoordsXY{ 0, 2 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ leftBottom + ScreenCoordsXY{ 2, 0 }, rightBottom - ScreenCoordsXY{ 2, 0 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ leftTop + ScreenCoordsXY{ 2, 0 }, rightTop - ScreenCoordsXY{ 2, 0 } },
            FilterPaletteID::PaletteDarken3);

        gfx_filter_rect(
            &dpi, ScreenRect{ rightTop + ScreenCoordsXY{ 1, 1 }, rightTop + ScreenCoordsXY{ 1, 1 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ rightTop + ScreenCoordsXY{ -1, 1 }, rightTop + ScreenCoordsXY{ -1, 1 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ leftBottom + ScreenCoordsXY{ 1, -1 }, leftBottom + ScreenCoordsXY{ 1, -1 } },
            FilterPaletteID::PaletteDarken3);
        gfx_filter_rect(
            &dpi, ScreenRect{ rightBottom - ScreenCoordsXY{ 1, 1 }, rightBottom - ScreenCoordsXY{ 1, 1 } },
            FilterPaletteID::PaletteDarken3);

        draw_string_centred_raw(
            &dpi, { leftTop + ScreenCoordsXY{ (width + 1) / 2 - 1, 1 } }, _window_error_num_lines, _window_error_text.data(),
            FontSpriteBase::MEDIUM);
    }

    void OnUnknown5() override
    {
        error.var_480++;
        if (error.var_480 >= 8)
            Close();
    }
};

rct_window* WindowErrorOpen(std::string_view title, std::string_view message)
{
    int32_t numLines, width, height, maxY;

    window_close_by_class(WindowClass::Error);
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

    width = gfx_get_string_width_new_lined(buffer.data(), FontStyle::Medium);
    width = std::clamp(width, 64, 196);

    gfx_wrap_string(buffer.data(), width + 1, FontStyle::Medium, &numLines);

    _window_error_num_lines = numLines;
    width = width + 3;
    height = (numLines + 1) * font_get_line_height(FontStyle::Medium) + 4;

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

    return WindowCreate<ErrorWindow>(
        WindowClass::Error, windowPosition, width, height, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_RESIZABLE);
}

rct_window* WindowErrorOpen(StringId title, StringId message, const Formatter& args)
{
    auto titlez = format_string(title, args.Data());
    auto messagez = format_string(message, args.Data());
    return WindowErrorOpen(titlez, messagez);
}
