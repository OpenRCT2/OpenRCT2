/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include <algorithm>
#include <unordered_map>
#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../core/Util.hpp"
#include "../ride/RideGroupManager.h"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/ShopItem.h"
#include "../ride/Track.h"
#include "../OpenRCT2.h"
#include "ObjectJsonHelpers.h"
#include "ObjectRepository.h"
#include "RideObject.h"

using namespace OpenRCT2;

void RideObject::ReadLegacy(IReadObjectContext * context, IStream * stream)
{
    stream->Seek(8, STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint32_t>();
    for (auto &rideType : _legacyType.ride_type)
    {
        rideType = stream->ReadValue<uint8_t>();
    }
    _legacyType.min_cars_in_train = stream->ReadValue<uint8_t>();
    _legacyType.max_cars_in_train = stream->ReadValue<uint8_t>();
    _legacyType.cars_per_flat_ride = stream->ReadValue<uint8_t>();
    _legacyType.zero_cars = stream->ReadValue<uint8_t>();
    _legacyType.tab_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.default_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.front_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.second_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.rear_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.third_vehicle = stream->ReadValue<uint8_t>();
    _legacyType.pad_019 = stream->ReadValue<uint8_t>();
    for (auto &vehicleEntry : _legacyType.vehicles)
    {
        ReadLegacyVehicle(context, stream, &vehicleEntry);
    }
    stream->Seek(4, STREAM_SEEK_CURRENT);
    _legacyType.excitement_multiplier = stream->ReadValue<int8_t>();
    _legacyType.intensity_multiplier = stream->ReadValue<int8_t>();
    _legacyType.nausea_multiplier = stream->ReadValue<int8_t>();
    _legacyType.max_height = stream->ReadValue<uint8_t>();
    _legacyType.enabledTrackPieces = stream->ReadValue<uint64_t>();
    _legacyType.category[0] = stream->ReadValue<uint8_t>();
    _legacyType.category[1] = stream->ReadValue<uint8_t>();
    _legacyType.shop_item = stream->ReadValue<uint8_t>();
    _legacyType.shop_item_secondary = stream->ReadValue<uint8_t>();

    GetStringTable().Read(context, stream, OBJ_STRING_ID_NAME);
    GetStringTable().Read(context, stream, OBJ_STRING_ID_DESCRIPTION);
    GetStringTable().Read(context, stream, OBJ_STRING_ID_CAPACITY);

    // Read preset colours, by default there are 32
    _presetColours.count = stream->ReadValue<uint8_t>();

    int32_t coloursCount = _presetColours.count;
    // To indicate a ride has different colours each train the count
    // is set to 255. There are only actually 32 colours though.
    if (coloursCount == 255)
    {
        coloursCount = 32;
    }

    for (uint8_t i = 0; i < coloursCount; i++)
    {
        _presetColours.list[i] = stream->ReadValue<vehicle_colour>();
    }

    // Read peep loading positions
    for (int32_t i = 0; i < RCT2_MAX_VEHICLES_PER_RIDE_ENTRY; i++)
    {
        _peepLoadingWaypoints[i].clear();
        _peepLoadingPositions[i].clear();

        uint16_t numPeepLoadingPositions = stream->ReadValue<uint8_t>();
        if (numPeepLoadingPositions == 255)
        {
            numPeepLoadingPositions = stream->ReadValue<uint16_t>();
        }
        
        if (_legacyType.vehicles[i].flags & VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS)
        {
            _legacyType.vehicles[i].peep_loading_waypoint_segments = stream->ReadValue<int8_t>() == 0 ? 0 : 4;
            if (_legacyType.ride_type[0] == RIDE_TYPE_ENTERPRISE)
            {
                _legacyType.vehicles[i].peep_loading_waypoint_segments = 8;
            }

            Guard::Assert(((numPeepLoadingPositions - 1) % 8) == 0, "Malformed peep loading positions");

            for (int32_t j = 1; j < numPeepLoadingPositions; j += 4 * 2)
            {
                std::array<sLocationXY8, 3> entry;
                entry[0].x = stream->ReadValue<int8_t>();
                entry[0].y = stream->ReadValue<int8_t>();
                entry[1].x = stream->ReadValue<int8_t>();
                entry[1].y = stream->ReadValue<int8_t>();
                entry[2].x = stream->ReadValue<int8_t>();
                entry[2].y = stream->ReadValue<int8_t>();
                stream->ReadValue<uint16_t>(); // Skip blanks

                _peepLoadingWaypoints[i].push_back(entry);
            }
        }
        else
        {
            _legacyType.vehicles[i].peep_loading_waypoint_segments = 0;

            auto data = stream->ReadArray<int8_t>(numPeepLoadingPositions);
            _peepLoadingPositions[i] = std::vector<int8_t>(data, data + numPeepLoadingPositions);
            Memory::Free(data);
        }
    }

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.excitement_multiplier > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Excitement multiplier too high.");
    }
    if (_legacyType.intensity_multiplier > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Intensity multiplier too high.");
    }
    if (_legacyType.nausea_multiplier > 75)
    {
        context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Nausea multiplier too high.");
    }
}

