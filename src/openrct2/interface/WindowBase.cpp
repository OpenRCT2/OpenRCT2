#include "WindowBase.h"

#include "../config/Config.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "Cursors.h"
#include "Viewport.h"

namespace OpenRCT2
{
    void WindowBase::SetLocation(const CoordsXYZ& coords)
    {
        WindowScrollToLocation(*this, coords);
        flags &= ~WF_SCROLLING_TO_LOCATION;

        // Immediately update the viewport position since we are not scrolling.
        if (viewport != nullptr)
        {
            viewport->viewPos = savedViewPos;
        }
    }

    void WindowBase::Invalidate()
    {
        GfxSetDirtyBlocks({ windowPos, windowPos + ScreenCoordsXY{ width, height } });
    }

    void WindowBase::RemoveViewport()
    {
        if (viewport == nullptr)
            return;

        ViewportRemove(viewport);
        viewport = nullptr;
    }

    void WindowBase::SetWidgets(const std::span<const Widget> newWidgets)
    {
        widgets.clear();
        widgets.insert(widgets.end(), newWidgets.begin(), newWidgets.end());

        ResizeFrame();
    }

    CursorID WindowBase::OnCursor(WidgetIndex, const ScreenCoordsXY&, CursorID)
    {
        return CursorID::Arrow;
    }

    static inline void repositionCloseButton(Widget& closeButton, int32_t windowWidth)
    {
        auto closeButtonSize = Config::Get().interface.EnlargedUi ? kCloseButtonSizeTouch : kCloseButtonSize;
        if (Config::Get().interface.WindowButtonsOnTheLeft)
        {
            closeButton.left = 2;
            closeButton.right = 2 + closeButtonSize;
        }
        else
        {
            closeButton.left = windowWidth - 3 - closeButtonSize;
            closeButton.right = windowWidth - 3;
        }

        // Set appropriate close button
        bool useWhite = closeButton.string == kCloseBoxStringWhiteLarge || closeButton.string == kCloseBoxStringWhiteNormal;
        if (closeButtonSize == kCloseButtonSizeTouch)
            closeButton.string = !useWhite ? kCloseBoxStringBlackLarge : kCloseBoxStringWhiteLarge;
        else
            closeButton.string = !useWhite ? kCloseBoxStringBlackNormal : kCloseBoxStringWhiteNormal;
    }

    void WindowBase::ResizeFrame()
    {
        if (widgets.size() < 3)
            return;

        // Frame
        auto& frameWidget = widgets[0];
        if (frameWidget.type == WindowWidgetType::Frame)
        {
            frameWidget.right = width - 1;
            frameWidget.bottom = height - 1;
        }

        // Title/caption
        auto& titleWidget = widgets[1];
        bool hasTitleWidget = titleWidget.type == WindowWidgetType::Caption;
        if (hasTitleWidget)
            titleWidget.right = width - 2;

        // Close button
        auto& closeButton = widgets[2];
        if (closeButton.type == WindowWidgetType::CloseBox || closeButton.type == WindowWidgetType::Empty)
            repositionCloseButton(closeButton, width);

        // Page/resize widget
        if (widgets.size() >= 4)
        {
            auto& pageWidget = widgets[3];
            if (pageWidget.type == WindowWidgetType::Resize)
            {
                pageWidget.right = width - 1;
                pageWidget.bottom = height - 1;
            }
        }

        // Figure out if we need to push the other widgets down to accommodate a resized title/caption
        auto preferredHeight = getTitleBarTargetHeight();
        auto currentHeight = titleWidget.height();
        auto heightDifference = preferredHeight - currentHeight;

        if (!hasTitleWidget || heightDifference == 0)
            return;

        Invalidate();

        // Offset title and close button
        titleWidget.bottom += heightDifference;
        closeButton.bottom += heightDifference;

        height += heightDifference;
        min_height += heightDifference;
        max_height += heightDifference;

        Invalidate();

        // Resize frame again to match new height
        frameWidget.bottom = height - 1;

        // Offset body widgets
        // NB: we're offsetting page widget as well!
        for (WidgetIndex i = 3; i < widgets.size(); i++)
        {
            widgets[i].top += heightDifference;
            widgets[i].bottom += heightDifference;
        }

        // Offset viewport
        if (viewport != nullptr)
            viewport->pos.y += heightDifference;
    }

    int16_t WindowBase::getTitleBarTargetHeight() const
    {
        return Config::Get().interface.EnlargedUi ? kTitleHeightLarge : kTitleHeightNormal;
    }

    int16_t WindowBase::getTitleBarCurrentHeight() const
    {
        if (!(flags & WF_NO_TITLE_BAR) && widgets.size() > 2)
            return widgets[1].height();
        else
            return 0;
    }

    int16_t WindowBase::getTitleBarDiffTarget() const
    {
        return getTitleBarTargetHeight() - getTitleBarCurrentHeight();
    }

    int16_t WindowBase::getTitleBarDiffNormal() const
    {
        return getTitleBarCurrentHeight() - kTitleHeightNormal;
    }
} // namespace OpenRCT2
