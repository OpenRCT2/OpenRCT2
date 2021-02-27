/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Context.h"
#include "../../Game.h"
#include "../../GameState.h"
#include "../../config/Config.h"
#include "../../drawing/LightFX.h"
#include "../../interface/Viewport.h"
#include "../../localisation/Localisation.h"
#include "../../object/StationObject.h"
#include "../../ride/RideData.h"
#include "../../ride/TrackDesign.h"
#include "../../world/Banner.h"
#include "../../world/Entrance.h"
#include "../../world/Footpath.h"
#include "../../world/Park.h"
#include "../Paint.h"
#include "../Supports.h"
#include "Paint.TileElement.h"

/**
 *
 *  rct2: 0x0066508C, 0x00665540
 */
static void ride_entrance_exit_paint(paint_session* session, uint8_t direction, int32_t height, const TileElement* tile_element)
{
    uint8_t is_exit = tile_element->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_RIDE_EXIT;

    if (gTrackDesignSaveMode || (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
    {
        if (tile_element->AsEntrance()->GetRideIndex() != gTrackDesignSaveRideIndex)
            return;
    }

#ifdef __ENABLE_LIGHTFX__
    if (lightfx_is_available())
    {
        if (!is_exit)
        {
            lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, 0, height + 45, LightType::Lantern3);
        }

        switch (tile_element->GetDirection())
        {
            case 0:
                lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 16, 0, height + 16, LightType::Lantern2);
                break;
            case 1:
                lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, -16, height + 16, LightType::Lantern2);
                break;
            case 2:
                lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, -16, 0, height + 16, LightType::Lantern2);
                break;
            case 3:
                lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, 16, height + 16, LightType::Lantern2);
                break;
        }
    }
