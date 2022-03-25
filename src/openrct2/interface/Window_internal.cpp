#include "Window_internal.h"

#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "Viewport.h"

void rct_window::SetLocation(const CoordsXYZ& coords)
{
    window_scroll_to_location(this, coords);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}

void rct_window::ScrollToViewport()
{
    if (viewport == nullptr || !focus.has_value())
        return;

    auto mainWindow = window_get_main();
    if (mainWindow != nullptr)
    {
        CoordsXYZ newCoords = focus.value().GetPos();
        window_scroll_to_location(mainWindow, newCoords);
    }
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
