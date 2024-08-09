#include "Window_internal.h"

#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "Cursors.h"
#include "Viewport.h"

void WindowBase::SetLocation(const CoordsXYZ& coords)
{
    WindowScrollToLocation(*this, coords);
    flags &= ~WF_SCROLLING_TO_LOCATION;
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

CursorID WindowBase::OnCursor(WidgetIndex, const ScreenCoordsXY&, CursorID)
{
    return CursorID::Arrow;
}
