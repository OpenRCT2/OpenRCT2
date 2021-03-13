#include "Window_internal.h"

#include "../world/EntityList.h"
#include "../world/Sprite.h"

void rct_window::SetLocation(const CoordsXYZ& coords)
{
    window_scroll_to_location(this, coords);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}

void rct_window::ScrollToViewport()
{
    // In original checked to make sure x and y were not -1 as well.
    if (viewport == nullptr || viewport_focus_coordinates.y == -1)
        return;

    CoordsXYZ newCoords = {};
    if (viewport_focus_sprite.type & VIEWPORT_FOCUS_TYPE_SPRITE)
    {
        auto* sprite = GetEntity(viewport_focus_sprite.sprite_id);
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
        newCoords.x = viewport_focus_coordinates.x;
        newCoords.y = viewport_focus_coordinates.y & VIEWPORT_FOCUS_Y_MASK;
        newCoords.z = viewport_focus_coordinates.z;
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
