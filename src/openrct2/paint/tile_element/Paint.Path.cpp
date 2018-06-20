/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../config/Config.h"
#include "../../Game.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../object/ObjectList.h"
#include "../../peep/Staff.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../ride/TrackPaint.h"
#include "../../world/Footpath.h"
#include "../../world/Scenery.h"
#include "../../world/Sprite.h"
#include "../../world/Surface.h"
#include "../Paint.h"
#include "../Supports.h"
#include "Paint.TileElement.h"
#include "Paint.Surface.h"
#include "../../world/Map.h"
#include "../../drawing/LightFX.h"

bool gPaintWidePathsAsGhost = false;

// clang-format off
const uint8_t byte_98D800[] = {
    12, 9, 3, 6
};

static constexpr const uint8_t byte_98D6E0[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    0, 1, 2, 20, 4, 5, 6, 22, 8, 9, 10, 26, 12, 13, 14, 36,
    0, 1, 2, 3, 4, 5, 21, 23, 8, 9, 10, 11, 12, 13, 33, 37,
    0, 1, 2, 3, 4, 5, 6, 24, 8, 9, 10, 11, 12, 13, 14, 38,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 29, 30, 34, 39,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 40,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 35, 41,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 42,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 25, 10, 27, 12, 31, 14, 43,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 28, 12, 13, 14, 44,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 45,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 46,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 32, 14, 47,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 48,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 49,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 50
};

static constexpr const int16_t stru_98D804[][4] = {
    {3, 3, 26, 26},
    {0, 3, 29, 26},
    {3, 3, 26, 29},
    {0, 3, 29, 29},
    {3, 3, 29, 26},
    {0, 3, 32, 26},
    {3, 3, 29, 29},
    {0, 3, 32, 29},
    {3, 0, 26, 29},
    {0, 0, 29, 29},
    {3, 0, 26, 32},
    {0, 0, 29, 32},
    {3, 0, 29, 29},
    {0, 0, 32, 29},
    {3, 0, 29, 32},
    {0, 0, 32, 32},
};

static constexpr const uint8_t byte_98D8A4[] = {
    0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0
};
// clang-format on

void path_paint_box_support(
    paint_session *          session,
    const rct_tile_element * tileElement,
    int32_t                   height,
    rct_footpath_entry *     footpathEntry,
    bool                     hasFences,
    uint32_t                   imageFlags,
    uint32_t                   sceneryImageFlags);
void path_paint_pole_support(
    paint_session *          session,
    const rct_tile_element * tileElement,
    int16_t                   height,
    rct_footpath_entry *     footpathEntry,
    bool                     hasFences,
    uint32_t                   imageFlags,
    uint32_t                   sceneryImageFlags);

/* rct2: 0x006A5AE5 */
static void path_bit_lights_paint(
    paint_session *          session,
    rct_scenery_entry *      pathBitEntry,
    const rct_tile_element * tileElement,
    int32_t                   height,
    uint8_t                    edges,
    uint32_t                   pathBitImageFlags)
{
    if (footpath_element_is_sloped(tileElement))
        height += 8;

    uint32_t imageId;

    if (!(edges & EDGE_NE)) {
        imageId = pathBitEntry->image + 1;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 2, 16, 1, 1, 23, height, 3, 16, height + 2);
    }
    if (!(edges & EDGE_SE)) {
        imageId = pathBitEntry->image + 2;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 30, 1, 0, 23, height, 16, 29, height + 2);
    }

    if (!(edges & EDGE_SW)) {
        imageId = pathBitEntry->image + 3;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 30, 16, 0, 1, 23, height, 29, 16, height + 2);
    }

    if (!(edges & EDGE_NW)) {
        imageId = pathBitEntry->image + 4;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 2, 1, 1, 23, height, 16, 3, height + 2);
    }
}

