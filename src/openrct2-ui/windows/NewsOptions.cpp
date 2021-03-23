/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cstddef>
#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/sprites.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_NOTIFICATION_SETTINGS;
static constexpr const int32_t WH = 300;
static constexpr const int32_t WW = 400;

// clang-format off
enum {
    NOTIFICATION_CATEGORY_PARK,
    NOTIFICATION_CATEGORY_RIDE,
    NOTIFICATION_CATEGORY_GUEST
};

struct notification_def {
    uint8_t category;
    rct_string_id caption;
    size_t config_offset;
};

static constexpr const notification_def NewsItemOptionDefinitions[] = {
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_AWARD,                        offsetof(NotificationConfiguration, park_award)                         },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_MARKETING_CAMPAIGN_FINISHED,  offsetof(NotificationConfiguration, park_marketing_campaign_finished)   },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_WARNINGS,                     offsetof(NotificationConfiguration, park_warnings)                      },
    { NOTIFICATION_CATEGORY_PARK,   STR_NOTIFICATION_PARK_RATING_WARNINGS,              offsetof(NotificationConfiguration, park_rating_warnings)               },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_BROKEN_DOWN,                  offsetof(NotificationConfiguration, ride_broken_down)                   },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_CRASHED,                      offsetof(NotificationConfiguration, ride_crashed)                       },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_CASUALTIES,                   offsetof(NotificationConfiguration, ride_casualties)                    },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_WARNINGS,                     offsetof(NotificationConfiguration, ride_warnings)                      },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_RESEARCHED,                   offsetof(NotificationConfiguration, ride_researched)                    },
    { NOTIFICATION_CATEGORY_RIDE,   STR_NOTIFICATION_RIDE_VEHICLE_STALLED,              offsetof(NotificationConfiguration, ride_stalled_vehicles)              },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_WARNINGS,                    offsetof(NotificationConfiguration, guest_warnings)                     },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_LEFT_PARK,                   offsetof(NotificationConfiguration, guest_left_park)                    },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_QUEUING_FOR_RIDE,            offsetof(NotificationConfiguration, guest_queuing_for_ride)             },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_ON_RIDE,                     offsetof(NotificationConfiguration, guest_on_ride)                      },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_LEFT_RIDE,                   offsetof(NotificationConfiguration, guest_left_ride)                    },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_BOUGHT_ITEM,                 offsetof(NotificationConfiguration, guest_bought_item)                  },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_USED_FACILITY,               offsetof(NotificationConfiguration, guest_used_facility)                },
    { NOTIFICATION_CATEGORY_GUEST,  STR_NOTIFICATION_GUEST_DIED,                        offsetof(NotificationConfiguration, guest_died)                         },
};

enum WINDOW_NEWS_OPTIONS_WIDGET_IDX {
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
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 0, 43}, {400, 257}, WindowWidgetType::Resize,   WindowColour::Secondary), // tab content panel
    MakeTab   ({ 3, 17}                                                   ), // tab 1
    MakeTab   ({34, 17}                                                   ), // tab 2
    MakeTab   ({65, 17}                                                   ), // tab 2
    MakeWidget({ 7, 49}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    { WIDGETS_END },
};

static void window_news_options_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_news_options_update(rct_window *w);
static void window_news_options_invalidate(rct_window *w);
static void window_news_options_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_news_options_events([](auto& events)
{
    events.mouse_up = &window_news_options_mouseup;
    events.update = &window_news_options_update;
    events.invalidate = &window_news_options_invalidate;
    events.paint = &window_news_options_paint;
});
// clang-format on

static void window_news_options_set_page(rct_window* w, int32_t page);
static void window_news_options_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi);
static bool* get_notification_value_ptr(const notification_def* ndef);

rct_window* window_news_options_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_NOTIFICATION_OPTIONS);
    if (window == nullptr)
    {
        window = WindowCreateCentred(400, 300, &window_news_options_events, WC_NOTIFICATION_OPTIONS, 0);
        window->widgets = window_news_options_widgets;
        window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TAB_PARK) | (1 << WIDX_TAB_RIDE) | (1 << WIDX_TAB_GUEST);
        WindowInitScrollWidgets(window);
        window->colours[0] = COLOUR_GREY;
        window->colours[1] = COLOUR_LIGHT_BLUE;
        window->colours[2] = COLOUR_LIGHT_BLUE;
    }

    return window;
}

