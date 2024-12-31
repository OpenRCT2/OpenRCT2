/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../GameState.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/Formatting.h"
#include "../../localisation/StringIds.h"
#include "../../object/BannerObject.h"
#include "../../object/ObjectEntryManager.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Banner.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "../../world/tile_element/BannerElement.h"
#include "Paint.TileElement.h"

using namespace OpenRCT2;

// BannerBoundBoxes[rotation][0] is for the pole in the back
// BannerBoundBoxes[rotation][1] is for the pole and the banner in the front
constexpr CoordsXY BannerBoundBoxes[][2] = {
    { { 1, 2 }, { 1, 29 } },
    { { 2, 32 }, { 29, 32 } },
    { { 32, 2 }, { 32, 29 } },
    { { 2, 1 }, { 29, 1 } },
};

static void PaintBannerScrollingText(
    PaintSession& session, const BannerSceneryEntry& bannerEntry, Banner& banner, const BannerElement& bannerElement,
    Direction direction, int32_t height, const CoordsXYZ& bbOffset)
{
    PROFILED_FUNCTION();

    // If text on hidden direction or ghost
    direction = DirectionReverse(direction) - 1;
    if (direction >= 2 || (bannerElement.IsGhost()))
        return;

    auto scrollingMode = bannerEntry.scrolling_mode + (direction & 3);
    if (scrollingMode >= kMaxScrollingTextModes)
    {
        return;
    }

    auto ft = Formatter();
    banner.FormatTextTo(ft, true);

    char text[256];
    if (Config::Get().general.UpperCaseBanners)
    {
        FormatStringToUpper(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    else
    {
        OpenRCT2::FormatStringLegacy(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }

    auto stringWidth = GfxGetStringWidth(text, FontStyle::Tiny);
    auto scroll = stringWidth > 0 ? (GetGameState().CurrentTicks / 2) % stringWidth : 0;
    auto imageId = ScrollingTextSetup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, COLOUR_BLACK);
    PaintAddImageAsChild(session, imageId, { 0, 0, height + 22 }, { bbOffset, { 1, 1, 21 } });
}

void PaintBanner(PaintSession& session, uint8_t direction, int32_t height, const BannerElement& bannerElement)
{
    PROFILED_FUNCTION();

    if (session.DPI.zoom_level > ZoomLevel{ 1 } || gTrackDesignSaveMode
        || (session.ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return;

    auto banner = bannerElement.GetBanner();
    if (banner == nullptr)
    {
        return;
    }

    auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
    if (bannerEntry == nullptr)
    {
        return;
    }

    session.InteractionType = ViewportInteractionItem::Banner;

    height -= 16;

    direction += bannerElement.GetPosition();
    direction &= 3;

    ImageId imageTemplate;
    if (bannerElement.IsGhost())
    {
        session.InteractionType = ViewportInteractionItem::None;
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }
    else if (session.SelectedElement == reinterpret_cast<const TileElement*>(&bannerElement))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::PaletteGhost);
    }
    else
    {
        imageTemplate = ImageId().WithPrimary(banner->colour);
    }

    auto imageIndex = (direction << 1) + bannerEntry->image;
    auto imageId = imageTemplate.WithIndex(imageIndex);
    auto bbOffset = CoordsXYZ(BannerBoundBoxes[direction][0], height + 2);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { bbOffset, { 1, 1, 21 } });

    bbOffset = CoordsXYZ(BannerBoundBoxes[direction][1], height + 2);
    PaintAddImageAsParent(session, imageId.WithIndexOffset(1), { 0, 0, height }, { bbOffset, { 1, 1, 21 } });

    PaintBannerScrollingText(session, *bannerEntry, *banner, bannerElement, direction, height, bbOffset);
}
