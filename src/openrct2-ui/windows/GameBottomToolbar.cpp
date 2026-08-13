/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.String.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/RenderTarget.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepSpriteIds.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowGameBottomToolbarWidgetIdx : WidgetIndex
    {
        WIDX_MIDDLE_OUTSET,
        WIDX_MIDDLE_INSET,
        WIDX_NEWS_SUBJECT,
        WIDX_NEWS_LOCATE,
    };

    // clang-format off
    static constexpr Widget kGameBottomToolbarWidgets[] =
    {
        makeWidget({142,  0}, {356, 34}, WidgetType::imgBtn,      WindowColour::tertiary                                                    ), // Middle outset panel
        makeWidget({144,  2}, {352, 30}, WidgetType::hiddenButton,WindowColour::tertiary                                                    ), // Middle inset panel
        makeWidget({147,  5}, { 24, 24}, WidgetType::flatBtn,     WindowColour::secondary, 0xFFFFFFFF, STR_SHOW_SUBJECT_TIP                 ), // Associated news item window
        makeWidget({469,  5}, { 24, 24}, WidgetType::flatBtn,     WindowColour::secondary, ImageId(SPR_LOCATE), STR_LOCATE_SUBJECT_TIP      )  // Scroll to news item target
    };
    // clang-format on

    class GameBottomToolbar final : public Window
    {
    private:
        void DrawNewsItem(RenderTarget& rt)
        {
            const auto& middleOutsetWidget = widgets[WIDX_MIDDLE_OUTSET];
            auto* newsItem = News::GetItem(0);

            // Current news item
            Rectangle::fillInset(
                rt,

                { windowPos + ScreenCoordsXY{ middleOutsetWidget.left + 1, middleOutsetWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget.right - 1, middleOutsetWidget.bottom - 1 } },
                colours[2], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Text
            auto screenCoords = windowPos + ScreenCoordsXY{ middleOutsetWidget.midX(), middleOutsetWidget.top + 11 };
            int32_t itemWidth = middleOutsetWidget.width() - 63;
            drawNewsTicker(
                rt, screenCoords, itemWidth, Drawing::Colour::brightGreen, STR_BOTTOM_TOOLBAR_NEWS_TEXT, newsItem->text,
                newsItem->ticks);

            const auto& newsSubjectWidget = widgets[WIDX_NEWS_SUBJECT];
            screenCoords = windowPos + ScreenCoordsXY{ newsSubjectWidget.left, newsSubjectWidget.top };
            switch (newsItem->type)
            {
                case News::ItemType::ride:
                    GfxDrawSprite(rt, ImageId(SPR_RIDE), screenCoords);
                    break;
                case News::ItemType::peepOnRide:
                case News::ItemType::peep:
                {
                    if (newsItem->hasButton())
                        break;

                    RenderTarget clippedRT;
                    if (!ClipRenderTarget(clippedRT, rt, screenCoords + ScreenCoordsXY{ 1, 1 }, 22, 22))
                    {
                        break;
                    }

                    auto peep = getGameState().entities.TryGetEntity<Peep>(EntityId::FromUnderlying(newsItem->assoc));
                    if (peep == nullptr)
                        return;

                    auto clipCoords = ScreenCoordsXY{ 10, 19 };
                    auto* staff = peep->as<Staff>();
                    if (staff != nullptr && staff->isEntertainer())
                    {
                        clipCoords.y += 3;
                    }

                    auto& objManager = GetContext()->GetObjectManager();
                    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

                    uint32_t image_id_base = animObj->GetPeepAnimation(peep->AnimationGroup).baseImage;
                    image_id_base += currentFrame & 0xFFFFFFFC;
                    image_id_base++;

                    auto image_id = ImageId(image_id_base, peep->TshirtColour, peep->TrousersColour);
                    GfxDrawSprite(clippedRT, image_id, clipCoords);

                    auto* guest = peep->as<Guest>();
                    if (guest == nullptr)
                        return;

                    // There are only 6 walking frames available for each item,
                    // as well as 1 sprite for sitting and 1 for standing still.
                    auto itemFrame = (currentFrame / 4) % 6;

                    if (guest->AnimationGroup == PeepAnimationGroup::hat)
                    {
                        auto itemOffset = kPeepSpriteHatItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->hatColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->AnimationGroup == PeepAnimationGroup::balloon)
                    {
                        auto itemOffset = kPeepSpriteBalloonItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->balloonColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->AnimationGroup == PeepAnimationGroup::umbrella)
                    {
                        auto itemOffset = kPeepSpriteUmbrellaItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->umbrellaColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }
                    break;
                }
                case News::ItemType::money:
                case News::ItemType::campaign:
                    GfxDrawSprite(rt, ImageId(SPR_FINANCE), screenCoords);
                    break;
                case News::ItemType::research:
                    GfxDrawSprite(rt, ImageId(newsItem->assoc < 0x10000 ? SPR_NEW_SCENERY : SPR_NEW_RIDE), screenCoords);
                    break;
                case News::ItemType::peeps:
                    GfxDrawSprite(rt, ImageId(SPR_GUESTS), screenCoords);
                    break;
                case News::ItemType::award:
                    GfxDrawSprite(rt, ImageId(SPR_AWARD), screenCoords);
                    break;
                case News::ItemType::graph:
                    GfxDrawSprite(rt, ImageId(SPR_GRAPH), screenCoords);
                    break;
                case News::ItemType::null:
                case News::ItemType::blank:
                case News::ItemType::count:
                    break;
            }
        }

        void DrawMiddlePanel(RenderTarget& rt)
        {
            Widget* middleOutsetWidget = &widgets[WIDX_MIDDLE_OUTSET];

            Rectangle::fillInset(
                rt,
                { windowPos + ScreenCoordsXY{ middleOutsetWidget->left + 1, middleOutsetWidget->top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget->right - 1, middleOutsetWidget->bottom - 1 } },
                colours[0], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            ScreenCoordsXY middleWidgetCoords(
                windowPos.x + middleOutsetWidget->midX(), windowPos.y + middleOutsetWidget->top + line_height + 1);
            int32_t panelWidth = middleOutsetWidget->width() - 63;

            // Check if there is a map tooltip to draw
            StringId stringId;
            auto ft = GetMapTooltip();
            std::memcpy(&stringId, ft.Data(), sizeof(StringId));
            if (stringId == kStringIdNone)
            {
                // TODO: this string probably shouldn't be reused for this
                drawTextWrapped(
                    rt, middleWidgetCoords, panelWidth, STR_TITLE_SEQUENCE_OPENRCT2, ft, { colours[0], TextAlignment::centre });
            }
            else
            {
                // Show tooltip in bottom toolbar
                drawTextWrapped(rt, middleWidgetCoords, panelWidth, STR_STRINGID, ft, { colours[0], TextAlignment::centre });
            }
        }

    public:
        GameBottomToolbar()
        {
            setWidgets(kGameBottomToolbarWidgets);

            currentFrame = 0;
            initScrollWidgets();

            // Reset the middle widget to not show by default.
            // If it is required to be shown news_update will reshow it.
            widgets[WIDX_MIDDLE_OUTSET].setHidden();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            News::Item* newsItem;

            switch (widgetIndex)
            {
                case WIDX_MIDDLE_INSET:
                    if (News::IsQueueEmpty())
                    {
                        ContextOpenWindow(WindowClass::recentNews);
                    }
                    else
                    {
                        News::CloseCurrentItem();
                    }
                    break;
                case WIDX_NEWS_SUBJECT:
                    newsItem = News::GetItem(0);
                    News::OpenSubject(newsItem->type, newsItem->assoc);
                    break;
                case WIDX_NEWS_LOCATE:
                    if (News::IsQueueEmpty())
                        break;

                    {
                        newsItem = News::GetItem(0);

                        auto subjectLoc = News::GetSubjectLocation(newsItem->type, newsItem->assoc);

                        if (!subjectLoc.has_value())
                            break;

                        WindowBase* mainWindow = WindowGetMain();
                        if (mainWindow != nullptr)
                            WindowScrollToLocation(*mainWindow, subjectLoc.value());
                    }
                    break;
            }
        }

        void onPrepareDraw() override
        {
            // Figure out how much line height we have to work with.
            uint32_t line_height = FontGetLineHeight(FontStyle::medium);

            // Reset dimensions as appropriate -- in case we're switching languages.
            height = line_height * 2 + 12;
            windowPos.y = ContextGetHeight() - height;

            // Change height of widgets in accordance with line height.
            widgets[WIDX_MIDDLE_OUTSET].bottom = line_height * 3 + 3;
            widgets[WIDX_MIDDLE_INSET].bottom = line_height * 3 + 1;

            // Anchor the middle and right panel to the right
            const auto newWidth = ContextGetWidth();
            auto x = newWidth - 1 - 2 - 137 - 2 - 1;
            widgets[WIDX_MIDDLE_OUTSET].right = x;
            x -= 2;
            widgets[WIDX_MIDDLE_INSET].right = x;
            x -= 3;
            widgets[WIDX_NEWS_LOCATE].right = x;
            x -= 23;
            widgets[WIDX_NEWS_LOCATE].left = x;

            if (News::IsQueueEmpty())
            {
                bool useFullToolbar = ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR;
                widgets[WIDX_MIDDLE_OUTSET].setVisible(useFullToolbar);
                widgets[WIDX_MIDDLE_INSET].setVisible(useFullToolbar);
                widgets[WIDX_NEWS_SUBJECT].setHidden();
                widgets[WIDX_NEWS_LOCATE].setHidden();

                if (useFullToolbar)
                {
                    widgets[WIDX_MIDDLE_OUTSET].colour = 0;
                    widgets[WIDX_MIDDLE_INSET].colour = 0;
                }
            }
            else
            {
                News::Item* newsItem = News::GetItem(0);
                widgets[WIDX_MIDDLE_OUTSET].setVisible();
                widgets[WIDX_MIDDLE_INSET].setVisible();
                widgets[WIDX_NEWS_SUBJECT].setVisible();
                widgets[WIDX_NEWS_LOCATE].setVisible();
                widgets[WIDX_MIDDLE_OUTSET].colour = 2;
                widgets[WIDX_MIDDLE_INSET].colour = 2;
                setWidgetDisabled(WIDX_NEWS_SUBJECT, false);
                setWidgetDisabled(WIDX_NEWS_LOCATE, false);

                // Find out if the news item is no longer valid
                auto subjectLoc = News::GetSubjectLocation(newsItem->type, newsItem->assoc);

                if (!subjectLoc.has_value())
                    setWidgetDisabled(WIDX_NEWS_LOCATE, true);

                if (!(newsItem->typeHasSubject()))
                {
                    setWidgetDisabled(WIDX_NEWS_SUBJECT, true);
                    widgets[WIDX_NEWS_SUBJECT].setHidden();
                }

                if (newsItem->hasButton())
                {
                    setWidgetDisabled(WIDX_NEWS_SUBJECT, true);
                    setWidgetDisabled(WIDX_NEWS_LOCATE, true);
                }
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            const auto& middleWidget = widgets[WIDX_MIDDLE_OUTSET];

            if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                // Draw grey background
                auto leftTop = windowPos + ScreenCoordsXY{ middleWidget.left, middleWidget.top };
                auto rightBottom = windowPos + ScreenCoordsXY{ middleWidget.right, middleWidget.bottom };
                Rectangle::filter(rt, { leftTop, rightBottom }, FilterPaletteID::palette51);
            }

            drawWidgets(rt);

            if (!News::IsQueueEmpty())
            {
                DrawNewsItem(rt);
            }
            else if (ThemeGetFlags() & UITHEME_FLAG_USE_FULL_BOTTOM_TOOLBAR)
            {
                DrawMiddlePanel(rt);
            }
        }

        void onUpdate() override
        {
            currentFrame++;
            if (currentFrame >= 24)
                currentFrame = 0;
        }
    };

    /**
     * Creates the main game bottom toolbar window.
     */
    WindowBase* GameBottomToolbarOpen()
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();

        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t toolbarHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<GameBottomToolbar>(
            WindowClass::bottomToolbar, ScreenCoordsXY(0, screenHeight - toolbarHeight), { screenWidth, toolbarHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }

    void WindowGameBottomToolbarInvalidateNewsItem()
    {
        if (gLegacyScene == LegacyScene::playing)
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->InvalidateWidgetByClass(WindowClass::bottomToolbar, WIDX_MIDDLE_OUTSET);
        }
    }
} // namespace OpenRCT2::Ui::Windows
