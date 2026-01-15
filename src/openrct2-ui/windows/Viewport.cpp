/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Map.h>

namespace OpenRCT2::Ui::Windows
{
    enum WindowViewportWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_VIEWPORT,
        WIDX_ZOOM_IN,
        WIDX_ZOOM_OUT,
        WIDX_LOCATE,
        WIDX_ROTATE,
    };

#pragma region MEASUREMENTS

    static constexpr StringId kWindowTitle = STR_VIEWPORT_NO;
    static constexpr ScreenSize kWindowSize = { 200, 200 };

    static constexpr ScreenSize kButtonSize = { 24, 24 };

#pragma endregion

    // clang-format off
    static constexpr auto _viewportWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({      0, 14}, kWindowSize - ScreenSize( 1, 1),  WidgetType::resize,   WindowColour::secondary                                         ), // resize
        makeWidget({      3, 17}, kWindowSize - ScreenSize(26, 3),  WidgetType::viewport, WindowColour::primary                                           ), // viewport
        makeWidget({kWindowSize.width - 25, 17}, kButtonSize,       WidgetType::flatBtn,  WindowColour::primary  , ImageId(SPR_G2_ZOOM_IN),  STR_ZOOM_IN_TIP       ), // zoom in
        makeWidget({kWindowSize.width - 25, 41}, kButtonSize,       WidgetType::flatBtn,  WindowColour::primary  , ImageId(SPR_G2_ZOOM_OUT), STR_ZOOM_OUT_TIP      ), // zoom out
        makeWidget({kWindowSize.width - 25, 65}, kButtonSize,       WidgetType::flatBtn,  WindowColour::primary  , ImageId(SPR_LOCATE),      STR_LOCATE_SUBJECT_TIP), // locate
        makeWidget({kWindowSize.width - 25, 89}, kButtonSize,       WidgetType::flatBtn,  WindowColour::primary  , ImageId(SPR_ROTATE_ARROW),STR_LOCATE_SUBJECT_TIP)  // rotate
    );
    // clang-format on

    class ViewportWindow final : public Window
    {
    private:
        void GetFreeViewportNumber()
        {
            number = 1;
            WindowVisitEach([&](WindowBase* w) {
                if (w != nullptr && w != this && w->classification == WindowClass::viewport)
                {
                    if (w->number >= number)
                        number = w->number + 1;
                }
            });
        }

    public:
        void onOpen() override
        {
            GetFreeViewportNumber();

            setWidgets(_viewportWidgets);

            // Create viewport
            ViewportCreate(*this, windowPos, width, height, Focus(TileCoordsXYZ(128, 128, 0).ToCoordsXYZ()));
            if (viewport == nullptr)
            {
                close();
                ErrorOpen("Unexpected Error", "Failed to create viewport window.");
                return;
            }

            auto* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                Viewport* mainViewport = mainWindow->viewport;
                int32_t x = mainViewport->viewPos.x + (mainViewport->ViewWidth() / 2);
                int32_t y = mainViewport->viewPos.y + (mainViewport->ViewHeight() / 2);
                savedViewPos = { x - (viewport->ViewWidth() / 2), y - (viewport->ViewHeight() / 2) };
            }

            viewport->flags |= VIEWPORT_FLAG_SOUND_ON | VIEWPORT_FLAG_INDEPENDENT_ROTATION;

            WindowSetResize(*this, kWindowSize, { (ContextGetWidth() * 4) / 5, (ContextGetHeight() * 4) / 5 });
        }

        void onUpdate() override
        {
            auto* mainWindow = WindowGetMain();
            if (mainWindow == nullptr)
                return;

            if (viewport != nullptr && viewport->flags != (mainWindow->viewport->flags | VIEWPORT_FLAG_INDEPENDENT_ROTATION))
            {
                viewport->flags = mainWindow->viewport->flags | VIEWPORT_FLAG_INDEPENDENT_ROTATION;
                invalidateWidget(WIDX_VIEWPORT);
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_ZOOM_IN:
                {
                    WindowZoomIn(*this, false);
                    break;
                }
                case WIDX_ZOOM_OUT:
                {
                    WindowZoomOut(*this, false);
                    break;
                }
                case WIDX_LOCATE:
                {
                    auto* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                    {
                        auto info = GetMapCoordinatesFromPos(
                            { windowPos.x + (width / 2), windowPos.y + (height / 2) }, kViewportInteractionItemAll);
                        WindowScrollToLocation(*mainWindow, { info.Loc, TileElementHeight(info.Loc) });
                    }
                    break;
                }
                case WIDX_ROTATE:
                {
                    ViewportRotateSingle(this, 1);
                    invalidate();
                    break;
                }
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            // Draw viewport
            if (viewport != nullptr)
                WindowDrawViewport(rt, *this);
        }

        void onResize() override
        {
            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();

            maxWidth = (screenWidth * 4) / 5;
            maxHeight = (screenHeight * 4) / 5;

            minWidth = kWindowSize.width;
            minHeight = kWindowSize.height;

            WindowSetResize(*this, { minWidth, minHeight }, { maxWidth, maxHeight });
        }

        void onPrepareDraw() override
        {
            widgets[WIDX_ZOOM_IN].left = width - 27;
            widgets[WIDX_ZOOM_IN].right = width - 2;
            widgets[WIDX_ZOOM_OUT].left = width - 27;
            widgets[WIDX_ZOOM_OUT].right = width - 2;
            widgets[WIDX_LOCATE].left = width - 27;
            widgets[WIDX_LOCATE].right = width - 2;
            widgets[WIDX_ROTATE].left = width - 27;
            widgets[WIDX_ROTATE].right = width - 2;
            widgets[WIDX_VIEWPORT].right = widgets[WIDX_ZOOM_IN].left - 1;
            widgets[WIDX_VIEWPORT].bottom = widgets[WIDX_BACKGROUND].bottom - 3;

            // Set title
            Formatter::Common().Add<uint32_t>(number);

            // Set disabled widgets
            disabledWidgets = 0;
            if (viewport != nullptr && viewport->zoom == ZoomLevel::min())
                disabledWidgets |= 1uLL << WIDX_ZOOM_IN;
            if (viewport != nullptr && viewport->zoom >= ZoomLevel::max())
                disabledWidgets |= 1uLL << WIDX_ZOOM_OUT;

            if (viewport != nullptr)
            {
                Widget* viewportWidget = &widgets[WIDX_VIEWPORT];
                viewport->pos = windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 };
                viewport->width = widgets[WIDX_VIEWPORT].width() - 2;
                viewport->height = widgets[WIDX_VIEWPORT].height() - 2;
            }
        }
    };

    WindowBase* ViewportOpen()
    {
        return GetWindowManager()->Create<ViewportWindow>(WindowClass::viewport, kWindowSize, WindowFlag::resizable);
    }
} // namespace OpenRCT2::Ui::Windows
