/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Formatter.h"
#include "../../localisation/Localisation.h"
#include "../../object/BannerObject.h"
#include "../../profiling/Profiling.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/Banner.h"
#include "../../world/Scenery.h"
#include "../../world/TileInspector.h"
#include "Paint.TileElement.h"

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
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES)
    {
        return;
    }

    auto ft = Formatter();
    banner.FormatTextTo(ft, true);

    char text[256];
    if (gConfigGeneral.UpperCaseBanners)
    {
        format_string_to_upper(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    else
    {
        format_string(text, sizeof(text), STR_BANNER_TEXT_FORMAT, ft.Data());
    }

    auto stringWidth = gfx_get_string_width(text, FontStyle::Tiny);
    auto scroll = (gCurrentTicks / 2) % stringWidth;
    auto imageId = scrolling_text_setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, COLOUR_BLACK);
    PaintAddImageAsChild(session, imageId, { 0, 0, height + 22 }, { 1, 1, 21 }, bbOffset);
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

    auto* bannerEntry = GetBannerEntry(banner->type);
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
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
    }
    else if (OpenRCT2::TileInspector::IsElementSelected(reinterpret_cast<const TileElement*>(&bannerElement)))
    {
        imageTemplate = ImageId().WithRemap(FilterPaletteID::Palette44);
    }
    else
    {
        imageTemplate = ImageId().WithPrimary(banner->colour);
    }

    auto imageIndex = (direction << 1) + bannerEntry->image;
    auto imageId = imageTemplate.WithIndex(imageIndex);
    auto bbOffset = CoordsXYZ(BannerBoundBoxes[direction][0], height + 2);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, { 1, 1, 21 }, bbOffset);

    bbOffset = CoordsXYZ(BannerBoundBoxes[direction][1], height + 2);
    PaintAddImageAsParent(session, imageId.WithIndexOffset(1), { 0, 0, height }, { 1, 1, 21 }, bbOffset);

    PaintBannerScrollingText(session, *bannerEntry, *banner, bannerElement, direction, height, bbOffset);
}
