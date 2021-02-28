/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Game.h"
#include "../../config/Config.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/TrackDesign.h"
#include "../../sprites.h"
#include "../../world/Banner.h"
#include "../../world/Scenery.h"
#include "../Paint.h"
#include "Paint.TileElement.h"

/** rct2: 0x0098D884 */
// BannerBoundBoxes[rotation][0] is for the pole in the back
// BannerBoundBoxes[rotation][1] is for the pole and the banner in the front
constexpr CoordsXY BannerBoundBoxes[][2] = {
    { { 1, 2 }, { 1, 29 } },
    { { 2, 32 }, { 29, 32 } },
    { { 32, 2 }, { 32, 29 } },
    { { 2, 1 }, { 29, 1 } },
};

/**
 *
 *  rct2: 0x006B9CC4
 */
void banner_paint(paint_session* session, uint8_t direction, int32_t height, const TileElement* tile_element)
{
    rct_drawpixelinfo* dpi = &session->DPI;

    session->InteractionType = ViewportInteractionItem::Banner;

    if (dpi->zoom_level > 1 || gTrackDesignSaveMode || (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return;

    height -= 16;

    auto bannerElement = tile_element->AsBanner();
    if (bannerElement == nullptr)
    {
        return;
    }

    auto banner = bannerElement->GetBanner();
    if (banner == nullptr)
    {
        return;
    }

    auto banner_scenery = get_banner_entry(banner->type);
    if (banner_scenery == nullptr)
    {
        return;
    }

    direction += bannerElement->GetPosition();
    direction &= 3;

    CoordsXYZ boundBoxOffset = CoordsXYZ(BannerBoundBoxes[direction][0], height + 2);

    uint32_t base_id = (direction << 1) + banner_scenery->image;
    uint32_t image_id = base_id;

    if (tile_element->IsGhost()) // if being placed
    {
        session->InteractionType = ViewportInteractionItem::None;
        image_id |= CONSTRUCTION_MARKER;
    }
    else
    {
        image_id |= (banner->colour << 19) | IMAGE_TYPE_REMAP;
    }

    PaintAddImageAsParent(session, image_id, 0, 0, 1, 1, 0x15, height, boundBoxOffset.x, boundBoxOffset.y, boundBoxOffset.z);
    boundBoxOffset.x = BannerBoundBoxes[direction][1].x;
    boundBoxOffset.y = BannerBoundBoxes[direction][1].y;

    image_id++;
    PaintAddImageAsParent(session, image_id, 0, 0, 1, 1, 0x15, height, boundBoxOffset.x, boundBoxOffset.y, boundBoxOffset.z);

    // Opposite direction
    direction = direction_reverse(direction);
    direction--;
    // If text not showing / ghost
    if (direction >= 2 || (tile_element->IsGhost()))
        return;

    uint16_t scrollingMode = banner_scenery->banner.scrolling_mode;
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES)
    {
        return;
    }

    scrollingMode += direction;

    auto ft = Formatter();
    banner->FormatTextTo(ft, /*addColour*/ true);

    if (gConfigGeneral.upper_case_banners)
    {
        format_string_to_upper(gCommonStringFormatBuffer, sizeof(gCommonStringFormatBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
    }
    else
    {
        format_string(gCommonStringFormatBuffer, sizeof(gCommonStringFormatBuffer), STR_BANNER_TEXT_FORMAT, ft.Data());
    }

    uint16_t string_width = gfx_get_string_width(gCommonStringFormatBuffer, FontSpriteBase::TINY);
    uint16_t scroll = (gCurrentTicks / 2) % string_width;
    auto scrollIndex = scrolling_text_setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, COLOUR_BLACK);
    PaintAddImageAsChild(
        session, scrollIndex, 0, 0, 1, 1, 0x15, height + 22, boundBoxOffset.x, boundBoxOffset.y, boundBoxOffset.z);
}
