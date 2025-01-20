/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/sprites.h>
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

    static constexpr StringId WINDOW_TITLE = STR_VIEWPORT_NO;
    static constexpr int32_t WW = 200;
    static constexpr int32_t WH = 200;

    static constexpr ScreenSize VIEWPORT_BUTTON = { 24, 24 };

#pragma endregion

    // clang-format off
    static constexpr Widget _viewportWidgets[] =
    {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({      0, 14}, { WW - 1, WH - 1}, WindowWidgetType::Resize,   WindowColour::Secondary                                         ), // resize
        MakeWidget({      3, 17}, {WW - 26, WH - 3}, WindowWidgetType::Viewport, WindowColour::Primary                                           ), // viewport
        MakeWidget({WW - 25, 17}, VIEWPORT_BUTTON,   WindowWidgetType::FlatBtn,  WindowColour::Primary  , ImageId(SPR_G2_ZOOM_IN),  STR_ZOOM_IN_TIP       ), // zoom in
        MakeWidget({WW - 25, 41}, VIEWPORT_BUTTON,   WindowWidgetType::FlatBtn,  WindowColour::Primary  , ImageId(SPR_G2_ZOOM_OUT), STR_ZOOM_OUT_TIP      ), // zoom out
        MakeWidget({WW - 25, 65}, VIEWPORT_BUTTON,   WindowWidgetType::FlatBtn,  WindowColour::Primary  , ImageId(SPR_LOCATE),      STR_LOCATE_SUBJECT_TIP), // locate
        MakeWidget({WW - 25, 89}, VIEWPORT_BUTTON,   WindowWidgetType::FlatBtn,  WindowColour::Primary  , ImageId(SPR_ROTATE_ARROW),STR_LOCATE_SUBJECT_TIP), // rotate
    };
    // clang-format on

    class ViewportWindow final : public Window
    {
    private:
        void GetFreeViewportNumber()
        {
            number = 1;
            WindowVisitEach([&](WindowBase* w) {
                if (w != nullptr && w != this && w->classification == WindowClass::Viewport)
                {
                    if (w->number >= number)
                        number = w->number + 1;
                }
            });
        }

    public:
        void OnOpen() override
        {
            GetFreeViewportNumber();

            SetWidgets(_viewportWidgets);

            // Create viewport
            ViewportCreate(this, windowPos, width, height, Focus(TileCoordsXYZ(128, 128, 0).ToCoordsXYZ()));
            if (viewport == nullptr)
            {
                Close();
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

            viewport->flags |= VIEWPORT_FLAG_SOUND_ON | VIEWPORT_FLAG_INDEPEDENT_ROTATION;

            min_width = WW;
            min_height = WH;
            max_width = WW;
            max_height = WH;
        }

        void OnUpdate() override
        {
            auto* mainWindow = WindowGetMain();
            if (mainWindow == nullptr)
                return;

            if (viewport != nullptr && viewport->flags != mainWindow->viewport->flags)
            {
                viewport->flags = mainWindow->viewport->flags | VIEWPORT_FLAG_INDEPEDENT_ROTATION;
                Invalidate();
            }

            // Not sure how to invalidate part of the viewport that has changed, this will have to do for now
            // widget_invalidate(*this, WIDX_VIEWPORT);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_ZOOM_IN:
                {
                    if (viewport != nullptr && viewport->zoom > ZoomLevel::min())
                    {
                        viewport->zoom--;
                        Invalidate();
                    }
                    break;
                }
                case WIDX_ZOOM_OUT:
                {
                    if (viewport != nullptr && viewport->zoom < ZoomLevel::max())
                    {
                        viewport->zoom++;
                        Invalidate();
                    }
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
                    Invalidate();
                    break;
                }
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            // Draw viewport
            if (viewport != nullptr)
                WindowDrawViewport(dpi, *this);
        }

        void OnResize() override
        {
            int32_t screenWidth = ContextGetWidth();
            int32_t screenHeight = ContextGetHeight();

            max_width = (screenWidth * 4) / 5;
            max_height = (screenHeight * 4) / 5;

            min_width = WW;
            min_height = WH;

            WindowSetResize(*this, min_width, min_height, max_width, max_height);
        }

        void OnPrepareDraw() override
        {
            Widget* viewportWidget = &widgets[WIDX_VIEWPORT];

            ResizeFrameWithPage();
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
            disabled_widgets = 0;
            if (viewport != nullptr && viewport->zoom == ZoomLevel::min())
                disabled_widgets |= 1uLL << WIDX_ZOOM_IN;
            if (viewport != nullptr && viewport->zoom >= ZoomLevel::max())
                disabled_widgets |= 1uLL << WIDX_ZOOM_OUT;

            if (viewport != nullptr)
            {
                viewport->pos = windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 };
                viewport->width = widgets[WIDX_VIEWPORT].width() - 1;
                viewport->height = widgets[WIDX_VIEWPORT].height() - 1;
            }
        }
    };

    WindowBase* ViewportOpen()
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        int32_t width = (screenWidth / 2);
        int32_t height = (screenHeight / 2);

        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->Create<ViewportWindow>(
            WindowClass::Viewport, std::max(WW, width), std::max(WH, height), WF_RESIZABLE);

        if (w != nullptr)
            return w;
        return nullptr;
    }
} // namespace OpenRCT2::Ui::Windows
