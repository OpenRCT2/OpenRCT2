/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>
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
    static Widget kProgressWindowWidgets[] = {
        MakeWidget({                0, 0}, {    kWindowWidth, kWindowHeight}, WindowWidgetType::Frame,    WindowColour::Primary                                     ), // panel / background
        MakeWidget({                1, 1}, {kWindowWidth - 3,            14}, WindowWidgetType::Caption,  WindowColour::Primary, STR_STRINGID,  STR_WINDOW_TITLE_TIP), // title bar
        MakeWidget({kWindowWidth - 12, 2}, {              11,            12}, WindowWidgetType::CloseBox, WindowColour::Primary, STR_CLOSE_X,   STR_CLOSE_WINDOW_TIP), // close x button
        kWidgetsEnd,
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
        std::string _captionTemplate;
        std::string _currentCaption;
        uint32_t _currentProgress;
        uint32_t _totalCount;
        uint8_t style;

    public:
        void OnOpen() override
        {
            widgets = kProgressWindowWidgets;
            WindowInitScrollWidgets(*this);

            widgets[WIDX_CLOSE].type = WindowWidgetType::Empty;

            frame_no = 0;
            min_width = kWindowWidth;
            min_height = kWindowHeight;
            max_width = min_width;
            max_height = min_height;

            ApplyStyle();
        }

        void OnUpdate() override
        {
            Invalidate();
        }

        void OnPrepareDraw() override
        {
            ResizeFrame();
            PrepareCaption();
        }

        void ApplyStyle()
        {
            style = nextStyle++;
            nextStyle %= std::size(kVehicleStyles);
        }

        void PrepareCaption()
        {
            if (_totalCount > 0)
            {
                std::stringstream caption;
                caption << _captionTemplate;
                caption << " (" << _currentProgress << " / " << _totalCount << ")";
                _currentCaption = caption.str();
            }
            else
                _currentCaption = _captionTemplate;

            // Set window title
            auto ft = Formatter::Common();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(_currentCaption.c_str());
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

            if (_totalCount == 0)
                return;

            // Draw vehicle to indicate progress
            auto* vehicle = GfxGetG1Element(variant.vehicle);
            auto position = (-vehicle->width + 2) + (frame_no % width);
            GfxDrawSprite(clipDPI, variant.vehicle, ScreenCoordsXY(position, widget.bottom + 1));
        }

        void SetCaptionTemplate(const std::string& text)
        {
            _captionTemplate = text;
            _currentProgress = 0;
            _totalCount = 0;

            ApplyStyle();
            Invalidate();
        }

        void SetProgress(uint32_t currentProgress, uint32_t totalCount)
        {
            _currentProgress = currentProgress;
            _totalCount = totalCount;
            Invalidate();
        }
    };

    WindowBase* ProgressWindowOpen(const std::string& text)
    {
        ProgressWindow* window;
        if ((window = static_cast<ProgressWindow*>(WindowFindByClass(WindowClass::ProgressWindow))) != nullptr)
        {
            WindowBringToFront(*window);
        }
        else
        {
            window = WindowCreate<ProgressWindow>(
                WindowClass::ProgressWindow, kWindowWidth, kWindowHeight,
                WF_10 | WF_TRANSPARENT | WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        }

        window->SetCaptionTemplate(text);
        return window;
    }

    void ProgressWindowSet(uint32_t currentProgress, uint32_t totalCount)
    {
        auto window = WindowFindByClass(WindowClass::ProgressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->SetProgress(currentProgress, totalCount);
    }

    void ProgressWindowClose()
    {
        auto window = WindowFindByClass(WindowClass::ProgressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->Close();
    }
} // namespace OpenRCT2::Ui::Windows
