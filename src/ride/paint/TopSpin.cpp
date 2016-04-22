#include "paint.h"

extern "C"
{
    #include "../../world/sprite.h"
    #include "../../interface/viewport.h"
}

namespace TopSpin
{
    /**
     *
     *  rct2: 0x0076750D
     */
    void DrawVehicle(rct_ride * ride, uint8 direction, sint8 al, sint8 cl, TileDrawingContext * dc)
    {
        uint16 boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ;
        rct_ride_entry * rideEntry = get_ride_entry_by_ride(ride);
        /**
         *
         *  rct2: 0x0142811C
         * Can be calculated as Rounddown(34*sin(x)+0.5)
         * where x is in 7.5 deg segments.
         */
        const static sint8 TopSpinSeatPositionOffset[] = {
            0,   4,   9,  13,  17,  21,  24,  27,  29,  31,  33,  34,  34,  34,  33,  31,
            29,  27,  24,  21,  17,  13,   9,   4,   0,  -3,  -8, -12, -16, -20, -23, -26,
            -28, -30, -32, -33, -33, -33, -32, -30, -28, -26, -23, -20, -16, -12,  -8,  -3,
            0
        };

        // As we will be drawing a vehicle we need to backup the mapElement that
        // is assigned to the drawings.
        rct_map_element * curMapElement = RCT2_GLOBAL(0x009DE578, rct_map_element*);

        int height = 3;

        rct_vehicle * vehicle = NULL;

        uint8 seatRotation = 0;
        sint8 armRotation = 0;

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK &&
            ride->vehicles[0] != SPRITE_INDEX_NULL)
        {
            vehicle = GET_VEHICLE(ride->vehicles[0]);

            RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_SPRITE;
            RCT2_GLOBAL(0x009DE578, rct_vehicle*) = vehicle;

            armRotation = vehicle->vehicle_sprite_type;
            seatRotation = vehicle->bank_rotation;
        }

        boundBoxOffsetX = al + 16;
        boundBoxOffsetY = cl + 16;
        boundBoxOffsetZ = height;

        uint8 lengthX = 24; // di
        uint8 lengthY = 24; // si

        uint32 image_id = RCT2_GLOBAL(0x00F441A0, uint32);
        if (image_id == 0x20000000)
        {
            image_id = 0xA0000000 |
                       (ride->track_colour_main[0] << 19) |
                       (ride->track_colour_supports[0] << 24);
        }

        image_id += (direction & 1) << 1;
        image_id += rideEntry->vehicles[0].base_image_id;
        // Left back bottom support
        image_id += 572;

        dc->draw_98197C(image_id, al, cl, lengthX, lengthY, 90, height, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);

        image_id = RCT2_GLOBAL(0x00F441A0, uint32);
        if (image_id == 0x20000000) {
            image_id = 0xA0000000 |
                       (ride->track_colour_main[0] << 19) |
                       (ride->track_colour_additional[0] << 24);
        }

        sint32 var_1F = armRotation;
        if (direction & 2)
        {
            var_1F = -var_1F;
            if (var_1F != 0)
            {
                var_1F += 48;
            }
        }
        image_id += var_1F;
        image_id += (direction & 1) * 48;
        image_id += rideEntry->vehicles[0].base_image_id;
        // Left hand arm
        image_id += 380;

        dc->draw_98199C(image_id,
                        al,
                        cl,
                        lengthX,
                        lengthY,
                        90,
                        height,
                        boundBoxOffsetX,
                        boundBoxOffsetY,
                        boundBoxOffsetZ);

        uint32 seatImageId;

        if (vehicle != NULL && vehicle->restraints_position >= 64)
        {
            // Open Restraints
            image_id = (vehicle->restraints_position - 64) >> 6;
            image_id += direction * 3;
            image_id += rideEntry->vehicles[0].base_image_id;
            image_id += 64;
            seatImageId = image_id;
        }
        else
        {
            image_id = direction * 16;
            // Var_20 Rotation of seats
            image_id += seatRotation;
            image_id += rideEntry->vehicles[0].base_image_id;
            seatImageId = image_id;
        }

        image_id = RCT2_GLOBAL(0x00F441A0, uint32);
        if (image_id == 0x20000000)
        {
            image_id = 0xA0000000 |
                       (ride->vehicle_colours[0].body_colour << 19) |
                       (ride->vehicle_colours[0].trim_colour << 24);
        }
        image_id += seatImageId;

