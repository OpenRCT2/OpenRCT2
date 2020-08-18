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
#include "../../drawing/LightFX.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../object/ObjectList.h"
#include "../../peep/Staff.h"
#include "../../ride/Track.h"
#include "../../ride/TrackDesign.h"
#include "../../ride/TrackPaint.h"
#include "../../world/Footpath.h"
#include "../../world/Map.h"
#include "../../world/Scenery.h"
#include "../../world/Sprite.h"
#include "../../world/Surface.h"
#include "../Paint.h"
#include "../Supports.h"
#include "Paint.Surface.h"
#include "Paint.TileElement.h"

bool gPaintWidePathsAsGhost = false;

// clang-format off
const uint8_t PathSlopeToLandSlope[] =
{
    TILE_ELEMENT_SLOPE_SW_SIDE_UP,
    TILE_ELEMENT_SLOPE_NW_SIDE_UP,
    TILE_ELEMENT_SLOPE_NE_SIDE_UP,
    TILE_ELEMENT_SLOPE_SE_SIDE_UP
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
    paint_session* session, const TileElement* tileElement, int32_t height, PathSurfaceEntry* footpathEntry,
    PathRailingsEntry* railingEntry, bool hasSupports, uint32_t imageFlags, uint32_t sceneryImageFlags);
void path_paint_pole_support(
    paint_session* session, const TileElement* tileElement, int16_t height, PathSurfaceEntry* footpathEntry,
    PathRailingsEntry* railingEntry, bool hasSupports, uint32_t imageFlags, uint32_t sceneryImageFlags);

/* rct2: 0x006A5AE5 */
static void path_bit_lights_paint(
    paint_session* session, rct_scenery_entry* pathBitEntry, const TileElement* tileElement, int32_t height, uint8_t edges,
    uint32_t pathBitImageFlags)
{
    if (tileElement->AsPath()->IsSloped())
        height += 8;

    uint32_t imageId;

    if (!(edges & EDGE_NE))
    {
        imageId = pathBitEntry->image + 1;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 2, 16, 1, 1, 23, height, 3, 16, height + 2);
    }
    if (!(edges & EDGE_SE))
    {
        imageId = pathBitEntry->image + 2;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 30, 1, 0, 23, height, 16, 29, height + 2);
    }

    if (!(edges & EDGE_SW))
    {
        imageId = pathBitEntry->image + 3;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 30, 16, 0, 1, 23, height, 29, 16, height + 2);
    }

    if (!(edges & EDGE_NW))
    {
        imageId = pathBitEntry->image + 4;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 2, 1, 1, 23, height, 16, 3, height + 2);
    }
}

/* rct2: 0x006A5C94 */
static void path_bit_bins_paint(
    paint_session* session, rct_scenery_entry* pathBitEntry, const TileElement* tileElement, int32_t height, uint8_t edges,
    uint32_t pathBitImageFlags)
{
    if (tileElement->AsPath()->IsSloped())
        height += 8;

    uint32_t imageId;
    bool binsAreVandalised = tileElement->AsPath()->IsBroken();

    if (!(edges & EDGE_NE))
    {
        imageId = pathBitEntry->image + 5;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised)
        {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->AsPath()->GetAdditionStatus() & ror8(0x3, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 7, 16, 1, 1, 7, height, 7, 16, height + 2);
    }
    if (!(edges & EDGE_SE))
    {
        imageId = pathBitEntry->image + 6;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised)
        {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->AsPath()->GetAdditionStatus() & ror8(0xC, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 16, 25, 1, 1, 7, height, 16, 25, height + 2);
    }

    if (!(edges & EDGE_SW))
    {
        imageId = pathBitEntry->image + 7;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised)
        {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->AsPath()->GetAdditionStatus() & ror8(0x30, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 25, 16, 1, 1, 7, height, 25, 16, height + 2);
    }

    if (!(edges & EDGE_NW))
    {
        imageId = pathBitEntry->image + 8;

        imageId |= pathBitImageFlags;

        bool binIsFull = false;
        if (!binsAreVandalised)
        {
            imageId -= 4;

            // Edges have been rotated around the rotation to check addition status
            // this will also need to be rotated.
            binIsFull = !(tileElement->AsPath()->GetAdditionStatus() & ror8(0xC0, (2 * session->CurrentRotation)));
            if (binIsFull)
                imageId += 8;
        }

        if (!(session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) || binIsFull || binsAreVandalised)
            sub_98197C(session, imageId, 16, 7, 1, 1, 7, height, 16, 7, height + 2);
    }
}

