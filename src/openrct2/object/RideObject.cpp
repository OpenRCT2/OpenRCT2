#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "ObjectRepository.h"
#include "RideObject.h"

extern "C"
{
    #include "../config/Config.h"
    #include "../drawing/drawing.h"
    #include "../localisation/localisation.h"
    #include "../rct1.h"
    #include "../ride/track.h"
}

RideObject::~RideObject()
{
    for (sint32 i = 0; i < 4; i++)
    {
        Memory::Free(_peepLoadingPositions[i]);
    }
}

void RideObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(8, STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint32>();
    for (sint32 i = 0; i < 3; i++)
    {
        _legacyType.ride_type[i] = stream->ReadValue<uint8>();
    }
    _legacyType.min_cars_in_train = stream->ReadValue<uint8>();
    _legacyType.max_cars_in_train = stream->ReadValue<uint8>();
    _legacyType.cars_per_flat_ride = stream->ReadValue<uint8>();
    _legacyType.zero_cars = stream->ReadValue<uint8>();
    _legacyType.tab_vehicle = stream->ReadValue<uint8>();
    _legacyType.default_vehicle = stream->ReadValue<uint8>();
    _legacyType.front_vehicle = stream->ReadValue<uint8>();
    _legacyType.second_vehicle = stream->ReadValue<uint8>();
    _legacyType.rear_vehicle = stream->ReadValue<uint8>();
    _legacyType.third_vehicle = stream->ReadValue<uint8>();
    _legacyType.pad_019 = stream->ReadValue<uint8>();
    for (sint32 i = 0; i < 4; i++)
    {
        rct_ride_entry_vehicle * entry = &_legacyType.vehicles[i];
        ReadLegacyVehicle(context, stream, entry);
    }
    stream->Seek(4, STREAM_SEEK_CURRENT);
    _legacyType.excitement_multipler = stream->ReadValue<sint8>();
    _legacyType.intensity_multipler = stream->ReadValue<sint8>();
    _legacyType.nausea_multipler = stream->ReadValue<sint8>();
    _legacyType.max_height = stream->ReadValue<uint8>();
    _legacyType.enabledTrackPieces = stream->ReadValue<uint64>();
    _legacyType.category[0] = stream->ReadValue<uint8>();
    _legacyType.category[1] = stream->ReadValue<uint8>();
    _legacyType.shop_item = stream->ReadValue<uint8>();
    _legacyType.shop_item_secondary = stream->ReadValue<uint8>();

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_NAME);
    GetStringTable()->Read(context, stream, OBJ_STRING_ID_DESCRIPTION);

    // TODO: Move to its own function when ride construction window is merged.
    if (gConfigInterface.select_by_track_type)
    {
        _legacyType.enabledTrackPieces = 0xFFFFFFFFFFFFFFFF;
    }
    else
    {
        // When not in select by track type mode, add boosters if the track type is eligible
        for (sint32 i = 0; i < 3; i++)
        {
            if (_legacyType.ride_type[i] == RIDE_TYPE_LOOPING_ROLLER_COASTER ||
                _legacyType.ride_type[i] == RIDE_TYPE_CORKSCREW_ROLLER_COASTER ||
                _legacyType.ride_type[i] == RIDE_TYPE_TWISTER_ROLLER_COASTER ||
                _legacyType.ride_type[i] == RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER ||
                _legacyType.ride_type[i] == RIDE_TYPE_GIGA_COASTER ||
                _legacyType.ride_type[i] == RIDE_TYPE_JUNIOR_ROLLER_COASTER)
            {

                _legacyType.enabledTrackPieces |= (1ULL << TRACK_BOOSTER);
            }
        }
    }

    GetStringTable()->Read(context, stream, OBJ_STRING_ID_CAPACITY);

    // Read preset colours, by default there are 32
    _presetColours.count = stream->ReadValue<uint8>();

    sint32 coloursCount = _presetColours.count;
    // To indicate a ride has different colours each train the count
    // is set to 255. There are only actually 32 colours though.
    if (coloursCount == 255)
    {
        coloursCount = 32;
    }

    for (uint8 i = 0; i < coloursCount; i++)
    {
        _presetColours.list[i] = stream->ReadValue<vehicle_colour>();
    }

    // Read peep loading positions
    for (sint32 i = 0; i < 4; i++)
    {
        uint16 numPeepLoadingPositions = stream->ReadValue<uint8>();
        if (numPeepLoadingPositions == 255)
        {
            numPeepLoadingPositions = stream->ReadValue<uint16>();
        }
        _peepLoadingPositions[i] = stream->ReadArray<sint8>(numPeepLoadingPositions);
    }

    GetImageTable()->Read(context, stream);

    // Validate properties
    if (_legacyType.excitement_multipler > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Excitement multiplier too high.");
    }
    if (_legacyType.intensity_multipler > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Intensity multiplier too high.");
    }
    if (_legacyType.nausea_multipler > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Nausea multiplier too high.");
    }

    PerformRCT1CompatibilityFixes();
}

