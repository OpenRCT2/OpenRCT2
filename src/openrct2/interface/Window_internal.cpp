#include "Window_internal.h"

void rct_window::SetLocation(int32_t newX, int32_t newY, int32_t newZ)
{
    window_scroll_to_location(this, newX, newY, newZ);
    flags &= ~WF_SCROLLING_TO_LOCATION;
}
