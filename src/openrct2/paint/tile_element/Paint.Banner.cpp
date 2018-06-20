/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Paint.h"
#include "../../config/Config.h"
#include "../../Game.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../ride/TrackDesign.h"
#include "Paint.TileElement.h"
#include "../../world/Banner.h"
#include "../../world/Scenery.h"

/** rct2: 0x0098D884 */
// BannerBoundBoxes[rotation][0] is for the pole in the back
// BannerBoundBoxes[rotation][1] is for the pole and the banner in the front
const LocationXY16 BannerBoundBoxes[][2] = {
    {{ 1,  2}, { 1, 29}},
    {{ 2, 32}, {29, 32}},
    {{32,  2}, {32, 29}},
    {{ 2,  1}, {29,  1}},
};

/**
 *
 *  rct2: 0x006B9CC4
 */
void banner_paint(paint_session * session, uint8_t direction, int32_t height, const rct_tile_element * tile_element)
{
    uint16_t boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ;
    rct_drawpixelinfo* dpi = session->DPI;

    session->InteractionType = VIEWPORT_INTERACTION_ITEM_BANNER;

    if (dpi->zoom_level > 1 || gTrackDesignSaveMode || (gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)) return;

    height -= 16;

    rct_scenery_entry* banner_scenery = get_banner_entry(gBanners[tile_element->properties.banner.index].type);

    if (banner_scenery == nullptr) {
        return;
    }

    direction += tile_element->properties.banner.position;
    direction &= 3;

    boundBoxOffsetX = BannerBoundBoxes[direction][0].x;
    boundBoxOffsetY = BannerBoundBoxes[direction][0].y;
    boundBoxOffsetZ = height + 2;

    uint32_t base_id = (direction << 1) + banner_scenery->image;
    uint32_t image_id = base_id;

    if (tile_element->flags & TILE_ELEMENT_FLAG_GHOST) // if being placed
    {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        image_id |= CONSTRUCTION_MARKER;
    }
    else{
        image_id |=
            (gBanners[tile_element->properties.banner.index].colour << 19) |
            IMAGE_TYPE_REMAP;
    }

    sub_98197C(session, image_id, 0, 0, 1, 1, 0x15, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
    boundBoxOffsetX = BannerBoundBoxes[direction][1].x;
    boundBoxOffsetY = BannerBoundBoxes[direction][1].y;

    image_id++;
    sub_98197C(session, image_id, 0, 0, 1, 1, 0x15, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);

    // Opposite direction
    direction ^= 2;
    direction--;
    // If text not showing / ghost
    if (direction >= 2 || (tile_element->flags & TILE_ELEMENT_FLAG_GHOST)) return;

    uint16_t scrollingMode = banner_scenery->banner.scrolling_mode;
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES) {
        return;
    }

    scrollingMode += direction;

    set_format_arg(0, uint32_t, 0);
    set_format_arg(4, uint32_t, 0);

    rct_string_id string_id = STR_NO_ENTRY;
    if (!(gBanners[tile_element->properties.banner.index].flags & BANNER_FLAG_NO_ENTRY))
    {
        set_format_arg(0, rct_string_id, gBanners[tile_element->properties.banner.index].string_idx);
        string_id = STR_BANNER_TEXT_FORMAT;
    }
    if (gConfigGeneral.upper_case_banners) {
        format_string_to_upper(gCommonStringFormatBuffer, 256, string_id, gCommonFormatArgs);
    } else {
        format_string(gCommonStringFormatBuffer, 256, string_id, gCommonFormatArgs);
    }

    gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

    uint16_t string_width = gfx_get_string_width(gCommonStringFormatBuffer);
    uint16_t scroll = (gCurrentTicks / 2) % string_width;

    sub_98199C(
        session, scrolling_text_setup(session, string_id, scroll, scrollingMode), 0, 0, 1, 1, 0x15, height + 22,
        boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
}
