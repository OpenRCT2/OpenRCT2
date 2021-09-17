#include "Window_internal.h"

#include "../world/Entity.h"
#include "../world/EntityList.h"
#include "Viewport.h"

void rct_window::SetLocation(const CoordsXYZ& coords)
{
    window_scroll_to_location(this, coords);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}

void rct_window::ScrollToViewport()
{
    if (viewport == nullptr || !focus2.HasFocus())
        return;

    CoordsXYZ newCoords = {};
    if (focus2.GetFocus() == Focus2::Type::Entity)
    {
        auto* sprite = GetEntity(std::get<Focus2::EntityFocus>(focus2.data));
        if (sprite == nullptr)
        {
            return;
        }
        newCoords.x = sprite->x;
        newCoords.y = sprite->y;
        newCoords.z = sprite->z;
    }
    else
    {
        auto& coordFocus = std::get<Focus2::CoordinateFocus>(focus2.data);
        newCoords = coordFocus;
    }

    auto mainWindow = window_get_main();
    if (mainWindow != nullptr)
        window_scroll_to_location(mainWindow, newCoords);
}

void rct_window::Invalidate()
{
    gfx_set_dirty_blocks({ windowPos, windowPos + ScreenCoordsXY{ width, height } });
}

void rct_window::RemoveViewport()
{
    if (viewport == nullptr)
        return;

    viewport_remove(viewport);
    viewport = nullptr;
}
