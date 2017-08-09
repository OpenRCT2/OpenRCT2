#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <openrct2/config/Config.h>
#include <openrct2/core/Util.hpp>
#include <openrct2-ui/windows/Window.h>

extern "C"
{
    #include <openrct2/interface/widget.h>
    #include <openrct2/localisation/localisation.h>
    #include <openrct2/sprites.h>
}

enum {
    NOTIFICATION_CATEGORY_PARK,
    NOTIFICATION_CATEGORY_RIDE,
    NOTIFICATION_CATEGORY_GUEST
};

typedef struct notification_def {
    uint8 category;
    rct_string_id caption;
    size_t config_offset;
} notification_def;

static const notification_def NewsItemOptionDefinitions[] = {
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_AWARD,                        offsetof(NotificationConfiguration, park_award)                         },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_MARKETING_CAMPAIGN_FINISHED,  offsetof(NotificationConfiguration, park_marketing_campaign_finished)   },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_WARNINGS,                     offsetof(NotificationConfiguration, park_warnings)                      },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_RATING_WARNINGS,              offsetof(NotificationConfiguration, park_rating_warnings)               },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_BROKEN_DOWN,                  offsetof(NotificationConfiguration, ride_broken_down)                   },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_CRASHED,                      offsetof(NotificationConfiguration, ride_crashed)                       },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_WARNINGS,                     offsetof(NotificationConfiguration, ride_warnings)                      },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_RESEARCHED,                   offsetof(NotificationConfiguration, ride_researched)                    },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_WARNINGS,                    offsetof(NotificationConfiguration, guest_warnings)                     },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_LOST,                        offsetof(NotificationConfiguration, guest_lost)                         },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_LEFT_PARK,                   offsetof(NotificationConfiguration, guest_left_park)                    },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_QUEUING_FOR_RIDE,            offsetof(NotificationConfiguration, guest_queuing_for_ride)             },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_ON_RIDE,                     offsetof(NotificationConfiguration, guest_on_ride)                      },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_LEFT_RIDE,                   offsetof(NotificationConfiguration, guest_left_ride)                    },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_BOUGHT_ITEM,                 offsetof(NotificationConfiguration, guest_bought_item)                  },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_USED_FACILITY,               offsetof(NotificationConfiguration, guest_used_facility)                },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_DIED,                        offsetof(NotificationConfiguration, guest_died)                         },
};

enum WINDOW_NEWS_WIDGET_IDX {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TAB_CONTENT_PANEL,
    WIDX_TAB_PARK,
    WIDX_TAB_RIDE,
    WIDX_TAB_GUEST,
    WIDX_CHECKBOX_0
};

static rct_widget window_news_options_widgets[] = {
    { WWT_FRAME,            0,  0,          399,    0,      299,    0xFFFFFFFF,                     STR_NONE },             // panel / background
    { WWT_CAPTION,          0,  1,          398,    1,      14,     STR_NOTIFICATION_SETTINGS,      STR_WINDOW_TITLE_TIP }, // title bar
    { WWT_CLOSEBOX,         0,  387,        397,    2,      13,     STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP }, // close x button
    { WWT_RESIZE,           1,  0,          399,    43,     299,    0xFFFFFFFF,                     STR_NONE },             // tab content panel
    { WWT_TAB,              1,  3,          33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_NONE },             // tab 1
    { WWT_TAB,              1,  34,         64,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_NONE },             // tab 2
    { WWT_TAB,              1,  65,         95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,           STR_NONE },             // tab 2

    { WWT_CHECKBOX,         2,  3,          349,    46,     59,     STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },
    { WWT_CHECKBOX,         2,  0,          0,      0,      0,      STR_NONE,                       STR_NONE },

    { WIDGETS_END },
};

static void window_news_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_news_options_update(rct_window *w);
static void window_news_options_invalidate(rct_window *w);
static void window_news_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_news_options_events = {
    NULL,
    window_news_options_mouseup,
    NULL,
    NULL,
    NULL,
    NULL,
    window_news_options_update,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    window_news_options_invalidate,
    window_news_options_paint,
    NULL
};

static void window_news_options_set_page(rct_window *w, sint32 page);
static void window_news_options_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi);
static bool *get_notification_value_ptr(const notification_def *ndef);

rct_window * window_news_options_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_NOTIFICATION_OPTIONS);
    if (window == NULL) {
        window = window_create_centred(
            400,
            300,
            &window_news_options_events,
            WC_NOTIFICATION_OPTIONS,
            0
        );
        window->widgets = window_news_options_widgets;
        window->enabled_widgets =
            (1 << WIDX_CLOSE) |
            (1 << WIDX_TAB_PARK) |
            (1 << WIDX_TAB_RIDE) |
            (1 << WIDX_TAB_GUEST);
        window_init_scroll_widgets(window);
        window->colours[0] = COLOUR_GREY;
        window->colours[1] = COLOUR_LIGHT_BLUE;
        window->colours[2] = COLOUR_LIGHT_BLUE;
    }

    return window;
}