        rct_xyz16 seatCoords;
        seatCoords.x = al;
        seatCoords.y = cl;
        seatCoords.z = height;
        seatCoords.z += RCT2_ADDRESS(0x14280BC, sint16)[armRotation];

        assert(armRotation < sizeof(TopSpinSeatPositionOffset));
        switch (direction) {
            case 0:
                seatCoords.x -= TopSpinSeatPositionOffset[armRotation];
                break;
            case 1:
                seatCoords.y += TopSpinSeatPositionOffset[armRotation];
                break;
            case 2:
                seatCoords.x += TopSpinSeatPositionOffset[armRotation];
                break;
            case 3:
                seatCoords.y -= TopSpinSeatPositionOffset[armRotation];
                break;
        }

        RCT2_GLOBAL(0x014280B8, sint8) = (sint8)seatCoords.x;
        RCT2_GLOBAL(0x014280B9, sint8) = (sint8)seatCoords.y;
        RCT2_GLOBAL(0x014280BA, sint16) = seatCoords.z;

        dc->draw_98199C(image_id,
                        (sint8)seatCoords.x,
                        (sint8)seatCoords.y,
                        lengthX,
                        lengthY,
                        90,
                        seatCoords.z,
                        boundBoxOffsetX,
                        boundBoxOffsetY,
                        boundBoxOffsetZ);

        rct_drawpixelinfo * dpi = RCT2_GLOBAL(0x140E9A8, rct_drawpixelinfo*);
        if (dpi->zoom_level < 2 && vehicle != NULL && vehicle->num_peeps != 0)
        {
            image_id = (vehicle->peep_tshirt_colours[0] << 19) |
                       (vehicle->peep_tshirt_colours[1] << 24);
            image_id += seatImageId;
            image_id += 0xA0000000;
            image_id += 76;

            dc->draw_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);

            if (vehicle->num_peeps > 2)
            {
                image_id = (vehicle->peep_tshirt_colours[2] << 19) |
                           (vehicle->peep_tshirt_colours[3] << 24);
                image_id += seatImageId;
                image_id += 0xA0000000;
                image_id += 152;

                dc->draw_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
            }

            if (vehicle->num_peeps > 4)
            {
                image_id = (vehicle->peep_tshirt_colours[4] << 19) |
                           (vehicle->peep_tshirt_colours[5] << 24);
                image_id += seatImageId;
                image_id += 0xA0000000;
                image_id += 228;

                dc->draw_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
            }

