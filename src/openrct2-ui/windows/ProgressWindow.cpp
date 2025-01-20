/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <random>
#include <sstream>

namespace OpenRCT2::Ui::Windows
{
    enum ProgressWindowWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
    };

    static constexpr int32_t kWindowWidth = 400;
    static constexpr int32_t kWindowHeight = 90;

    // clang-format off
    static constexpr Widget kProgressWindowWidgets[] = {
        MakeWidget({                0, 0}, {    kWindowWidth, kWindowHeight}, WindowWidgetType::Frame,    WindowColour::Primary                                     ), // panel / background
        MakeWidget({                1, 1}, {kWindowWidth - 3,            14}, WindowWidgetType::Caption,  WindowColour::Primary, STR_STRINGID,  STR_WINDOW_TITLE_TIP), // title bar
        MakeWidget({kWindowWidth - 12, 2}, {              11,            12}, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X,   STR_CLOSE_WINDOW_TIP), // close x button
    };

    struct LoaderVehicleStyle
    {
        ImageId supports;
        ImageId track;
        ImageId vehicle;
    };

    static std::array<LoaderVehicleStyle, 3> kVehicleStyles = { {
        {
            ImageId{SPR_G2_LOADER_HYBRID_SUPPORTS, COLOUR_LIGHT_ORANGE, COLOUR_DARK_BROWN },
            ImageId{SPR_G2_LOADER_HYBRID_TRACK, COLOUR_LIGHT_ORANGE, COLOUR_LIGHT_ORANGE },
            ImageId{SPR_G2_LOADER_HYBRID_VEHICLE, COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_BRIGHT_RED }
        },
        {
            ImageId{SPR_G2_LOADER_STEEL_SUPPORTS, COLOUR_LIGHT_BROWN, COLOUR_BLACK },
            ImageId{SPR_G2_LOADER_STEEL_TRACK, COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN },
            ImageId{SPR_G2_LOADER_STEEL_VEHICLE, COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_LIGHT_BLUE }
        },
        {
            ImageId{SPR_G2_LOADER_WOODEN_SUPPORTS, COLOUR_BLACK, COLOUR_WHITE },
            ImageId{SPR_G2_LOADER_WOODEN_TRACK, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
            ImageId{SPR_G2_LOADER_WOODEN_VEHICLE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN}
        },
    } };

    static uint8_t nextStyle = 0;

    // clang-format on
    class ProgressWindow final : public Window
    {
    private:
        CloseCallback _onClose = nullptr;

        StringId _progressFormat;
        std::string _progressTitle;
        std::string _currentCaption;

        uint32_t _currentProgress;
        uint32_t _totalCount;
        int8_t style = -1;

    public:
        void OnOpen() override
        {
            SetWidgets(kProgressWindowWidgets);
            WindowInitScrollWidgets(*this);

            frame_no = 0;
            min_width = kWindowWidth;
            min_height = kWindowHeight;
            max_width = min_width;
            max_height = min_height;

            ApplyStyle();
        }

        void OnClose() override
        {
            if (_onClose != nullptr)
            {
                _onClose();
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
            }
        }

        void OnUpdate() override
        {
            Invalidate();
        }

        void OnPrepareDraw() override
        {
            if (_onClose != nullptr)
                widgets[WIDX_CLOSE].type = WindowWidgetType::Button;
            else
                widgets[WIDX_CLOSE].type = WindowWidgetType::Empty;

            ResizeFrame();
            PrepareCaption();
        }

        void ApplyStyle()
        {
            if (style >= 0)
            {
                // Take the next available style, rotating
                style = nextStyle++;
                nextStyle %= std::size(kVehicleStyles);
            }
            else
            {
                // Pick a random style to start off with
                std::random_device r;
                auto upperBound = static_cast<int8_t>(std::size(kVehicleStyles)) - 1;
                std::uniform_int_distribution<int> uniform_dist(0, upperBound);
                std::default_random_engine e(r());
                style = static_cast<int8_t>(uniform_dist(e));
            }
        }

        void PrepareCaption()
        {
            if (_totalCount > 0)
            {
                auto ft = Formatter();
                ft.Add<const char*>(_progressTitle.c_str());
                ft.Add<uint32_t>(_currentProgress);
                ft.Add<uint32_t>(_totalCount);

                _currentCaption = FormatStringIDLegacy(_progressFormat, ft.Data());
            }
            else
                _currentCaption = _progressTitle;

            // Set window title
            {
                auto ft = Formatter::Common();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(_currentCaption.c_str());
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            WindowDrawWidgets(*this, dpi);

            auto& widget = widgets[WIDX_TITLE];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.bottom + 1 };

            DrawPixelInfo clipDPI;
            if (!ClipDrawPixelInfo(clipDPI, dpi, screenCoords, width - 3, height - widget.bottom - 3))
                return;

            auto& variant = kVehicleStyles[style];

            // Draw supports sprite -- twice, to fill the window
            auto trackCoords = ScreenCoordsXY{ widget.left, widget.bottom + 1 };
            GfxDrawSprite(clipDPI, variant.supports, trackCoords);
            GfxDrawSprite(clipDPI, variant.supports, trackCoords + ScreenCoordsXY(256, 0));

            // Draw track sprite -- twice, to fill the window
            GfxDrawSprite(clipDPI, variant.track, trackCoords);
            GfxDrawSprite(clipDPI, variant.track, trackCoords + ScreenCoordsXY(256, 0));

            // Figure out where to position the vehicle to indicate progress
            auto* vehicle = GfxGetG1Element(variant.vehicle);
            int16_t vehicleWidth = (vehicle != nullptr) ? vehicle->width : 0;
            int32_t position;
            if (_totalCount > 0)
                position = (-vehicleWidth + 2) + width * _currentProgress / _totalCount;
            else
                position = (vehicleWidth + width) / 2;

            GfxDrawSprite(clipDPI, variant.vehicle, ScreenCoordsXY(position, widget.bottom + 1));
        }

        void SetCaption(const std::string& text)
        {
            _progressTitle = text;
            _currentProgress = 0;
            _totalCount = 0;

            Invalidate();
        }

        void SetCloseCallback(CloseCallback onClose)
        {
            _onClose = onClose;
        }

        void SetProgress(uint32_t currentProgress, uint32_t totalCount, StringId format)
        {
            if (format == kStringIdNone)
                _progressFormat = STR_STRING_M_OF_N;
            else
                _progressFormat = format;

            _currentProgress = currentProgress;
            _totalCount = totalCount;
            Invalidate();
        }
    };

    WindowBase* ProgressWindowOpen(const std::string& text, CloseCallback onClose)
    {
        ContextForceCloseWindowByClass(WindowClass::NetworkStatus);

        auto* windowMgr = GetWindowManager();

        ProgressWindow* window;
        if ((window = static_cast<ProgressWindow*>(windowMgr->FindByClass(WindowClass::ProgressWindow))) != nullptr)
        {
            windowMgr->BringToFront(*window);
        }
        else
        {
            window = windowMgr->Create<ProgressWindow>(
                WindowClass::ProgressWindow, kWindowWidth, kWindowHeight,
                WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        }

        window->SetCaption(text);
        window->SetCloseCallback(onClose);
        return window;
    }

    void ProgressWindowSet(uint32_t currentProgress, uint32_t totalCount, StringId format)
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::ProgressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->SetProgress(currentProgress, totalCount, format);
    }

    // Closes the window, deliberately *without* executing the callback.
    void ProgressWindowClose()
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::ProgressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->SetCloseCallback(nullptr);
        progressWindow->Close();
    }
} // namespace OpenRCT2::Ui::Windows
