/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
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

    static constexpr ScreenSize kWindowSize = { 400, 90 };

    // clang-format off
    static constexpr auto kProgressWindowWidgets = makeWidgets(
        makeWindowShim(STR_STRINGID, kWindowSize)
    );

    struct LoaderVehicleStyle
    {
        ImageId supports;
        ImageId track;
        ImageId vehicle;
    };

    static std::array<LoaderVehicleStyle, 4> kVehicleStyles = { {
        {
            ImageId{SPR_G2_LOADER_HYBRID_SUPPORTS, Drawing::Colour::lightOrange, Drawing::Colour::darkBrown },
            ImageId{SPR_G2_LOADER_HYBRID_TRACK, Drawing::Colour::lightOrange, Drawing::Colour::lightOrange },
            ImageId{SPR_G2_LOADER_HYBRID_VEHICLE, Drawing::Colour::oliveGreen, Drawing::Colour::oliveGreen, Drawing::Colour::brightRed }
        },
        {
            ImageId{SPR_G2_LOADER_STEEL_SUPPORTS, Drawing::Colour::lightBrown, Drawing::Colour::black },
            ImageId{SPR_G2_LOADER_STEEL_TRACK, Drawing::Colour::lightBrown, Drawing::Colour::lightBrown },
            ImageId{SPR_G2_LOADER_STEEL_VEHICLE, Drawing::Colour::lightBlue, Drawing::Colour::white, Drawing::Colour::lightBlue }
        },
        {
            ImageId{SPR_G2_LOADER_WOODEN_SUPPORTS, Drawing::Colour::black, Drawing::Colour::white },
            ImageId{SPR_G2_LOADER_WOODEN_TRACK, Drawing::Colour::bordeauxRed, Drawing::Colour::black },
            ImageId{SPR_G2_LOADER_WOODEN_VEHICLE, Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::saturatedBrown}
        },
        {
            ImageId{SPR_G2_LOADER_GO_KARTS_SUPPORTS, Drawing::Colour::darkBrown, Drawing::Colour::darkBrown },
            ImageId{SPR_G2_LOADER_GO_KARTS_TRACK, Drawing::Colour::black, Drawing::Colour::black },
            ImageId{SPR_G2_LOADER_GO_KARTS_VEHICLES }
        },
    } };

    static uint8_t nextStyle = 0;

    // clang-format on
    class ProgressWindow final : public Window
    {
    private:
        CloseCallback _onClose = nullptr;

        StringId _progressFormat{ kStringIdEmpty };
        std::string _progressTitle;
        std::string _currentCaption;

        uint32_t _currentProgress{};
        uint32_t _totalCount{};
        int8_t style = -1;

    public:
        void onOpen() override
        {
            Audio::StopSFX();

            setWidgets(kProgressWindowWidgets);
            WindowSetResize(*this, kWindowSize, kWindowSize);

            currentFrame = 0;

            ApplyStyle();
            resizeFrame();
        }

        void onClose() override
        {
            if (_onClose != nullptr)
            {
                _onClose();
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
            }
        }

        void onUpdate() override
        {
            invalidate();
        }

        void onPrepareDraw() override
        {
            if (_onClose != nullptr)
                widgets[WIDX_CLOSE].type = WidgetType::closeBox;
            else
                widgets[WIDX_CLOSE].type = WidgetType::empty;

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

        void onDraw(Drawing::RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);

            auto& widget = widgets[WIDX_TITLE];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.bottom + 1 };

            Drawing::RenderTarget clipRT;
            if (!ClipRenderTarget(clipRT, rt, screenCoords, width - 3, height - widget.bottom - 3))
                return;

            auto& variant = kVehicleStyles[style];

            // Draw supports sprite -- twice, to fill the window
            auto trackCoords = ScreenCoordsXY{ widget.left, widget.bottom + 1 };
            GfxDrawSprite(clipRT, variant.supports, trackCoords);
            GfxDrawSprite(clipRT, variant.supports, trackCoords + ScreenCoordsXY(256, 0));

            // Draw track sprite -- twice, to fill the window
            GfxDrawSprite(clipRT, variant.track, trackCoords);
            GfxDrawSprite(clipRT, variant.track, trackCoords + ScreenCoordsXY(256, 0));

            // Figure out where to position the vehicle to indicate progress
            auto* vehicle = GfxGetG1Element(variant.vehicle);
            int16_t vehicleWidth = (vehicle != nullptr) ? vehicle->width : 0;
            int32_t position;
            if (_totalCount > 0)
            {
                const auto progress = static_cast<double>(_currentProgress) / _totalCount;
                position = static_cast<int32_t>((-vehicleWidth + 2) + width * progress);
            }
            else
            {
                position = (vehicleWidth + width) / 2;
            }

            GfxDrawSprite(clipRT, variant.vehicle, ScreenCoordsXY(position, widget.bottom + 1));
        }

        void setCaption(const std::string& text)
        {
            _progressTitle = text;
            invalidate();
        }

        void setCloseCallback(CloseCallback callback)
        {
            _onClose = callback;
        }

        void setProgress(uint32_t currentProgress, uint32_t totalCount, StringId format)
        {
            if (format == kStringIdNone)
                _progressFormat = STR_STRING_M_OF_N;
            else
                _progressFormat = format;

            _currentProgress = currentProgress;
            _totalCount = totalCount;
            invalidate();
        }
    };

    WindowBase* ProgressWindowOpen(const std::string& text, CloseCallback onClose)
    {
        ContextForceCloseWindowByClass(WindowClass::networkStatus);

        auto* windowMgr = GetWindowManager();

        ProgressWindow* window;
        if ((window = static_cast<ProgressWindow*>(windowMgr->FindByClass(WindowClass::progressWindow))) != nullptr)
        {
            windowMgr->BringToFront(*window);
        }
        else
        {
            window = windowMgr->Create<ProgressWindow>(
                WindowClass::progressWindow, kWindowSize,
                { WindowFlag::higherContrastOnPress, WindowFlag::transparent, WindowFlag::centreScreen,
                  WindowFlag::stickToFront });
        }

        window->setCaption(text);
        window->setCloseCallback(onClose);
        return window;
    }

    void ProgressWindowSet(uint32_t currentProgress, uint32_t totalCount, StringId format)
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::progressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->setProgress(currentProgress, totalCount, format);
    }

    // Closes the window, deliberately *without* executing the callback.
    void ProgressWindowClose()
    {
        auto* windowMgr = GetWindowManager();
        auto window = windowMgr->FindByClass(WindowClass::progressWindow);
        if (window == nullptr)
        {
            return;
        }
        auto progressWindow = static_cast<ProgressWindow*>(window);
        progressWindow->setCloseCallback(nullptr);
        progressWindow->close();
    }
} // namespace OpenRCT2::Ui::Windows