void RideObject::Load()
{
    GetStringTable()->Sort();
    _legacyType.name = language_allocate_object_string(GetName());
    _legacyType.description = language_allocate_object_string(GetDescription());
    _legacyType.images_offset = gfx_object_allocate_images(GetImageTable()->GetImages(), GetImageTable()->GetCount());
    _legacyType.vehicle_preset_list = &_presetColours;

    sint32 cur_vehicle_images_offset = _legacyType.images_offset + 3;
    for (sint32 i = 0; i < 4; i++)
    {
        rct_ride_entry_vehicle * vehicleEntry = &_legacyType.vehicles[i];
        if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT)
        {
            sint32 al = 1;
            if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_SWINGING)
            {
                al = 13;
                if ((vehicleEntry->flags_b & (VEHICLE_ENTRY_FLAG_B_5 | VEHICLE_ENTRY_FLAG_B_11)) != (VEHICLE_ENTRY_FLAG_B_5 | VEHICLE_ENTRY_FLAG_B_11))
                {
                    al = 7;
                    if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_5))
                    {
                        if (!(vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_11))
                        {
                            al = 5;
                            if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_9)
                            {
                                al = 3;
                            }
                        }
                    }
                }
            }
            vehicleEntry->var_03 = al;
            // 0x6DE90B
            al = 0x20;
            if (!(vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_14))
            {
                al = 1;
                if (vehicleEntry->flags_b & VEHICLE_ENTRY_FLAG_B_7)
                {
                    if (vehicleEntry->var_11 != 6)
                    {
                        al = 2;
                        if (!(vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_7))
                        {
                            al = 4;
                        }
                    }
                }
            }
            if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_12)
            {
                al = vehicleEntry->special_frames;
            }
            vehicleEntry->var_02 = al;
            // 0x6DE946

            vehicleEntry->var_16 = vehicleEntry->var_02 * vehicleEntry->var_03;
            vehicleEntry->base_image_id = cur_vehicle_images_offset;
            sint32 image_index = vehicleEntry->base_image_id;

            if (vehicleEntry->car_visual != VEHICLE_VISUAL_RIVER_RAPIDS)
            {
                sint32 b = vehicleEntry->var_16 * 32;

                if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_11) b /= 2;
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_15) b /= 8;

                image_index += b;

                // Incline 25
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPES)
                {
                    vehicleEntry->var_20 = image_index;
                    b = vehicleEntry->var_16 * 72;
                    if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_14)
                    {
                        b = vehicleEntry->var_16 * 16;
                    }
                    image_index += b;
                }

                // Incline 60
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_STEEP_SLOPES)
                {
                    vehicleEntry->var_24 = image_index;
                    b = vehicleEntry->var_16 * 80;
                    image_index += b;
                }

                // Vertical
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_VERTICAL_SLOPES)
                {
                    vehicleEntry->var_28 = image_index;
                    b = vehicleEntry->var_16 * 116;
                    image_index += b;
                }

                // Unknown
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_DIAGONAL_SLOPES)
                {
                    vehicleEntry->var_2C = image_index;
                    b = vehicleEntry->var_16 * 24;
                    image_index += b;
                }

                // Bank
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_BANKED)
                {
                    vehicleEntry->var_30 = image_index;
                    b = vehicleEntry->var_16 * 80;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_INLINE_TWISTS)
                {
                    vehicleEntry->var_34 = image_index;
                    b = vehicleEntry->var_16 * 40;
                    image_index += b;
                }

                // Track half? Up/Down
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->var_38 = image_index;
                    b = vehicleEntry->var_16 * 128;
                    image_index += b;
                }

                // Unknown
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->var_3C = image_index;
                    b = vehicleEntry->var_16 * 16;
                    image_index += b;
                }

                // Unknown
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->var_40 = image_index;
                    b = vehicleEntry->var_16 * 16;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS)
                {
                    vehicleEntry->var_44 = image_index;
                    b = vehicleEntry->var_16 * 128;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->var_48 = image_index;
                    b = vehicleEntry->var_16 * 16;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_CORKSCREWS)
                {
                    vehicleEntry->var_4C = image_index;
                    b = vehicleEntry->var_16 * 80;
                    image_index += b;
                }

                // Unknown
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION)
                {
                    vehicleEntry->var_1C = image_index;
                    b = vehicleEntry->var_16 * 12;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_14)
                {
                    // Same offset as above???
                    vehicleEntry->var_4C = image_index;
                    b = vehicleEntry->var_16 * 32;
                    image_index += b;
                }
            }
            else
            {
                image_index += vehicleEntry->var_16 * 36;
            }

            // No vehicle images
            vehicleEntry->no_vehicle_images = image_index - cur_vehicle_images_offset;

            // Move the offset over this vehicles images. Including peeps
            cur_vehicle_images_offset = image_index + vehicleEntry->no_seating_rows * vehicleEntry->no_vehicle_images;
            // 0x6DEB0D

            if (!(vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_10))
            {
                sint32 num_images = cur_vehicle_images_offset - vehicleEntry->base_image_id;
                if (vehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_13)
                {
                    num_images *= 2;
                }

                set_vehicle_type_image_max_sizes(vehicleEntry, num_images);
            }
            vehicleEntry->peep_loading_positions = _peepLoadingPositions[i];
        }
    }
}

