/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../interface/Viewport.h"
#include "../../paint/Paint.h"
#include "../../paint/Supports.h"
#include "../Track.h"
#include "../TrackPaint.h"
#include "../../world/Sprite.h"

enum
{
    PLANE_BACK,
    PLANE_FRONT,
};

enum
{
    SPR_MAGIC_CARPET_FRAME_NW    = 22002,
    SPR_MAGIC_CARPET_FRAME_SE    = 22003,
    SPR_MAGIC_CARPET_FRAME_NE    = 22004,
    SPR_MAGIC_CARPET_FRAME_SW    = 22005,
    SPR_MAGIC_CARPET_PENDULUM_NW = 22006,
    SPR_MAGIC_CARPET_PENDULUM_NE = 22038,
    SPR_MAGIC_CARPET_PENDULUM_SE = 22070,
    SPR_MAGIC_CARPET_PENDULUM_SW = 22102,
};

struct bound_box
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
};

/** rct2: 0x01428220 */
static constexpr const int16_t MagicCarpetOscillationZ[] = { -2, -1, 1,  5,  10, 16, 23, 30, 37, 45, 52, 59, 65, 70, 74, 76,
                                                  77, 76, 74, 70, 65, 59, 52, 45, 37, 30, 23, 16, 10, 5,  1,  -1 };

/** rct2: 0x01428260 */
static constexpr const int8_t MagicCarpetOscillationXY[] = { 0, 6,  12,  18,  23,  27,  30,  31,  32,  31,  30,  27,  23,  18,  12,  6,
                                                  0, -5, -11, -17, -22, -26, -29, -30, -31, -30, -29, -26, -22, -17, -11, -5 };

/** rct2: 0x014281F0 */
static constexpr const bound_box MagicCarpetBounds[] = { { 0, 8, 32, 16 }, { 8, 0, 16, 32 }, { 0, 8, 32, 16 }, { 8, 0, 16, 32 } };

static rct_vehicle * get_first_vehicle(Ride * ride)
{
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        uint16_t vehicleSpriteIndex = ride->vehicles[0];
        if (vehicleSpriteIndex != SPRITE_INDEX_NULL)
        {
            return GET_VEHICLE(vehicleSpriteIndex);
        }
    }
    return nullptr;
}

static void paint_magic_carpet_frame(paint_session * session, uint8_t plane, uint8_t direction, LocationXYZ16 offset,
                                     LocationXYZ16 bbOffset, LocationXYZ16 bbSize)
{
    uint32_t imageId;
    if (direction & 1)
    {
        imageId = plane == PLANE_BACK ? SPR_MAGIC_CARPET_FRAME_NE : SPR_MAGIC_CARPET_FRAME_SW;
    }
    else
    {
        imageId = plane == PLANE_BACK ? SPR_MAGIC_CARPET_FRAME_NW : SPR_MAGIC_CARPET_FRAME_SE;
    }
    imageId |= session->TrackColours[SCHEME_TRACK];
    if (plane == PLANE_BACK)
    {
        sub_98197C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y,
            bbOffset.z);
    }
    else
    {
        sub_98199C(
            session, imageId, (int8_t)offset.x, (int8_t)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y,
            bbOffset.z);
    }
}

static void paint_magic_carpet_pendulum(paint_session * session, uint8_t plane, uint32_t swingImageId, uint8_t direction,
                                        LocationXYZ16 offset, LocationXYZ16 bbOffset, LocationXYZ16 bbSize)
{
    uint32_t imageId = swingImageId;
    if (direction & 2)
    {
        imageId = (0 - ((int32_t)imageId)) & 31;
    }
    if (direction & 1)
    {
        imageId += plane == PLANE_BACK ? SPR_MAGIC_CARPET_PENDULUM_NE : SPR_MAGIC_CARPET_PENDULUM_SW;
    }
    else
    {
        imageId += plane == PLANE_BACK ? SPR_MAGIC_CARPET_PENDULUM_NW : SPR_MAGIC_CARPET_PENDULUM_SE;
    }
    imageId |= session->TrackColours[SCHEME_TRACK];
    sub_98199C(
        session, imageId, (int8_t)offset.x, (int8_t)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x, bbOffset.y,
        bbOffset.z);
}

