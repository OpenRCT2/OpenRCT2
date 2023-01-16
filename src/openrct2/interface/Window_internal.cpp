#include "Window_internal.h"

#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "Cursors.h"
#include "Viewport.h"

void rct_window::SetLocation(const CoordsXYZ& coords)
{
    WindowScrollToLocation(*this, coords);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}

void rct_window::ScrollToViewport()
{
    if (viewport == nullptr || !focus.has_value())
        return;

    CoordsXYZ newCoords = focus.value().GetPos();

    auto mainWindow = WindowGetMain();
    if (mainWindow != nullptr)
        WindowScrollToLocation(*mainWindow, newCoords);
}

void rct_window::Invalidate()
{
    GfxSetDirtyBlocks({ windowPos, windowPos + ScreenCoordsXY{ width, height } });
}

void rct_window::RemoveViewport()
{
    if (viewport == nullptr)
        return;

    viewport_remove(viewport);
    viewport = nullptr;
}

CursorID rct_window::OnCursor(WidgetIndex, const ScreenCoordsXY&, CursorID)
{
    return CursorID::Arrow;
}