static void window_news_options_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
            int32_t checkBoxIndex = widgetIndex - WIDX_CHECKBOX_0;
            if (checkBoxIndex >= 0)
            {
                int32_t matchIndex = 0;
                for (size_t i = 0; i < std::size(NewsItemOptionDefinitions); i++)
                {
                    const notification_def* ndef = &NewsItemOptionDefinitions[i];
                    if (ndef->category != w->page)
                        continue;

                    if (matchIndex == checkBoxIndex)
                    {
                        // Toggle value
                        bool* configValue = get_notification_value_ptr(ndef);
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

static void window_news_options_update(rct_window* w)
{
    w->frame_no++;
    widget_invalidate(w, WIDX_TAB_PARK + w->page);
}

static void window_news_options_invalidate(rct_window* w)
{
    // Set pressed tab
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_PARK);
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_RIDE);
    w->pressed_widgets &= ~(1ULL << WIDX_TAB_GUEST);
    w->pressed_widgets |= (1ULL << (WIDX_TAB_PARK + w->page));

    // Set checkboxes
    rct_widget* baseCheckBox = &w->widgets[WIDX_CHECKBOX_0];
    int32_t y = baseCheckBox->top;

    int32_t checkboxWidgetIndex = WIDX_CHECKBOX_0;
    rct_widget* checkboxWidget = &w->widgets[checkboxWidgetIndex];
    for (size_t i = 0; i < std::size(NewsItemOptionDefinitions); i++)
    {
        const notification_def* ndef = &NewsItemOptionDefinitions[i];
        if (ndef->category != w->page)
            continue;

        w->enabled_widgets |= (1ULL << checkboxWidgetIndex);

        checkboxWidget->type = WindowWidgetType::Checkbox;
        checkboxWidget->left = baseCheckBox->left;
        checkboxWidget->right = baseCheckBox->right;
        checkboxWidget->top = y;
        checkboxWidget->bottom = checkboxWidget->top + LIST_ROW_HEIGHT + 3;
        checkboxWidget->text = ndef->caption;

        const bool* configValue = get_notification_value_ptr(ndef);
        WidgetSetCheckboxValue(w, checkboxWidgetIndex, *configValue);

        checkboxWidgetIndex++;
        checkboxWidget++;
        y += LIST_ROW_HEIGHT + 3;
    }

    // Remove unused checkboxes
    while (checkboxWidget->type != WindowWidgetType::Last)
    {
        w->enabled_widgets &= ~(1ULL << checkboxWidgetIndex);

        checkboxWidget->type = WindowWidgetType::Empty;
        checkboxWidgetIndex++;
        checkboxWidget++;
    }

    // Resize window to fit checkboxes exactly
    y += 3;

    if (w->height != y)
    {
        w->Invalidate();
        w->height = y;
        w->widgets[WIDX_BACKGROUND].bottom = y - 1;
        w->widgets[WIDX_TAB_CONTENT_PANEL].bottom = y - 1;
        w->Invalidate();
    }
}

static void window_news_options_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
    window_news_options_draw_tab_images(w, dpi);
}

static void window_news_options_set_page(rct_window* w, int32_t page)
{
    if (w->page != page)
    {
        w->page = page;
        w->frame_no = 0;
        w->Invalidate();
    }
}

const int32_t window_news_option_tab_animation_divisor[] = { 1, 4, 4 };
const int32_t window_news_option_tab_animation_frames[] = { 1, 16, 8 };

static void window_news_options_draw_tab_image(rct_window* w, rct_drawpixelinfo* dpi, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_PARK + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        if (w->page == page)
        {
            int32_t numFrames = window_news_option_tab_animation_frames[w->page];
            if (numFrames > 1)
            {
                int32_t frame = w->frame_no / window_news_option_tab_animation_divisor[w->page];
                spriteIndex += (frame % numFrames);
            }
        }

        gfx_draw_sprite(
            dpi, ImageId(spriteIndex),
            w->windowPos + ScreenCoordsXY{ w->widgets[widgetIndex].left, w->widgets[widgetIndex].top });
    }
}

static void window_news_options_draw_tab_images(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_PARK, SPR_TAB_PARK);
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_RIDE, SPR_TAB_RIDE_0);
    window_news_options_draw_tab_image(w, dpi, NOTIFICATION_CATEGORY_GUEST, SPR_TAB_GUESTS_0);
}

static bool* get_notification_value_ptr(const notification_def* ndef)
{
    bool* configValue = reinterpret_cast<bool*>(reinterpret_cast<size_t>(&gConfigNotifications) + ndef->config_offset);
    return configValue;
}
