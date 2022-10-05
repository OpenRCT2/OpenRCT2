/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cstddef>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/sprites.h>

static constexpr const StringId WINDOW_TITLE = STR_NOTIFICATION_SETTINGS;
static constexpr const int32_t WH = 300;
static constexpr const int32_t WW = 400;

// clang-format off
enum
{
    NOTIFICATION_CATEGORY_PARK,
    NOTIFICATION_CATEGORY_RIDE,
    NOTIFICATION_CATEGORY_GUEST,
    NOTIFICATION_CATEGORY_COUNT
};

struct NotificationDef
{
    uint8_t category;
    StringId caption;
    size_t config_offset;
};

static constexpr const NotificationDef NewsItemOptionDefinitions[] = {
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

enum WindowNewsOptionsWidgetIdx
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TAB_CONTENT_PANEL,
    WIDX_FIRST_TAB,
    WIDX_TAB_PARK = WIDX_FIRST_TAB,
    WIDX_TAB_RIDE,
    WIDX_TAB_GUEST,
    WIDX_CHECKBOX_0
};

static rct_widget WindowNewsOptionsWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 0, 43}, {400, 257}, WindowWidgetType::Resize,   WindowColour::Secondary), // Tab content panel
    MakeTab   ({ 3, 17}                                                                 ), // Park tab
    MakeTab   ({34, 17}                                                                 ), // Ride tab
    MakeTab   ({65, 17}                                                                 ), // Guest tab
    MakeWidget({ 7, 49}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    MakeWidget({ 0,  0}, {343,  14}, WindowWidgetType::Checkbox, WindowColour::Tertiary ),
    WIDGETS_END,
};
// clang-format on

class NewsOptionsWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = WindowNewsOptionsWidgets;
        InitScrollWidgets();
        colours[0] = COLOUR_GREY;
        colours[1] = COLOUR_LIGHT_BLUE;
        colours[2] = COLOUR_LIGHT_BLUE;
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_PARK:
            case WIDX_TAB_RIDE:
            case WIDX_TAB_GUEST:
                SetPage(widgetIndex - WIDX_FIRST_TAB);
                break;
            default:
            {
                int32_t checkBoxIndex = widgetIndex - WIDX_CHECKBOX_0;
                if (checkBoxIndex >= 0)
                {
                    int32_t matchIndex = 0;
                    for (size_t i = 0; i < std::size(NewsItemOptionDefinitions); i++)
                    {
                        const NotificationDef* ndef = &NewsItemOptionDefinitions[i];
                        if (ndef->category != page)
                            continue;

                        if (matchIndex == checkBoxIndex)
                        {
                            // Toggle value
                            bool* configValue = GetNotificationValuePtr(ndef);
                            *configValue = !(*configValue);

                            config_save_default();

                            InvalidateWidget(widgetIndex);
                            break;
                        }
                        matchIndex++;
                    }
                }
                break;
            }
        }
    }

    void OnUpdate() override
    {
        // Tab animation
        frame_no++;
        InvalidateWidget(WIDX_FIRST_TAB + page);
    }

    void OnPrepareDraw() override
    {
        SetPressedTab();

        // Set checkboxes
        const auto& baseCheckBox = widgets[WIDX_CHECKBOX_0];
        int32_t y = baseCheckBox.top;

        int32_t checkboxWidgetIndex = WIDX_CHECKBOX_0;
        rct_widget* checkboxWidget = &widgets[checkboxWidgetIndex];
        for (size_t i = 0; i < std::size(NewsItemOptionDefinitions); i++)
        {
            const NotificationDef* ndef = &NewsItemOptionDefinitions[i];
            if (ndef->category != page)
                continue;

            checkboxWidget->type = WindowWidgetType::Checkbox;
            checkboxWidget->left = baseCheckBox.left;
            checkboxWidget->right = baseCheckBox.right;
            checkboxWidget->top = y;
            checkboxWidget->bottom = checkboxWidget->top + LIST_ROW_HEIGHT + 3;
            checkboxWidget->text = ndef->caption;

            const bool* configValue = GetNotificationValuePtr(ndef);
            SetCheckboxValue(checkboxWidgetIndex, *configValue);

            checkboxWidgetIndex++;
            checkboxWidget++;
            y += LIST_ROW_HEIGHT + 3;
        }

        // Remove unused checkboxes
        while (checkboxWidget->type != WindowWidgetType::Last)
        {
            checkboxWidget->type = WindowWidgetType::Empty;
            checkboxWidgetIndex++;
            checkboxWidget++;
        }

        // Resize window to fit checkboxes exactly
        y += 3;

        if (height != y)
        {
            Invalidate();
            height = y;
            widgets[WIDX_BACKGROUND].bottom = y - 1;
            widgets[WIDX_TAB_CONTENT_PANEL].bottom = y - 1;
            Invalidate();
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(&dpi);
    }

private:
    void SetPage(int32_t p)
    {
        if (page != p)
        {
            page = p;
            frame_no = 0;
            Invalidate();
        }
    }

    void DrawTabImages(rct_drawpixelinfo* dpi)
    {
        DrawTabImage(dpi, NOTIFICATION_CATEGORY_PARK, SPR_TAB_PARK);
        DrawTabImage(dpi, NOTIFICATION_CATEGORY_RIDE, SPR_TAB_RIDE_0);
        DrawTabImage(dpi, NOTIFICATION_CATEGORY_GUEST, SPR_TAB_GUESTS_0);
    }

    void DrawTabImage(rct_drawpixelinfo* dpi, int32_t p, int32_t spriteIndex)
    {
        WidgetIndex widgetIndex = WIDX_FIRST_TAB + p;

        if (!WidgetIsDisabled(*this, widgetIndex))
        {
            if (page == p)
            {
                int32_t numFrames = TabAnimationFrames[page];
                if (numFrames > 1)
                {
                    int32_t frame = frame_no / TabAnimationDivisor[page];
                    spriteIndex += (frame % numFrames);
                }
            }

            const auto& widget = widgets[widgetIndex];
            gfx_draw_sprite(dpi, ImageId(spriteIndex), windowPos + ScreenCoordsXY{ widget.left, widget.top });
        }
    }

    void SetPressedTab()
    {
        for (int32_t i = 0; i < NOTIFICATION_CATEGORY_COUNT; i++)
            pressed_widgets &= ~(1 << (WIDX_FIRST_TAB + i));
        pressed_widgets |= 1LL << (WIDX_FIRST_TAB + page);
    }

    bool* GetNotificationValuePtr(const NotificationDef* ndef)
    {
        bool* configValue = reinterpret_cast<bool*>(reinterpret_cast<size_t>(&gConfigNotifications) + ndef->config_offset);
        return configValue;
    }

    static constexpr int32_t TabAnimationDivisor[3] = {
        1, // Park
        4, // Ride
        4, // Guest
    };
    static constexpr int32_t TabAnimationFrames[3] = {
        1,  // Park
        16, // Ride
        8,  // Guest
    };
};

rct_window* WindowNewsOptionsOpen()
{
    return WindowFocusOrCreate<NewsOptionsWindow>(WindowClass::NotificationOptions, WW, WH, WF_CENTRE_SCREEN);
}
