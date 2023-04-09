/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
void VehicleVisualSubmarine(
    PaintSession& session, int32_t x, int32_t imageDirection, int32_t y, int32_t z, const Vehicle* vehicle,
    const CarEntry* carEntry)
{
    auto baseImageId = SubmarineVehicleGetBaseImageId(vehicle, carEntry, imageDirection);
    auto imageId0 = ImageId(baseImageId + 0, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    auto imageId1 = ImageId(baseImageId + 1, vehicle->colours.Body, vehicle->colours.Trim, vehicle->colours.Tertiary);
    if (vehicle->IsGhost())
    {
        imageId0 = ImageId(baseImageId + 0).WithRemap(FilterPaletteID::PaletteGhost);
        imageId1 = ImageId(baseImageId + 1).WithRemap(FilterPaletteID::PaletteGhost);
    }

    const auto& bb = VehicleBoundboxes[carEntry->draw_order][OpenRCT2::Entity::Yaw::YawTo16(imageDirection)];
    PaintAddImageAsParent(
        session, imageId0, { 0, 0, z },
        { { bb.offset_x, bb.offset_y, bb.offset_z + z }, { bb.length_x, bb.length_y, bb.length_z } });
    PaintAddImageAsParent(
        session, imageId1, { 0, 0, z },
        { { bb.offset_x, bb.offset_y, bb.offset_z + z - 10 }, { bb.length_x, bb.length_y, 2 } });
    assert(carEntry->effect_visual == 1);
}

static void SubmarineRidePaintTrackStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    const auto* stationObj = ride.GetStationObject();
    int32_t heightLower = height - 16;
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { { 6, 0, heightLower }, { 20, 32, 3 } });

        PaintUtilPushTunnelRight(session, height, TUNNEL_SQUARE_FLAT);
        TrackPaintUtilDrawPier(
            session, ride, stationObj, session.MapPosition, direction, height, trackElement, session.CurrentRotation);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { { 0, 6, heightLower }, { 32, 20, 3 } });

        PaintUtilPushTunnelLeft(session, height, TUNNEL_SQUARE_FLAT);
        TrackPaintUtilDrawPier(
            session, ride, stationObj, session.MapPosition, direction, height, trackElement, session.CurrentRotation);
    }

    PaintUtilSetSegmentSupportHeight(session, SEGMENTS_ALL, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 32, 0x20);
}

static void SubmarineRidePaintTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    int32_t heightLower = height - 16;
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_SE_NW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { { 6, 0, heightLower }, { 20, 32, 3 } });
        PaintUtilPushTunnelRight(session, heightLower, TUNNEL_0);
    }
    else
    {
        imageId = session.TrackColours[SCHEME_TRACK].WithIndex(SPR_TRACK_SUBMARINE_RIDE_MINI_HELICOPTERS_FLAT_NE_SW);
        PaintAddImageAsParent(session, imageId, { 0, 0, heightLower }, { { 0, 6, heightLower }, { 32, 20, 3 } });
        PaintUtilPushTunnelLeft(session, heightLower, TUNNEL_0);
    }

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetup(
            session, (direction & 1) ? METAL_SUPPORTS_STICK_ALT : METAL_SUPPORTS_STICK, 4, -1, heightLower,
            session.TrackColours[SCHEME_SUPPORTS]);
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16, 0x20);
}

static void SubmarineRidePaintTrackLeftQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilLeftQuarterTurn3TilesPaint(
        session, 3, height - 16, direction, trackSequence, session.TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn3Tiles);
    TrackPaintUtilLeftQuarterTurn3TilesTunnel(session, height - 16, TUNNEL_0, direction, trackSequence);

    switch (trackSequence)
    {
        case 0:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_STICK, 4, -1, height - 16, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_D0 | SEGMENT_C4 | SEGMENT_CC | SEGMENT_B4, direction), 0xFFFF, 0);
            break;
        case 2:
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0 | SEGMENT_B8, direction), 0xFFFF, 0);
            break;
        case 3:
            MetalASupportsPaintSetup(session, METAL_SUPPORTS_STICK, 4, -1, height - 16, session.TrackColours[SCHEME_SUPPORTS]);
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D4 | SEGMENT_C0, direction), 0xFFFF, 0);
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
static void SubmarineRidePaintTrackRightQuarterTurn3Tiles(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    trackSequence = submarine_ride_right_quarter_turn_3_tiles_to_left_turn_map[trackSequence];
    SubmarineRidePaintTrackLeftQuarterTurn3Tiles(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

static void SubmarineRidePaintTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 1, height - 16, 0, direction, session.TrackColours[SCHEME_TRACK],
        trackSpritesSubmarineRideMiniHelicoptersQuarterTurn1Tile);
    TrackPaintUtilLeftQuarterTurn1TileTunnel(session, direction, height - 16, 0, TUNNEL_0, 0, TUNNEL_0);

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(SEGMENT_B8 | SEGMENT_C8 | SEGMENT_C4 | SEGMENT_D0, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 16, 0x20);
}

static void SubmarineRidePaintTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement)
{
    SubmarineRidePaintTrackLeftQuarterTurn1Tile(session, ride, trackSequence, (direction + 3) % 4, height, trackElement);
}

/**
 * rct2: 0x008995D4
 */
TRACK_PAINT_FUNCTION GetTrackPaintFunctionSubmarineRide(int32_t trackType)
{
    switch (trackType)
    {
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return SubmarineRidePaintTrackStation;

        case TrackElemType::Flat:
            return SubmarineRidePaintTrackFlat;

        case TrackElemType::LeftQuarterTurn3Tiles:
            return SubmarineRidePaintTrackLeftQuarterTurn3Tiles;
        case TrackElemType::RightQuarterTurn3Tiles:
            return SubmarineRidePaintTrackRightQuarterTurn3Tiles;

        case TrackElemType::LeftQuarterTurn1Tile:
            return SubmarineRidePaintTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return SubmarineRidePaintTrackRightQuarterTurn1Tile;
    }

    return nullptr;
}
