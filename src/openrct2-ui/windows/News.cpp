/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Peep.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/sprites.h>

static constexpr const StringId WINDOW_TITLE = STR_RECENT_MESSAGES;
static constexpr const int32_t WH = 300;
static constexpr const int32_t WW = 400;

// clang-format off
enum WindowNewsWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_SETTINGS,
    WIDX_SCROLL
};


static rct_widget window_news_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({372, 18}, { 24,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, SPR_TAB_GEARS_0), // settings
    MakeWidget({  4, 44}, {392, 252}, WindowWidgetType::Scroll,  WindowColour::Primary, SCROLL_VERTICAL), // scroll
    WIDGETS_END,
};

// clang-format on

class NewsWindow final : public Window
{
private:
    int32_t _pressedNewsItemIndex{}, _pressedButtonIndex{}, _suspendUpdateTicks{};
    static int32_t CalculateItemHeight()
    {
        return 4 * font_get_line_height(FontSpriteBase::SMALL) + 2;
    }

public:
    void OnOpen() override
    {
        widgets = window_news_widgets;
        WindowInitScrollWidgets(*this);
        _pressedNewsItemIndex = -1;

        int32_t w = 0, h = 0;
        rct_widget* widget = &widgets[WIDX_SCROLL];
        window_get_scroll_size(this, 0, &w, &h);
        scrolls[0].v_top = std::max(0, h - (widget->height() - 1));
        WidgetScrollUpdateThumbs(*this, WIDX_SCROLL);
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_SETTINGS:
                context_open_window(WC_NOTIFICATION_OPTIONS);
                break;
        }
    }

    void OnUpdate() override
    {
        if (_pressedNewsItemIndex == -1 || --_suspendUpdateTicks != 0)
        {
            return;
        }

        Invalidate();
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click2, 0, windowPos.x + (width / 2));

        size_t j = _pressedNewsItemIndex;
        _pressedNewsItemIndex = -1;

        if (j >= gNewsItems.GetArchived().size())
        {
            return;
        }

        const auto& newsItem = gNewsItems.GetArchived()[j];
        if (newsItem.HasButton())
        {
            return;
        }

        if (_pressedButtonIndex == 1)
        {
            News::OpenSubject(newsItem.Type, newsItem.Assoc);
        }
        else if (_pressedButtonIndex > 1)
        {
            static rct_window* _mainWindow;
            auto subjectLoc = News::GetSubjectLocation(newsItem.Type, newsItem.Assoc);
            if (subjectLoc.has_value() && (_mainWindow = window_get_main()) != nullptr)
            {
                window_scroll_to_location(*_mainWindow, subjectLoc.value());
            }
        }
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        int32_t scrollHeight = static_cast<int32_t>(gNewsItems.GetArchived().size()) * CalculateItemHeight();
        return { WW, scrollHeight };
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        int32_t itemHeight = CalculateItemHeight();
        int32_t i = 0;
        int32_t buttonIndex = 0;
        auto mutableScreenCoords = screenCoords;
        for (const auto& newsItem : gNewsItems.GetArchived())
        {
            if (mutableScreenCoords.y < itemHeight)
            {
                if (newsItem.HasButton() || mutableScreenCoords.y < 14 || mutableScreenCoords.y >= 38
                    || mutableScreenCoords.x < 328)
                {
                    buttonIndex = 0;
                    break;
                }
                if (mutableScreenCoords.x < 351 && newsItem.TypeHasSubject())
                {
                    buttonIndex = 1;
                    break;
                }
                if (mutableScreenCoords.x < 376 && newsItem.TypeHasLocation())
                {
                    buttonIndex = 2;
                    break;
                }
            }
            mutableScreenCoords.y -= itemHeight;
            i++;
        }

        if (buttonIndex != 0)
        {
            _pressedNewsItemIndex = i;
            _pressedButtonIndex = buttonIndex;
            _suspendUpdateTicks = 4;
            Invalidate();
            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, windowPos.x + (width / 2));
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        int32_t lineHeight = font_get_line_height(FontSpriteBase::SMALL);
        int32_t itemHeight = CalculateItemHeight();
        int32_t y = 0;
        int32_t i = 0;

        for (const auto& newsItem : gNewsItems.GetArchived())
        {
            if (y >= dpi.y + dpi.height)
                break;
            if (y + itemHeight < dpi.y)
            {
                y += itemHeight;
                i++;
                continue;
            }

            // Background
            gfx_fill_rect_inset(
                &dpi, { -1, y, 383, y + itemHeight - 1 }, colours[1],
                (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_GREY));

            // Date text
            {
                auto ft = Formatter();
                ft.Add<StringId>(DateDayNames[newsItem.Day - 1]);
                ft.Add<StringId>(DateGameMonthNames[date_get_month(newsItem.MonthYear)]);
                DrawTextBasic(&dpi, { 2, y }, STR_NEWS_DATE_FORMAT, ft, { COLOUR_WHITE, FontSpriteBase::SMALL });
            }
            // Item text
            {
                auto ft = Formatter();
                ft.Add<const char*>(newsItem.Text.c_str());
                DrawTextWrapped(&dpi, { 2, y + lineHeight }, 325, STR_BOTTOM_TOOLBAR_NEWS_TEXT, ft, { FontSpriteBase::SMALL });
            }
            // Subject button
            if ((newsItem.TypeHasSubject()) && !(newsItem.HasButton()))
            {
                auto screenCoords = ScreenCoordsXY{ 328, y + lineHeight + 4 };

                int32_t press = 0;
                if (_pressedNewsItemIndex != -1)
                {
                    News::IsValidIndex(_pressedNewsItemIndex + News::ItemHistoryStart);
                    if (i == _pressedNewsItemIndex && _pressedButtonIndex == 1)
                    {
                        press = INSET_RECT_FLAG_BORDER_INSET;
                    }
                }
                gfx_fill_rect_inset(&dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 23, 23 } }, colours[2], press);

                switch (newsItem.Type)
                {
                    case News::ItemType::Ride:
                        gfx_draw_sprite(&dpi, ImageId(SPR_RIDE), screenCoords);
                        break;
                    case News::ItemType::Peep:
                    case News::ItemType::PeepOnRide:
                    {
                        rct_drawpixelinfo cliped_dpi;
                        if (!clip_drawpixelinfo(&cliped_dpi, &dpi, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
                        {
                            break;
                        }

                        auto peep = TryGetEntity<Peep>(EntityId::FromUnderlying(newsItem.Assoc));
                        if (peep == nullptr)
                        {
                            break;
                        }

                        auto clipCoords = ScreenCoordsXY{ 10, 19 };

                        // If normal peep set sprite to normal (no food)
                        // If staff set sprite to staff sprite
                        auto spriteType = PeepSpriteType::Normal;
                        auto* staff = peep->As<Staff>();
                        if (staff != nullptr)
                        {
                            spriteType = staff->SpriteType;
                            if (staff->AssignedStaffType == StaffType::Entertainer)
                            {
                                clipCoords.y += 3;
                            }
                        }

                        uint32_t image_id = GetPeepAnimation(spriteType).base_image;
                        image_id += 0xA0000001;
                        image_id |= (peep->TshirtColour << 19) | (peep->TrousersColour << 24);

                        gfx_draw_sprite(&cliped_dpi, ImageId::FromUInt32(image_id), clipCoords);
                        break;
                    }
                    case News::ItemType::Money:
                        gfx_draw_sprite(&dpi, ImageId(SPR_FINANCE), screenCoords);
                        break;
                    case News::ItemType::Research:
                        gfx_draw_sprite(&dpi, ImageId(newsItem.Assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords);
                        break;
                    case News::ItemType::Peeps:
                        gfx_draw_sprite(&dpi, ImageId(SPR_GUESTS), screenCoords);
                        break;
                    case News::ItemType::Award:
                        gfx_draw_sprite(&dpi, ImageId(SPR_AWARD), screenCoords);
                        break;
                    case News::ItemType::Graph:
                        gfx_draw_sprite(&dpi, ImageId(SPR_GRAPH), screenCoords);
                        break;
                    case News::ItemType::Null:
                    case News::ItemType::Blank:
                    case News::ItemType::Count:
                        break;
                }
            }

            // Location button
            if ((newsItem.TypeHasLocation()) && !(newsItem.HasButton()))
            {
                auto screenCoords = ScreenCoordsXY{ 352, y + lineHeight + 4 };

                int32_t press = 0;
                if (_pressedNewsItemIndex != -1)
                {
                    News::IsValidIndex(_pressedNewsItemIndex + News::ItemHistoryStart);
                    if (i == _pressedNewsItemIndex && _pressedButtonIndex == 2)
                        press = 0x20;
                }
                gfx_fill_rect_inset(&dpi, { screenCoords, screenCoords + ScreenCoordsXY{ 23, 23 } }, colours[2], press);
                gfx_draw_sprite(&dpi, ImageId(SPR_LOCATE), screenCoords);
            }

            y += itemHeight;
            i++;
        }
    }
};

rct_window* WindowNewsOpen()
{
    return WindowFocusOrCreate<NewsWindow>(WC_RECENT_NEWS, WW, WH, 0);
}
