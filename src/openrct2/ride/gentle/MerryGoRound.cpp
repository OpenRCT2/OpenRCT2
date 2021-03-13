/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../../world/Sprite.h"
#include "../Track.h"
#include "../TrackPaint.h"

/** rct2: 0x0142805C */
static constexpr const uint32_t merry_go_round_rider_offsets[] = { 0, 32, 64, 96, 16, 48, 80, 112 };

/** rct2: 0x0142807C */
static constexpr const uint16_t merry_go_round_breakdown_vibration[] = { 0, 1, 2, 3, 4, 3, 2, 1, 0, 0 };

/**
 * rct2: 0x0076287D
 */
static void paint_merry_go_round_structure(
    paint_session* session, ride_id_t rideIndex, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height)
{
    const TileElement* savedTileElement = static_cast<const TileElement*>(session->CurrentlyDrawnItem);
    height += 7;

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    uint32_t baseImageId = rideEntry->vehicles[0].base_image_id;
    auto vehicle = GetEntity<Vehicle>(ride->vehicles[0]);
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        session->InteractionType = ViewportInteractionItem::Entity;
        session->CurrentlyDrawnItem = vehicle;

        if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN)
            && ride->breakdown_reason_pending == BREAKDOWN_CONTROL_FAILURE && ride->breakdown_sound_modifier >= 128)
        {
            height += merry_go_round_breakdown_vibration[(vehicle->current_time >> 1) & 7];
        }
    }

    uint32_t rotationOffset = 0;
    if (vehicle != nullptr)
    {
        uint32_t rotation = ((vehicle->sprite_direction >> 3) + session->CurrentRotation) << 5;
        rotationOffset = (vehicle->vehicle_sprite_type + rotation) % 128;
    }

    uint32_t imageOffset = rotationOffset & 0x1F;

    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].Body, ride->vehicle_colours[0].Trim);
    }

    uint32_t imageId = (baseImageId + imageOffset) | imageColourFlags;
    PaintAddImageAsParent(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height);

    rct_drawpixelinfo* dpi = &session->DPI;
    if (dpi->zoom_level == 0 && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK && vehicle != nullptr)
    {
        for (int32_t peep = 0; peep <= 14; peep += 2)
        {
            if (vehicle->num_peeps <= peep)
            {
                break;
            }

            imageOffset = (merry_go_round_rider_offsets[peep / 2] + rotationOffset) % 128;
            imageOffset -= 13;

            if (imageOffset >= 68)
            {
                continue;
            }

            imageColourFlags = SPRITE_ID_PALETTE_COLOUR_2(
                vehicle->peep_tshirt_colours[peep], vehicle->peep_tshirt_colours[peep + 1]);
            imageId = (baseImageId + 32 + imageOffset) | imageColourFlags;
            PaintAddImageAsChild(session, imageId, xOffset, yOffset, 24, 24, 48, height, xOffset + 16, yOffset + 16, height);
        }
    }

    session->CurrentlyDrawnItem = savedTileElement;
    session->InteractionType = ViewportInteractionItem::Ride;
}

/**
 * rct2: 0x00761B0C
 */
static void paint_merry_go_round(
    paint_session* session, ride_id_t rideIndex, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TileElement* tileElement)
{
    trackSequence = track_map_3x3[direction][trackSequence];

    int32_t edges = edges_3x3[trackSequence];

    wooden_a_supports_paint_setup(session, (direction & 1), 0, height, session->TrackColours[SCHEME_MISC], nullptr);

    track_paint_util_paint_floor(session, edges, session->TrackColours[SCHEME_TRACK], height, floorSpritesCork);

    auto ride = get_ride(rideIndex);
    if (ride != nullptr)
    {
        track_paint_util_paint_fences(
            session, edges, session->MapPosition, tileElement, ride, session->TrackColours[SCHEME_MISC], height,
            fenceSpritesRope, session->CurrentRotation);
    }

    switch (trackSequence)
    {
        case 1:
            paint_merry_go_round_structure(session, rideIndex, direction, 32, 32, height);
            break;
        case 3:
            paint_merry_go_round_structure(session, rideIndex, direction, 32, -32, height);
            break;
        case 5:
            paint_merry_go_round_structure(session, rideIndex, direction, 0, -32, height);
            break;
        case 6:
            paint_merry_go_round_structure(session, rideIndex, direction, -32, 32, height);
            break;
        case 7:
            paint_merry_go_round_structure(session, rideIndex, direction, -32, -32, height);
            break;
        case 8:
            paint_merry_go_round_structure(session, rideIndex, direction, -32, 0, height);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 1:
            // top
            cornerSegments = SEGMENT_B4 | SEGMENT_C8 | SEGMENT_CC;
            break;
        case 3:
            // right
            cornerSegments = SEGMENT_CC | SEGMENT_BC | SEGMENT_D4;
            break;
        case 6:
            // left
            cornerSegments = SEGMENT_C8 | SEGMENT_B8 | SEGMENT_D0;
            break;
        case 7:
            // bottom
            cornerSegments = SEGMENT_D0 | SEGMENT_C0 | SEGMENT_D4;
            break;
    }

    paint_util_set_segment_support_height(session, cornerSegments, height + 2, 0x20);
    paint_util_set_segment_support_height(session, SEGMENTS_ALL & ~cornerSegments, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 64, 0x20);
}

/**
 * rct2: 0x0076190C
 */
TRACK_PAINT_FUNCTION get_track_paint_function_merry_go_round(int32_t trackType)
{
    if (trackType != TrackElemType::FlatTrack3x3)
    {
        return nullptr;
    }

    return paint_merry_go_round;
}