static void paint_magic_carpet_vehicle(paint_session * session, Ride * ride, uint8_t direction, uint32_t swingImageId,
                                       LocationXYZ16 offset, LocationXYZ16 bbOffset, LocationXYZ16 bbSize)
{
    rct_ride_entry * rideEntry      = get_ride_entry_by_ride(ride);
    uint32_t           vehicleImageId = rideEntry->vehicles[0].base_image_id + direction;

    // Vehicle
    uint32_t imageColourFlags = session->TrackColours[SCHEME_MISC];
    if (imageColourFlags == IMAGE_TYPE_REMAP)
    {
        imageColourFlags =
            SPRITE_ID_PALETTE_COLOUR_2(ride->vehicle_colours[0].body_colour, ride->vehicle_colours[0].trim_colour);
    }

    int8_t directionalOffset = MagicCarpetOscillationXY[swingImageId];
    switch (direction)
    {
    case 0:
        offset.x -= directionalOffset;
        break;
    case 1:
        offset.y += directionalOffset;
        break;
    case 2:
        offset.x += directionalOffset;
        break;
    case 3:
        offset.y -= directionalOffset;
        break;
    }
    offset.z += MagicCarpetOscillationZ[swingImageId];

    sub_98199C(
        session, vehicleImageId | imageColourFlags, (int8_t)offset.x, (int8_t)offset.y, bbSize.x, bbSize.y, 127, offset.z,
        bbOffset.x, bbOffset.y, bbOffset.z);

    // Riders
    rct_drawpixelinfo * dpi = session->DPI;
    if (dpi->zoom_level <= 1 && (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        rct_vehicle * vehicle = get_first_vehicle(ride);
        if (vehicle != nullptr)
        {
            uint32_t baseImageId = IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS | (vehicleImageId + 4);
            for (uint8_t peepIndex = 0; peepIndex < vehicle->num_peeps; peepIndex += 2)
            {
                uint32_t imageId = baseImageId + (peepIndex * 2);
                imageId |= (vehicle->peep_tshirt_colours[peepIndex + 0] << 19);
                imageId |= (vehicle->peep_tshirt_colours[peepIndex + 1] << 24);
                sub_98199C(
                    session, imageId, (int8_t)offset.x, (int8_t)offset.y, bbSize.x, bbSize.y, 127, offset.z, bbOffset.x,
                    bbOffset.y, bbOffset.z);
            }
        }
    }
}

/** rct2: 0x00899104 */
static void paint_magic_carpet_structure(paint_session * session, Ride * ride, uint8_t direction, int8_t axisOffset, uint16_t height)
{
    const rct_tile_element * savedTileElement = static_cast<const rct_tile_element *>(session->CurrentlyDrawnItem);
    rct_vehicle *     vehicle         = get_first_vehicle(ride);

    uint32_t swingImageId = 0;
    if (vehicle != nullptr)
    {
        swingImageId                = vehicle->vehicle_sprite_type;
        session->InteractionType    = VIEWPORT_INTERACTION_ITEM_SPRITE;
        session->CurrentlyDrawnItem = vehicle;
    }

    bound_box bb = MagicCarpetBounds[direction];
    LocationXYZ16 offset, bbOffset, bbSize;
    offset.x   = (direction & 1) ? 0 : axisOffset;
    offset.y   = (direction & 1) ? axisOffset : 0;
    offset.z   = height + 7;
    bbOffset.x = bb.x;
    bbOffset.y = bb.y;
    bbOffset.z = height + 7;
    bbSize.x   = bb.width;
    bbSize.y   = bb.height;
    bbSize.z   = 127;

    paint_magic_carpet_frame(session, PLANE_BACK, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_pendulum(session, PLANE_BACK, swingImageId, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_vehicle(session, ride, direction, swingImageId, offset, bbOffset, bbSize);
    paint_magic_carpet_pendulum(session, PLANE_FRONT, swingImageId, direction, offset, bbOffset, bbSize);
    paint_magic_carpet_frame(session, PLANE_FRONT, direction, offset, bbOffset, bbSize);

    session->InteractionType    = VIEWPORT_INTERACTION_ITEM_RIDE;
    session->CurrentlyDrawnItem = savedTileElement;
}

/** rct2: 0x00898514 */
static void paint_magic_carpet(
    paint_session *          session,
    uint8_t                    rideIndex,
    uint8_t                    trackSequence,
    uint8_t                    direction,
    int32_t                   height,
    const rct_tile_element * tileElement)
{
    uint8_t relativeTrackSequence = track_map_1x4[direction][trackSequence];

    // The end tiles do not have a platform
    switch (relativeTrackSequence)
    {
    case 0:
    case 2:
        if (direction & 1)
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 6, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 7, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }
        else
        {
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 5, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
            metal_a_supports_paint_setup(session, METAL_SUPPORTS_TUBES, 8, 0, height, session->TrackColours[SCHEME_SUPPORTS]);
        }

        uint32_t imageId = SPR_STATION_BASE_D | session->TrackColours[SCHEME_SUPPORTS];
        sub_98196C(session, imageId, 0, 0, 32, 32, 1, height);
        break;
    }

    Ride * ride = get_ride(rideIndex);
    switch (relativeTrackSequence)
    {
    case 3:
        paint_magic_carpet_structure(session, ride, direction, -48, height);
        break;
    case 0:
        paint_magic_carpet_structure(session, ride, direction, -16, height);
        break;
    case 2:
        paint_magic_carpet_structure(session, ride, direction, 16, height);
        break;
    case 1:
        paint_magic_carpet_structure(session, ride, direction, 48, height);
        break;
    }

    paint_util_set_segment_support_height(session, SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_general_support_height(session, height + 176, 0x20);
}

/**
 *
 *  rct2: 0x00898384
 */
TRACK_PAINT_FUNCTION get_track_paint_function_magic_carpet(int32_t trackType, int32_t direction)
{
    switch (trackType)
    {
    case FLAT_TRACK_ELEM_1_X_4_A:
        return paint_magic_carpet;
    }
    return nullptr;
}