void RideObject::Load()
{
    _legacyType.obj = this;

    GetStringTable().Sort();
    _legacyType.naming.name = language_allocate_object_string(GetName());
    _legacyType.naming.description = language_allocate_object_string(GetDescription());
    _legacyType.capacity = language_allocate_object_string(GetCapacity());
    _legacyType.images_offset = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.vehicle_preset_list = &_presetColours;

    int32_t cur_vehicle_images_offset = _legacyType.images_offset + MAX_RIDE_TYPES_PER_RIDE_ENTRY;
    for (int32_t i = 0; i < RCT2_MAX_VEHICLES_PER_RIDE_ENTRY; i++)
    {
        rct_ride_entry_vehicle * vehicleEntry = &_legacyType.vehicles[i];
        if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT)
        {
            // RCT2 calculates num_vertical_frames and num_horizontal_frames and overwrites these properties on the vehicle entry.
            // Immediately afterwards, the two were multiplied in order to calculate base_num_frames and were never used again.
            // This has been changed to use the calculation results directly -
            // num_vertical_frames and num_horizontal_frames are no longer set on the vehicle entry.
            // 0x6DE946
            vehicleEntry->base_num_frames = CalculateNumVerticalFrames(vehicleEntry) * CalculateNumHorizontalFrames(vehicleEntry);
            vehicleEntry->base_image_id = cur_vehicle_images_offset;
            int32_t image_index = vehicleEntry->base_image_id;

            if (vehicleEntry->car_visual != VEHICLE_VISUAL_RIVER_RAPIDS)
            {
                int32_t b = vehicleEntry->base_num_frames * 32;

                if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_11) b /= 2;
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_15) b /= 8;

                image_index += b;

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPES)
                {
                    vehicleEntry->gentle_slope_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 72;
                    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES)
                    {
                        b = vehicleEntry->base_num_frames * 16;
                    }
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_STEEP_SLOPES)
                {
                    vehicleEntry->steep_slope_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 80;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_VERTICAL_SLOPES)
                {
                    vehicleEntry->vertical_slope_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 116;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_DIAGONAL_SLOPES)
                {
                    vehicleEntry->diagonal_slope_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 24;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_BANKED)
                {
                    vehicleEntry->banked_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 80;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_INLINE_TWISTS)
                {
                    vehicleEntry->inline_twist_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 40;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->flat_to_gentle_bank_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 128;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->diagonal_to_gentle_slope_bank_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 16;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->gentle_slope_to_bank_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 16;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS)
                {
                    vehicleEntry->gentle_slope_bank_turn_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 128;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS)
                {
                    vehicleEntry->flat_bank_to_gentle_slope_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 16;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_CORKSCREWS)
                {
                    vehicleEntry->corkscrew_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 80;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION)
                {
                    vehicleEntry->restraint_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 12;
                    image_index += b;
                }

                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_CURVED_LIFT_HILL)
                {
                    // Same offset as corkscrew
                    vehicleEntry->curved_lift_hill_image_id = image_index;
                    b = vehicleEntry->base_num_frames * 32;
                    image_index += b;
                }
            }
            else
            {
                image_index += vehicleEntry->base_num_frames * 36;
            }

            // No vehicle images
            vehicleEntry->no_vehicle_images = image_index - cur_vehicle_images_offset;

            // Move the offset over this vehicles images. Including peeps
            cur_vehicle_images_offset = image_index + vehicleEntry->no_seating_rows * vehicleEntry->no_vehicle_images;
            // 0x6DEB0D

            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_10))
            {
                int32_t num_images = cur_vehicle_images_offset - vehicleEntry->base_image_id;
                if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_13)
                {
                    num_images *= 2;
                }

                if (!gOpenRCT2NoGraphics) {
                    set_vehicle_type_image_max_sizes(vehicleEntry, num_images);
                }
            }

            if (!_peepLoadingPositions[i].empty())
            {
                vehicleEntry->peep_loading_positions = std::move(_peepLoadingPositions[i]);
            }

            if (!_peepLoadingWaypoints[i].empty())
            {
                vehicleEntry->peep_loading_waypoints = std::move(_peepLoadingWaypoints[i]);
            }
        }
    }
}

void RideObject::Unload()
{
    language_free_object_string(_legacyType.naming.name);
    language_free_object_string(_legacyType.naming.description);
    language_free_object_string(_legacyType.capacity);
    gfx_object_free_images(_legacyType.images_offset, GetImageTable().GetCount());

    _legacyType.naming.name = 0;
    _legacyType.naming.description = 0;
    _legacyType.capacity = 0;
    _legacyType.images_offset = 0;
}

void RideObject::DrawPreview(rct_drawpixelinfo* dpi, [[maybe_unused]] int32_t width, [[maybe_unused]] int32_t height) const
{
    uint32_t imageId = _legacyType.images_offset;

    for (auto rideType : _legacyType.ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
            break;
        else
            imageId++;
    }

    gfx_draw_sprite(dpi, imageId, 0, 0, 0);
}

std::string RideObject::GetDescription() const
{
    return GetString(OBJ_STRING_ID_DESCRIPTION);
}

std::string RideObject::GetCapacity() const
{
    return GetString(OBJ_STRING_ID_CAPACITY);
}

void RideObject::SetRepositoryItem(ObjectRepositoryItem * item) const
{
    for (int32_t i = 0; i < RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        item->RideInfo.RideType[i] = _legacyType.ride_type[i];
    }
    for (int32_t i = 0; i < RCT2_MAX_CATEGORIES_PER_RIDE; i++)
    {
        item->RideInfo.RideCategory[i] = _legacyType.category[i];
    }

    uint8_t flags = 0;
    item->RideInfo.RideFlags = flags;

    // Find the first non-null ride type, to be used when checking the ride group
    uint8_t rideTypeIdx = ride_entry_get_first_non_null_ride_type(&_legacyType);

    // Determines the ride group. Will fall back to 0 if there is none found.
    uint8_t rideGroupIndex = 0;

    const RideGroup * rideGroup = RideGroupManager::GetRideGroup(rideTypeIdx, &_legacyType);

    // If the ride group is nullptr, the track type does not have ride groups.
    if (rideGroup != nullptr)
    {
        for (uint8_t i = rideGroupIndex + 1; i < MAX_RIDE_GROUPS_PER_RIDE_TYPE; i++)
        {
            const RideGroup * irg = RideGroupManager::RideGroupFind(rideTypeIdx, i);

            if (irg != nullptr)
            {
                if (irg->Equals(rideGroup))
                {
                    rideGroupIndex = i;
                    break;
                }
            }
        }
    }

    item->RideInfo.RideGroupIndex = rideGroupIndex;
}