/* rct2: 0x006A5C94 */
static void path_bit_bins_paint(
    paint_session *          session,
    rct_scenery_entry *      pathBitEntry,
    const rct_tile_element * tileElement,
    int32_t                   height,
    uint8_t                    edges,
    uint32_t                   pathBitImageFlags)
{
    if (footpath_element_is_sloped(tileElement))
        height += 8;

    uint32_t imageId;
    bool binsAreVandalised = tileElement->flags & TILE_ELEMENT_FLAG_BROKEN;

    if (!(edges & EDGE_NE)) {
        imageId = pathBitEntry->image + 5;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->properties.path.addition_status & ror8(0x3, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 7, 16, 1, 1, 7, height, 7, 16, height + 2);
    }
    if (!(edges & EDGE_SE)) {
        imageId = pathBitEntry->image + 6;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->properties.path.addition_status & ror8(0xC, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 16, 25, 1, 1, 7, height, 16, 25, height + 2);
    }

    if (!(edges & EDGE_SW)) {
        imageId = pathBitEntry->image + 7;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->properties.path.addition_status & ror8(0x30, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 25, 16, 1, 1, 7, height, 25, 16, height + 2);
    }

    if (!(edges & EDGE_NW)) {
        imageId = pathBitEntry->image + 8;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised) {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->properties.path.addition_status & ror8(0xC0, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 16, 7, 1, 1, 7, height, 16, 7, height + 2);
    }
}

/* rct2: 0x006A5E81 */
static void path_bit_benches_paint(
    paint_session *          session,
    rct_scenery_entry *      pathBitEntry,
    const rct_tile_element * tileElement,
    int32_t                   height,
    uint8_t                    edges,
    uint32_t                   pathBitImageFlags)
{
    uint32_t imageId;

    if (!(edges & EDGE_NE)) {
        imageId = pathBitEntry->image + 1;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 7, 16, 0, 16, 7, height, 6, 8, height + 2);
    }
    if (!(edges & EDGE_SE)) {
        imageId = pathBitEntry->image + 2;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 25, 16, 0, 7, height, 8, 23, height + 2);
    }

    if (!(edges & EDGE_SW)) {
        imageId = pathBitEntry->image + 3;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 25, 16, 0, 16, 7, height, 23, 8, height + 2);
    }

    if (!(edges & EDGE_NW)) {
        imageId = pathBitEntry->image + 4;

        if (tileElement->flags & TILE_ELEMENT_FLAG_BROKEN)
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 7, 16, 0, 7, height, 8, 6, height + 2);
    }
}

/* rct2: 0x006A6008 */
static void path_bit_jumping_fountains_paint(
    paint_session *          session,
    rct_scenery_entry *      pathBitEntry,
    int32_t                   height,
    uint32_t                   pathBitImageFlags,
    rct_drawpixelinfo *      dpi)
{
    if (dpi->zoom_level != 0)
        return;

    uint32_t imageId = pathBitEntry->image;
    imageId |= pathBitImageFlags;

    sub_98197C(session, imageId + 1, 0, 0, 1, 1, 2, height, 3, 3, height + 2);
    sub_98197C(session, imageId + 2, 0, 0, 1, 1, 2, height, 3, 29, height + 2);
    sub_98197C(session, imageId + 3, 0, 0, 1, 1, 2, height, 29, 29, height + 2);
    sub_98197C(session, imageId + 4, 0, 0, 1, 1, 2, height, 29, 3, height + 2);
}

/**
 * rct2: 0x006A4101
 * @param tile_element (esi)
 */
