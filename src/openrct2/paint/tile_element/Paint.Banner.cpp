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
void banner_paint(paint_session * session, uint8 direction, sint32 height, const rct_tile_element * tile_element)
{
    uint16 boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ;
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

    uint32 base_id = (direction << 1) + banner_scenery->image;
    uint32 image_id = base_id;

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

    uint16 scrollingMode = banner_scenery->banner.scrolling_mode;
    if (scrollingMode >= MAX_SCROLLING_TEXT_MODES) {
        return;
    }

    scrollingMode += direction;

    set_format_arg(0, uint32, 0);
    set_format_arg(4, uint32, 0);

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

    uint16 string_width = gfx_get_string_width(gCommonStringFormatBuffer);
    uint16 scroll = (gCurrentTicks / 2) % string_width;

    sub_98199C(
        session, scrolling_text_setup(session, string_id, scroll, scrollingMode), 0, 0, 1, 1, 0x15, height + 22,
        boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
}