void RideObject::ReadLegacyVehicle(
    [[maybe_unused]] IReadObjectContext* context, IStream* stream, rct_ride_entry_vehicle* vehicle)
{
    vehicle->rotation_frame_mask = stream->ReadValue<uint16_t>();
    stream->Seek(2 * 1, STREAM_SEEK_CURRENT);
    vehicle->spacing = stream->ReadValue<uint32_t>();
    vehicle->car_mass = stream->ReadValue<uint16_t>();
    vehicle->tab_height = stream->ReadValue<int8_t>();
    vehicle->num_seats = stream->ReadValue<uint8_t>();
    vehicle->sprite_flags = stream->ReadValue<uint16_t>();
    vehicle->sprite_width = stream->ReadValue<uint8_t>();
    vehicle->sprite_height_negative = stream->ReadValue<uint8_t>();
    vehicle->sprite_height_positive = stream->ReadValue<uint8_t>();
    vehicle->animation = stream->ReadValue<uint8_t>();
    vehicle->flags = stream->ReadValue<uint32_t>();
    vehicle->base_num_frames = stream->ReadValue<uint16_t>();
    stream->Seek(15 * 4, STREAM_SEEK_CURRENT);
    vehicle->no_seating_rows = stream->ReadValue<uint8_t>();
    vehicle->spinning_inertia = stream->ReadValue<uint8_t>();
    vehicle->spinning_friction = stream->ReadValue<uint8_t>();
    vehicle->friction_sound_id = stream->ReadValue<uint8_t>();
    vehicle->log_flume_reverser_vehicle_type = stream->ReadValue<uint8_t>();
    vehicle->sound_range = stream->ReadValue<uint8_t>();
    vehicle->double_sound_frequency = stream->ReadValue<uint8_t>();
    vehicle->powered_acceleration = stream->ReadValue<uint8_t>();
    vehicle->powered_max_speed = stream->ReadValue<uint8_t>();
    vehicle->car_visual = stream->ReadValue<uint8_t>();
    vehicle->effect_visual = stream->ReadValue<uint8_t>();
    vehicle->draw_order = stream->ReadValue<uint8_t>();
    vehicle->num_vertical_frames_override = stream->ReadValue<uint8_t>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
}

uint8_t RideObject::CalculateNumVerticalFrames(const rct_ride_entry_vehicle * vehicleEntry)
{
    // 0x6DE90B
    uint8_t numVerticalFrames;
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES)
    {
        numVerticalFrames = vehicleEntry->num_vertical_frames_override;
    }
    else
    {
        if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES))
        {
            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION && vehicleEntry->animation != VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER)
            {
                if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS))
                {
                    numVerticalFrames = 4;
                }
                else
                {
                    numVerticalFrames = 2;
                }
            }
            else
            {
                numVerticalFrames = 1;
            }
        }
        else
        {
            numVerticalFrames = 32;
        }
    }

    return numVerticalFrames;
}

uint8_t RideObject::CalculateNumHorizontalFrames(const rct_ride_entry_vehicle * vehicleEntry)
{
    uint8_t numHorizontalFrames;
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SWINGING)
    {
        if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_21) && !(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING))
        {
            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_25)
            {
                numHorizontalFrames = 3;
            }
            else
            {
                numHorizontalFrames = 5;
            }
        }
        else if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_21) || !(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING))
        {
            numHorizontalFrames = 7;
        }
        else
        {
            numHorizontalFrames = 13;
        }
    }
    else
    {
        numHorizontalFrames = 1;
    }

    return numHorizontalFrames;
}

void RideObject::ReadJson(IReadObjectContext * context, const json_t * root)
{
    auto properties = json_object_get(root, "properties");

    auto rideTypes = ObjectJsonHelpers::GetJsonStringArray(json_object_get(properties, "type"));
    for (size_t i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        uint8_t rideType = RIDE_TYPE_NULL;
        if (i < rideTypes.size())
        {
            rideType = ParseRideType(rideTypes[i]);
            if (rideType == RIDE_TYPE_NULL)
            {
                context->LogError(OBJECT_ERROR_INVALID_PROPERTY, "Unknown ride type");
            }
        }

        _legacyType.ride_type[i] = rideType;
    }

    auto rideCategories = ObjectJsonHelpers::GetJsonStringArray(json_object_get(properties, "category"));
    if (rideCategories.size() >= 1)
    {
        _legacyType.category[0] = ParseRideCategory(rideCategories[0]);
        _legacyType.category[1] = _legacyType.category[0];
    }
    if (rideCategories.size() >= 2)
    {
        _legacyType.category[1] = ParseRideCategory(rideCategories[1]);
    }

    _legacyType.max_height = ObjectJsonHelpers::GetInteger(properties, "maxHeight");

    // This needs to be set for both shops/facilities _and_ regular rides.
    _legacyType.shop_item = SHOP_ITEM_NONE;
    _legacyType.shop_item_secondary = SHOP_ITEM_NONE;

    if (IsRideTypeShopOrFacility(_legacyType.ride_type[0]))
    {
        // Standard car info for a shop
        auto &car = _legacyType.vehicles[0];
        car.spacing = 544;
        car.sprite_flags = VEHICLE_SPRITE_FLAG_FLAT;
        car.sprite_width = 1;
        car.sprite_height_negative = 1;
        car.sprite_height_positive = 1;
        car.flags = VEHICLE_ENTRY_FLAG_SPINNING;
        car.car_visual = VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE;
        car.friction_sound_id = 0xFF;
        car.sound_range = 0xFF;
        car.draw_order = 6;

        // Shop item
        auto rideSells = ObjectJsonHelpers::GetJsonStringArray(json_object_get(json_object_get(root, "properties"), "sells"));
        for (size_t i = 0; i < rideSells.size(); i++)
        {
            auto shopItem = ParseShopItem(rideSells[i]);
            if (shopItem == SHOP_ITEM_NONE)
            {
                context->LogWarning(OBJECT_ERROR_INVALID_PROPERTY, "Unknown shop item");
            }

            if (i == 0)
            {
                _legacyType.shop_item = ParseShopItem(rideSells[0]);
            }
            else if (i == 1)
            {
                _legacyType.shop_item_secondary = ParseShopItem(rideSells[1]);
            }
            else
            {
                // More than 2 shop items not supported yet!
            }
        }
    }
    else
    {
        ReadJsonVehicleInfo(context, properties);

        auto swingMode = ObjectJsonHelpers::GetInteger(properties, "swingMode");
        if (swingMode == 1)
        {
            _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1;
        }
        else if (swingMode == 2)
        {
            _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1;
            _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2;
        }

        auto rotationMode = ObjectJsonHelpers::GetInteger(properties, "rotationMode");
        if (rotationMode == 1)
        {
            _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1;
        }
        else if (rotationMode == 2)
        {
            _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2;
        }

        auto ratingMultiplier = json_object_get(properties, "ratingMultipler");
        if (ratingMultiplier != nullptr)
        {
            _legacyType.excitement_multiplier = ObjectJsonHelpers::GetInteger(ratingMultiplier, "excitement");
            _legacyType.intensity_multiplier = ObjectJsonHelpers::GetInteger(ratingMultiplier, "intensity");
            _legacyType.nausea_multiplier = ObjectJsonHelpers::GetInteger(ratingMultiplier, "nausea");
        }

        auto availableTrackPieces = ObjectJsonHelpers::GetJsonStringArray(json_object_get(properties, "availableTrackPieces"));
        _presetColours = ReadJsonCarColours(json_object_get(properties, "carColours"));
    }

    _legacyType.flags |= ObjectJsonHelpers::GetFlags<uint32_t>(properties, {
        { "noInversions", RIDE_ENTRY_FLAG_NO_INVERSIONS },
        { "noBanking", RIDE_ENTRY_FLAG_NO_BANKED_TRACK },
        { "playDepartSound", RIDE_ENTRY_FLAG_PLAY_DEPART_SOUND },
        // Skipping "disallowWandering", no vehicle sets this flag.
        { "playSplashSound", RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND },
        { "playSplashSoundSlide", RIDE_ENTRY_FLAG_PLAY_SPLASH_SOUND_SLIDE },
        { "hasShelter", RIDE_ENTRY_FLAG_COVERED_RIDE },
        { "limitAirTimeBonus", RIDE_ENTRY_FLAG_LIMIT_AIRTIME_BONUS },
        { "disableBreakdown", RIDE_ENTRY_FLAG_CANNOT_BREAK_DOWN },
        // Skipping noDoorsOverTrack, moved to ride groups.
        { "noCollisionCrashes", RIDE_ENTRY_FLAG_DISABLE_COLLISION_CRASHES },
        { "disablePainting", RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB } });

    ObjectJsonHelpers::LoadStrings(root, GetStringTable());
    ObjectJsonHelpers::LoadImages(context, root, GetImageTable());
}

