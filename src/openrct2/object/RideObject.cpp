/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "RideObject.h"

#include "../OpenRCT2.h"
#include "../audio/audio.h"
#include "../core/EnumMap.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../localisation/Language.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Track.h"
#include "../ride/Vehicle.h"
#include "ObjectRepository.h"

#include <algorithm>
#include <iterator>
#include <unordered_map>

using namespace OpenRCT2;

static void RideObjectUpdateRideType(rct_ride_entry* rideEntry)
{
    if (rideEntry == nullptr)
    {
        return;
    }

    for (auto i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        auto oldRideType = rideEntry->ride_type[i];
        if (oldRideType != RideType::RIDE_TYPE_NULL)
        {
            rideEntry->ride_type[i] = RCT2RideTypeToOpenRCT2RideType(oldRideType, rideEntry);
        }
    }
}

void RideObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
{
    stream->Seek(8, STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint32_t>();
    for (auto& rideType : _legacyType.ride_type)
    {
        rideType = stream->ReadValue<RideType>();
        if (!RideTypeIsValid(rideType))
            rideType = RideType::RIDE_TYPE_NULL;
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

    _legacyType.BuildMenuPriority = 0;
    // Skip pad_019
    stream->Seek(1, STREAM_SEEK_CURRENT);

    for (auto& vehicleEntry : _legacyType.vehicles)
    {
        ReadLegacyVehicle(context, stream, &vehicleEntry);
    }
    stream->Seek(4, STREAM_SEEK_CURRENT);
    _legacyType.excitement_multiplier = stream->ReadValue<int8_t>();
    _legacyType.intensity_multiplier = stream->ReadValue<int8_t>();
    _legacyType.nausea_multiplier = stream->ReadValue<int8_t>();
    _legacyType.max_height = stream->ReadValue<uint8_t>();
    // Skipping a uint64_t for the enabled track pieces and two uint8_ts for the categories.
    stream->Seek(10, STREAM_SEEK_CURRENT);
    _legacyType.shop_item[0] = static_cast<ShopItem>(stream->ReadValue<uint8_t>());
    _legacyType.shop_item[1] = static_cast<ShopItem>(stream->ReadValue<uint8_t>());

    GetStringTable().Read(context, stream, ObjectStringID::NAME);
    GetStringTable().Read(context, stream, ObjectStringID::DESCRIPTION);
    GetStringTable().Read(context, stream, ObjectStringID::CAPACITY);

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

    if (IsRideTypeShopOrFacility(_legacyType.ride_type[0]))
    {
        // This used to be hard-coded. JSON objects set this themselves.
        _presetColours.count = 1;
        _presetColours.list[0] = { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED };

        if (_legacyType.ride_type[0] == RideType::FOOD_STALL || _legacyType.ride_type[0] == RideType::DRINK_STALL)
        {
            // In RCT2, no food or drink stall could be recoloured.
            _legacyType.flags |= RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB;
        }
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
            if (_legacyType.ride_type[0] == RideType::ENTERPRISE)
            {
                _legacyType.vehicles[i].peep_loading_waypoint_segments = 8;
            }

            Guard::Assert(((numPeepLoadingPositions - 1) % 8) == 0, "Malformed peep loading positions");

            for (int32_t j = 1; j < numPeepLoadingPositions; j += 4 * 2)
            {
                std::array<CoordsXY, 3> entry;
                entry[0].x = stream->ReadValue<int8_t>();
                entry[0].y = stream->ReadValue<int8_t>();
                entry[1].x = stream->ReadValue<int8_t>();
                entry[1].y = stream->ReadValue<int8_t>();
                entry[2].x = stream->ReadValue<int8_t>();
                entry[2].y = stream->ReadValue<int8_t>();
                stream->ReadValue<uint16_t>(); // Skip blanks

                _peepLoadingWaypoints[i].push_back(std::move(entry));
            }
        }
        else
        {
            _legacyType.vehicles[i].peep_loading_waypoint_segments = 0;

            auto data = stream->ReadArray<int8_t>(numPeepLoadingPositions);
            _peepLoadingPositions[i] = std::vector<int8_t>(data.get(), data.get() + numPeepLoadingPositions);
        }
    }

    GetImageTable().Read(context, stream);

    // Validate properties
    if (_legacyType.excitement_multiplier > 75)
    {
        context->LogError(ObjectError::InvalidProperty, "Excitement multiplier too high.");
    }
    if (_legacyType.intensity_multiplier > 75)
    {
        context->LogError(ObjectError::InvalidProperty, "Intensity multiplier too high.");
    }
    if (_legacyType.nausea_multiplier > 75)
    {
        context->LogError(ObjectError::InvalidProperty, "Nausea multiplier too high.");
    }
    RideObjectUpdateRideType(&_legacyType);
}

void RideObject::Load()
{
    _legacyType.obj = this;

    GetStringTable().Sort();
    _legacyType.naming.Name = language_allocate_object_string(GetName());
    _legacyType.naming.Description = language_allocate_object_string(GetDescription());
    _legacyType.capacity = language_allocate_object_string(GetCapacity());
    _legacyType.images_offset = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.vehicle_preset_list = &_presetColours;

    int32_t cur_vehicle_images_offset = _legacyType.images_offset + MAX_RIDE_TYPES_PER_RIDE_ENTRY;
    for (int32_t i = 0; i < RCT2_MAX_VEHICLES_PER_RIDE_ENTRY; i++)
    {
        rct_ride_entry_vehicle* vehicleEntry = &_legacyType.vehicles[i];
        if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_FLAT)
        {
            // RCT2 calculates num_vertical_frames and num_horizontal_frames and overwrites these properties on the vehicle
            // entry. Immediately afterwards, the two were multiplied in order to calculate base_num_frames and were never used
            // again. This has been changed to use the calculation results directly - num_vertical_frames and
            // num_horizontal_frames are no longer set on the vehicle entry.
            // 0x6DE946
            vehicleEntry->base_num_frames = CalculateNumVerticalFrames(vehicleEntry)
                * CalculateNumHorizontalFrames(vehicleEntry);
            vehicleEntry->base_image_id = cur_vehicle_images_offset;
            int32_t image_index = vehicleEntry->base_image_id;

            if (vehicleEntry->car_visual != VEHICLE_VISUAL_RIVER_RAPIDS)
            {
                int32_t b = vehicleEntry->base_num_frames * 32;

                if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_USE_16_ROTATION_FRAMES)
                    b /= 2;
                if (vehicleEntry->sprite_flags & VEHICLE_SPRITE_FLAG_USE_4_ROTATION_FRAMES)
                    b /= 8;

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

            if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS))
            {
                int32_t num_images = cur_vehicle_images_offset - vehicleEntry->base_image_id;
                if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET)
                {
                    num_images *= 2;
                }

                if (!gOpenRCT2NoGraphics)
                {
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
    language_free_object_string(_legacyType.naming.Name);
    language_free_object_string(_legacyType.naming.Description);
    language_free_object_string(_legacyType.capacity);
    gfx_object_free_images(_legacyType.images_offset, GetImageTable().GetCount());

    _legacyType.naming.Name = 0;
    _legacyType.naming.Description = 0;
    _legacyType.capacity = 0;
    _legacyType.images_offset = 0;
}

void RideObject::DrawPreview(rct_drawpixelinfo* dpi, [[maybe_unused]] int32_t width, [[maybe_unused]] int32_t height) const
{
    uint32_t imageId = _legacyType.images_offset;

    for (auto rideType : _legacyType.ride_type)
    {
        if (rideType != RideType::RIDE_TYPE_NULL)
            break;
        else
            imageId++;
    }

    gfx_draw_sprite(dpi, imageId, { 0, 0 }, 0);
}

std::string RideObject::GetDescription() const
{
    return GetString(ObjectStringID::DESCRIPTION);
}

std::string RideObject::GetCapacity() const
{
    return GetString(ObjectStringID::CAPACITY);
}

void RideObject::SetRepositoryItem(ObjectRepositoryItem* item) const
{
    // Find the first non-null ride type, to be used when checking the ride group and determining the category.
    auto firstRideType = ride_entry_get_first_non_null_ride_type(&_legacyType);
    uint8_t category = GetRideTypeDescriptor(firstRideType).Category;

    for (int32_t i = 0; i < RCT2_MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
    {
        item->RideInfo.RideType[i] = _legacyType.ride_type[i];
    }
    for (int32_t i = 0; i < RCT2_MAX_CATEGORIES_PER_RIDE; i++)
    {
        item->RideInfo.RideCategory[i] = category;
    }

    item->RideInfo.RideFlags = 0;
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
    vehicle->friction_sound_id = stream->ReadValue<OpenRCT2::Audio::SoundId>();
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

uint8_t RideObject::CalculateNumVerticalFrames(const rct_ride_entry_vehicle* vehicleEntry)
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
            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION
                && vehicleEntry->animation != VEHICLE_ENTRY_ANIMATION_OBSERVATION_TOWER)
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

uint8_t RideObject::CalculateNumHorizontalFrames(const rct_ride_entry_vehicle* vehicleEntry)
{
    uint8_t numHorizontalFrames;
    if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SWINGING)
    {
        if (!(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SUSPENDED_SWING)
            && !(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING))
        {
            if (vehicleEntry->flags & VEHICLE_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING)
            {
                numHorizontalFrames = 3;
            }
            else
            {
                numHorizontalFrames = 5;
            }
        }
        else if (
            !(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SUSPENDED_SWING)
            || !(vehicleEntry->flags & VEHICLE_ENTRY_FLAG_SLIDE_SWING))
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

void RideObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "RideObject::ReadJson expects parameter root to be object");

    json_t properties = root["properties"];

    if (properties.is_object())
    {
        // This will convert a string to an array
        json_t rideTypes = Json::AsArray(properties["type"]);
        size_t numRideTypes = rideTypes.size();

        for (size_t i = 0; i < MAX_RIDE_TYPES_PER_RIDE_ENTRY; i++)
        {
            auto rideType = RideType::RIDE_TYPE_NULL;

            if (i < numRideTypes)
            {
                rideType = ParseRideType(Json::GetString(rideTypes[i]));

                if (rideType == RideType::RIDE_TYPE_NULL)
                {
                    context->LogError(ObjectError::InvalidProperty, "Unknown ride type");
                }
            }

            _legacyType.ride_type[i] = rideType;
        }

        _legacyType.max_height = Json::GetNumber<uint8_t>(properties["maxHeight"]);

        // This needs to be set for both shops/facilities _and_ regular rides.
        for (auto& item : _legacyType.shop_item)
        {
            item = ShopItem::None;
        }

        auto carColours = Json::AsArray(properties["carColours"]);
        _presetColours = ReadJsonCarColours(carColours);

        if (IsRideTypeShopOrFacility(_legacyType.ride_type[0]))
        {
            // Standard car info for a shop
            auto& car = _legacyType.vehicles[0];
            car.spacing = 544;
            car.sprite_flags = VEHICLE_SPRITE_FLAG_FLAT;
            car.sprite_width = 1;
            car.sprite_height_negative = 1;
            car.sprite_height_positive = 1;
            car.flags = VEHICLE_ENTRY_FLAG_SPINNING;
            car.car_visual = VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE;
            car.friction_sound_id = OpenRCT2::Audio::SoundId::Null;
            car.sound_range = 0xFF;
            car.draw_order = 6;

            // Shop item
            auto rideSells = Json::AsArray(properties["sells"]);
            auto numShopItems = std::min(static_cast<size_t>(NUM_SHOP_ITEMS_PER_RIDE), rideSells.size());
            for (size_t i = 0; i < numShopItems; i++)
            {
                auto shopItem = ParseShopItem(Json::GetString(rideSells[i]));
                if (shopItem == ShopItem::None)
                {
                    context->LogWarning(ObjectError::InvalidProperty, "Unknown shop item");
                }

                _legacyType.shop_item[i] = shopItem;
            }
        }
        else
        {
            ReadJsonVehicleInfo(context, properties);

            auto swingMode = Json::GetNumber<int32_t>(properties["swingMode"]);
            if (swingMode == 1)
            {
                _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1;
            }
            else if (swingMode == 2)
            {
                _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_1;
                _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_SWING_MODE_2;
            }

            auto rotationMode = Json::GetNumber<int32_t>(properties["rotationMode"]);
            if (rotationMode == 1)
            {
                _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_1;
            }
            else if (rotationMode == 2)
            {
                _legacyType.flags |= RIDE_ENTRY_FLAG_ALTERNATIVE_ROTATION_MODE_2;
            }

            auto ratingMultiplier = properties["ratingMultipler"];
            if (ratingMultiplier.is_object())
            {
                _legacyType.excitement_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["excitement"]);
                _legacyType.intensity_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["intensity"]);
                _legacyType.nausea_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["nausea"]);
            }
        }

        _legacyType.BuildMenuPriority = Json::GetNumber<uint8_t>(properties["buildMenuPriority"]);
        _legacyType.flags |= Json::GetFlags<uint32_t>(
            properties,
            {
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
                { "disablePainting", RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB },
            });
    }

    RideObjectUpdateRideType(&_legacyType);
    PopulateTablesFromJson(context, root);
}