/* rct2: 0x006A5E81 */
static void path_bit_benches_paint(
    paint_session* session, rct_scenery_entry* pathBitEntry, const TileElement* tileElement, int32_t height, uint8_t edges,
    uint32_t pathBitImageFlags)
{
    uint32_t imageId;

    if (!(edges & EDGE_NE))
    {
        imageId = pathBitEntry->image + 1;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 7, 16, 0, 16, 7, height, 6, 8, height + 2);
    }
    if (!(edges & EDGE_SE))
    {
        imageId = pathBitEntry->image + 2;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 25, 16, 0, 7, height, 8, 23, height + 2);
    }

    if (!(edges & EDGE_SW))
    {
        imageId = pathBitEntry->image + 3;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 25, 16, 0, 16, 7, height, 23, 8, height + 2);
    }

    if (!(edges & EDGE_NW))
    {
        imageId = pathBitEntry->image + 4;

        if (tileElement->AsPath()->IsBroken())
            imageId += 4;

        imageId |= pathBitImageFlags;

        sub_98197C(session, imageId, 16, 7, 16, 0, 7, height, 8, 6, height + 2);
    }
}

/* rct2: 0x006A6008 */
static void path_bit_jumping_fountains_paint(
    paint_session* session, rct_scenery_entry* pathBitEntry, int32_t height, uint32_t pathBitImageFlags, rct_drawpixelinfo* dpi)
{
    if (dpi->zoom_level > 0)
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
    paint_session* session, const TileElement* tile_element, uint16_t height, uint32_t connectedEdges, bool word_F3F038,
    PathRailingsEntry* railingEntry, uint32_t imageFlags)
{
    uint32_t base_image_id = railingEntry->railings_image | imageFlags;

    if (tile_element->AsPath()->IsQueue())
    {
        uint8_t local_ebp = connectedEdges & 0x0F;
        if (tile_element->AsPath()->IsSloped())
        {
            switch ((tile_element->AsPath()->GetSlopeDirection() + session->CurrentRotation)
                    & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
            {
                case 0:
                    sub_98197C(session, 22 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                    sub_98197C(session, 22 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                    break;
                case 1:
                    sub_98197C(session, 21 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                    sub_98197C(session, 21 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                    break;
                case 2:
                    sub_98197C(session, 23 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                    sub_98197C(session, 23 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                    break;
                case 3:
                    sub_98197C(session, 20 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                    sub_98197C(session, 20 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                    break;
            }
        }
        else
        {
            switch (local_ebp)
            {
                case 1:
                    sub_98197C(session, 17 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 17 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    break;
                case 2:
                    sub_98197C(session, 18 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 18 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    break;
                case 3:
                    sub_98197C(session, 17 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(
                        session, 18 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4,
                        height + 2); // bound_box_offset_y seems to be a bug
                    sub_98197C(session, 25 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                    break;
                case 4:
                    sub_98197C(session, 19 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 19 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    break;
                case 5:
                    sub_98197C(session, 15 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 15 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                    break;
                case 6:
                    sub_98197C(session, 18 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 19 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                    sub_98197C(session, 26 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                    break;
                case 8:
                    sub_98197C(session, 16 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 16 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    break;
                case 9:
                    sub_98197C(session, 16 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                    sub_98197C(session, 17 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                    sub_98197C(session, 24 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                    break;
                case 10:
                    sub_98197C(session, 14 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                    sub_98197C(session, 14 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                    break;
                case 12:
                    sub_98197C(session, 16 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                    sub_98197C(
                        session, 19 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28,
                        height + 2); // bound_box_offset_x seems to be a bug
                    sub_98197C(session, 27 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                    break;
                default:
                    // purposely left empty
                    break;
            }
        }

        if (!tile_element->AsPath()->HasQueueBanner())
        {
            return;
        }

        uint8_t direction = tile_element->AsPath()->GetQueueBannerDirection();
        // Draw ride sign
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_RIDE;
        if (tile_element->AsPath()->IsSloped())
        {
            if (tile_element->AsPath()->GetSlopeDirection() == direction)
                height += 16;
        }
        direction += session->CurrentRotation;
        direction &= 3;

        CoordsXYZ boundBoxOffsets = CoordsXYZ(BannerBoundBoxes[direction][0], height + 2);

        uint32_t imageId = (direction << 1) + base_image_id + 28;

        // Draw pole in the back
        sub_98197C(session, imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);

        // Draw pole in the front and banner
        boundBoxOffsets.x = BannerBoundBoxes[direction][1].x;
        boundBoxOffsets.y = BannerBoundBoxes[direction][1].y;
        imageId++;
        sub_98197C(session, imageId, 0, 0, 1, 1, 21, height, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);

        direction--;
        // If text shown
        auto ride = get_ride(tile_element->AsPath()->GetRideIndex());
        if (direction < 2 && ride != nullptr && imageFlags == 0)
        {
            uint16_t scrollingMode = railingEntry->scrolling_mode;
            scrollingMode += direction;

            // This is required due to scrolling_test_setup doing a memcmp of another set
            // of args and see if it matches these args
            Formatter::Common().Add<uint32_t>(0).Add<uint32_t>(0);

            auto ft = Formatter::Common();

            if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
            {
                ft.Add<rct_string_id>(STR_RIDE_ENTRANCE_NAME);
                ride->FormatNameTo(ft);
            }
            else
            {
                ft.Add<rct_string_id>(STR_RIDE_ENTRANCE_CLOSED);
            }
            if (gConfigGeneral.upper_case_banners)
            {
                format_string_to_upper(
                    gCommonStringFormatBuffer, sizeof(gCommonStringFormatBuffer), STR_BANNER_TEXT_FORMAT, gCommonFormatArgs);
            }
            else
            {
                format_string(
                    gCommonStringFormatBuffer, sizeof(gCommonStringFormatBuffer), STR_BANNER_TEXT_FORMAT, gCommonFormatArgs);
            }

            gCurrentFontSpriteBase = FONT_SPRITE_BASE_TINY;

            uint16_t stringWidth = gfx_get_string_width(gCommonStringFormatBuffer);
            uint16_t scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;

            sub_98199C(
                session, scrolling_text_setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, scrollingMode, COLOUR_BLACK), 0, 0,
                1, 1, 21, height + 7, boundBoxOffsets.x, boundBoxOffsets.y, boundBoxOffsets.z);
        }

        session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;
        if (imageFlags != 0)
        {
            session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        }
        return;
    }

    uint32_t drawnCorners = 0;
    // If the path is not drawn over the supports, then no corner sprites will be drawn (making double-width paths
    // look like connected series of intersections).
    if (tile_element->AsPath()->ShouldDrawPathOverSupports())
    {
        drawnCorners = (connectedEdges & FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK) >> 4;
    }

    if (tile_element->AsPath()->IsSloped())
    {
        switch ((tile_element->AsPath()->GetSlopeDirection() + session->CurrentRotation)
                & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
        {
            case 0:
                sub_98197C(session, 8 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                sub_98197C(session, 8 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                break;
            case 1:
                sub_98197C(session, 7 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                sub_98197C(session, 7 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                break;
            case 2:
                sub_98197C(session, 9 + base_image_id, 0, 4, 32, 1, 23, height, 0, 4, height + 2);
                sub_98197C(session, 9 + base_image_id, 0, 28, 32, 1, 23, height, 0, 28, height + 2);
                break;
            case 3:
                sub_98197C(session, 6 + base_image_id, 4, 0, 1, 32, 23, height, 4, 0, height + 2);
                sub_98197C(session, 6 + base_image_id, 28, 0, 1, 32, 23, height, 28, 0, height + 2);
                break;
        }
    }
    else
    {
        if (!word_F3F038)
        {
            return;
        }

        switch (connectedEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK)
        {
            case 0:
                // purposely left empty
                break;
            case 1:
                sub_98197C(session, 3 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 3 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                break;
            case 2:
                sub_98197C(session, 4 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 4 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                break;
            case 4:
                sub_98197C(session, 5 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 5 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                break;
            case 5:
                sub_98197C(session, 1 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                sub_98197C(session, 1 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                break;
            case 8:
                sub_98197C(session, 2 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 2 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                break;
            case 10:
                sub_98197C(session, 0 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                sub_98197C(session, 0 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                break;

            case 3:
                sub_98197C(session, 3 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                sub_98197C(
                    session, 4 + base_image_id, 28, 0, 1, 28, 7, height, 28, 4,
                    height + 2); // bound_box_offset_y seems to be a bug
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    sub_98197C(session, 11 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                break;
            case 6:
                sub_98197C(session, 4 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(session, 5 + base_image_id, 0, 4, 28, 1, 7, height, 0, 4, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    sub_98197C(session, 12 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                break;
            case 9:
                sub_98197C(session, 2 + base_image_id, 28, 0, 1, 28, 7, height, 28, 0, height + 2);
                sub_98197C(session, 3 + base_image_id, 0, 28, 28, 1, 7, height, 0, 28, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    sub_98197C(session, 10 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;
            case 12:
                sub_98197C(session, 2 + base_image_id, 4, 0, 1, 28, 7, height, 4, 0, height + 2);
                sub_98197C(
                    session, 5 + base_image_id, 0, 28, 28, 1, 7, height, 4, 28,
                    height + 2); // bound_box_offset_x seems to be a bug
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    sub_98197C(session, 13 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                break;

            case 7:
                sub_98197C(session, 1 + base_image_id, 0, 4, 32, 1, 7, height, 0, 4, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    sub_98197C(session, 11 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    sub_98197C(session, 12 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                break;
            case 13:
                sub_98197C(session, 1 + base_image_id, 0, 28, 32, 1, 7, height, 0, 28, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    sub_98197C(session, 13 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    sub_98197C(session, 10 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;
            case 14:
                sub_98197C(session, 0 + base_image_id, 4, 0, 1, 32, 7, height, 4, 0, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    sub_98197C(session, 12 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    sub_98197C(session, 13 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                break;
            case 11:
                sub_98197C(session, 0 + base_image_id, 28, 0, 1, 32, 7, height, 28, 0, height + 2);
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    sub_98197C(session, 11 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    sub_98197C(session, 10 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
                }
                break;

            case 15:
                if (!(drawnCorners & FOOTPATH_CORNER_0))
                {
                    sub_98197C(session, 11 + base_image_id, 0, 0, 4, 4, 7, height, 0, 28, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_1))
                {
                    sub_98197C(session, 12 + base_image_id, 0, 0, 4, 4, 7, height, 28, 28, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_2))
                {
                    sub_98197C(session, 13 + base_image_id, 0, 0, 4, 4, 7, height, 28, 0, height + 2);
                }
                if (!(drawnCorners & FOOTPATH_CORNER_3))
                {
                    sub_98197C(session, 10 + base_image_id, 0, 0, 4, 4, 7, height, 0, 0, height + 2);
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
 * @param railingEntry (0x00F3EF6C)
 * @param imageFlags (0x00F3EF70)
 * @param sceneryImageFlags (0x00F3EF74)
 */
static void sub_6A3F61(
    paint_session* session, const TileElement* tile_element, uint16_t connectedEdges, uint16_t height,
    PathRailingsEntry* railingEntry, uint32_t imageFlags, uint32_t sceneryImageFlags, bool word_F3F038)
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

    rct_drawpixelinfo* dpi = &session->DPI;

    if (dpi->zoom_level <= 1)
    {
        bool paintScenery = true;

        if (!gTrackDesignSaveMode)
        {
            if (tile_element->AsPath()->HasAddition())
            {
                session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH_ITEM;
                if (sceneryImageFlags != 0)
                {
                    session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                }

                // Draw additional path bits (bins, benches, lamps, queue screens)
                rct_scenery_entry* sceneryEntry = tile_element->AsPath()->GetAdditionEntry();

                // Can be null if the object is not loaded.
                if (sceneryEntry == nullptr)
                {
                    paintScenery = false;
                }
                else if (
                    (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES) && !(tile_element->AsPath()->IsBroken())
                    && !(sceneryEntry->path_bit.draw_type == PATH_BIT_DRAW_TYPE_BINS))
                {
                    paintScenery = false;
                }
                else
                {
                    switch (sceneryEntry->path_bit.draw_type)
                    {
                        case PATH_BIT_DRAW_TYPE_LIGHTS:
                            path_bit_lights_paint(
                                session, sceneryEntry, tile_element, height, static_cast<uint8_t>(connectedEdges),
                                sceneryImageFlags);
                            break;
                        case PATH_BIT_DRAW_TYPE_BINS:
                            path_bit_bins_paint(
                                session, sceneryEntry, tile_element, height, static_cast<uint8_t>(connectedEdges),
                                sceneryImageFlags);
                            break;
                        case PATH_BIT_DRAW_TYPE_BENCHES:
                            path_bit_benches_paint(
                                session, sceneryEntry, tile_element, height, static_cast<uint8_t>(connectedEdges),
                                sceneryImageFlags);
                            break;
                        case PATH_BIT_DRAW_TYPE_JUMPING_FOUNTAINS:
                            path_bit_jumping_fountains_paint(session, sceneryEntry, height, sceneryImageFlags, dpi);
                            break;
                    }

                    session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

                    if (sceneryImageFlags != 0)
                    {
                        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
                    }
                }
            }
        }

        // Redundant zoom-level check removed

        if (paintScenery)
            sub_6A4101(session, tile_element, height, connectedEdges, word_F3F038, railingEntry, imageFlags);
    }

    // This is about tunnel drawing
    uint8_t direction = (tile_element->AsPath()->GetSlopeDirection() + session->CurrentRotation)
        & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    bool sloped = tile_element->AsPath()->IsSloped();

    if (connectedEdges & EDGE_SE)
    {
        // Bottom right of tile is a tunnel
        if (sloped && direction == EDGE_NE)
        {
            // Path going down into the tunnel
            paint_util_push_tunnel_right(session, height + 16, TUNNEL_PATH_AND_MINI_GOLF);
        }
        else if (connectedEdges & EDGE_NE)
        {
            // Flat path with edge to the right (north-east)
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_11);
        }
        else
        {
            // Path going up, or flat and not connected to the right
            paint_util_push_tunnel_right(session, height, TUNNEL_PATH_AND_MINI_GOLF);
        }
    }

    if (!(connectedEdges & EDGE_SW))
    {
        return;
    }

    // Bottom left of the tile is a tunnel
    if (sloped && direction == EDGE_SE)
    {
        // Path going down into the tunnel
        paint_util_push_tunnel_left(session, height + 16, TUNNEL_PATH_AND_MINI_GOLF);
    }
    else if (connectedEdges & EDGE_NW)
    {
        // Flat path with edge to the left (north-west)
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_11);
    }
    else
    {
        // Path going up, or flat and not connected to the left
        paint_util_push_tunnel_left(session, height, TUNNEL_PATH_AND_MINI_GOLF);
    }
}

/**
 * rct2: 0x0006A3590
 */
void path_paint(paint_session* session, uint16_t height, const TileElement* tile_element)
{
    session->InteractionType = VIEWPORT_INTERACTION_ITEM_FOOTPATH;

    bool hasSupports = false;

    uint32_t sceneryImageFlags = 0;
    uint32_t imageFlags = 0;

    if (gTrackDesignSaveMode)
    {
        if (tile_element->AsPath()->IsQueue())
        {
            if (tile_element->AsPath()->GetRideIndex() != gTrackDesignSaveRideIndex)
            {
                return;
            }
        }

        if (!track_design_save_contains_tile_element(tile_element))
        {
            imageFlags = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
        }
    }

    if (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES)
    {
        imageFlags = SPRITE_ID_PALETTE_COLOUR_1(PALETTE_46);
    }

    if (tile_element->AsPath()->AdditionIsGhost())
    {
        sceneryImageFlags = CONSTRUCTION_MARKER;
    }

    if (tile_element->IsGhost())
    {
        session->InteractionType = VIEWPORT_INTERACTION_ITEM_NONE;
        imageFlags = CONSTRUCTION_MARKER;
    }

    // For debugging purpose, show blocked tiles with a colour
    if (gPaintBlockedTiles && tile_element->AsPath()->IsBlockedByVehicle())
    {
        imageFlags = COLOUR_BRIGHT_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP;
    }

    // Draw wide flags as ghosts, leaving only the "walkable" paths to be drawn normally
    if (gPaintWidePathsAsGhost && tile_element->AsPath()->IsWide())
    {
        imageFlags &= 0x7FFFF;
        imageFlags |= CONSTRUCTION_MARKER;
    }

    auto surface = map_get_surface_element_at(session->MapPosition);

    if (surface == nullptr)
    {
        hasSupports = true;
    }
    else if (surface->GetBaseZ() != height)
    {
        hasSupports = true;
    }
    else
    {
        if (tile_element->AsPath()->IsSloped())
        {
            // Diagonal path

            if (surface->GetSlope() != PathSlopeToLandSlope[tile_element->AsPath()->GetSlopeDirection()])
            {
                hasSupports = true;
            }
        }
        else
        {
            if (surface->GetSlope() != TILE_ELEMENT_SLOPE_FLAT)
            {
                hasSupports = true;
            }
        }
    }

    if (gStaffDrawPatrolAreas != 0xFFFF)
    {
        int32_t staffIndex = gStaffDrawPatrolAreas;
        uint8_t staffType = staffIndex & 0x7FFF;
        bool is_staff_list = staffIndex & 0x8000;
        uint8_t patrolColour = COLOUR_LIGHT_BLUE;

        if (!is_staff_list)
        {
            Staff* staff = GetEntity<Staff>(staffIndex);
            if (staff == nullptr)
            {
                log_error("Invalid staff index for draw patrol areas!");
            }
            else
            {
                if (!staff->IsPatrolAreaSet(session->MapPosition))
                {
                    patrolColour = COLOUR_GREY;
                }
                staffType = static_cast<uint8_t>(staff->AssignedStaffType);
            }
        }

        if (staff_is_patrol_area_set_for_type(static_cast<StaffType>(staffType), session->MapPosition))
        {
            uint32_t imageId = 2618;
            int32_t patrolAreaBaseZ = tile_element->GetBaseZ();
            if (tile_element->AsPath()->IsSloped())
            {
                imageId = 2619 + ((tile_element->AsPath()->GetSlopeDirection() + session->CurrentRotation) & 3);
                patrolAreaBaseZ += 16;
            }

            sub_98196C(session, imageId | patrolColour << 19 | IMAGE_TYPE_REMAP, 16, 16, 1, 1, 0, patrolAreaBaseZ + 2);
        }
    }

    if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_PATH_HEIGHTS))
    {
        uint16_t heightMarkerBaseZ = tile_element->GetBaseZ() + 3;
        if (tile_element->AsPath()->IsSloped())
        {
            heightMarkerBaseZ += 8;
        }
        uint32_t imageId = (SPR_HEIGHT_MARKER_BASE + heightMarkerBaseZ / 16) | COLOUR_GREY << 19 | IMAGE_TYPE_REMAP;
        imageId += get_height_marker_offset();
        imageId -= gMapBaseZ;
        sub_98196C(session, imageId, 16, 16, 1, 1, 0, heightMarkerBaseZ);
    }

    PathSurfaceEntry* footpathEntry = tile_element->AsPath()->GetSurfaceEntry();
    PathRailingsEntry* railingEntry = tile_element->AsPath()->GetRailingEntry();

    if (footpathEntry != nullptr && railingEntry != nullptr)
    {
        if (railingEntry->support_type == RailingEntrySupportType::Pole)
        {
            path_paint_pole_support(
                session, tile_element, height, footpathEntry, railingEntry, hasSupports, imageFlags, sceneryImageFlags);
        }
        else
        {
            path_paint_box_support(
                session, tile_element, height, footpathEntry, railingEntry, hasSupports, imageFlags, sceneryImageFlags);
        }
    }

#ifdef __ENABLE_LIGHTFX__
    if (lightfx_is_available())
    {
        if (tile_element->AsPath()->HasAddition() && !(tile_element->AsPath()->IsBroken()))
        {
            rct_scenery_entry* sceneryEntry = tile_element->AsPath()->GetAdditionEntry();
            if (sceneryEntry != nullptr && sceneryEntry->path_bit.flags & PATH_BIT_FLAG_LAMP)
            {
                if (!(tile_element->AsPath()->GetEdges() & EDGE_NE))
                {
                    lightfx_add_3d_light_magic_from_drawing_tile(
                        session->MapPosition, -16, 0, height + 23, LightType::Lantern3);
                }
                if (!(tile_element->AsPath()->GetEdges() & EDGE_SE))
                {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, 16, height + 23, LightType::Lantern3);
                }
                if (!(tile_element->AsPath()->GetEdges() & EDGE_SW))
                {
                    lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 16, 0, height + 23, LightType::Lantern3);
                }
                if (!(tile_element->AsPath()->GetEdges() & EDGE_NW))
                {
                    lightfx_add_3d_light_magic_from_drawing_tile(
                        session->MapPosition, 0, -16, height + 23, LightType::Lantern3);
                }
            }
        }
    }
#endif
}

void path_paint_box_support(
    paint_session* session, const TileElement* tileElement, int32_t height, PathSurfaceEntry* footpathEntry,
    PathRailingsEntry* railingEntry, bool hasSupports, uint32_t imageFlags, uint32_t sceneryImageFlags)
{
    PathElement* pathElement = tileElement->AsPath();

    // Rol edges around rotation
    uint8_t edges = ((tileElement->AsPath()->GetEdges() << session->CurrentRotation) & 0xF)
        | (((tileElement->AsPath()->GetEdges()) << session->CurrentRotation) >> 4);

    uint8_t corners = (((tileElement->AsPath()->GetCorners()) << session->CurrentRotation) & 0xF)
        | (((tileElement->AsPath()->GetCorners()) << session->CurrentRotation) >> 4);

    LocationXY16 boundBoxOffset = { stru_98D804[edges][0], stru_98D804[edges][1] };
    LocationXY16 boundBoxSize = { stru_98D804[edges][2], stru_98D804[edges][3] };

    uint16_t edi = edges | (corners << 4);

    uint32_t imageId;
    if (tileElement->AsPath()->IsSloped())
    {
        imageId = ((tileElement->AsPath()->GetSlopeDirection() + session->CurrentRotation)
                   & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
            + 16;
    }
    else
    {
        imageId = byte_98D6E0[edi];
    }

    imageId += footpathEntry->image;

    if (!session->DidPassSurface)
    {
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
        if (session->TrackElementOnSameHeight->AsTrack()->GetTrackType() == TrackElemType::Flat)
        {
            boundingBoxZOffset = 2;
        }
    }

    if (!hasSupports || !session->DidPassSurface)
    {
        sub_98197C(
            session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);
    }
    else
    {
        uint32_t image_id;
        if (tileElement->AsPath()->IsSloped())
        {
            image_id = ((tileElement->AsPath()->GetSlopeDirection() + session->CurrentRotation)
                        & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
                + railingEntry->bridge_image + 51;
        }
        else
        {
            image_id = byte_98D8A4[edges] + railingEntry->bridge_image + 49;
        }

        sub_98197C(
            session, image_id | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);

        // TODO: Revert this when path import works correctly.
        if (!pathElement->IsQueue() && !pathElement->ShouldDrawPathOverSupports())
        {
            // don't draw
        }
        else
        {
            sub_98199C(
                session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
                boundBoxOffset.y, height + boundingBoxZOffset);
        }
    }

    sub_6A3F61(session, tileElement, edi, height, railingEntry, imageFlags, sceneryImageFlags, hasSupports);

    uint16_t ax = 0;
    if (tileElement->AsPath()->IsSloped())
    {
        ax = ((tileElement->AsPath()->GetSlopeDirection() + session->CurrentRotation) & 0x3) + 1;
    }

    if (byte_98D8A4[edges] == 0)
    {
        path_a_supports_paint_setup(session, 0, ax, height, imageFlags, railingEntry, nullptr);
    }
    else
    {
        path_a_supports_paint_setup(session, 1, ax, height, imageFlags, railingEntry, nullptr);
    }

    height += 32;
    if (tileElement->AsPath()->IsSloped())
    {
        height += 16;
    }

    paint_util_set_general_support_height(session, height, 0x20);

    if (pathElement->IsQueue() || (tileElement->AsPath()->GetEdgesAndCorners() != 0xFF && hasSupports))
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        return;
    }

    if (tileElement->AsPath()->GetEdgesAndCorners() == 0xFF)
    {
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, 0xFFFF, 0);
        return;
    }

    paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);

    if (edges & 1)
    {
        paint_util_set_segment_support_height(session, SEGMENT_CC, 0xFFFF, 0);
    }

    if (edges & 2)
    {
        paint_util_set_segment_support_height(session, SEGMENT_D4, 0xFFFF, 0);
    }

    if (edges & 4)
    {
        paint_util_set_segment_support_height(session, SEGMENT_D0, 0xFFFF, 0);
    }

    if (edges & 8)
    {
        paint_util_set_segment_support_height(session, SEGMENT_C8, 0xFFFF, 0);
    }
}

void path_paint_pole_support(
    paint_session* session, const TileElement* tileElement, int16_t height, PathSurfaceEntry* footpathEntry,
    PathRailingsEntry* railingEntry, bool hasSupports, uint32_t imageFlags, uint32_t sceneryImageFlags)
{
    PathElement* pathElement = tileElement->AsPath();

    // Rol edges around rotation
    uint8_t edges = ((tileElement->AsPath()->GetEdges() << session->CurrentRotation) & 0xF)
        | (((tileElement->AsPath()->GetEdges()) << session->CurrentRotation) >> 4);

    LocationXY16 boundBoxOffset = { stru_98D804[edges][0], stru_98D804[edges][1] };

    LocationXY16 boundBoxSize = { stru_98D804[edges][2], stru_98D804[edges][3] };

    uint8_t corners = (((tileElement->AsPath()->GetCorners()) << session->CurrentRotation) & 0xF)
        | (((tileElement->AsPath()->GetCorners()) << session->CurrentRotation) >> 4);

    uint16_t edi = edges | (corners << 4);

    uint32_t imageId;
    if (tileElement->AsPath()->IsSloped())
    {
        imageId = ((tileElement->AsPath()->GetSlopeDirection() + session->CurrentRotation) & 3) + 16;
    }
    else
    {
        imageId = byte_98D6E0[edi];
    }

    imageId += footpathEntry->image;

    // Below Surface
    if (!session->DidPassSurface)
    {
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
        if (session->TrackElementOnSameHeight->AsTrack()->GetTrackType() == TrackElemType::Flat)
        {
            boundingBoxZOffset = 2;
        }
    }

    if (!hasSupports || !session->DidPassSurface)
    {
        sub_98197C(
            session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x, boundBoxOffset.y,
            height + boundingBoxZOffset);
    }
    else
    {
        uint32_t bridgeImage;
        if (tileElement->AsPath()->IsSloped())
        {
            bridgeImage = ((tileElement->AsPath()->GetSlopeDirection() + session->CurrentRotation)
                           & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK)
                + railingEntry->bridge_image + 16;
        }
        else
        {
            bridgeImage = edges + railingEntry->bridge_image;
            bridgeImage |= imageFlags;
        }

        sub_98197C(
            session, bridgeImage | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
            boundBoxOffset.y, height + boundingBoxZOffset);

        // TODO: Revert this when path import works correctly.
        if (pathElement->IsQueue() || pathElement->ShouldDrawPathOverSupports())
        {
            sub_98199C(
                session, imageId | imageFlags, 0, 0, boundBoxSize.x, boundBoxSize.y, 0, height, boundBoxOffset.x,
                boundBoxOffset.y, height + boundingBoxZOffset);
        }
    }

    sub_6A3F61(session, tileElement, edi, height, railingEntry, imageFlags, sceneryImageFlags, hasSupports); // TODO: arguments

    uint16_t ax = 0;
    if (tileElement->AsPath()->IsSloped())
    {
        ax = 8;
    }

    uint8_t supports[] = {
        6,
        8,
        7,
        5,
    };

    for (int8_t i = 3; i > -1; --i)
    {
        if (!(edges & (1 << i)))
        {
            path_b_supports_paint_setup(session, supports[i], ax, height, imageFlags, railingEntry);
        }
    }

    height += 32;
    if (tileElement->AsPath()->IsSloped())
    {
        height += 16;
    }

    paint_util_set_general_support_height(session, height, 0x20);

    if (pathElement->IsQueue() || (tileElement->AsPath()->GetEdgesAndCorners() != 0xFF && hasSupports))
    {
        paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
        return;
    }

    if (tileElement->AsPath()->GetEdgesAndCorners() == 0xFF)
    {
        paint_util_set_segment_support_height(session, SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4, 0xFFFF, 0);
        return;
    }

    paint_util_set_segment_support_height(session, SEGMENT_C4, 0xFFFF, 0);

    if (edges & EDGE_NE)
    {
        paint_util_set_segment_support_height(session, SEGMENT_CC, 0xFFFF, 0);
    }

    if (edges & EDGE_SE)
    {
        paint_util_set_segment_support_height(session, SEGMENT_D4, 0xFFFF, 0);
    }

    if (edges & EDGE_SW)
    {
        paint_util_set_segment_support_height(session, SEGMENT_D0, 0xFFFF, 0);
    }

    if (edges & EDGE_NW)
    {
        paint_util_set_segment_support_height(session, SEGMENT_C8, 0xFFFF, 0);
    }
}