#endif

    auto ride = get_ride(tile_element->AsEntrance()->GetRideIndex());
    if (ride == nullptr)
    {
        return;
    }

    auto stationObj = ride_get_station_object(ride);
    if (stationObj == nullptr || stationObj->BaseImageId == 0)
    {
        return;
    }

    uint8_t colour_1, colour_2;
    uint32_t transparant_image_id = 0, image_id = 0;
    if (stationObj->Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
    {
        colour_1 = EnumValue(GlassPaletteIds[ride->track_colour[0].main]);
        transparant_image_id = (colour_1 << 19) | IMAGE_TYPE_TRANSPARENT;
    }

    colour_1 = ride->track_colour[0].main;
    colour_2 = ride->track_colour[0].additional;
    image_id = (colour_1 << 19) | (colour_2 << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS;

    session->InteractionType = ViewportInteractionItem::Ride;
    uint32_t entranceImageId = 0;

    if (tile_element->IsGhost())
    {
        session->InteractionType = ViewportInteractionItem::None;
        image_id = CONSTRUCTION_MARKER;
        entranceImageId = image_id;
        if (transparant_image_id)
            transparant_image_id = image_id;
    }

    if (is_exit)
    {
        image_id |= stationObj->BaseImageId + direction + 8;
    }
    else
    {
        image_id |= stationObj->BaseImageId + direction;
    }

    // Format modified to stop repeated code

    // Each entrance is split into 2 images for drawing
    // Certain entrance styles have another 2 images to draw for coloured windows

    int8_t ah = is_exit ? 0x23 : 0x33;

    int16_t lengthY = (direction & 1) ? 28 : 2;
    int16_t lengthX = (direction & 1) ? 2 : 28;

    PaintAddImageAsParent(session, image_id, 0, 0, lengthX, lengthY, ah, height, 2, 2, height);

    if (transparant_image_id)
    {
        if (is_exit)
        {
            transparant_image_id |= stationObj->BaseImageId + direction + 24;
        }
        else
        {
            transparant_image_id |= stationObj->BaseImageId + direction + 16;
        }

        PaintAddImageAsChild(session, transparant_image_id, 0, 0, lengthX, lengthY, ah, height, 2, 2, height);
    }

    image_id += 4;

    PaintAddImageAsParent(
        session, image_id, 0, 0, lengthX, lengthY, ah, height, (direction & 1) ? 28 : 2, (direction & 1) ? 2 : 28, height);

    if (transparant_image_id)
    {
        transparant_image_id += 4;
        PaintAddImageAsChild(
            session, transparant_image_id, 0, 0, lengthX, lengthY, ah, height, (direction & 1) ? 28 : 2,
            (direction & 1) ? 2 : 28, height);
    }

    if (direction & 1)
    {
        paint_util_push_tunnel_right(session, height, TUNNEL_SQUARE_FLAT);
    }
    else
    {
        paint_util_push_tunnel_left(session, height, TUNNEL_SQUARE_FLAT);
    }

    if (!is_exit && !(tile_element->IsGhost()) && tile_element->AsEntrance()->GetRideIndex() != RIDE_ID_NULL
        && stationObj->ScrollingMode != SCROLLING_MODE_NONE)
    {
        auto ft = Formatter();
        ft.Add<rct_string_id>(STR_RIDE_ENTRANCE_NAME);

        if (ride->status == RIDE_STATUS_OPEN && !(ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN))
        {
            ride->FormatNameTo(ft);
        }
        else
        {
            ft.Add<rct_string_id>(STR_RIDE_ENTRANCE_CLOSED);
        }

        utf8 entrance_string[256];
        if (gConfigGeneral.upper_case_banners)
        {
            format_string_to_upper(entrance_string, sizeof(entrance_string), STR_BANNER_TEXT_FORMAT, ft.Data());
        }
        else
        {
            format_string(entrance_string, sizeof(entrance_string), STR_BANNER_TEXT_FORMAT, ft.Data());
        }

        uint16_t stringWidth = gfx_get_string_width(entrance_string, FontSpriteBase::TINY);
        uint16_t scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;

        PaintAddImageAsChild(
            session, scrolling_text_setup(session, STR_BANNER_TEXT_FORMAT, ft, scroll, stationObj->ScrollingMode, COLOUR_BLACK),
            0, 0, 0x1C, 0x1C, 0x33, height + stationObj->Height, 2, 2, height + stationObj->Height);
    }

    image_id = entranceImageId;
    if (image_id == 0)
    {
        image_id = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_SATURATED_BROWN);
    }
    wooden_a_supports_paint_setup(session, direction & 1, 0, height, image_id, nullptr);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);

    height += is_exit ? 40 : 56;
    paint_util_set_general_support_height(session, height, 0x20);
}

/**
 *
 *  rct2: 0x006658ED
 */