void RideObject::ReadJsonVehicleInfo([[maybe_unused]] IReadObjectContext* context, json_t& properties)
{
    Guard::Assert(properties.is_object(), "RideObject::ReadJsonVehicleInfo expects parameter properties to be object");

    _legacyType.min_cars_in_train = Json::GetNumber<uint8_t>(properties["minCarsPerTrain"], 1);
    _legacyType.max_cars_in_train = Json::GetNumber<uint8_t>(properties["maxCarsPerTrain"], 1);
    _legacyType.cars_per_flat_ride = Json::GetNumber<uint8_t>(properties["carsPerFlatRide"], 255);
    _legacyType.zero_cars = Json::GetNumber<uint8_t>(properties["numEmptyCars"]);

    // Train formation from car indices
    _legacyType.default_vehicle = Json::GetNumber<uint8_t>(properties["defaultCar"]);
    _legacyType.tab_vehicle = Json::GetNumber<uint8_t>(properties["tabCar"]);

    float tabScale = Json::GetNumber<float>(properties["tabScale"]);
    if (tabScale != 0 && tabScale <= 0.5f)
    {
        _legacyType.flags |= RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF;
    }

    json_t headCars = Json::AsArray(properties["headCars"]);
    json_t tailCars = Json::AsArray(properties["tailCars"]);

    // 0xFF means N/A.
    _legacyType.front_vehicle = Json::GetNumber<uint8_t>(headCars[0], 0xFF);
    _legacyType.second_vehicle = Json::GetNumber<uint8_t>(headCars[1], 0xFF);
    _legacyType.third_vehicle = Json::GetNumber<uint8_t>(headCars[2], 0xFF);
    _legacyType.rear_vehicle = Json::GetNumber<uint8_t>(tailCars[0], 0xFF);

    auto cars = ReadJsonCars(properties["cars"]);
    auto numCars = std::min(std::size(_legacyType.vehicles), cars.size());
    for (size_t i = 0; i < numCars; i++)
    {
        _legacyType.vehicles[i] = cars[i];
    }
}

