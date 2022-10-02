/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../RideData.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../Vehicle.h"
#include "../VehiclePaint.h"

static uint32_t SubmarineVehicleGetBaseImageId(const Vehicle* vehicle, const CarEntry* carEntry, int32_t imageDirection)
{
    uint32_t result = imageDirection;
    if (vehicle->restraints_position >= 64)
    {
        if ((carEntry->GroupEnabled(SpriteGroupType::RestraintAnimation)) && !(imageDirection & 3))
        {
            auto restraintFrame = ((vehicle->restraints_position - 64) / 64) * 4;
            result = (carEntry->SpriteByYaw(imageDirection, SpriteGroupType::RestraintAnimation) + restraintFrame)
                    * carEntry->base_num_frames
                + carEntry->GroupImageId(SpriteGroupType::RestraintAnimation);
        }
    }
    else
    {
        result = (carEntry->SpriteByYaw(imageDirection, SpriteGroupType::SlopeFlat) * carEntry->base_num_frames)
            + carEntry->GroupImageId(SpriteGroupType::SlopeFlat) + vehicle->SwingSprite;
    }
    return result;
}

/**
 *
 *  rct2: 0x006D44D5
 */
void vehicle_visual_submarine(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    auto baseImageId = SubmarineVehicleGetBaseImageId(vehicle, carEntry, imageDirection);
    auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    if (vehicle->IsGhost())
    {
        imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::Palette44);
        imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::Palette44);
    }

    const auto& bb = VehicleBoundboxes[carEntry->draw_order][OpenRCT2::Entity::Yaw::YawTo16(imageDirection)];
    PaintAddImageAsParent(
        session, imageId0, { 0, 0, z }, { bb.length_x, bb.length_y, bb.length_z },
        { bb.offset_x, bb.offset_y, bb.offset_z + z });
    PaintAddImageAsParent(
        session, imageId1, { 0, 0, z }, { bb.length_x, bb.length_y, 2 }, { bb.offset_x, bb.offset_y, bb.offset_z + z - 10 });
    assert(carEntry->effect_visual == 1);
}

static void submarine_ride_paint_track_station(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const auto* stationObj = ride.GetStationObject();
    int32_t heightLower = height - 16;
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { 20, 32, 3 }, { 6, 0, heightLower });

        PaintUtilPushTunnelRight(session, height, TunnelSquareFlat);
        track_paint_util_draw_pier(
            session, ride, stationObj, session.MapPosition, direction, height, trackElement, session.CurrentRotation);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { 32, 20, 3 }, { 0, 6, heightLower });

        PaintUtilPushTunnelLeft(session, height, TunnelSquareFlat);
        track_paint_util_draw_pier(
            session, ride, stationObj, session.MapPosition, direction, height, trackElement, session.CurrentRotation);
    }

    PaintUtilSetSegmentSupportHeight(session, SegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void submarine_ride_paint_track_flat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    int32_t heightLower = height - 16;
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { 20, 32, 3 }, { 6, 0, heightLower });
        PaintUtilPushTunnelRight(session, heightLower, Tunnel0);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { 32, 20, 3 }, { 0, 6, heightLower });
        PaintUtilPushTunnelLeft(session, heightLower, Tunnel0);
    }

    if (track_paint_util_should_paint_supports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, (direction & 1) ? MetalSupportsStickAlt : MetalSupportsStick, 4, -1, heightLower,
            session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(SegmentD0 | SegmentC4 | SegmentCC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16, 0x20);
}

static void submarine_ride_paint_track_left_quarter_turn_3_tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_3_tiles_paint(
        session, 3, height - 16, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles);
    track_paint_util_left_quarter_turn_3_tiles_tunnel(session, height - 16, Tunnel0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, MetalSupportsStick, 4, -1, height - 16, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentD0 | SegmentC4 | SegmentCC | SegmentB4, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC8 | SegmentC4 | SegmentD0 | SegmentB8, direction), 0xFFFF, 0);
            break;
        case 3:
            MetalASupportsPaintSetup(session, MetalSupportsStick, 4, -1, height - 16, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SegmentC8 | SegmentC4 | SegmentD4 | SegmentC0, direction), 0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 16, 0x20);
}

static constexpr const uint8_t submarine_ride_right_quarter_turn_3_tiles_to_left_turn_map[] = {
    3,
    1,
    2,
    0,
};
static void submarine_ride_paint_track_right_quarter_turn_3_tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = submarine_ride_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    submarine_ride_paint_track_left_quarter_turn_3_tiles(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static void submarine_ride_paint_track_left_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    track_paint_util_left_quarter_turn_1_tile_paint(
        session, 1, height - 16, 0, direction, session.TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile);
    track_paint_util_left_quarter_turn_1_tile_tunnel(session, direction, height - 16, 0, Tunnel0, 0, Tunnel0);

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SegmentB8 | SegmentC8 | SegmentC4 | SegmentD0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16, 0x20);
}

static void submarine_ride_paint_track_right_quarter_turn_1_tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    submarine_ride_paint_track_left_quarter_turn_1_tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/**
 * rct2: 0x008995D4
 */
TRACK_PAINT_FUNCTION get_track_paint_function_submarine_ride(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return submarine_ride_paint_track_station;

        case TrackElemType::Flat:
            return submarine_ride_paint_track_flat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return submarine_ride_paint_track_left_quarter_turn_3_tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return submarine_ride_paint_track_right_quarter_turn_3_tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return submarine_ride_paint_track_left_quarter_turn_1_tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return submarine_ride_paint_track_right_quarter_turn_1_tile;
    }

    return nullptr;
}