static void park_entrance_paint(paint_session* session, uint8_t direction, int32_t height, const TileElement* tile_element)
{
    if (gTrackDesignSaveMode || (session->ViewFlags & VIEWPORT_FLAG_HIGHLIGHT_PATH_ISSUES))
        return;

#ifdef __ENABLE_LIGHTFX__
    if (lightfx_is_available())
    {
        lightfx_add_3d_light_magic_from_drawing_tile(session->MapPosition, 0, 0, 155, LightType::Lantern3);
    }
#endif

    session->InteractionType = ViewportInteractionItem::ParkEntrance;
    uint32_t image_id, ghost_id = 0;
    if (tile_element->IsGhost())
    {
        session->InteractionType = ViewportInteractionItem::None;
        ghost_id = CONSTRUCTION_MARKER;
    }

    // Index to which part of the entrance
    // Middle, left, right
    uint8_t part_index = tile_element->AsEntrance()->GetSequenceIndex();
    PathSurfaceEntry* path_entry = nullptr;

    // The left and right of the park entrance often have this set to 127.
    // So only attempt to get the footpath type if we're dealing with the middle bit of the entrance.
    if (part_index == 0)
        path_entry = get_path_surface_entry(tile_element->AsEntrance()->GetPathType());

    rct_entrance_type* entrance;
    uint8_t di = ((direction / 2 + part_index / 2) & 1) ? 0x1A : 0x20;

    switch (part_index)
    {
        case 0:
            if (path_entry != nullptr)
            {
                image_id = (path_entry->image + 5 * (1 + (direction & 1))) | ghost_id;
                PaintAddImageAsParent(session, image_id, 0, 0, 32, 0x1C, 0, height, 0, 2, height);
            }

            entrance = static_cast<rct_entrance_type*>(object_entry_get_chunk(ObjectType::ParkEntrance, 0));
            if (entrance == nullptr)
            {
                return;
            }
            image_id = (entrance->image_id + direction * 3) | ghost_id;
            PaintAddImageAsParent(session, image_id, 0, 0, 0x1C, 0x1C, 0x2F, height, 2, 2, height + 32);

            if ((direction + 1) & (1 << 1))
                break;
            if (ghost_id != 0)
                break;

            {
                auto ft = Formatter();

                if (gParkFlags & PARK_FLAGS_PARK_OPEN)
                {
                    const auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                    auto name = park.Name.c_str();
                    ft.Add<rct_string_id>(STR_STRING);
                    ft.Add<const char*>(name);
                }
                else
                {
                    ft.Add<rct_string_id>(STR_BANNER_TEXT_CLOSED);
                    ft.Add<uint32_t>(0);
                }

                utf8 park_name[256];
                if (gConfigGeneral.upper_case_banners)
                {
                    format_string_to_upper(park_name, sizeof(park_name), STR_BANNER_TEXT_FORMAT, ft.Data());
                }
                else
                {
                    format_string(park_name, sizeof(park_name), STR_BANNER_TEXT_FORMAT, ft.Data());
                }

                uint16_t stringWidth = gfx_get_string_width(park_name, FontSpriteBase::TINY);
                uint16_t scroll = stringWidth > 0 ? (gCurrentTicks / 2) % stringWidth : 0;

                if (entrance->scrolling_mode == SCROLLING_MODE_NONE)
                    break;

                int32_t stsetup = scrolling_text_setup(
                    session, STR_BANNER_TEXT_FORMAT, ft, scroll, entrance->scrolling_mode + direction / 2, COLOUR_BLACK);
                int32_t text_height = height + entrance->text_height;
                PaintAddImageAsChild(session, stsetup, 0, 0, 0x1C, 0x1C, 0x2F, text_height, 2, 2, text_height);
            }
            break;
        case 1:
        case 2:
            entrance = static_cast<rct_entrance_type*>(object_entry_get_chunk(ObjectType::ParkEntrance, 0));
            if (entrance == nullptr)
            {
                return;
            }
            image_id = (entrance->image_id + part_index + direction * 3) | ghost_id;
            PaintAddImageAsParent(session, image_id, 0, 0, 0x1A, di, 0x4F, height, 3, 3, height);
            break;
    }

    image_id = ghost_id;
    if (image_id == 0)
    {
        image_id = SPRITE_ID_PALETTE_COLOUR_1(COLOUR_SATURATED_BROWN);
    }
    wooden_a_supports_paint_setup(session, direction & 1, 0, height, image_id, nullptr);

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 80, 0x20);
}

/**
 *
 *  rct2: 0x00664FD4
 */
void entrance_paint(paint_session* session, uint8_t direction, int32_t height, const TileElement* tile_element)
{
    session->InteractionType = ViewportInteractionItem::Label;

    if (PaintShouldShowHeightMarkers(session, VIEWPORT_FLAG_PATH_HEIGHTS))
    {
        if (entrance_get_directions(tile_element) & 0xF)
        {
            int32_t z = tile_element->GetBaseZ() + 3;
            uint32_t image_id = 0x20101689 + get_height_marker_offset() + (z / 16);
            image_id -= gMapBaseZ;

            PaintAddImageAsParent(session, image_id, 16, 16, 1, 1, 0, height, 31, 31, z + 64);
        }
    }

    switch (tile_element->AsEntrance()->GetEntranceType())
    {
        case ENTRANCE_TYPE_RIDE_ENTRANCE:
        case ENTRANCE_TYPE_RIDE_EXIT:
            ride_entrance_exit_paint(session, direction, height, tile_element);
            break;
        case ENTRANCE_TYPE_PARK_ENTRANCE:
            park_entrance_paint(session, direction, height, tile_element);
            break;
    }
}