            if (vehicle->num_peeps > 6)
            {
                image_id = (vehicle->peep_tshirt_colours[6] << 19) |
                           (vehicle->peep_tshirt_colours[7] << 24);
                image_id += seatImageId;
                image_id += 0xA0000000;
                image_id += 304;

                dc->draw_98199C(image_id, (sint8)seatCoords.x, (sint8)seatCoords.y, lengthX, lengthY, 90, seatCoords.z, boundBoxOffsetX, boundBoxOffsetY, boundBoxOffsetZ);
            }
        }

        image_id = RCT2_GLOBAL(0x00F441A0, uint32);
        if (image_id == 0x20000000)
        {
            image_id = 0xA0000000 |
                       (ride->track_colour_main[0] << 19) |
                       (ride->track_colour_additional[0] << 24);
        }

        image_id += var_1F;
        image_id += (direction & 1) * 48;
        image_id += rideEntry->vehicles[0].base_image_id;
        // Right hand arm
        image_id += 476;

        dc->draw_98199C(image_id,
                        al,
                        cl,
                        lengthX,
                        lengthY,
                        90,
                        height,
                        boundBoxOffsetX,
                        boundBoxOffsetY,
                        boundBoxOffsetZ);

        image_id = RCT2_GLOBAL(0x00F441A0, uint32);
        if (image_id == 0x20000000)
        {
            image_id = 0xA0000000 |
                       (ride->track_colour_main[0] << 19) |
                       (ride->track_colour_supports[0] << 24);
        }

        image_id += (direction & 1) << 1;
        image_id += rideEntry->vehicles[0].base_image_id;
        // Right back bottom support
        image_id += 573;

        dc->draw_98199C(image_id,
                        al,
                        cl,
                        lengthX,
                        lengthY,
                        90,
                        height,
                        boundBoxOffsetX,
                        boundBoxOffsetY,
                        boundBoxOffsetZ);

        RCT2_GLOBAL(0x009DE578, rct_map_element*) = curMapElement;
        RCT2_GLOBAL(RCT2_ADDRESS_PAINT_SETUP_CURRENT_TYPE, uint8) = VIEWPORT_INTERACTION_ITEM_RIDE;
    }

    static uint16 TransformArg(uint16 arg, sint32 height)
    {
        if (arg == 2) return height + 2;
        return arg;
    }

    /**
     *
     *  rct2: 0x0076659C
     */
    void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element * mapElement, TileDrawingContext * context)
    {
        if (trackType != FLAT_RIDE_ELEM_TOP_SPIN) return;
        if (trackSequence > 8) return;

        struct TopSpinTileInfo
        {
            uint8 fences;
            sint8 vehicle_offset_x;
            sint8 vehicle_offset_y;
            uint8 max_height;
            uint8 mapDirection[4];
            uint16 unk_args[12];
        };

        const static TopSpinTileInfo TopSpinTrackSeqFenceMap[] = {
            { 0,                  0,   0, 112, 0, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
            { DF_NE | DF_NW,     32,  32, 110, 1, 3, 7, 6, 2,          32, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32,      2,     32, 0xFFFF, 0xFFFF },
            { DF_NE,              0,   0, 110, 2, 5, 8, 4, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
            { DF_NE | DF_SE,     32, -32, 110, 3, 7, 6, 1, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF,      2,     32 },
            { DF_NW,              0,   0, 110, 4, 2, 5, 8, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
            { DF_SE,              0, -32, 112, 5, 8, 4, 2, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
            { DF_SW | DF_NW,    -32,  32, 110, 6, 1, 3, 7, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF,      2,     32, 0xFFFF },
            { DF_SW | DF_SE,    -32, -32, 110, 7, 6, 1, 3, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32, 0xFFFF, 0xFFFF, 0xFFFF,      2,     32,      2,     32 },
            { DF_SW,            -32,   0, 112, 8, 4, 2, 5, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
        };


        context->DrawSupports(SUPPORT_STYLE_WOOD, 0, context->Z, RCT2_GLOBAL(0x00F441A0, uint32));
        RideDrawingUtils::DrawFloor(FLOOR_STYLE_BROWN_RUBBER, RCT2_GLOBAL(0x00F44198, uint32), context->Z, ride, context);
        uint8 fences = TopSpinTrackSeqFenceMap[trackSequence].fences;
        RideDrawingUtils::DrawFencesChecked(fences, 0, context->Z, ride, context);

        int correctedSequence = TopSpinTrackSeqFenceMap[direction].mapDirection[direction];
        const TopSpinTileInfo * ti = &TopSpinTrackSeqFenceMap[correctedSequence];

        if (ti->vehicle_offset_x != 0 || ti->vehicle_offset_y != 0)
        {
            DrawVehicle(ride, direction, ti->vehicle_offset_x, ti->vehicle_offset_y, context);
        }

        context->SetSupportSegmentZ(-1, -1, TransformArg(ti->unk_args[0], context->Z));
        RCT2_GLOBAL(0x141E9B6, uint16) = TransformArg(ti->unk_args[1], context->Z);
        context->SetSupportSegmentZ(-1, +1, TransformArg(ti->unk_args[2], context->Z));
        context->SetSupportSegmentZ(+1, -1, TransformArg(ti->unk_args[3], context->Z));
        context->SetSupportSegmentZ(+1, +1, TransformArg(ti->unk_args[4], context->Z));
        context->SetSupportSegmentZ(0, 0, TransformArg(ti->unk_args[5], context->Z));
        context->SetSupportSegmentZ(-1, 0, TransformArg(ti->unk_args[6], context->Z));
        RCT2_GLOBAL(0x141E9CA, uint16) = TransformArg(ti->unk_args[7], context->Z);
        context->SetSupportSegmentZ(0, -1, TransformArg(ti->unk_args[8], context->Z));
        RCT2_GLOBAL(0x141E9CE, uint16) = TransformArg(ti->unk_args[9], context->Z);
        context->SetSupportSegmentZ(0, +1, TransformArg(ti->unk_args[10], context->Z));
        context->SetSupportSegmentZ(+1, 0, TransformArg(ti->unk_args[11], context->Z));

        context->UpdateTileMaxHeight(ti->max_height, 0x20);
    }
}