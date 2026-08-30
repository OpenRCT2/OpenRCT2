/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/drawing/Drawing.String.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/RenderTarget.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/management/NewsItem.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepSpriteIds.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum NewsTickerWidgetIdx : WidgetIndex
    {
        WIDX_PANEL_OUTSET,
        WIDX_PANEL_INSET,
        WIDX_NEWS_SUBJECT,
        WIDX_NEWS_LOCATE,
    };

    // clang-format off
    static constexpr Widget kNewsTickerWidgets[] =
    {
        makeWidget({  0, 0}, {356, 34}, WidgetType::imgBtn,       WindowColour::primary                                               ), // Middle outset panel
        makeWidget({  2, 2}, {352, 30}, WidgetType::hiddenButton, WindowColour::primary                                               ), // Middle inset panel
        makeWidget({  5, 5}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary,          0xFFFFFFFF, STR_SHOW_SUBJECT_TIP  ), // Associated news item window
        makeWidget({327, 5}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_LOCATE), STR_LOCATE_SUBJECT_TIP)  // Scroll to news item target
    };
    // clang-format on

    class NewsTicker final : public Window
    {
    private:
        void drawNewsItem(RenderTarget& rt)
        {
            const auto* newsItem = News::GetItem(0);
            if (newsItem == nullptr)
                return;

            // Current news item
            const auto& middleOutsetWidget = widgets[WIDX_PANEL_OUTSET];
            Rectangle::fillInset(
                rt,

                { windowPos + ScreenCoordsXY{ middleOutsetWidget.left + 1, middleOutsetWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ middleOutsetWidget.right - 1, middleOutsetWidget.bottom - 1 } },
                colours[2], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::light, Rectangle::FillMode::none);

            // Text
            auto screenCoords = windowPos + ScreenCoordsXY{ middleOutsetWidget.midX(), middleOutsetWidget.top + 11 };
            int32_t itemWidth = middleOutsetWidget.width() - 63;
            drawNewsTicker(
                rt, screenCoords, itemWidth, Drawing::Colour::brightGreen, STR_NEWS_ITEM_TEXT, newsItem->text,
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

                    auto peep = getGameState().entities.tryGetEntity<Peep>(EntityId::FromUnderlying(newsItem->assoc));
                    if (peep == nullptr)
                        return;

                    auto clipCoords = ScreenCoordsXY{ 10, 19 };
                    auto* staff = peep->as<Staff>();
                    if (staff != nullptr && staff->isEntertainer())
                    {
                        clipCoords.y += 3;
                    }

                    auto& objManager = GetContext()->GetObjectManager();
                    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->animationObjectIndex);

                    uint32_t image_id_base = animObj->GetPeepAnimation(peep->animationGroup).baseImage;
                    image_id_base += currentFrame & 0xFFFFFFFC;
                    image_id_base++;

                    auto image_id = ImageId(image_id_base, peep->tShirtColour, peep->trousersColour);
                    GfxDrawSprite(clippedRT, image_id, clipCoords);

                    auto* guest = peep->as<Guest>();
                    if (guest == nullptr)
                        return;

                    // There are only 6 walking frames available for each item,
                    // as well as 1 sprite for sitting and 1 for standing still.
                    auto itemFrame = (currentFrame / 4) % 6;

                    if (guest->animationGroup == PeepAnimationGroup::hat)
                    {
                        auto itemOffset = kPeepSpriteHatItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->hatColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->animationGroup == PeepAnimationGroup::balloon)
                    {
                        auto itemOffset = kPeepSpriteBalloonItemStart + 1;
                        auto imageId = ImageId(itemOffset + itemFrame * 4, guest->balloonColour);
                        GfxDrawSprite(clippedRT, imageId, clipCoords);
                        return;
                    }

                    if (guest->animationGroup == PeepAnimationGroup::umbrella)
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

    public:
        NewsTicker()
        {
            setWidgets(kNewsTickerWidgets);

            currentFrame = 0;
            initScrollWidgets();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            const auto* newsItem = News::GetItem(0);
            if (newsItem == nullptr)
                return;

            switch (widgetIndex)
            {
                case WIDX_PANEL_INSET:
                    News::CloseCurrentItem();
                    break;
                case WIDX_NEWS_SUBJECT:
                    News::OpenSubject(newsItem->type, newsItem->assoc);
                    break;
                case WIDX_NEWS_LOCATE:
                {
                    auto subjectLoc = News::GetSubjectLocation(newsItem->type, newsItem->assoc);
                    if (!subjectLoc.has_value())
                        break;

                    WindowBase* mainWindow = WindowGetMain();
                    if (mainWindow != nullptr)
                        WindowScrollToLocation(*mainWindow, subjectLoc.value());

                    break;
                }
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
            widgets[WIDX_PANEL_OUTSET].bottom = line_height * 3 + 3;
            widgets[WIDX_PANEL_INSET].bottom = line_height * 3 + 1;

            // Anchor the middle and right panel to the right
            widgets[WIDX_PANEL_OUTSET].right = width - 1;
            widgets[WIDX_PANEL_INSET].right = width - 3;
            widgets[WIDX_NEWS_LOCATE].right = width - 6;
            widgets[WIDX_NEWS_LOCATE].left = widgets[WIDX_NEWS_LOCATE].right - 24;

            auto* newsItem = News::GetItem(0);
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

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            drawNewsItem(rt);
        }

        void onUpdate() override
        {
            currentFrame++;
            if (currentFrame >= 24)
                currentFrame = 0;

            if (News::IsQueueEmpty())
            {
                auto* windowMgr = GetWindowManager();
                windowMgr->CloseByClass(WindowClass::newsTicker);
            }
        }
    };

    WindowBase* newsTickerOpen()
    {
        // TODO: query ParkInfoPanel, DateInfoPanel
        constexpr auto kPanelWidth = 142;

        // Toolbar width depends on whether we're showing panels besides
        // TODO: read config when applicable
        auto toolbarWidth = ContextGetWidth() - kPanelWidth * 2;

        // Figure out how much line height we have to work with.
        uint32_t lineHeight = FontGetLineHeight(FontStyle::medium);
        int32_t toolbarHeight = lineHeight * 2 + 12;

        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->Create<NewsTicker>(
            WindowClass::newsTicker, ScreenCoordsXY(kPanelWidth, ContextGetHeight() - toolbarHeight),
            { toolbarWidth, toolbarHeight },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noBackground, WindowFlag::noTitleBar });

        return window;
    }

    void newsTickerInvalidateNewsItem()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::newsTicker);
        if (window != nullptr)
            window->invalidate();
        else
            newsTickerOpen();
    }
} // namespace OpenRCT2::Ui::Windows
