/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Font.h>
#include <openrct2/interface/Screenshot.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/ui/WindowManager.h>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND,
    };

    static constexpr auto kMinWidth = 70;
    static constexpr auto kMaxWidth = 250;
    static constexpr auto kPadding = 4;

    static constexpr Widget window_error_widgets[] = {
        MakeWidget({ 0, 0 }, { 200, 42 }, WindowWidgetType::Frame, WindowColour::Primary),
    };

    class ErrorWindow final : public Window
    {
    private:
        std::string _text;
        uint16_t _numLines;
        uint8_t _staleCount;
        bool _autoClose;

    public:
        ErrorWindow(std::string text, uint16_t numLines, bool autoClose)
            : _text(std::move(text))
            , _numLines(numLines)
            , _autoClose(autoClose)
        {
        }

        void OnOpen() override
        {
            SetWidgets(window_error_widgets);

            widgets[WIDX_BACKGROUND].right = width - 1;
            widgets[WIDX_BACKGROUND].bottom = height - 1;

            _staleCount = 0;

            if (!gDisableErrorWindowSound)
            {
                Audio::Play(Audio::SoundId::Error, 0, windowPos.x + (width / 2));
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            WindowDrawWidgets(*this, dpi);

            auto screenCoords = windowPos + ScreenCoordsXY{ (width + 1) / 2 - 1, kPadding - 1 };
            DrawStringCentredRaw(dpi, screenCoords, _numLines, _text.data(), FontStyle::Medium);
        }

        void OnPeriodicUpdate() override
        {
            // Close the window after 8 seconds of showing
            _staleCount++;
            if (_staleCount >= 8)
            {
                Close();
            }
        }

        void OnUpdate() override
        {
            // Automatically close previous screenshot messages before new screenshot is taken
            if (_autoClose && gScreenshotCountdown > 0)
            {
                Close();
            }
        }
    };

    WindowBase* ErrorOpen(std::string_view title, std::string_view message, bool autoClose)
    {
        std::string buffer = "{WINDOW_COLOUR_1}";
        buffer.append(title);

        // Format the message
        if (!message.empty())
        {
            buffer.push_back('\n');
            buffer.append(message);
        }

        LOG_VERBOSE("show error, %s", buffer.c_str() + 1);

        // Don't do unnecessary work in headless. Also saves checking if cursor state is null.
        if (gOpenRCT2Headless)
        {
            return nullptr;
        }

        // Check if there is any text to display
        if (buffer.size() <= 1)
        {
            return nullptr;
        }

        // Close any existing error windows if they exist.
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Error);

        // How wide is the error string?
        int32_t width = GfxGetStringWidthNewLined(buffer.data(), FontStyle::Medium);
        width = std::clamp(width + 2 * kPadding, kMinWidth, kMaxWidth);

        // How high is the error string?
        int32_t numLines{};
        GfxWrapString(buffer, width + 1, FontStyle::Medium, &buffer, &numLines);
        int32_t height = (numLines + 1) * FontGetLineHeight(FontStyle::Medium) + (2 * kPadding);

        // Position error message around the cursor
        const CursorState* state = ContextGetCursorState();
        ScreenCoordsXY windowPosition = state->position - ScreenCoordsXY(width / 2, -26);
        windowPosition.x = std::clamp(windowPosition.x, 0, ContextGetWidth() - width - 40);
        windowPosition.y = std::clamp(windowPosition.y, 22, ContextGetHeight() - height - 40);

        auto errorWindow = std::make_unique<ErrorWindow>(std::move(buffer), numLines, autoClose);

        return windowMgr->Create(
            std::move(errorWindow), WindowClass::Error, windowPosition, width, height, WF_STICK_TO_FRONT | WF_TRANSPARENT);
    }

    WindowBase* ErrorOpen(StringId title, StringId message, const Formatter& args, bool autoClose)
    {
        auto titlez = FormatStringIDLegacy(title, args.Data());
        auto messagez = FormatStringIDLegacy(message, args.Data());
        return ErrorOpen(titlez, messagez, autoClose);
    }
} // namespace OpenRCT2::Ui::Windows