std::vector<rct_ride_entry_vehicle> RideObject::ReadJsonCars(json_t& jCars)
{
    std::vector<rct_ride_entry_vehicle> cars;

    if (jCars.is_array())
    {
        for (auto& jCar : jCars)
        {
            if (jCar.is_object())
            {
                cars.push_back(ReadJsonCar(jCar));
            }
        }
    }
    else if (jCars.is_object())
    {
        cars.push_back(ReadJsonCar(jCars));
    }

    return cars;
}

rct_ride_entry_vehicle RideObject::ReadJsonCar(json_t& jCar)
{
    Guard::Assert(jCar.is_object(), "RideObject::ReadJsonCar expects parameter jCar to be object");

    rct_ride_entry_vehicle car = {};
    car.rotation_frame_mask = Json::GetNumber<uint16_t>(jCar["rotationFrameMask"]);
    car.spacing = Json::GetNumber<uint32_t>(jCar["spacing"]);
    car.car_mass = Json::GetNumber<uint16_t>(jCar["mass"]);
    car.tab_height = Json::GetNumber<int8_t>(jCar["tabOffset"]);
    car.num_seats = Json::GetNumber<uint8_t>(jCar["numSeats"]);
    if (Json::GetBoolean(jCar["seatsInPairs"], true) && car.num_seats > 1)
    {
        car.num_seats |= VEHICLE_SEAT_PAIR_FLAG;
    }

    car.sprite_width = Json::GetNumber<uint8_t>(jCar["spriteWidth"]);
    car.sprite_height_negative = Json::GetNumber<uint8_t>(jCar["spriteHeightNegative"]);
    car.sprite_height_positive = Json::GetNumber<uint8_t>(jCar["spriteHeightPositive"]);
    car.animation = Json::GetNumber<uint8_t>(jCar["animation"]);
    car.base_num_frames = Json::GetNumber<uint16_t>(jCar["baseNumFrames"]);
    car.no_vehicle_images = Json::GetNumber<uint32_t>(jCar["numImages"]);
    car.no_seating_rows = Json::GetNumber<uint8_t>(jCar["numSeatRows"]);
    car.spinning_inertia = Json::GetNumber<uint8_t>(jCar["spinningInertia"]);
    car.spinning_friction = Json::GetNumber<uint8_t>(jCar["spinningFriction"]);
    car.friction_sound_id = Json::GetEnum<OpenRCT2::Audio::SoundId>(jCar["frictionSoundId"], OpenRCT2::Audio::SoundId::Null);
    car.log_flume_reverser_vehicle_type = Json::GetNumber<uint8_t>(jCar["logFlumeReverserVehicleType"]);
    car.sound_range = Json::GetNumber<uint8_t>(jCar["soundRange"], 255);
    car.double_sound_frequency = Json::GetNumber<uint8_t>(jCar["doubleSoundFrequency"]);
    car.powered_acceleration = Json::GetNumber<uint8_t>(jCar["poweredAcceleration"]);
    car.powered_max_speed = Json::GetNumber<uint8_t>(jCar["poweredMaxSpeed"]);
    car.car_visual = Json::GetNumber<uint8_t>(jCar["carVisual"]);
    car.effect_visual = Json::GetNumber<uint8_t>(jCar["effectVisual"], 1);
    car.draw_order = Json::GetNumber<uint8_t>(jCar["drawOrder"]);
    car.num_vertical_frames_override = Json::GetNumber<uint8_t>(jCar["numVerticalFramesOverride"]);

    auto jLoadingPositions = jCar["loadingPositions"];
    if (jLoadingPositions.is_array())
    {
        for (auto& jPos : jLoadingPositions)
        {
            car.peep_loading_positions.push_back(Json::GetNumber<int8_t>(jPos));
        }
    }
    else
    {
        auto jLoadingWaypoints = jCar["loadingWaypoints"];
        if (jLoadingWaypoints.is_array())
        {
            car.flags |= VEHICLE_ENTRY_FLAG_LOADING_WAYPOINTS;
            car.peep_loading_waypoint_segments = Json::GetNumber<uint8_t>(jCar["numSegments"]);

            for (auto& jRoute : jLoadingWaypoints)
            {
                if (jRoute.is_array())
                {
                    std::array<CoordsXY, 3> entry;

                    for (size_t j = 0; j < 3; ++j)
                    {
                        auto jWaypoint = jRoute[j];
                        if (jWaypoint.is_array() && jWaypoint.size() >= 2)
                        {
                            int32_t x = Json::GetNumber<int32_t>(jWaypoint[0]);
                            int32_t y = Json::GetNumber<int32_t>(jWaypoint[1]);
                            entry[j] = { x, y };
                        }
                    }

                    car.peep_loading_waypoints.push_back(std::move(entry));
                }
            }
        }
    }

    auto jFrames = jCar["frames"];
    if (jFrames.is_object())
    {
        car.sprite_flags = Json::GetFlags<uint32_t>(
            jFrames,
            {
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
                { "flatToGentleSlopeWhileBankedTransitions",
                  VEHICLE_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS },
                { "corkscrews", VEHICLE_SPRITE_FLAG_CORKSCREWS },
                { "restraintAnimation", VEHICLE_SPRITE_FLAG_RESTRAINT_ANIMATION },
                { "curvedLiftHill", VEHICLE_SPRITE_FLAG_CURVED_LIFT_HILL },
                { "VEHICLE_SPRITE_FLAG_15", VEHICLE_SPRITE_FLAG_USE_4_ROTATION_FRAMES },
            });
    }

    car.flags |= Json::GetFlags<uint32_t>(
        jCar,
        {
            { "isPoweredRideWithUnrestrictedGravity", VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY },
            { "hasNoUpstopWheels", VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS },
            { "hasNoUpstopWheelsBobsleigh", VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH },
            { "isMiniGolf", VEHICLE_ENTRY_FLAG_MINI_GOLF },
            { "isReverserBogie", VEHICLE_ENTRY_FLAG_REVERSER_BOGIE },
            { "isReverserPassengerCar", VEHICLE_ENTRY_FLAG_REVERSER_PASSENGER_CAR },
            { "hasInvertedSpriteSet", VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET },
            { "hasDodgemInUseLights", VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS },
            { "hasAdditionalColour2", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2 },
            { "recalculateSpriteBounds", VEHICLE_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS },
            { "VEHICLE_ENTRY_FLAG_11", VEHICLE_ENTRY_FLAG_USE_16_ROTATION_FRAMES },
            { "overrideNumberOfVerticalFrames", VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES },
            { "spriteBoundsIncludeInvertedSet", VEHICLE_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET },
            { "hasAdditionalSpinningFrames", VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES },
            { "isLift", VEHICLE_ENTRY_FLAG_LIFT },
            { "hasAdditionalColour1", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1 },
            { "hasSwinging", VEHICLE_ENTRY_FLAG_SWINGING },
            { "hasSpinning", VEHICLE_ENTRY_FLAG_SPINNING },
            { "isPowered", VEHICLE_ENTRY_FLAG_POWERED },
            { "hasScreamingRiders", VEHICLE_ENTRY_FLAG_RIDERS_SCREAM },
            { "useSuspendedSwing", VEHICLE_ENTRY_FLAG_SUSPENDED_SWING },
            { "useBoatHireCollisionDetection", VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION },
            { "hasVehicleAnimation", VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION },
            { "hasRiderAnimation", VEHICLE_ENTRY_FLAG_RIDER_ANIMATION },
            { "useWoodenWildMouseSwing", VEHICLE_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING },
            { "useSlideSwing", VEHICLE_ENTRY_FLAG_SLIDE_SWING },
            { "isChairlift", VEHICLE_ENTRY_FLAG_CHAIRLIFT },
            { "isWaterRide", VEHICLE_ENTRY_FLAG_WATER_RIDE },
            { "isGoKart", VEHICLE_ENTRY_FLAG_GO_KART },
            { "useDodgemCarPlacement", VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT },

            // Obsolete flags that have been replaced with named and camelCased values, but kept for now for backward
            // compatibility.
            { "VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY", VEHICLE_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY },
            { "VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS", VEHICLE_ENTRY_FLAG_NO_UPSTOP_WHEELS },
            { "VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH", VEHICLE_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH },
            { "VEHICLE_ENTRY_FLAG_MINI_GOLF", VEHICLE_ENTRY_FLAG_MINI_GOLF },
            { "VEHICLE_ENTRY_FLAG_4", VEHICLE_ENTRY_FLAG_REVERSER_BOGIE },
            { "VEHICLE_ENTRY_FLAG_5", VEHICLE_ENTRY_FLAG_REVERSER_PASSENGER_CAR },
            { "VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET", VEHICLE_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET },
            { "VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS", VEHICLE_ENTRY_FLAG_DODGEM_INUSE_LIGHTS },
            { "VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2 },
            { "VEHICLE_ENTRY_FLAG_10", VEHICLE_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS },
            { "VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES", VEHICLE_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES },
            { "VEHICLE_ENTRY_FLAG_13", VEHICLE_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET },
            { "VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES", VEHICLE_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES },
            { "VEHICLE_ENTRY_FLAG_LIFT", VEHICLE_ENTRY_FLAG_LIFT },
            { "VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1", VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1 },
            { "VEHICLE_ENTRY_FLAG_SWINGING", VEHICLE_ENTRY_FLAG_SWINGING },
            { "VEHICLE_ENTRY_FLAG_SPINNING", VEHICLE_ENTRY_FLAG_SPINNING },
            { "VEHICLE_ENTRY_FLAG_POWERED", VEHICLE_ENTRY_FLAG_POWERED },
            { "VEHICLE_ENTRY_FLAG_RIDERS_SCREAM", VEHICLE_ENTRY_FLAG_RIDERS_SCREAM },
            { "VEHICLE_ENTRY_FLAG_21", VEHICLE_ENTRY_FLAG_SUSPENDED_SWING },
            { "VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION", VEHICLE_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION },
            { "VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION", VEHICLE_ENTRY_FLAG_VEHICLE_ANIMATION },
            { "VEHICLE_ENTRY_FLAG_RIDER_ANIMATION", VEHICLE_ENTRY_FLAG_RIDER_ANIMATION },
            { "VEHICLE_ENTRY_FLAG_25", VEHICLE_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING },
            { "VEHICLE_ENTRY_FLAG_SLIDE_SWING", VEHICLE_ENTRY_FLAG_SLIDE_SWING },
            { "VEHICLE_ENTRY_FLAG_CHAIRLIFT", VEHICLE_ENTRY_FLAG_CHAIRLIFT },
            { "VEHICLE_ENTRY_FLAG_WATER_RIDE", VEHICLE_ENTRY_FLAG_WATER_RIDE },
            { "VEHICLE_ENTRY_FLAG_GO_KART", VEHICLE_ENTRY_FLAG_GO_KART },
            { "VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT", VEHICLE_ENTRY_FLAG_DODGEM_CAR_PLACEMENT },
        });

    return car;
}

vehicle_colour_preset_list RideObject::ReadJsonCarColours(json_t& jCarColours)
{
    Guard::Assert(jCarColours.is_array(), "RideObject::ReadJsonCarColours expects parameter jCarColours to be array");

    // The JSON supports multiple configurations of per car colours, but
    // the ride entry structure currently doesn't allow for it. Assume that
    // a single configuration with multiple colour entries is per car scheme.
    if (jCarColours.size() == 1)
    {
        auto firstElement = Json::AsArray(jCarColours[0]);
        auto numColours = firstElement.size();
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
    for (size_t index = 0; index < jCarColours.size(); index++)
    {
        auto config = ReadJsonColourConfiguration(jCarColours[index]);
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

std::vector<vehicle_colour> RideObject::ReadJsonColourConfiguration(json_t& jColourConfig)
{
    std::vector<vehicle_colour> config;

    for (auto& jColours : jColourConfig)
    {
        vehicle_colour carColour = {};

        auto colours = Json::AsArray(jColours);
        if (colours.size() >= 1)
        {
            carColour.main = Colour::FromString(Json::GetString(colours[0]));
            carColour.additional_1 = carColour.main;
            carColour.additional_2 = carColour.main;
            if (colours.size() >= 2)
            {
                carColour.additional_1 = Colour::FromString(Json::GetString(colours[1]));
            }
            if (colours.size() >= 3)
            {
                carColour.additional_2 = Colour::FromString(Json::GetString(colours[2]));
            }
        }
        config.push_back(carColour);
    }
    return config;
}

bool RideObject::IsRideTypeShopOrFacility(RideType rideType)
{
    switch (rideType)
    {
        case RideType::TOILETS:
        case RideType::SHOP:
        case RideType::DRINK_STALL:
        case RideType::FOOD_STALL:
        case RideType::INFORMATION_KIOSK:
        case RideType::CASH_MACHINE:
        case RideType::FIRST_AID:
            return true;
        default:
            return false;
    }
}

static const EnumMap<RideType> RideTypeLookupTable{
    { "spiral_rc", RideType::SPIRAL_ROLLER_COASTER },
    { "stand_up_rc", RideType::STAND_UP_ROLLER_COASTER },
    { "suspended_swinging_rc", RideType::SUSPENDED_SWINGING_COASTER },
    { "inverted_rc", RideType::INVERTED_ROLLER_COASTER },
    { "junior_rc", RideType::JUNIOR_ROLLER_COASTER },
    { "miniature_railway", RideType::MINIATURE_RAILWAY },
    { "monorail", RideType::MONORAIL },
    { "mini_suspended_rc", RideType::MINI_SUSPENDED_COASTER },
    { "boat_hire", RideType::BOAT_HIRE },
    { "wooden_wild_mouse", RideType::WOODEN_WILD_MOUSE },
    { "steeplechase", RideType::STEEPLECHASE },
    { "car_ride", RideType::CAR_RIDE },
    { "launched_freefall", RideType::LAUNCHED_FREEFALL },
    { "bobsleigh_rc", RideType::BOBSLEIGH_COASTER },
    { "observation_tower", RideType::OBSERVATION_TOWER },
    { "looping_rc", RideType::LOOPING_ROLLER_COASTER },
    { "dinghy_slide", RideType::DINGHY_SLIDE },
    { "mine_train_rc", RideType::MINE_TRAIN_COASTER },
    { "chairlift", RideType::CHAIRLIFT },
    { "corkscrew_rc", RideType::CORKSCREW_ROLLER_COASTER },
    { "maze", RideType::MAZE },
    { "spiral_slide", RideType::SPIRAL_SLIDE },
    { "go_karts", RideType::GO_KARTS },
    { "log_flume", RideType::LOG_FLUME },
    { "river_rapids", RideType::RIVER_RAPIDS },
    { "dodgems", RideType::DODGEMS },
    { "swinging_ship", RideType::SWINGING_SHIP },
    { "swinging_inverter_ship", RideType::SWINGING_INVERTER_SHIP },
    { "food_stall", RideType::FOOD_STALL },
    { "drink_stall", RideType::DRINK_STALL },
    { "shop", RideType::SHOP },
    { "merry_go_round", RideType::MERRY_GO_ROUND },
    { "information_kiosk", RideType::INFORMATION_KIOSK },
    { "toilets", RideType::TOILETS },
    { "ferris_wheel", RideType::FERRIS_WHEEL },
    { "motion_simulator", RideType::MOTION_SIMULATOR },
    { "3d_cinema", RideType::_3D_CINEMA },
    { "top_spin", RideType::TOP_SPIN },
    { "space_rings", RideType::SPACE_RINGS },
    { "reverse_freefall_rc", RideType::REVERSE_FREEFALL_COASTER },
    { "lift", RideType::LIFT },
    { "vertical_drop_rc", RideType::VERTICAL_DROP_ROLLER_COASTER },
    { "cash_machine", RideType::CASH_MACHINE },
    { "twist", RideType::TWIST },
    { "haunted_house", RideType::HAUNTED_HOUSE },
    { "first_aid", RideType::FIRST_AID },
    { "circus", RideType::CIRCUS },
    { "ghost_train", RideType::GHOST_TRAIN },
    { "twister_rc", RideType::TWISTER_ROLLER_COASTER },
    { "wooden_rc", RideType::WOODEN_ROLLER_COASTER },
    { "side_friction_rc", RideType::SIDE_FRICTION_ROLLER_COASTER },
    { "steel_wild_mouse", RideType::STEEL_WILD_MOUSE },
    { "multi_dimension_rc", RideType::MULTI_DIMENSION_ROLLER_COASTER },
    { "flying_rc", RideType::FLYING_ROLLER_COASTER },
    { "virginia_reel", RideType::VIRGINIA_REEL },
    { "splash_boats", RideType::SPLASH_BOATS },
    { "mini_helicopters", RideType::MINI_HELICOPTERS },
    { "lay_down_rc", RideType::LAY_DOWN_ROLLER_COASTER },
    { "suspended_monorail", RideType::SUSPENDED_MONORAIL },
    { "reverser_rc", RideType::REVERSER_ROLLER_COASTER },
    { "heartline_twister_rc", RideType::HEARTLINE_TWISTER_COASTER },
    { "mini_golf", RideType::MINI_GOLF },
    { "giga_rc", RideType::GIGA_COASTER },
    { "roto_drop", RideType::ROTO_DROP },
    { "flying_saucers", RideType::FLYING_SAUCERS },
    { "crooked_house", RideType::CROOKED_HOUSE },
    { "monorail_cycles", RideType::MONORAIL_CYCLES },
    { "compact_inverted_rc", RideType::COMPACT_INVERTED_COASTER },
    { "water_coaster", RideType::WATER_COASTER },
    { "air_powered_vertical_rc", RideType::AIR_POWERED_VERTICAL_COASTER },
    { "inverted_hairpin_rc", RideType::INVERTED_HAIRPIN_COASTER },
    { "magic_carpet", RideType::MAGIC_CARPET },
    { "submarine_ride", RideType::SUBMARINE_RIDE },
    { "river_rafts", RideType::RIVER_RAFTS },
    { "enterprise", RideType::ENTERPRISE },
    { "inverted_impulse_rc", RideType::INVERTED_IMPULSE_COASTER },
    { "mini_rc", RideType::MINI_ROLLER_COASTER },
    { "mine_ride", RideType::MINE_RIDE },
    { "lim_launched_rc", RideType::LIM_LAUNCHED_ROLLER_COASTER },
    { "hypercoaster", RideType::HYPERCOASTER },
    { "hyper_twister", RideType::HYPER_TWISTER },
    { "monster_trucks", RideType::MONSTER_TRUCKS },
    { "spinning_wild_mouse", RideType::SPINNING_WILD_MOUSE },
    { "classic_mini_rc", RideType::CLASSIC_MINI_ROLLER_COASTER },
    { "hybrid_rc", RideType::HYBRID_COASTER },
    { "single_rail_rc", RideType::SINGLE_RAIL_ROLLER_COASTER },
};

RideType RideObject::ParseRideType(const std::string& s)
{
    auto result = RideTypeLookupTable.find(s);
    return (result != RideTypeLookupTable.end()) ? result->second : RideType::RIDE_TYPE_NULL;
}

static const EnumMap<uint8_t> RideCategoryLookupTable{
    { "transport", RIDE_CATEGORY_TRANSPORT },
    { "gentle", RIDE_CATEGORY_GENTLE },
    { "rollercoaster", RIDE_CATEGORY_ROLLERCOASTER },
    { "thrill", RIDE_CATEGORY_THRILL },
    { "water", RIDE_CATEGORY_WATER },
    { "stall", RIDE_CATEGORY_SHOP },
};

uint8_t RideObject::ParseRideCategory(const std::string& s)
{
    auto result = RideCategoryLookupTable.find(s);
    return (result != RideCategoryLookupTable.end()) ? result->second : static_cast<uint8_t>(RIDE_CATEGORY_TRANSPORT);
}

static const EnumMap<ShopItem> ShopItemLookupTable{
    { "burger", ShopItem::Burger },
    { "chips", ShopItem::Chips },
    { "ice_cream", ShopItem::IceCream },
    { "candyfloss", ShopItem::Candyfloss },
    { "pizza", ShopItem::Pizza },
    { "popcorn", ShopItem::Popcorn },
    { "hot_dog", ShopItem::HotDog },
    { "tentacle", ShopItem::Tentacle },
    { "toffee_apple", ShopItem::ToffeeApple },
    { "doughnut", ShopItem::Doughnut },
    { "chicken", ShopItem::Chicken },
    { "pretzel", ShopItem::Pretzel },
    { "funnel_cake", ShopItem::FunnelCake },
    { "beef_noodles", ShopItem::BeefNoodles },
    { "fried_rice_noodles", ShopItem::FriedRiceNoodles },
    { "wonton_soup", ShopItem::WontonSoup },
    { "meatball_soup", ShopItem::MeatballSoup },
    { "sub_sandwich", ShopItem::SubSandwich },
    { "cookie", ShopItem::Cookie },
    { "roast_sausage", ShopItem::RoastSausage },
    { "drink", ShopItem::Drink },
    { "coffee", ShopItem::Coffee },
    { "lemonade", ShopItem::Lemonade },
    { "chocolate", ShopItem::Chocolate },
    { "iced_tea", ShopItem::IcedTea },
    { "fruit_juice", ShopItem::FruitJuice },
    { "soybean_milk", ShopItem::SoybeanMilk },
    { "sujeonggwa", ShopItem::Sujeonggwa },
    { "balloon", ShopItem::Balloon },
    { "toy", ShopItem::Toy },
    { "map", ShopItem::Map },
    { "photo", ShopItem::Photo },
    { "umbrella", ShopItem::Umbrella },
    { "voucher", ShopItem::Voucher },
    { "hat", ShopItem::Hat },
    { "tshirt", ShopItem::TShirt },
    { "sunglasses", ShopItem::Sunglasses },
};

ShopItem RideObject::ParseShopItem(const std::string& s)
{
    auto result = ShopItemLookupTable.find(s);
    return (result != ShopItemLookupTable.end()) ? result->second : ShopItem::None;
}
