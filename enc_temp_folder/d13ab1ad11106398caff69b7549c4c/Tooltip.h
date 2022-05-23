#pragma once
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Input.h>

static widget_ref gTooltipWidget;
static ScreenCoordsXY lastTooltipCursorXY;

void WindowTooltipFastOpen();
void WindowTooltipClose();
void WindowTooltipPaint(rct_window* w, rct_drawpixelinfo* dpi);
void WindowTooltipOpen(rct_window* widgetWindow, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCords);
void WindowTooltipReset(const ScreenCoordsXY& screenCoords);
void WindowTooltipShow(const OpenRCT2String& message, ScreenCoordsXY screenCoords);
void WindowTooltipUpdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