static void sub_6A4101(
    paint_session *          session,
    const rct_tile_element * tile_element,
    uint16_t                   height,
    uint32_t                   ebp,
    bool                     word_F3F038,
    rct_footpath_entry *     footpathEntry,
    uint32_t                   base_image_id,
    uint32_t                   imageFlags)
{
    if (footpath_element_is_queue(tile_element)) {
        uint8_t local_ebp = ebp & 0x0F;
        if (footpath_element_is_sloped(tile_element)) {
            switch ((footpath_element_get_slope_direction(tile_element) + session->CurrentRotation) &
                    FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
            {
                case 0:
                    sub_98197C(session, 95 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                    sub_98197C(session, 95 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                    break;
                case 1:
                    sub_98197C(session, 94 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                    sub_98197C(session, 94 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                    break;
                case 2:
                    sub_98197C(session, 96 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                    sub_98197C(session, 96 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                    break;
                case 3:
                    sub_98197C(session, 93 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                    sub_98197C(session, 93 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                    break;
            }
        } else {
            switch (local_ebp) {
                case 1:
                    sub_98197C(session, 90 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 90 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    break;
                case 2:
                    sub_98197C(session, 91 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 91 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    break;
                case 3:
                    sub_98197C(session, 90 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(
                        session, 91 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4,
                        height + 2); // bound_box_offset_y seems to be a bug
                    sub_98197C(session, 98 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                    break;
                case 4:
                    sub_98197C(session, 92 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 92 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    break;
                case 5:
                    sub_98197C(session, 88 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 88 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                    break;
                case 6:
                    sub_98197C(session, 91 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 92 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 99 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                    break;
                case 8:
                    sub_98197C(session, 89 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 89 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    break;
                case 9:
                    sub_98197C(session, 89 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    sub_98197C(session, 90 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    sub_98197C(session, 97 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                    break;
                case 10:
                    sub_98197C(session, 87 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 87 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                    break;
                case 12:
                    sub_98197C(session, 89 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(
                        session, 92 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28,
                        height + 2); // bound_box_offset_x seems to be a bug
                    sub_98197C(session, 100 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                    break;
                default:
                    // purposely left empty
                    break;
            }
        }

        if (!footpath_element_has_queue_banner(tile_element)) {
            return;
        }

        uint8_t direction = footpath_element_get_direction(tile_element);
        // Draw ride sign
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        if (footpath_element_is_sloped(tile_element)) {
            if (footpath_element_get_slope_direction(tile_element) == direction)
                height += 16;
        }
        direction += session->CurrentRotation;
        direction &= 3;

        LocationXYZ16 boundBoxOffsets = {
            BannerBoundBoxes[direction][0].x,
            BannerBoundBoxes[direction][0].y,
            static_cast<int16_t>(height + 2)
        };

        uint32_t imageId = (direction << 1) + base_image_id + 101;

        // Draw pole in the back
        sub_98197C(session, imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);

        // Draw pole in the front and banner
        boundBoxOffsets.x = BannerBoundBoxes[direction][1].x;
        boundBoxOffsets.y = BannerBoundBoxes[direction][1].y;
        imageId++;
        sub_98197C(session, imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);

        direction--;
        // If text shown
        if (direction < 2 && tile_element->properties.path.ride_index != RIDE_ID_NULL && imageFlags == 0) {
            uint16_t scrollingMode = footpathEntry->scrolling_mode;
            scrollingMode += direction;

            set_format_arg(0, uint32_t, 0);
            set_format_arg(4, uint32_t, 0);

            Ride* ride = get_ride(tile_element->properties.path.ride_index);
            rct_string_id string_id = STR_RIDE_ENTRANCE_CLOSED;
            if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)){
                set_format_arg(0, rct_string_id, ride->name);
                set_format_arg(2, uint32_t, ride->name_arguments);
                string_id = STR_RIDE_ENTRANCE_NAME;
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
                session, scrolling_text_setup(session, string_id, scroll, scrollingMode), 0, 0, 1, 1, 21, height + 7,
                boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);
        }

        session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
        if (imageFlags != 0) {
            session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        }
        return;
    }


    // save ecx, ebp, esi
    uint32_t dword_F3EF80 = ebp;
    if (!(footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
        dword_F3EF80 &= 0x0F;
    }

    if (footpath_element_is_sloped(tile_element)) {
        switch ((footpath_element_get_slope_direction(tile_element) + session->CurrentRotation) &
                FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
        {
            case 0:
                sub_98197C(session, 81 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                sub_98197C(session, 81 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                break;
            case 1:
                sub_98197C(session, 80 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                sub_98197C(session, 80 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                break;
            case 2:
                sub_98197C(session, 82 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                sub_98197C(session, 82 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                break;
            case 3:
                sub_98197C(session, 79 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                sub_98197C(session, 79 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                break;
        }
    } else {
        if (!word_F3F038) {
            return;
        }

        uint8_t local_ebp = ebp & 0x0F;
        switch (local_ebp) {
            case 0:
                // purposely left empty
                break;
            case 1:
                sub_98197C(session, 76 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 76 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                break;
            case 2:
                sub_98197C(session, 77 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 77 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                break;
            case 4:
                sub_98197C(session, 78 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 78 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                break;
            case 5:
                sub_98197C(session, 74 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 74 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                break;
            case 8:
                sub_98197C(session, 75 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 75 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                break;
            case 10:
                sub_98197C(session, 73 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                sub_98197C(session, 73 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                break;

            case 3:
                sub_98197C(session, 76 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(
                    session, 77 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4,
                    height + 2); // bound_box_offset_y seems to be a bug
                if (!(dword_F3EF80 & 0x10)) {
                    sub_98197C(session, 84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                break;
            case 6:
                sub_98197C(session, 77 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 78 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                if (!(dword_F3EF80 & 0x20)) {
                    sub_98197C(session, 85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                break;
            case 9:
                sub_98197C(session, 75 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                sub_98197C(session, 76 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                if (!(dword_F3EF80 & 0x80)) {
                    sub_98197C(session, 83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;
            case 12:
                sub_98197C(session, 75 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(
                    session, 78 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28,
                    height + 2); // bound_box_offset_x seems to be a bug
                if (!(dword_F3EF80 & 0x40)) {
                    sub_98197C(session, 86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                break;

            case 7:
                sub_98197C(session, 74 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                if (!(dword_F3EF80 & 0x10)) {
                    sub_98197C(session, 84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(dword_F3EF80 & 0x20)) {
                    sub_98197C(session, 85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                break;
            case 13:
                sub_98197C(session, 74 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                if (!(dword_F3EF80 & 0x40)) {
                    sub_98197C(session, 86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                if (!(dword_F3EF80 & 0x80)) {
                    sub_98197C(session, 83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;
            case 14:
                sub_98197C(session, 73 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                if (!(dword_F3EF80 & 0x20)) {
                    sub_98197C(session, 85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                if (!(dword_F3EF80 & 0x40)) {
                    sub_98197C(session, 86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                break;
            case 11:
                sub_98197C(session, 73 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                if (!(dword_F3EF80 & 0x10)) {
                    sub_98197C(session, 84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(dword_F3EF80 & 0x80)) {
                    sub_98197C(session, 83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;

            case 15:
                if (!(dword_F3EF80 & 0x10)) {
                    sub_98197C(session, 84 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(dword_F3EF80 & 0x20)) {
                    sub_98197C(session, 85 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                if (!(dword_F3EF80 & 0x40)) {
                    sub_98197C(session, 86 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                if (!(dword_F3EF80 & 0x80)) {
                    sub_98197C(session, 83 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;


        }
    }

}

/**
 * rct2: 0x006A3F61
 * @param tile_element (esp[0])
 * @param connectedEdges (bp) (relative to the camera's rotation)
 * @param height (dx)
 * @param footpathEntry (0x00F3EF6C)
 * @param imageFlags (0x00F3EF70)
 * @param sceneryImageFlags (0x00F3EF74)
 */
static void sub_6A3F61(
    paint_session *          session,
    const rct_tile_element * tile_element,
    uint16_t                   connectedEdges,
    uint16_t                   height,
    rct_footpath_entry *     footpathEntry,
    uint32_t                   imageFlags,
    uint32_t                   sceneryImageFlags,
    bool                     word_F3F038)
{
    // eax --
    // ebx --
    // ecx
    // edx
    // esi --
    // edi --
    // ebp
    // esp: [ esi, ???, 000]

    // Probably drawing benches etc.

    rct_drawpixelinfo * dpi = session->DPI;

    if (dpi->zoom_level <= 1) {
        if (!gTrackDesignSaveMode) {
            if (footpath_element_has_path_scenery(tile_element)) {
                session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM;
                if (sceneryImageFlags != 0) {
                    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                }

                // Draw additional path bits (bins, benches, lamps, queue screens)
                rct_scenery_entry* sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));

                if ((gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) &&
                    !(tile_element->flags & TILE_ELEMENT_FLAG_BROKEN) &&
                    !(sceneryEntry->path_bit.draw_type == PATH_BIT_DRAW_TYPE_BINS))
                {
                    return;
                }

                switch (sceneryEntry->path_bit.draw_type) {
                case PATH_BIT_DRAW_TYPE_LIGHTS:
                    path_bit_lights_paint(session, sceneryEntry, tile_element, height, (uint8_t)connectedEdges, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_BINS:
                    path_bit_bins_paint(session, sceneryEntry, tile_element, height, (uint8_t)connectedEdges, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_BENCHES:
                    path_bit_benches_paint(session, sceneryEntry, tile_element, height, (uint8_t)connectedEdges, sceneryImageFlags);
                    break;
                case PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS:
                    path_bit_jumping_fountains_paint(session, sceneryEntry, height, sceneryImageFlags, dpi);
                    break;
                }

                session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

                if (sceneryImageFlags != 0) {
                    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                }
            }
        }

        // Redundant zoom-level check removed

        sub_6A4101(session, tile_element, height, connectedEdges, word_F3F038, footpathEntry, footpathEntry->image | imageFlags, imageFlags);
    }

    // This is about tunnel drawing
    uint8_t direction = (footpath_element_get_slope_direction(tile_element) + session->CurrentRotation) &
                      FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    bool  sloped    = footpath_element_is_sloped(tile_element);

    if (connectedEdges & EDGE_SE) {
        // Bottom right of tile is a tunnel
        if (sloped && direction == EDGE_NE) {
            // Path going down into the tunnel
            paint_util_push_tunnel_right(session, height + 16, TUNNEL_10);
        } else if (connectedEdges & EDGE_NE) {
            // Flat path with edge to the right (north-east)
            paint_util_push_tunnel_right(session, height, TUNNEL_11);
        } else {
            // Path going up, or flat and not connected to the right
            paint_util_push_tunnel_right(session, height, TUNNEL_10);
        }
    }

    if (!(connectedEdges & EDGE_SW)) {
        return;
    }

    // Bottom left of the tile is a tunnel
    if (sloped && direction == EDGE_SE) {
        // Path going down into the tunnel
        paint_util_push_tunnel_left(session, height + 16, TUNNEL_10);
    } else if (connectedEdges & EDGE_NW) {
        // Flat path with edge to the left (north-west)
        paint_util_push_tunnel_left(session, height , TUNNEL_11);
    } else {
        // Path going up, or flat and not connected to the left
        paint_util_push_tunnel_left(session, height , TUNNEL_10);
    }
}

/**
 * rct2: 0x0006A3590
 */
void path_paint(paint_session * session, uint16_t height, const rct_tile_element * tile_element)
{
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

    bool word_F3F038 = false;

    uint32_t sceneryImageFlags = 0;
    uint32_t imageFlags = 0;

    if (gTrackDesignSaveMode) {
        if (footpath_element_is_queue(tile_element)) {
            if (tile_element->properties.path.ride_index != gTrackDesignSaveRideIndex) {
                return;
            }
        }

        if (!track_design_save_contains_tile_element(tile_element)) {
            imageFlags = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
        }
    }

    if (gCurrentViewportFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        imageFlags = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
    }

    if (footpath_element_path_scenery_is_ghost(tile_element)) {
        sceneryImageFlags = CONSTRUCTION_MARKER;
    }

    if (tile_element->flags & TILE_ELEMENT_FLAG_GHOST) {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        imageFlags = CONSTRUCTION_MARKER;
    }

    // For debugging purpose, show blocked tiles with a colour
    if (gPaintBlockedTiles && (tile_element->flags & TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE))
    {
        imageFlags = COLOUR_BRIGHT_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP;
    }

    // Draw wide flags as ghosts, leaving only the "walkable" paths to be drawn normally
    if (gPaintWidePathsAsGhost && footpath_element_is_wide(tile_element))
    {
        imageFlags &= 0x7FFFF;
        imageFlags |= CONSTRUCTION_MARKER;
    }

    int16_t x = session->MapPosition.x, y = session->MapPosition.y;

    rct_tile_element * surface = map_get_surface_element_at({session->MapPosition.x, session->MapPosition.y});

    uint16_t bl = height / 8;
    if (surface == nullptr) {
        word_F3F038 = true;
    } else if (surface->base_height != bl) {
        word_F3F038 = true;
    } else {
        if (footpath_element_is_sloped(tile_element)) {
            // Diagonal path

            if ((surface->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK) != byte_98D800[footpath_element_get_slope_direction(tile_element)]) {
                word_F3F038 = true;
            }
        } else {
            if (surface->properties.surface.slope & TILE_ELEMENT_SURFACE_SLOPE_MASK) {
                word_F3F038 = true;
            }
        }
    }

    if (gStaffDrawPatrolAreas != 0xFFFF) {
        int32_t staffIndex = gStaffDrawPatrolAreas;
        uint8_t staffType = staffIndex & 0x7FFF;
        bool is_staff_list = staffIndex & 0x8000;
        x = session->MapPosition.x;
        y = session->MapPosition.y;

        uint8_t patrolColour = COLOUR_LIGHT_BLUE;

        if (!is_staff_list) {
            rct_peep * staff = GET_PEEP(staffIndex);
            if (!staff_is_patrol_area_set(staff->staff_id, x, y)) {
                patrolColour = COLOUR_GREY;
            }
            staffType = staff->staff_type;
        }

        if (staff_is_patrol_area_set(200 + staffType, x, y)) {
            uint32_t imageId = 2618;
            int32_t height2 = tile_element->base_height * 8;
            if (footpath_element_is_sloped(tile_element)) {
                imageId = 2619 + ((footpath_element_get_slope_direction(tile_element) + session->CurrentRotation) & 3);
                height2 += 16;
            }

            sub_98196C(session, imageId | patrolColour << 19 | IMAGE_TYPE_REMAP, 16, 16, 1, 1, 0, height2 + 2);
        }
    }


    if (gCurrentViewportFlags & VIEWPORT_FLAG_PATH_HEIGHTS) {
        uint16_t height2 = 3 + tile_element->base_height * 8;
        if (footpath_element_is_sloped(tile_element)) {
            height2 += 8;
        }
        uint32_t imageId = (SPR_HEIGHT_MARKER_BASE + height2 / 16) | COLOUR_GREY << 19 | IMAGE_TYPE_REMAP;
        imageId += get_height_marker_offset();
        imageId -= gMapBaseZ;
        sub_98196C(session, imageId, 16, 16, 1, 1, 0, height2);
    }

    uint8_t pathType = footpath_element_get_type(tile_element);
    rct_footpath_entry * footpathEntry = get_footpath_entry(pathType);

    if (footpathEntry != nullptr) {
        if (footpathEntry->support_type == FOOTPATH_ENTRY_SUPPORT_TYPE_POLE) {
            path_paint_pole_support(session, tile_element, height, footpathEntry, word_F3F038, imageFlags, sceneryImageFlags);
        }
        else {
            path_paint_box_support(session, tile_element, height, footpathEntry, word_F3F038, imageFlags, sceneryImageFlags);
        }
    }

#ifdef __ENABLE_LIGHTFX__
    if (lightfx_is_available()) {
        if (footpath_element_has_path_scenery(tile_element) && !(tile_element->flags & TILE_ELEMENT_FLAG_BROKEN)) {
            rct_scenery_entry *sceneryEntry = get_footpath_item_entry(footpath_element_get_path_scenery_index(tile_element));
            if (sceneryEntry->path_bit.flags & PATH_BIT_FLAG_LAMP) {
                if (!(tile_element->properties.path.edges & EDGE_NE)) {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, -16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(tile_element->properties.path.edges & EDGE_SE)) {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, 16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(tile_element->properties.path.edges & EDGE_SW)) {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 16, 0, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
                if (!(tile_element->properties.path.edges & EDGE_NW)) {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, -16, height + 23, LIGHTFX_LIGHT_TYPE_LANTERN_3);
                }
            }
        }
    }
#endif
}

void path_paint_box_support(
    paint_session *          session,
    const rct_tile_element * tileElement,
    int32_t                   height,
    rct_footpath_entry *     footpathEntry,
    bool                     hasFences,
    uint32_t                   imageFlags,
    uint32_t                   sceneryImageFlags)
{
    // Rol edges around rotation
    uint8_t edges = ((tileElement->properties.path.edges << session->CurrentRotation) & 0xF) |
                  (((tileElement->properties.path.edges & 0xF) << session->CurrentRotation) >> 4);

    uint8_t corners = (((tileElement->properties.path.edges >> 4) << session->CurrentRotation) & 0xF) |
                    (((tileElement->properties.path.edges >> 4) << session->CurrentRotation) >> 4);

    LocationXY16 boundBoxOffset = {stru_98D804[edges][0], stru_98D804[edges][1]};
    LocationXY16 boundBoxSize = {stru_98D804[edges][2], stru_98D804[edges][3]};

    uint16_t edi = edges | (corners << 4);

    uint32_t imageId;
    if (footpath_element_is_sloped(tileElement)) {
        imageId = ((footpath_element_get_slope_direction(tileElement) + session->CurrentRotation) &
                   FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) +
                  16;
    } else {
        imageId = byte_98D6E0[edi];
    }

    imageId += footpathEntry->image;
    if (footpath_element_is_queue(tileElement)) {
        imageId += 51;
    }

    if (!session->DidPassSurface) {
        boundBoxOffset.x = 3;
        boundBoxOffset.y = 3;
        boundBoxSize.x = 26;
        boundBoxSize.y = 26;
    }

    // By default, add 1 to the z bounding box to always clip above the surface
    uint8_t boundingBoxZOffset = 1;

    // If we are on the same tile as a straight track, add the offset 2 so we
    //  can clip above gravel part of the track sprite
    if (session->TrackElementOnSameHeight)
    {
        if (session->TrackElementOnSameHeight->properties.track.type == TRACK_ELEM_FLAT)
        {
            boundingBoxZOffset = 2;
        }
    }

    if (!hasFences || !session->DidPassSurface) {
        sub_98197C(
            session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);
    } else {
        uint32_t image_id;
        if (footpath_element_is_sloped(tileElement)) {
            image_id = ((footpath_element_get_slope_direction(tileElement) + session->CurrentRotation) &
                        FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) +
                       footpathEntry->bridge_image + 51;
        } else {
            image_id = byte_98D8A4[edges] + footpathEntry->bridge_image + 49;
        }

        sub_98197C(
            session, image_id | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);

        if (!footpath_element_is_queue(tileElement) && !(footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
            // don't draw
        } else {
            sub_98199C(
                session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
                boundBoxOffset.y, height + boundingBoxZOffset);
        }
    }


    sub_6A3F61(session, tileElement, edi, height, footpathEntry, imageFlags, sceneryImageFlags, hasFences);

    uint16_t ax = 0;
    if (footpath_element_is_sloped(tileElement)) {
        ax = ((footpath_element_get_slope_direction(tileElement) + session->CurrentRotation) & 0x3) + 1;
    }

    if (byte_98D8A4[edges] == 0) {
        path_a_supports_paint_setup(session, 0, ax, height, imageFlags, footpathEntry, nullptr);
    } else {
        path_a_supports_paint_setup(session, 1, ax, height, imageFlags, footpathEntry, nullptr);
    }

    height += 32;
    if (footpath_element_is_sloped(tileElement)) {
        height += 16;
    }

    paint_util_set_general_support_height(session, height, 0x20);

    if (footpath_element_is_queue(tileElement)
        || (tileElement->properties.path.edges != 0xFF && hasFences)
        ) {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        return;
    }

    if (tileElement->properties.path.edges == 0xFF) {
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, 0xFFFF, 0);
        return;
    }

    paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);

    if (edges & 1) {
        paint_util_set_segment_support_height(session, SEGMENT_CC, 0xFFFF, 0);
    }

    if (edges & 2) {
        paint_util_set_segment_support_height(session, SEGMENT_D4, 0xFFFF, 0);
    }

    if (edges & 4) {
        paint_util_set_segment_support_height(session, SEGMENT_D0, 0xFFFF, 0);
    }

    if (edges & 8) {
        paint_util_set_segment_support_height(session, SEGMENT_C8, 0xFFFF, 0);
    }
}

void path_paint_pole_support(
    paint_session *          session,
    const rct_tile_element * tileElement,
    int16_t                   height,
    rct_footpath_entry *     footpathEntry,
    bool                     hasFences,
    uint32_t                   imageFlags,
    uint32_t                   sceneryImageFlags)
{
    // Rol edges around rotation
    uint8_t edges = ((tileElement->properties.path.edges << session->CurrentRotation) & 0xF) |
                  (((tileElement->properties.path.edges & 0xF) << session->CurrentRotation) >> 4);

    LocationXY16 boundBoxOffset = {
        stru_98D804[edges][0],
        stru_98D804[edges][1]
    };

    LocationXY16 boundBoxSize = {
        stru_98D804[edges][2],
        stru_98D804[edges][3]
    };

    uint8_t corners = (((tileElement->properties.path.edges >> 4) << session->CurrentRotation) & 0xF) |
                    (((tileElement->properties.path.edges >> 4) << session->CurrentRotation) >> 4);

    uint16_t edi = edges | (corners << 4);

    uint32_t imageId;
    if (footpath_element_is_sloped(tileElement)) {
        imageId = ((footpath_element_get_slope_direction(tileElement) + session->CurrentRotation) & 3) + 16;
    }
    else {
        imageId = byte_98D6E0[edi];
    }


    imageId += footpathEntry->image;
    if (footpath_element_is_queue(tileElement)) {
        imageId += 51;
    }

    // Below Surface
    if (!session->DidPassSurface) {
        boundBoxOffset.x = 3;
        boundBoxOffset.y = 3;
        boundBoxSize.x = 26;
        boundBoxSize.y = 26;
    }

    // By default, add 1 to the z bounding box to always clip above the surface
    uint8_t boundingBoxZOffset = 1;

    // If we are on the same tile as a straight track, add the offset 2 so we
    //  can clip above gravel part of the track sprite
    if (session->TrackElementOnSameHeight)
    {
        if (session->TrackElementOnSameHeight->properties.track.type == TRACK_ELEM_FLAT)
        {
            boundingBoxZOffset = 2;
        }
    }

    if (!hasFences || !session->DidPassSurface) {
        sub_98197C(
            session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);
    }
    else {
        uint32_t bridgeImage;
        if (footpath_element_is_sloped(tileElement)) {
            bridgeImage = ((footpath_element_get_slope_direction(tileElement) + session->CurrentRotation) &
                           FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK) +
                          footpathEntry->bridge_image + 16;
        }
        else {
            bridgeImage = edges + footpathEntry->bridge_image;
            bridgeImage |= imageFlags;
        }

        sub_98197C(
            session, bridgeImage | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
            boundBoxOffset.y, height + boundingBoxZOffset);

        if (footpath_element_is_queue(tileElement) || (footpathEntry->flags & FOOTPATH_ENTRY_FLAG_HAS_PATH_BASE_SPRITE)) {
            sub_98199C(
                session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
                boundBoxOffset.y, height + boundingBoxZOffset);
        }
    }

    sub_6A3F61(session, tileElement, edi, height, footpathEntry, imageFlags, sceneryImageFlags, hasFences); // TODO: arguments

    uint16_t ax = 0;
    if (footpath_element_is_sloped(tileElement)) {
        ax = 8;
    }

    uint8_t supports[] = {
        6,
        8,
        7,
        5
    };

    for (int8_t i = 3; i > -1; --i) {
        if (!(edges & (1 << i))) {
            path_b_supports_paint_setup(session, supports[i], ax, height, imageFlags, footpathEntry);
        }
    }

    height += 32;
    if (footpath_element_is_sloped(tileElement)) {
        height += 16;
    }

    paint_util_set_general_support_height(session, height, 0x20);

    if (footpath_element_is_queue(tileElement)
        || (tileElement->properties.path.edges != 0xFF && hasFences)
        ) {

        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        return;
    }

    if (tileElement->properties.path.edges == 0xFF) {
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, 0xFFFF, 0);
        return;
    }

    paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);

    if (edges & EDGE_NE) {
        paint_util_set_segment_support_height(session, SEGMENT_CC, 0xFFFF, 0);
    }

    if (edges & EDGE_SE) {
        paint_util_set_segment_support_height(session, SEGMENT_D4, 0xFFFF, 0);
    }

    if (edges & EDGE_SW) {
        paint_util_set_segment_support_height(session, SEGMENT_D0, 0xFFFF, 0);
    }

    if (edges & EDGE_NW) {
        paint_util_set_segment_support_height(session, SEGMENT_C8, 0xFFFF, 0);
    }
}