static void window_news_options_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_TAB_PARK:
    case WIDX_TAB_RIDE:
    case WIDX_TAB_GUEST:
        window_news_options_set_page(w, widgetIndex - WIDX_TAB_PARK);
        break;
    default:
    {
        sint32 checkBoxIndex = widgetIndex - WIDX_CHECKBOX_0;
        if (checkBoxIndex >= 0) {
            sint32 matchIndex = 0;
            for (size_t i = 0; i < Util::CountOf(NewsItemOptionDefinitions); i++) {
                const notification_def *ndef = &NewsItemOptionDefinitions[i];
                if (ndef->category != w->page) continue;

                if (matchIndex == checkBoxIndex) {
                    // Toggle value
                    bool *configValue = get_notification_value_ptr(ndef);
                    *configValue = !(*configValue);

                    config_save_default();

                    widget_invalidate(w, widgetIndex);
                    break;
                }
                matchIndex++;
            }
        }
        break;
    }
    }
}

static void window_news_options_update(rct_window *w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_PARK + w->page);
}

static void window_news_options_invalidate(rct_window *w)
{
    // Set pressed tab
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_PARK);
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_RIDE);
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_GUEST);
    w->pressed_widgets |= (1ULL << (WIDX_TAB_PARK + w->page));

    // Set checkboxes
    rct_widget *baseCheckBox = &w->widgets[WIDX_CHECKBOX_0];
    sint32 y = baseCheckBox->top;

    sint32 checkboxWidgetIndex = WIDX_CHECKBOX_0;
    rct_widget *checkboxWidget = &w->widgets[checkboxWidgetIndex];
    for (size_t i = 0; i < Util::CountOf(NewsItemOptionDefinitions); i++) {
        const notification_def *ndef = &NewsItemOptionDefinitions[i];
        if (ndef->category != w->page) continue;

        w->enabled_widgets |= (1ULL << checkboxWidgetIndex);

        checkboxWidget->type = WWT_CHECKBOX;
        checkboxWidget->left = baseCheckBox->left;
        checkboxWidget->right = baseCheckBox->right;
        checkboxWidget->top = y;
        checkboxWidget->bottom = checkboxWidget->top + 13;
        checkboxWidget->text = ndef->caption;

        const bool *configValue = get_notification_value_ptr(ndef);
        widget_set_checkbox_value(w, checkboxWidgetIndex, *configValue);

        checkboxWidgetIndex++;
        checkboxWidget++;
        y += 13;
    }

    // Remove unused checkboxes
    while (checkboxWidget->type != WWT_LAST) {
        w->enabled_widgets &= ~(1ULL << checkboxWidgetIndex);

        checkboxWidget->type = WWT_EMPTY;
        checkboxWidgetIndex++;
        checkboxWidget++;
    }

    // Resize window to fit checkboxes exactly
    y += 3;

    if (w->height != y) {
        window_invalidate(w);
        w->height = y;
        w->widgets[WIDX_BACKGROUND].bottom = y - 1;
        w->widgets[WIDX_TAB_CONTENT_PANEL].bottom = y - 1;
        window_invalidate(w);
    }
}

static void window_news_options_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);
    window_news_options_draw_tab_images(w, dpi);
}

static void window_news_options_set_page(rct_window *w, sint32 page)
{
    if (w->page != page) {
        w->page = page;
        w->frame_no = 0;
        window_invalidate(w);
    }
}

const sint32 window_news_option_tab_animation_divisor[] = { 1, 4, 4 };
const sint32 window_news_option_tab_animation_frames[] = { 1, 16, 8 };

static void window_news_options_draw_tab_image(rct_window *w, rct_drawpixelinfo *dpi, sint32 page, sint32 spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_PARK + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex))) {
        if (w->page == page) {
            sint32 numFrames = window_news_option_tab_animation_frames[w->page];
            if (numFrames > 1) {
                sint32 frame = w->frame_no / window_news_option_tab_animation_divisor[w->page];
                spriteIndex += (frame % numFrames);
            }
        }

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

static void window_news_options_draw_tab_images(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_PARK, SPR_TAB_PARK);
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_RIDE, SPR_TAB_RIDE_0);
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_GUEST, SPR_TAB_GUESTS_0);
}

static bool *get_notification_value_ptr(const notification_def *ndef)
{
    bool *configValue = (bool*)((size_t)&gConfigNotifications + ndef->config_offset);
    return configValue;
}