void RideObject::Unload()
{
    language_free_object_string(_legacyType.name);
    language_free_object_string(_legacyType.description);
    gfx_object_free_images(_legacyType.images_offset, GetImageTable()->GetCount());

    _legacyType.name = 0;
    _legacyType.description = 0;
    _legacyType.images_offset = 0;
}

void RideObject::DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const
{
    uint32 imageId = _legacyType.images_offset;
    if (_legacyType.ride_type[0] == 0xFF)
    {
        imageId++;
        if (_legacyType.ride_type[1] == 0xFF)
        {
            imageId++;
        }
    }
    gfx_draw_sprite(dpi, imageId, 0, 0, 0);
}

const utf8 * RideObject::GetDescription() const
{
    return GetString(OBJ_STRING_ID_DESCRIPTION);
}

const utf8 * RideObject::GetCapacity() const
{
    return GetString(OBJ_STRING_ID_CAPACITY);
}

void RideObject::SetRepositoryItem(ObjectRepositoryItem * item) const
{
    for (sint32 i = 0; i < 3; i++)
    {
        item->RideType[i] = _legacyType.ride_type[i];
    }
    for (sint32 i = 0; i < 2; i++)
    {
        item->RideCategory[i] = _legacyType.category[i];
    }

    uint8 flags = 0;
    if ((_legacyType.flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) &&
        !rideTypeShouldLoseSeparateFlag(&_legacyType))
    {
        flags |= ORI_RIDE_FLAG_SEPARATE;
    }
    item->RideFlags = flags;
}

void RideObject::ReadLegacyVehicle(IReadObjectContext * context, IStream * stream, rct_ride_entry_vehicle * vehicle)
{
    vehicle->rotation_frame_mask = stream->ReadValue<uint16>();
    vehicle->var_02 = stream->ReadValue<uint8>();
    vehicle->var_03 = stream->ReadValue<uint8>();
    vehicle->spacing = stream->ReadValue<uint32>();
    vehicle->car_friction = stream->ReadValue<uint16>();
    vehicle->tab_height = stream->ReadValue<sint8>();
    vehicle->num_seats = stream->ReadValue<uint8>();
    vehicle->sprite_flags = stream->ReadValue<uint16>();
    vehicle->sprite_width = stream->ReadValue<uint8>();
    vehicle->sprite_height_negative = stream->ReadValue<uint8>();
    vehicle->sprite_height_positive = stream->ReadValue<uint8>();
    vehicle->var_11 = stream->ReadValue<uint8>();
    vehicle->flags_a = stream->ReadValue<uint16>();
    vehicle->flags_b = stream->ReadValue<uint16>();
    vehicle->var_16 = stream->ReadValue<uint16>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
    vehicle->var_1C = stream->ReadValue<uint32>();
    vehicle->var_20 = stream->ReadValue<uint32>();
    vehicle->var_24 = stream->ReadValue<uint32>();
    vehicle->var_28 = stream->ReadValue<uint32>();
    vehicle->var_2C = stream->ReadValue<uint32>();
    vehicle->var_30 = stream->ReadValue<uint32>();
    vehicle->var_34 = stream->ReadValue<uint32>();
    vehicle->var_38 = stream->ReadValue<uint32>();
    vehicle->var_3C = stream->ReadValue<uint32>();
    vehicle->var_40 = stream->ReadValue<uint32>();
    vehicle->var_44 = stream->ReadValue<uint32>();
    vehicle->var_48 = stream->ReadValue<uint32>();
    vehicle->var_4C = stream->ReadValue<uint32>();
    vehicle->no_vehicle_images = stream->ReadValue<uint32>();
    vehicle->no_seating_rows = stream->ReadValue<uint8>();
    vehicle->spinning_inertia = stream->ReadValue<uint8>();
    vehicle->spinning_friction = stream->ReadValue<uint8>();
    vehicle->friction_sound_id = stream->ReadValue<uint8>();
    vehicle->var_58 = stream->ReadValue<uint8>();
    vehicle->sound_range = stream->ReadValue<uint8>();
    vehicle->var_5A = stream->ReadValue<uint8>();
    vehicle->powered_acceleration = stream->ReadValue<uint8>();
    vehicle->powered_max_speed = stream->ReadValue<uint8>();
    vehicle->car_visual = stream->ReadValue<uint8>();
    vehicle->effect_visual = stream->ReadValue<uint8>();
    vehicle->draw_order = stream->ReadValue<uint8>();
    vehicle->special_frames = stream->ReadValue<uint8>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
}

void RideObject::PerformRCT1CompatibilityFixes()
{
    if (String::Equals(GetIdentifier(), "RCKC    ")) {
        // The rocket cars could take 3 cars per train in RCT1. Restore this.
        _legacyType.max_cars_in_train = 3 + _legacyType.zero_cars;
    }
}
