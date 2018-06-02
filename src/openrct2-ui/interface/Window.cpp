#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/Context.h>
#include <openrct2/interface/themes.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/world/Sprite.h>
#include <openrct2/ui/UiContext.h>
#include "Window.h"

#define RCT2_NEW_WINDOW         (gWindowNextSlot)
#define RCT2_LAST_WINDOW        (gWindowNextSlot - 1)

using namespace OpenRCT2;

static bool window_fits_between_others(sint32 x, sint32 y, sint32 width, sint32 height)
{
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        if (x + width <= w->x) continue;
        if (x >= w->x + w->width) continue;
        if (y + height <= w->y) continue;
        if (y >= w->y + w->height) continue;
        return false;
    }

    return true;
}

static bool window_fits_within_space(sint32 x, sint32 y, sint32 width, sint32 height)
{
    if (x < 0) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    if (x + width > context_get_width()) return false;
    if (y + height > context_get_height()) return false;
    return window_fits_between_others(x, y, width, height);
}

static bool window_fits_on_screen(sint32 x, sint32 y, sint32 width, sint32 height)
{
    uint16 screenWidth = context_get_width();
    uint16 screenHeight = context_get_height();
    sint32 unk;

    unk = -(width / 4);
    if (x < unk) return false;
    unk = screenWidth + (unk * 2);
    if (x > unk) return false;
    if (y <= TOP_TOOLBAR_HEIGHT && !(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO)) return false;
    unk = screenHeight - (height / 4);
    if (y > unk) return false;
    return window_fits_between_others(x, y, width, height);
}

rct_window *window_create(sint32 x, sint32 y, sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    // Check if there are any window slots left
    // include WINDOW_LIMIT_RESERVED for items such as the main viewport and toolbars to not appear to be counted.
    if (RCT2_NEW_WINDOW >= &(g_window_list[gConfigGeneral.window_limit + WINDOW_LIMIT_RESERVED])) {
        rct_window *w = nullptr;
        // Close least recently used window
        for (w = g_window_list; w < RCT2_NEW_WINDOW; w++)
            if (!(w->flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT | WF_NO_AUTO_CLOSE)))
                break;

        window_close(w);
    }

    rct_window *w = RCT2_NEW_WINDOW;

    // Flags
    if (flags & WF_STICK_TO_BACK) {
        for (; w >= g_window_list + 1; w--) {
            if ((w - 1)->flags & WF_STICK_TO_FRONT)
                continue;
            if ((w - 1)->flags & WF_STICK_TO_BACK)
                break;
        }
    }
    else if (!(flags & WF_STICK_TO_FRONT)) {
        for (; w >= g_window_list + 1; w--) {
            if (!((w - 1)->flags & WF_STICK_TO_FRONT))
                break;
        }
    }

    // Move w to new window slot
    if (w != RCT2_NEW_WINDOW)
        *RCT2_NEW_WINDOW = *w;

    // Setup window
    w->classification = cls;
    w->var_4B8 = -1;
    w->var_4B9 = -1;
    w->flags = flags;

    // Play sounds and flash the window
    if (!(flags & (WF_STICK_TO_BACK | WF_STICK_TO_FRONT))) {
        w->flags |= WF_WHITE_BORDER_MASK;
        audio_play_sound(SOUND_WINDOW_OPEN, 0, x + (width / 2));
    }

    w->number = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->viewport = nullptr;
    w->event_handlers = event_handlers;
    w->enabled_widgets = 0;
    w->disabled_widgets = 0;
    w->pressed_widgets = 0;
    w->hold_down_widgets = 0;
    w->viewport_focus_coordinates.var_480 = 0;
    w->viewport_focus_coordinates.x = 0;
    w->viewport_focus_coordinates.y = 0;
    w->viewport_focus_coordinates.z = 0;
    w->viewport_focus_coordinates.rotation = 0;
    w->page = 0;
    w->var_48C = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->var_492 = 0;
    w->selected_tab = 0;
    w->var_4AE = 0;
    w->viewport_smart_follow_sprite = SPRITE_INDEX_NULL;
    RCT2_NEW_WINDOW++;

    colour_scheme_update(w);
    window_invalidate(w);
    return w;
}

rct_window *window_create_auto_pos(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    // TODO dead code, looks like it is cascading the new window offset from an existing window
    // we will have to re-implement this in our own way.
    //
    // if (cls & 0x80) {
    //  cls &= ~0x80;
    //  rct_window *w = window_find_by_number(0, 0);
    //  if (w != nullptr) {
    //      if (w->x > -60 && w->x < screenWidth - 20) {
    //          if (w->y < screenHeight - 20) {
    //              sint32 x = w->x;
    //              if (w->x + width > screenWidth)
    //                  x = screenWidth - 20 - width;
    //              sint32 y = w->y;
    //              return window_create(x + 10, y + 10, width, height, event_handlers, cls, flags);
    //          }
    //      }
    //  }
    // }

    // Place window in an empty corner of the screen
    sint32 x = 0;
    sint32 y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = 30;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = 0;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    x = screenWidth - width;
    y = screenHeight - 34 - height;
    if (window_fits_within_space(x, y, width, height)) goto foundSpace;

    // Place window next to another
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y - w->height - 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x + w->width + 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y + w->height + 2;
        if (window_fits_within_space(x, y, width, height)) goto foundSpace;
    }

    // Overlap
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (w->flags & WF_STICK_TO_BACK)
            continue;

        x = w->x + w->width + 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x - w->width - 2;
        y = w->y;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y + w->height + 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;

        x = w->x;
        y = w->y - w->height - 2;
        if (window_fits_on_screen(x, y, width, height)) goto foundSpace;
    }

    // Cascade
    x = 0;
    y = 30;
    for (rct_window *w = g_window_list; w < RCT2_LAST_WINDOW; w++) {
        if (x != w->x || y != w->y)
            continue;

        x += 5;
        y += 5;
    }

    // Clamp to inside the screen
foundSpace:
    if (x < 0)
        x = 0;
    if (x + width > screenWidth)
        x = screenWidth - width;

    return window_create(x, y, width, height, event_handlers, cls, flags);
}

rct_window * window_create_centred(sint32 width, sint32 height, rct_window_event_list *event_handlers, rct_windowclass cls, uint16 flags)
{
    auto uiContext = GetContext()->GetUiContext();
    auto screenWidth = uiContext->GetWidth();
    auto screenHeight = uiContext->GetHeight();

    sint32 x = (screenWidth - width) / 2;
    sint32 y = std::max(TOP_TOOLBAR_HEIGHT + 1, (screenHeight - height) / 2);
    return window_create(x, y, width, height, event_handlers, cls, flags);
}