void RideObject::ReadJsonVehicleInfo([[maybe_unused]] IReadObjectContext* context, const json_t* properties)
{
    _legacyType.min_cars_in_train = ObjectJsonHelpers::GetInteger(properties, "minCarsPerTrain", 1);
    _legacyType.max_cars_in_train = ObjectJsonHelpers::GetInteger(properties, "maxCarsPerTrain", 1);
    _legacyType.cars_per_flat_ride = ObjectJsonHelpers::GetInteger(properties, "carsPerFlatRide", 255);
    _legacyType.zero_cars = json_integer_value(json_object_get(properties, "numEmptyCars"));

    // Train formation from car indices
    _legacyType.default_vehicle = json_integer_value(json_object_get(properties, "defaultCar"));
    _legacyType.tab_vehicle = json_integer_value(json_object_get(properties, "tabCar"));
    auto tabScale = ObjectJsonHelpers::GetFloat(properties, "tabScale");
    if (tabScale != 0 && ObjectJsonHelpers::GetFloat(properties, "tabScale") <= 0.5f)
    {
        _legacyType.flags |= RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF;
    }

    // 0xFF means N/A.
    _legacyType.front_vehicle = 0xFF;
    _legacyType.second_vehicle = 0xFF;
    _legacyType.third_vehicle = 0xFF;
    _legacyType.rear_vehicle = 0xFF;

    auto headCars = ObjectJsonHelpers::GetJsonIntegerArray(json_object_get(properties, "headCars"));
    if (headCars.size() >= 1)
    {
        _legacyType.front_vehicle = headCars[0];
    }
    if (headCars.size() >= 2)
    {
        _legacyType.second_vehicle = headCars[1];
    }
    if (headCars.size() >= 3)
    {
        _legacyType.third_vehicle = headCars[2];
    }
    if (headCars.size() >= 4)
    {
        // More than 3 head cars not supported yet!
    }

    auto tailCars = ObjectJsonHelpers::GetJsonIntegerArray(json_object_get(properties, "tailCars"));
    if (tailCars.size() >= 1)
    {
        _legacyType.rear_vehicle = tailCars[0];
    }
    if (tailCars.size() >= 2)
    {
        // More than 1 tail car not supported yet!
    }

    auto cars = ReadJsonCars(json_object_get(properties, "cars"));
    auto numCars = std::min(Util::CountOf(_legacyType.vehicles), cars.size());
    for (size_t i = 0; i < numCars; i++)
    {
        _legacyType.vehicles[i] = cars[i];
    }
}

std::vector<rct_ride_entry_vehicle> RideObject::ReadJsonCars(const json_t * jCars)
{
    std::vector<rct_ride_entry_vehicle> cars;

    if (json_is_array(jCars))
    {
        json_t * jCar;
        size_t index;
        json_array_foreach(jCars, index, jCar)
        {
            auto car = ReadJsonCar(jCar);
            cars.push_back(car);
        }
    }
    else if (json_is_object(jCars))
    {
        auto car = ReadJsonCar(jCars);
        cars.push_back(car);
    }
    return cars;
}

