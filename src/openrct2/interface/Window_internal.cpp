#include "Window_internal.h"

#include "../world/Sprite.h"

void rct_window::SetLocation(int32_t newX, int32_t newY, int32_t newZ)
{
    window_scroll_to_location(this, newX, newY, newZ);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}

void rct_window::ScrollToViewport()
{
    int32_t newX, newY, newZ;
    rct_window* mainWindow;

    // In original checked to make sure x and y were not -1 as well.
    if (viewport == nullptr || viewport_focus_coordinates.y == -1)
        return;

    if (viewport_focus_sprite.type & VIEWPORT_FOCUS_TYPE_SPRITE)
    {
        rct_sprite* sprite = get_sprite(viewport_focus_sprite.sprite_id);
        newX = sprite->generic.x;
        newY = sprite->generic.y;
        newZ = sprite->generic.z;
    }
    else
    {
        newX = viewport_focus_coordinates.x;
        newY = viewport_focus_coordinates.y & VIEWPORT_FOCUS_Y_MASK;
        newZ = viewport_focus_coordinates.z;
    }

    mainWindow = window_get_main();
    if (mainWindow != nullptr)
        window_scroll_to_location(mainWindow, newX, newY, newZ);
}

void rct_window::Invalidate()
{
    gfx_set_dirty_blocks(windowPos.x, windowPos.y, windowPos.x + width, windowPos.y + height);
}