rct_ride_entry_vehicle RideObject::ReadJsonCar(const json_t * jCar)
{
    rct_ride_entry_vehicle car = {};
    car.rotation_frame_mask = ObjectJsonHelpers::GetInteger(jCar, "rotationFrameMask");
    car.spacing = ObjectJsonHelpers::GetInteger(jCar, "spacing");
    car.car_mass = ObjectJsonHelpers::GetInteger(jCar, "mass");
    car.tab_height = ObjectJsonHelpers::GetInteger(jCar, "tabOffset");
    car.num_seats = ObjectJsonHelpers::GetInteger(jCar, "numSeats");
    if (ObjectJsonHelpers::GetBoolean(jCar, "seatsInPairs", true) && car.num_seats > 1)
    {
        car.num_seats |= VEHICLE_SEAT_PAIR_FLAG;
    }

    car.sprite_width = ObjectJsonHelpers::GetInteger(jCar, "spriteWidth");
    car.sprite_height_negative = ObjectJsonHelpers::GetInteger(jCar, "spriteHeightNegative");
    car.sprite_height_positive = ObjectJsonHelpers::GetInteger(jCar, "spriteHeightPositive");
    car.animation = ObjectJsonHelpers::GetInteger(jCar, "animation");
    car.base_num_frames = ObjectJsonHelpers::GetInteger(jCar, "baseNumFrames");
    car.no_vehicle_images = ObjectJsonHelpers::GetInteger(jCar, "numImages");
    car.no_seating_rows = ObjectJsonHelpers::GetInteger(jCar, "numSeatRows");
    car.spinning_inertia = ObjectJsonHelpers::GetInteger(jCar, "spinningInertia");
    car.spinning_friction = ObjectJsonHelpers::GetInteger(jCar, "spinningFriction");
    car.friction_sound_id = ObjectJsonHelpers::GetInteger(jCar, "frictionSoundId", 255);
    car.log_flume_reverser_vehicle_type = ObjectJsonHelpers::GetInteger(jCar, "logFlumeReverserVehicleType");
    car.sound_range = ObjectJsonHelpers::GetInteger(jCar, "soundRange", 255);
    car.double_sound_frequency = ObjectJsonHelpers::GetInteger(jCar, "doubleSoundFrequency");
    car.powered_acceleration = ObjectJsonHelpers::GetInteger(jCar, "poweredAcceleration");
    car.powered_max_speed = ObjectJsonHelpers::GetInteger(jCar, "poweredMaxSpeed");
    car.car_visual = ObjectJsonHelpers::GetInteger(jCar, "carVisual");
    car.effect_visual = ObjectJsonHelpers::GetInteger(jCar, "effectVisual", 1);
    car.draw_order = ObjectJsonHelpers::GetInteger(jCar, "drawOrder");
    car.num_vertical_frames_override = ObjectJsonHelpers::GetInteger(jCar, "numVerticalFramesOverride");

    auto& peepLoadingPositions = car.peep_loading_positions;
    auto jLoadingPositions = json_object_get(jCar, "loadingPositions");
    if (json_is_array(jLoadingPositions))
    {
        auto arr = ObjectJsonHelpers::GetJsonIntegerArray(jLoadingPositions);
        for (auto x : arr)
        {
            peepLoadingPositions.push_back(x);
        }
    }
    else
    {
        auto& peepLoadingWaypoints = car.peep_loading_waypoints;
        auto jLoadingWaypoints = json_object_get(jCar, "loadingWaypoints");
        if (json_is_array(jLoadingWaypoints))
        {
            car.flags |= VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS;

            auto numSegments = ObjectJsonHelpers::GetInteger(jCar, "numSegments");
            car.peep_loading_waypoint_segments = numSegments;
            
            size_t i;
            json_t * route;
            json_array_foreach(jLoadingWaypoints, i, route)
            {
                if (json_is_array(route))
                {
                    size_t j;
                    json_t * waypoint;
                    std::array<sLocationXY8, 3> entry;
                    json_array_foreach(route, j, waypoint)
                    {
                        if (json_is_array(waypoint) && json_array_size(waypoint) >= 2)
                        {
                            auto x = (int8_t)json_integer_value(json_array_get(waypoint, 0));
                            auto y = (int8_t)json_integer_value(json_array_get(waypoint, 1));
                            entry[j] = { x, y };
                        }
                    }
                    peepLoadingWaypoints.push_back(entry);
                }
            }
        }
    }

    auto jFrames = json_object_get(jCar, "frames");
    car.sprite_flags = ObjectJsonHelpers::GetFlags<uint16_t>(jFrames, {
        { "flat", VEHICLE_SPRITE_FLAG_FLAT },
        { "gentleSlopes", VEHICLE_SPRITE_FLAG_GENTLE_SLOPES },
        { "steepSlopes", VEHICLE_SPRITE_FLAG_STEEP_SLOPES },
        { "verticalSlopes", VEHICLE_SPRITE_FLAG_VERTICAL_SLOPES },
        { "diagonalSlopes", VEHICLE_SPRITE_FLAG_DIAGONAL_SLOPES },
        { "flatBanked", VEHICLE_SPRITE_FLAG_FLAT_BANKED },
        { "inlineTwists", VEHICLE_SPRITE_FLAG_INLINE_TWISTS },
        { "flatToGentleSlopeBankedTransitions", VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS },
        { "diagonalGentleSlopeBankedTransitions", VEHICLE_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS },
        { "gentleSlopeBankedTransitions", VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS },
        { "gentleSlopeBankedTurns", VEHICLE_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS },
        { "flatToGentleSlopeWhileBankedTransitions", VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS },
        { "corkscrews", VEHICLE_SPRITE_FLAG_CORKSCREWS },
        { "restraintAnimation", VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION },
        { "curvedLiftHill", VEHICLE_SPRITE_FLAG_CURVED_LIFT_HILL },
        { "VEHICLE_SPRITE_FLAG_15", VEHICLE_SPRITE_FLAG_15 } });

    car.flags |= ObjectJsonHelpers::GetFlags<uint32_t>(jCar, {
        { "VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY", VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY },
        { "VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS", VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS },
        { "VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH", VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH },
        { "VEHICLE_ENTRY_FLAG_MINI_GOLF", VEHICLE_ENTRY_FLAG_MINI_GOLF },
        { "VEHICLE_ENTRY_FLAG_4", VEHICLE_ENTRY_FLAG_4 },
        { "VEHICLE_ENTRY_FLAG_5", VEHICLE_ENTRY_FLAG_5 },
        { "VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET", VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET },
        { "VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS", VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS },
        { "VEHICLE_ENTRY_FLAG_ALLOW_DOORS_DEPRECATED", VEHICLE_ENTRY_FLAG_ALLOW_DOORS_DEPRECATED },
        { "VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2 },
        { "VEHICLE_ENTRY_FLAG_10", VEHICLE_ENTRY_FLAG_10 },
        { "VEHICLE_ENTRY_FLAG_11", VEHICLE_ENTRY_FLAG_11 },
        { "VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES", VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES },
        { "VEHICLE_ENTRY_FLAG_13", VEHICLE_ENTRY_FLAG_13 },
        { "VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES", VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES },
        { "VEHICLE_ENTRY_FLAG_LIFT", VEHICLE_ENTRY_FLAG_LIFT },
        { "VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1 },
        { "VEHICLE_ENTRY_FLAG_SWINGING", VEHICLE_ENTRY_FLAG_SWINGING },
        { "VEHICLE_ENTRY_FLAG_SPINNING", VEHICLE_ENTRY_FLAG_SPINNING },
        { "VEHICLE_ENTRY_FLAG_POWERED", VEHICLE_ENTRY_FLAG_POWERED },
        { "VEHICLE_ENTRY_FLAG_RIDERS_SCREAM", VEHICLE_ENTRY_FLAG_RIDERS_SCREAM },
        { "VEHICLE_ENTRY_FLAG_21", VEHICLE_ENTRY_FLAG_21 },
        { "VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION", VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION },
        { "VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION", VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION },
        { "VEHICLE_ENTRY_FLAG_RIDER_ANIMATION", VEHICLE_ENTRY_FLAG_RIDER_ANIMATION },
        { "VEHICLE_ENTRY_FLAG_25", VEHICLE_ENTRY_FLAG_25 },
        { "VEHICLE_ENTRY_FLAG_SLIDE_SWING", VEHICLE_ENTRY_FLAG_SLIDE_SWING },
        { "VEHICLE_ENTRY_FLAG_CHAIRLIFT", VEHICLE_ENTRY_FLAG_CHAIRLIFT },
        { "VEHICLE_ENTRY_FLAG_WATER_RIDE", VEHICLE_ENTRY_FLAG_WATER_RIDE },
        { "VEHICLE_ENTRY_FLAG_GO_KART", VEHICLE_ENTRY_FLAG_GO_KART },
        { "VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT", VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT } });
    return car;
}

vehicle_colour_preset_list RideObject::ReadJsonCarColours(const json_t * jCarColours)
{
    // The JSON supports multiple configurations of per car colours, but
    // the ride entry structure currently doesn't allow for it. Assume that
    // a single configuration with multiple colour entries is per car scheme.
    if (json_array_size(jCarColours) == 1)
    {
        auto firstElement = json_array_get(jCarColours, 0);
        auto numColours = json_array_size(firstElement);
        if (numColours >= 2)
        {
            // Read all colours from first config
            auto config = ReadJsonColourConfiguration(firstElement);
            vehicle_colour_preset_list list = {};
            list.count = 255;
            std::copy_n(config.data(), std::min<size_t>(numColours, 32), list.list);
            return list;
        }
    }

    // Read first colour for each config
    vehicle_colour_preset_list list = {};
    size_t index;
    const json_t * jConfiguration;
    json_array_foreach(jCarColours, index, jConfiguration)
    {
        auto config = ReadJsonColourConfiguration(jConfiguration);
        if (config.size() >= 1)
        {
            list.list[index] = config[0];
            list.count++;

            if (list.count == 254)
            {
                // Reached maximum number of configurations
                break;
            }
        }
    }
    return list;
}

std::vector<vehicle_colour> RideObject::ReadJsonColourConfiguration(const json_t * jColourConfig)
{
    std::vector<vehicle_colour> config;
    size_t index;
    const json_t * jColours;
    json_array_foreach(jColourConfig, index, jColours)
    {
        vehicle_colour carColour = {};
        auto colours = ObjectJsonHelpers::GetJsonStringArray(jColours);
        if (colours.size() >= 1)
        {
            carColour.main = ParseColour(colours[0]);
            carColour.additional_1 = carColour.main;
            carColour.additional_2 = carColour.main;
            if (colours.size() >= 2)
            {
                carColour.additional_1 = ParseColour(colours[1]);
            }
            if (colours.size() >= 3)
            {
                carColour.additional_2 = ParseColour(colours[2]);
            }
        }
        config.push_back(carColour);
    }
    return config;
}

bool RideObject::IsRideTypeShopOrFacility(uint8_t rideType)
{
    switch (rideType)
    {
    case RIDE_TYPE_TOILETS:
    case RIDE_TYPE_SHOP:
    case RIDE_TYPE_DRINK_STALL:
    case RIDE_TYPE_FOOD_STALL:
    case RIDE_TYPE_INFORMATION_KIOSK:
    case RIDE_TYPE_CASH_MACHINE:
    case RIDE_TYPE_FIRST_AID:
        return true;
    default:
        return false;
    }
}

uint8_t RideObject::ParseRideType(const std::string &s)
{
    static const std::unordered_map<std::string, uint8_t> LookupTable
    {
        { "spiral_rc",                  RIDE_TYPE_SPIRAL_ROLLER_COASTER },
        { "stand_up_rc",                RIDE_TYPE_STAND_UP_ROLLER_COASTER },
        { "suspended_swinging_rc",      RIDE_TYPE_SUSPENDED_SWINGING_COASTER },
        { "inverted_rc",                RIDE_TYPE_INVERTED_ROLLER_COASTER },
        { "junior_rc",                  RIDE_TYPE_JUNIOR_ROLLER_COASTER },
        { "miniature_railway",          RIDE_TYPE_MINIATURE_RAILWAY },
        { "monorail",                   RIDE_TYPE_MONORAIL },
        { "mini_suspended_rc",          RIDE_TYPE_MINI_SUSPENDED_COASTER },
        { "boat_hire",                  RIDE_TYPE_BOAT_HIRE },
        { "wooden_wild_mouse",          RIDE_TYPE_WOODEN_WILD_MOUSE },
        { "steeplechase",               RIDE_TYPE_STEEPLECHASE },
        { "car_ride",                   RIDE_TYPE_CAR_RIDE },
        { "launched_freefall",          RIDE_TYPE_LAUNCHED_FREEFALL },
        { "bobsleigh_rc",               RIDE_TYPE_BOBSLEIGH_COASTER },
        { "observation_tower",          RIDE_TYPE_OBSERVATION_TOWER },
        { "looping_rc",                 RIDE_TYPE_LOOPING_ROLLER_COASTER },
        { "dinghy_slide",               RIDE_TYPE_DINGHY_SLIDE },
        { "mine_train_rc",              RIDE_TYPE_MINE_TRAIN_COASTER },
        { "chairlift",                  RIDE_TYPE_CHAIRLIFT },
        { "corkscrew_rc",               RIDE_TYPE_CORKSCREW_ROLLER_COASTER },
        { "maze",                       RIDE_TYPE_MAZE },
        { "spiral_slide",               RIDE_TYPE_SPIRAL_SLIDE },
        { "go_karts",                   RIDE_TYPE_GO_KARTS },
        { "log_flume",                  RIDE_TYPE_LOG_FLUME },
        { "river_rapids",               RIDE_TYPE_RIVER_RAPIDS },
        { "dodgems",                    RIDE_TYPE_DODGEMS },
        { "swinging_ship",              RIDE_TYPE_SWINGING_SHIP },
        { "swinging_inverter_ship",     RIDE_TYPE_SWINGING_INVERTER_SHIP },
        { "food_stall",                 RIDE_TYPE_FOOD_STALL },
        { "drink_stall",                RIDE_TYPE_DRINK_STALL },
        { "shop",                       RIDE_TYPE_SHOP },
        { "merry_go_round",             RIDE_TYPE_MERRY_GO_ROUND },
        { "information_kiosk",          RIDE_TYPE_INFORMATION_KIOSK },
        { "toilets",                    RIDE_TYPE_TOILETS },
        { "ferris_wheel",               RIDE_TYPE_FERRIS_WHEEL },
        { "motion_simulator",           RIDE_TYPE_MOTION_SIMULATOR },
        { "3d_cinema",                  RIDE_TYPE_3D_CINEMA },
        { "top_spin",                   RIDE_TYPE_TOP_SPIN },
        { "space_rings",                RIDE_TYPE_SPACE_RINGS },
        { "reverse_freefall_rc",        RIDE_TYPE_REVERSE_FREEFALL_COASTER },
        { "lift",                       RIDE_TYPE_LIFT },
        { "vertical_drop_rc",           RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER },
        { "cash_machine",               RIDE_TYPE_CASH_MACHINE },
        { "twist",                      RIDE_TYPE_TWIST },
        { "haunted_house",              RIDE_TYPE_HAUNTED_HOUSE },
        { "first_aid",                  RIDE_TYPE_FIRST_AID },
        { "circus",                     RIDE_TYPE_CIRCUS },
        { "ghost_train",                RIDE_TYPE_GHOST_TRAIN },
        { "twister_rc",                 RIDE_TYPE_TWISTER_ROLLER_COASTER },
        { "wooden_rc",                  RIDE_TYPE_WOODEN_ROLLER_COASTER },
        { "side_friction_rc",           RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER },
        { "steel_wild_mouse",           RIDE_TYPE_STEEL_WILD_MOUSE },
        { "multi_dimension_rc",         RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER },
        { "flying_rc",                  RIDE_TYPE_FLYING_ROLLER_COASTER },
        { "virginia_reel",              RIDE_TYPE_VIRGINIA_REEL },
        { "splash_boats",               RIDE_TYPE_SPLASH_BOATS },
        { "mini_helicopters",           RIDE_TYPE_MINI_HELICOPTERS },
        { "lay_down_rc",                RIDE_TYPE_LAY_DOWN_ROLLER_COASTER },
        { "suspended_monorail",         RIDE_TYPE_SUSPENDED_MONORAIL },
        { "reverser_rc",                RIDE_TYPE_REVERSER_ROLLER_COASTER },
        { "heartline_twister_rc",       RIDE_TYPE_HEARTLINE_TWISTER_COASTER },
        { "mini_golf",                  RIDE_TYPE_MINI_GOLF },
        { "giga_rc",                    RIDE_TYPE_GIGA_COASTER },
        { "roto_drop",                  RIDE_TYPE_ROTO_DROP },
        { "flying_saucers",             RIDE_TYPE_FLYING_SAUCERS },
        { "crooked_house",              RIDE_TYPE_CROOKED_HOUSE },
        { "monorail_cycles",            RIDE_TYPE_MONORAIL_CYCLES },
        { "compact_inverted_rc",        RIDE_TYPE_COMPACT_INVERTED_COASTER },
        { "water_coaster",              RIDE_TYPE_WATER_COASTER },
        { "air_powered_vertical_rc",    RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER },
        { "inverted_hairpin_rc",        RIDE_TYPE_INVERTED_HAIRPIN_COASTER },
        { "magic_carpet",               RIDE_TYPE_MAGIC_CARPET },
        { "submarine_ride",             RIDE_TYPE_SUBMARINE_RIDE },
        { "river_rafts",                RIDE_TYPE_RIVER_RAFTS },
        { "enterprise",                 RIDE_TYPE_ENTERPRISE },
        { "inverted_impulse_rc",        RIDE_TYPE_INVERTED_IMPULSE_COASTER },
        { "mini_rc",                    RIDE_TYPE_MINI_ROLLER_COASTER },
        { "mine_ride",                  RIDE_TYPE_MINE_RIDE },
        { "lim_launched_rc",            RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER },
    };
    auto result = LookupTable.find(s);
    return (result != LookupTable.end()) ?
        result->second :
        RIDE_TYPE_NULL;
}

uint8_t RideObject::ParseRideCategory(const std::string &s)
{
    static const std::unordered_map<std::string, uint8_t> LookupTable
    {
        { "transport",      RIDE_CATEGORY_TRANSPORT },
        { "gentle",         RIDE_CATEGORY_GENTLE },
        { "rollercoaster",  RIDE_CATEGORY_ROLLERCOASTER },
        { "thrill",         RIDE_CATEGORY_THRILL },
        { "water",          RIDE_CATEGORY_WATER },
        { "stall",          RIDE_CATEGORY_SHOP },
    };
    auto result = LookupTable.find(s);
    return (result != LookupTable.end()) ?
        result->second :
        RIDE_CATEGORY_TRANSPORT;
}

uint8_t RideObject::ParseShopItem(const std::string &s)
{
    static const std::unordered_map<std::string, uint8_t> LookupTable
    {
        { "burger",             SHOP_ITEM_BURGER },
        { "chips",              SHOP_ITEM_CHIPS },
        { "ice_cream",          SHOP_ITEM_ICE_CREAM },
        { "candyfloss",         SHOP_ITEM_CANDYFLOSS },
        { "pizza",              SHOP_ITEM_PIZZA },
        { "popcorn",            SHOP_ITEM_POPCORN },
        { "hot_dog",            SHOP_ITEM_HOT_DOG },
        { "tentacle",           SHOP_ITEM_TENTACLE },
        { "toffee_apple",       SHOP_ITEM_TOFFEE_APPLE },
        { "doughnut",           SHOP_ITEM_DOUGHNUT },
        { "chicken",            SHOP_ITEM_CHICKEN },
        { "pretzel",            SHOP_ITEM_PRETZEL },
        { "funnel_cake",        SHOP_ITEM_FUNNEL_CAKE },
        { "beef_noodles",       SHOP_ITEM_BEEF_NOODLES },
        { "fried_rice_noodles", SHOP_ITEM_FRIED_RICE_NOODLES },
        { "wonton_soup",        SHOP_ITEM_WONTON_SOUP },
        { "meatball_soup",      SHOP_ITEM_MEATBALL_SOUP },
        { "sub_sandwich",       SHOP_ITEM_SUB_SANDWICH },
        { "cookie",             SHOP_ITEM_COOKIE },
        { "roast_sausage",      SHOP_ITEM_ROAST_SAUSAGE },
        { "drink",              SHOP_ITEM_DRINK },
        { "coffee",             SHOP_ITEM_COFFEE },
        { "lemonade",           SHOP_ITEM_LEMONADE },
        { "chocolate",          SHOP_ITEM_CHOCOLATE },
        { "iced_tea",           SHOP_ITEM_ICED_TEA },
        { "fruit_juice",        SHOP_ITEM_FRUIT_JUICE },
        { "soybean_milk",       SHOP_ITEM_SOYBEAN_MILK },
        { "sujeonggwa",         SHOP_ITEM_SUJEONGGWA },
        { "balloon",            SHOP_ITEM_BALLOON },
        { "toy",                SHOP_ITEM_TOY },
        { "map",                SHOP_ITEM_MAP },
        { "photo",              SHOP_ITEM_PHOTO },
        { "umbrella",           SHOP_ITEM_UMBRELLA },
        { "voucher",            SHOP_ITEM_VOUCHER },
        { "hat",                SHOP_ITEM_HAT },
        { "tshirt",             SHOP_ITEM_TSHIRT },
        { "sunglasses",         SHOP_ITEM_SUNGLASSES },
    };
    auto result = LookupTable.find(s);
    return (result != LookupTable.end()) ?
        result->second :
        SHOP_ITEM_NONE;
}

colour_t RideObject::ParseColour(const std::string &s)
{
    static const std::unordered_map<std::string, colour_t> LookupTable
    {
        { "black", COLOUR_BLACK },
        { "grey", COLOUR_GREY },
        { "white", COLOUR_WHITE },
        { "dark_purple", COLOUR_DARK_PURPLE },
        { "light_purple", COLOUR_LIGHT_PURPLE },
        { "bright_purple", COLOUR_BRIGHT_PURPLE },
        { "dark_blue", COLOUR_DARK_BLUE },
        { "light_blue", COLOUR_LIGHT_BLUE },
        { "icy_blue", COLOUR_ICY_BLUE },
        { "teal", COLOUR_TEAL },
        { "aquamarine", COLOUR_AQUAMARINE },
        { "saturated_green", COLOUR_SATURATED_GREEN },
        { "dark_green", COLOUR_DARK_GREEN },
        { "moss_green", COLOUR_MOSS_GREEN },
        { "bright_green", COLOUR_BRIGHT_GREEN },
        { "olive_green", COLOUR_OLIVE_GREEN },
        { "dark_olive_green", COLOUR_DARK_OLIVE_GREEN },
        { "bright_yellow", COLOUR_BRIGHT_YELLOW },
        { "yellow", COLOUR_YELLOW },
        { "dark_yellow", COLOUR_DARK_YELLOW },
        { "light_orange", COLOUR_LIGHT_ORANGE },
        { "dark_orange", COLOUR_DARK_ORANGE },
        { "light_brown", COLOUR_LIGHT_BROWN },
        { "saturated_brown", COLOUR_SATURATED_BROWN },
        { "dark_brown", COLOUR_DARK_BROWN },
        { "salmon_pink", COLOUR_SALMON_PINK },
        { "bordeaux_red", COLOUR_BORDEAUX_RED },
        { "saturated_red", COLOUR_SATURATED_RED },
        { "bright_red", COLOUR_BRIGHT_RED },
        { "dark_pink", COLOUR_DARK_PINK },
        { "bright_pink", COLOUR_BRIGHT_PINK },
        { "light_pink", COLOUR_LIGHT_PINK },
    };
    auto result = LookupTable.find(s);
    return (result != LookupTable.end()) ?
        result->second :
        COLOUR_BLACK;
}
