/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "../drawing/Image.h"
#include "../entity/Yaw.hpp"
#include "../localisation/Language.h"
#include "../rct2/DATLimits.h"
#include "../rct2/RCT2.h"
#include "../ride/CarEntry.h"
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
using namespace OpenRCT2::Entity::Yaw;

static const uint8_t SpriteGroupMultiplier[EnumValue(SpriteGroupType::Count)] = {
    1, 2, 2, 2, 2, 2, 2, 10, 1, 2, 2, 2, 2, 2, 2, 2, 6, 4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 20, 3, 1,
};

static constexpr SpritePrecision PrecisionFromNumFrames(uint8_t numRotationFrames)
{
    if (numRotationFrames == 0)
        return SpritePrecision::None;
    else
        return static_cast<SpritePrecision>(UtilBitScanForward(numRotationFrames) + 1);
}

static void RideObjectUpdateRideType(RideObjectEntry& rideEntry)
{
    for (auto i = 0; i < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; i++)
    {
        auto oldRideType = rideEntry.ride_type[i];
        if (oldRideType != RIDE_TYPE_NULL)
        {
            rideEntry.ride_type[i] = RCT2::RCT2RideTypeToOpenRCT2RideType(oldRideType, rideEntry);
        }
    }
}

void RideObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
{
    stream->Seek(8, STREAM_SEEK_CURRENT);
    _legacyType.flags = stream->ReadValue<uint32_t>();
    for (auto& rideType : _legacyType.ride_type)
    {
        rideType = stream->ReadValue<uint8_t>();
        if (!RideTypeIsValid(rideType))
            rideType = RIDE_TYPE_NULL;
    }
    _legacyType.min_cars_in_train = stream->ReadValue<uint8_t>();
    _legacyType.max_cars_in_train = stream->ReadValue<uint8_t>();
    _legacyType.cars_per_flat_ride = stream->ReadValue<uint8_t>();
    _legacyType.zero_cars = stream->ReadValue<uint8_t>();
    _legacyType.TabCar = stream->ReadValue<uint8_t>();
    _legacyType.DefaultCar = stream->ReadValue<uint8_t>();
    _legacyType.FrontCar = stream->ReadValue<uint8_t>();
    _legacyType.SecondCar = stream->ReadValue<uint8_t>();
    _legacyType.RearCar = stream->ReadValue<uint8_t>();
    _legacyType.ThirdCar = stream->ReadValue<uint8_t>();

    _legacyType.BuildMenuPriority = 0;
    // Skip Pad019
    stream->Seek(1, STREAM_SEEK_CURRENT);

    for (auto& carEntry : _legacyType.Cars)
    {
        ReadLegacyCar(context, stream, &carEntry);
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
        _presetColours.list[i] = stream->ReadValue<VehicleColour>();
    }

    if (IsRideTypeShopOrFacility(_legacyType.ride_type[0]))
    {
        // This used to be hard-coded. JSON objects set this themselves.
        _presetColours.count = 1;
        _presetColours.list[0] = { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED };

        if (_legacyType.ride_type[0] == RIDE_TYPE_FOOD_STALL || _legacyType.ride_type[0] == RIDE_TYPE_DRINK_STALL)
        {
            // In RCT2, no food or drink stall could be recoloured.
            _legacyType.flags |= RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB;
        }
    }

    // Read peep loading positions
    for (int32_t i = 0; i < RCT2::ObjectLimits::MaxCarTypesPerRideEntry; i++)
    {
        _peepLoadingWaypoints[i].clear();
        _peepLoadingPositions[i].clear();

        uint16_t numPeepLoadingPositions = stream->ReadValue<uint8_t>();
        if (numPeepLoadingPositions == 255)
        {
            numPeepLoadingPositions = stream->ReadValue<uint16_t>();
        }

        if (_legacyType.Cars[i].flags & CAR_ENTRY_FLAG_LOADING_WAYPOINTS)
        {
            _legacyType.Cars[i].peep_loading_waypoint_segments = stream->ReadValue<int8_t>() == 0 ? 0 : 4;
            if (_legacyType.ride_type[0] == RIDE_TYPE_ENTERPRISE)
            {
                _legacyType.Cars[i].peep_loading_waypoint_segments = 8;
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
            _legacyType.Cars[i].peep_loading_waypoint_segments = 0;

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
    RideObjectUpdateRideType(_legacyType);
    _legacyType.Clearance = GetDefaultClearance();
}

void RideObject::Load()
{
    _legacyType.obj = this;

    GetStringTable().Sort();
    _legacyType.naming.Name = LanguageAllocateObjectString(GetName());
    _legacyType.naming.Description = LanguageAllocateObjectString(GetDescription());
    _legacyType.capacity = LanguageAllocateObjectString(GetCapacity());
    _legacyType.images_offset = GfxObjectAllocateImages(GetImageTable().GetImages(), GetImageTable().GetCount());
    _legacyType.vehicle_preset_list = &_presetColours;

    int32_t currentCarImagesOffset = _legacyType.images_offset + RCT2::ObjectLimits::MaxRideTypesPerRideEntry;
    for (int32_t i = 0; i < RCT2::ObjectLimits::MaxCarTypesPerRideEntry; i++)
    {
        CarEntry& carEntry = _legacyType.Cars[i];
        if (carEntry.GroupEnabled(SpriteGroupType::SlopeFlat))
        {
            // RCT2 calculates num_vertical_frames and num_horizontal_frames and overwrites these properties on the car
            // entry. Immediately afterwards, the two were multiplied in order to calculate base_num_frames and were never used
            // again. This has been changed to use the calculation results directly - num_vertical_frames and
            // num_horizontal_frames are no longer set on the car entry.
            // 0x6DE946
            carEntry.base_num_frames = CalculateNumVerticalFrames(carEntry) * CalculateNumHorizontalFrames(carEntry);
            uint32_t baseImageId = currentCarImagesOffset;
            uint32_t imageIndex = baseImageId;
            carEntry.base_image_id = baseImageId;

            for (uint8_t spriteGroup = 0; spriteGroup < EnumValue(SpriteGroupType::Count); spriteGroup++)
            {
                if (carEntry.SpriteGroups[spriteGroup].Enabled())
                {
                    carEntry.SpriteGroups[spriteGroup].imageId = imageIndex;
                    const auto spriteCount = carEntry.base_num_frames
                        * carEntry.NumRotationSprites(static_cast<SpriteGroupType>(spriteGroup))
                        * SpriteGroupMultiplier[spriteGroup];
                    imageIndex += spriteCount;
                }
            }

            carEntry.NumCarImages = imageIndex - currentCarImagesOffset;

            // Move the offset over this car’s images. Including peeps
            currentCarImagesOffset = imageIndex + carEntry.no_seating_rows * carEntry.NumCarImages;
            // 0x6DEB0D

            if (!(carEntry.flags & CAR_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS))
            {
                int32_t num_images = currentCarImagesOffset - baseImageId;
                if (carEntry.flags & CAR_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET)
                {
                    num_images *= 2;
                }

                if (!gOpenRCT2NoGraphics)
                {
                    CarEntrySetImageMaxSizes(carEntry, num_images);
                }
            }

            if (!_peepLoadingPositions[i].empty())
            {
                carEntry.peep_loading_positions = std::move(_peepLoadingPositions[i]);
            }

            if (!_peepLoadingWaypoints[i].empty())
            {
                carEntry.peep_loading_waypoints = std::move(_peepLoadingWaypoints[i]);
            }
        }
    }
}

void RideObject::Unload()
{
    LanguageFreeObjectString(_legacyType.naming.Name);
    LanguageFreeObjectString(_legacyType.naming.Description);
    LanguageFreeObjectString(_legacyType.capacity);
    GfxObjectFreeImages(_legacyType.images_offset, GetImageTable().GetCount());

    _legacyType.naming.Name = 0;
    _legacyType.naming.Description = 0;
    _legacyType.capacity = 0;
    _legacyType.images_offset = 0;
}

void RideObject::DrawPreview(DrawPixelInfo* dpi, [[maybe_unused]] int32_t width, [[maybe_unused]] int32_t height) const
{
    uint32_t imageId = _legacyType.images_offset;

    for (auto rideType : _legacyType.ride_type)
    {
        if (rideType != RIDE_TYPE_NULL)
            break;

        imageId++;
    }

    GfxDrawSprite(dpi, ImageId(imageId), { 0, 0 });
}

std::string RideObject::GetDescription() const
{
    return GetString(ObjectStringID::DESCRIPTION);
}

std::string RideObject::GetCapacity() const
{
    return GetString(ObjectStringID::CAPACITY);
}

ImageIndex RideObject::GetPreviewImage(ride_type_t type)
{
    auto it = std::find(std::begin(_legacyType.ride_type), std::end(_legacyType.ride_type), type);
    if (it == std::end(_legacyType.ride_type))
    {
        return ImageIndexUndefined;
    }

    return _legacyType.images_offset + std::distance(std::begin(_legacyType.ride_type), it);
}

void RideObject::GetRepositoryItem(ObjectRepositoryItem* item) const
{
    // Find the first non-null ride type, to be used when checking the ride group and determining the category.
    auto firstRideType = _legacyType.GetFirstNonNullRideType();
    uint8_t category = GetRideTypeDescriptor(firstRideType).Category;

    for (int32_t i = 0; i < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; i++)
    {
        item->RideInfo.RideType[i] = _legacyType.ride_type[i];
    }
    for (int32_t i = 0; i < RCT2::ObjectLimits::MaxCategoriesPerRide; i++)
    {
        item->RideInfo.RideCategory[i] = category;
    }

    item->RideInfo.RideFlags = 0;
}

void RideObject::ReadLegacyCar([[maybe_unused]] IReadObjectContext* context, IStream* stream, CarEntry* car)
{
    car->TabRotationMask = stream->ReadValue<uint16_t>();
    stream->Seek(2 * 1, STREAM_SEEK_CURRENT);
    car->spacing = stream->ReadValue<uint32_t>();
    car->car_mass = stream->ReadValue<uint16_t>();
    car->tab_height = stream->ReadValue<int8_t>();
    car->num_seats = stream->ReadValue<uint8_t>();
    uint16_t spriteGroups = stream->ReadValue<uint16_t>();
    car->sprite_width = stream->ReadValue<uint8_t>();
    car->sprite_height_negative = stream->ReadValue<uint8_t>();
    car->sprite_height_positive = stream->ReadValue<uint8_t>();
    car->animation = stream->ReadValue<uint8_t>();
    car->flags = stream->ReadValue<uint32_t>();
    car->base_num_frames = stream->ReadValue<uint16_t>();
    stream->Seek(15 * 4, STREAM_SEEK_CURRENT);
    car->no_seating_rows = stream->ReadValue<uint8_t>();
    car->spinning_inertia = stream->ReadValue<uint8_t>();
    car->spinning_friction = stream->ReadValue<uint8_t>();
    car->friction_sound_id = stream->ReadValue<OpenRCT2::Audio::SoundId>();
    car->ReversedCarIndex = stream->ReadValue<uint8_t>();
    car->sound_range = stream->ReadValue<uint8_t>();
    car->double_sound_frequency = stream->ReadValue<uint8_t>();
    car->powered_acceleration = stream->ReadValue<uint8_t>();
    car->powered_max_speed = stream->ReadValue<uint8_t>();
    car->PaintStyle = stream->ReadValue<uint8_t>();
    car->effect_visual = stream->ReadValue<uint8_t>();
    car->draw_order = stream->ReadValue<uint8_t>();
    car->num_vertical_frames_override = stream->ReadValue<uint8_t>();
    stream->Seek(4, STREAM_SEEK_CURRENT);
    ReadLegacySpriteGroups(car, spriteGroups);
}

uint8_t RideObject::CalculateNumVerticalFrames(const CarEntry& carEntry)
{
    // 0x6DE90B
    uint8_t numVerticalFrames;
    if (carEntry.flags & CAR_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES)
    {
        numVerticalFrames = carEntry.num_vertical_frames_override;
    }
    else
    {
        if (!(carEntry.flags & CAR_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES))
        {
            if (carEntry.flags & CAR_ENTRY_FLAG_VEHICLE_ANIMATION
                && carEntry.animation != CAR_ENTRY_ANIMATION_OBSERVATION_TOWER)
            {
                if (!(carEntry.flags & CAR_ENTRY_FLAG_DODGEM_INUSE_LIGHTS))
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

uint8_t RideObject::CalculateNumHorizontalFrames(const CarEntry& carEntry)
{
    uint8_t numHorizontalFrames;
    if (carEntry.flags & CAR_ENTRY_FLAG_SWINGING)
    {
        if (!(carEntry.flags & CAR_ENTRY_FLAG_SUSPENDED_SWING) && !(carEntry.flags & CAR_ENTRY_FLAG_SLIDE_SWING))
        {
            if (carEntry.flags & CAR_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING)
            {
                numHorizontalFrames = 3;
            }
            else
            {
                numHorizontalFrames = 5;
            }
        }
        else if (!(carEntry.flags & CAR_ENTRY_FLAG_SUSPENDED_SWING) || !(carEntry.flags & CAR_ENTRY_FLAG_SLIDE_SWING))
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

        for (size_t i = 0; i < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; i++)
        {
            ObjectEntryIndex rideType = RIDE_TYPE_NULL;

            if (i < numRideTypes)
            {
                rideType = ParseRideType(Json::GetString(rideTypes[i]));

                if (rideType == RIDE_TYPE_NULL)
                {
                    context->LogError(ObjectError::InvalidProperty, "Unknown ride type");
                }
            }

            _legacyType.ride_type[i] = rideType;
        }

        _legacyType.max_height = Json::GetNumber<uint8_t>(properties["maxHeight"]);
        _legacyType.Clearance = Json::GetNumber<uint8_t>(properties["clearance"], GetDefaultClearance());

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
            auto& car = _legacyType.Cars[0];
            car.spacing = 544;
            car.SpriteGroups[EnumValue(SpriteGroupType::SlopeFlat)].spritePrecision = SpritePrecision::Sprites4;
            car.sprite_width = 1;
            car.sprite_height_negative = 1;
            car.sprite_height_positive = 1;
            car.flags = CAR_ENTRY_FLAG_SPINNING;
            car.PaintStyle = VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE;
            car.friction_sound_id = OpenRCT2::Audio::SoundId::Null;
            car.sound_range = 0xFF;
            car.draw_order = 6;

            // Shop item
            auto rideSells = Json::AsArray(properties["sells"]);
            auto numShopItems = std::min(static_cast<size_t>(RCT2::ObjectLimits::MaxShopItemsPerRideEntry), rideSells.size());
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
                { "riderControlsSpeed", RIDE_ENTRY_FLAG_RIDER_CONTROLS_SPEED },
                { "hideEmptyTrains", RIDE_ENTRY_FLAG_HIDE_EMPTY_TRAINS },
            });
    }

    RideObjectUpdateRideType(_legacyType);
    PopulateTablesFromJson(context, root);
}

void RideObject::ReadJsonVehicleInfo([[maybe_unused]] IReadObjectContext* context, json_t& properties)
{
    Guard::Assert(properties.is_object(), "RideObject::ReadJsonVehicleInfo expects parameter properties to be object");

    _legacyType.min_cars_in_train = Json::GetNumber<uint8_t>(properties["minCarsPerTrain"], 1);
    _legacyType.max_cars_in_train = Json::GetNumber<uint8_t>(properties["maxCarsPerTrain"], 1);
    _legacyType.cars_per_flat_ride = Json::GetNumber<uint8_t>(properties["carsPerFlatRide"], NoFlatRideCars);
    _legacyType.zero_cars = Json::GetNumber<uint8_t>(properties["numEmptyCars"]);

    // Train formation from car indices
    _legacyType.DefaultCar = Json::GetNumber<uint8_t>(properties["defaultCar"]);
    _legacyType.TabCar = Json::GetNumber<uint8_t>(properties["tabCar"]);

    float tabScale = Json::GetNumber<float>(properties["tabScale"]);
    if (tabScale != 0 && tabScale <= 0.5f)
    {
        _legacyType.flags |= RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF;
    }

    json_t headCars = Json::AsArray(properties["headCars"]);
    json_t tailCars = Json::AsArray(properties["tailCars"]);

    // 0xFF means N/A.
    _legacyType.FrontCar = Json::GetNumber<uint8_t>(headCars[0], 0xFF);
    _legacyType.SecondCar = Json::GetNumber<uint8_t>(headCars[1], 0xFF);
    _legacyType.ThirdCar = Json::GetNumber<uint8_t>(headCars[2], 0xFF);
    _legacyType.RearCar = Json::GetNumber<uint8_t>(tailCars[0], 0xFF);

    auto cars = ReadJsonCars(context, properties["cars"]);
    auto numCars = std::min(std::size(_legacyType.Cars), cars.size());
    for (size_t i = 0; i < numCars; i++)
    {
        _legacyType.Cars[i] = cars[i];
    }
}

std::vector<CarEntry> RideObject::ReadJsonCars([[maybe_unused]] IReadObjectContext* context, json_t& jCars)
{
    std::vector<CarEntry> cars;

    if (jCars.is_array())
    {
        for (auto& jCar : jCars)
        {
            if (jCar.is_object())
            {
                cars.push_back(ReadJsonCar(context, jCar));
            }
        }
    }
    else if (jCars.is_object())
    {
        cars.push_back(ReadJsonCar(context, jCars));
    }

    return cars;
}

CarEntry RideObject::ReadJsonCar([[maybe_unused]] IReadObjectContext* context, json_t& jCar)
{
    Guard::Assert(jCar.is_object(), "RideObject::ReadJsonCar expects parameter jCar to be object");

    CarEntry car = {};
    car.TabRotationMask = Json::GetNumber<uint16_t>(jCar["rotationFrameMask"]);
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
    car.NumCarImages = Json::GetNumber<uint32_t>(jCar["numImages"]);
    car.no_seating_rows = Json::GetNumber<uint8_t>(jCar["numSeatRows"]);
    car.spinning_inertia = Json::GetNumber<uint8_t>(jCar["spinningInertia"]);
    car.spinning_friction = Json::GetNumber<uint8_t>(jCar["spinningFriction"]);
    car.friction_sound_id = Json::GetEnum<OpenRCT2::Audio::SoundId>(jCar["frictionSoundId"], OpenRCT2::Audio::SoundId::Null);
    car.ReversedCarIndex = Json::GetNumber<uint8_t>(jCar["logFlumeReverserVehicleType"]);
    car.sound_range = Json::GetNumber<uint8_t>(jCar["soundRange"], 255);
    car.double_sound_frequency = Json::GetNumber<uint8_t>(jCar["doubleSoundFrequency"]);
    car.powered_acceleration = Json::GetNumber<uint8_t>(jCar["poweredAcceleration"]);
    car.powered_max_speed = Json::GetNumber<uint8_t>(jCar["poweredMaxSpeed"]);
    car.PaintStyle = Json::GetNumber<uint8_t>(jCar["carVisual"]);
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
            car.flags |= CAR_ENTRY_FLAG_LOADING_WAYPOINTS;
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

    car.flags |= Json::GetFlags<uint32_t>(
        jCar,
        {
            { "isPoweredRideWithUnrestrictedGravity", CAR_ENTRY_FLAG_POWERED_RIDE_UNRESTRICTED_GRAVITY },
            { "hasNoUpstopWheels", CAR_ENTRY_FLAG_NO_UPSTOP_WHEELS },
            { "hasNoUpstopWheelsBobsleigh", CAR_ENTRY_FLAG_NO_UPSTOP_BOBSLEIGH },
            { "isMiniGolf", CAR_ENTRY_FLAG_MINI_GOLF },
            { "isReverserBogie", CAR_ENTRY_FLAG_REVERSER_BOGIE },
            { "isReverserPassengerCar", CAR_ENTRY_FLAG_REVERSER_PASSENGER_CAR },
            { "hasInvertedSpriteSet", CAR_ENTRY_FLAG_HAS_INVERTED_SPRITE_SET },
            { "hasDodgemInUseLights", CAR_ENTRY_FLAG_DODGEM_INUSE_LIGHTS },
            { "hasAdditionalColour2", CAR_ENTRY_FLAG_ENABLE_TERNARY_COLOUR },
            { "recalculateSpriteBounds", CAR_ENTRY_FLAG_RECALCULATE_SPRITE_BOUNDS },
            { "overrideNumberOfVerticalFrames", CAR_ENTRY_FLAG_OVERRIDE_NUM_VERTICAL_FRAMES },
            { "spriteBoundsIncludeInvertedSet", CAR_ENTRY_FLAG_SPRITE_BOUNDS_INCLUDE_INVERTED_SET },
            { "hasAdditionalSpinningFrames", CAR_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES },
            { "isLift", CAR_ENTRY_FLAG_LIFT },
            { "hasAdditionalColour1", CAR_ENTRY_FLAG_ENABLE_TRIM_COLOUR },
            { "hasSwinging", CAR_ENTRY_FLAG_SWINGING },
            { "hasSpinning", CAR_ENTRY_FLAG_SPINNING },
            { "isPowered", CAR_ENTRY_FLAG_POWERED },
            { "hasScreamingRiders", CAR_ENTRY_FLAG_RIDERS_SCREAM },
            { "useSuspendedSwing", CAR_ENTRY_FLAG_SUSPENDED_SWING },
            { "useBoatHireCollisionDetection", CAR_ENTRY_FLAG_BOAT_HIRE_COLLISION_DETECTION },
            { "hasVehicleAnimation", CAR_ENTRY_FLAG_VEHICLE_ANIMATION },
            { "hasRiderAnimation", CAR_ENTRY_FLAG_RIDER_ANIMATION },
            { "useWoodenWildMouseSwing", CAR_ENTRY_FLAG_WOODEN_WILD_MOUSE_SWING },
            { "useSlideSwing", CAR_ENTRY_FLAG_SLIDE_SWING },
            { "isChairlift", CAR_ENTRY_FLAG_CHAIRLIFT },
            { "isWaterRide", CAR_ENTRY_FLAG_WATER_RIDE },
            { "isGoKart", CAR_ENTRY_FLAG_GO_KART },
            { "useDodgemCarPlacement", CAR_ENTRY_FLAG_DODGEM_CAR_PLACEMENT },

            // Obsolete flag, only used on Boat Hire. Remaining usages have not yet been updated as of 2022-07-11.
            { "VEHICLE_ENTRY_FLAG_11", CAR_ENTRY_FLAG_USE_16_ROTATION_FRAMES },
        });

    // legacy sprite groups
    auto jFrames = jCar["frames"];
    if (jFrames.is_object())
    {
        uint16_t spriteFlags = Json::GetFlags<uint32_t>(
            jFrames,
            {
                { "flat", CAR_SPRITE_FLAG_FLAT },
                { "gentleSlopes", CAR_SPRITE_FLAG_GENTLE_SLOPES },
                { "steepSlopes", CAR_SPRITE_FLAG_STEEP_SLOPES },
                { "verticalSlopes", CAR_SPRITE_FLAG_VERTICAL_SLOPES },
                { "diagonalSlopes", CAR_SPRITE_FLAG_DIAGONAL_SLOPES },
                { "flatBanked", CAR_SPRITE_FLAG_FLAT_BANKED },
                { "inlineTwists", CAR_SPRITE_FLAG_INLINE_TWISTS },
                { "flatToGentleSlopeBankedTransitions", CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS },
                { "diagonalGentleSlopeBankedTransitions", CAR_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS },
                { "gentleSlopeBankedTransitions", CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS },
                { "gentleSlopeBankedTurns", CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS },
                { "flatToGentleSlopeWhileBankedTransitions", CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS },
                { "corkscrews", CAR_SPRITE_FLAG_CORKSCREWS },
                { "restraintAnimation", CAR_SPRITE_FLAG_RESTRAINT_ANIMATION },
                { "curvedLiftHill", CAR_SPRITE_FLAG_CURVED_LIFT_HILL },
                { "VEHICLE_SPRITE_FLAG_15", CAR_SPRITE_FLAG_USE_4_ROTATION_FRAMES },
            });
        ReadLegacySpriteGroups(&car, spriteFlags);
        return car;
    }

    // OpenRCT2 sprite groups
    auto jRotationCount = jCar["spriteGroups"];
    if (jRotationCount.is_object())
    {
        for (uint8_t i = 0; i < EnumValue(SpriteGroupType::Count); i++)
        {
            auto numRotationFrames = Json::GetNumber<uint8_t>(jRotationCount[SpriteGroupNames[i]], 0);
            if (numRotationFrames != 0)
            {
                if (!IsPowerOf2(numRotationFrames))
                {
                    context->LogError(ObjectError::InvalidProperty, "spriteGroups values must be powers of 2");
                    continue;
                }
                car.SpriteGroups[i].spritePrecision = PrecisionFromNumFrames(numRotationFrames);
            }
        }
    }

    return car;
}

VehicleColourPresetList RideObject::ReadJsonCarColours(json_t& jCarColours)
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
            VehicleColourPresetList list = {};
            list.count = 255;
            std::copy_n(config.data(), std::min<size_t>(numColours, 32), list.list);
            return list;
        }
    }

    // Read first colour for each config
    VehicleColourPresetList list = {};
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

std::vector<VehicleColour> RideObject::ReadJsonColourConfiguration(json_t& jColourConfig)
{
    std::vector<VehicleColour> config;

    for (auto& jColours : jColourConfig)
    {
        VehicleColour carColour = {};

        auto colours = Json::AsArray(jColours);
        if (colours.size() >= 1)
        {
            carColour.Body = Colour::FromString(Json::GetString(colours[0]));
            carColour.Trim = carColour.Body;
            carColour.Tertiary = carColour.Body;
            if (colours.size() >= 2)
            {
                carColour.Trim = Colour::FromString(Json::GetString(colours[1]));
            }
            if (colours.size() >= 3)
            {
                carColour.Tertiary = Colour::FromString(Json::GetString(colours[2]));
            }
        }
        config.push_back(carColour);
    }
    return config;
}

bool RideObject::IsRideTypeShopOrFacility(ride_type_t rideType)
{
    return GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY);
}

ride_type_t RideObject::ParseRideType(const std::string& s)
{
    auto result = std::find_if(
        std::begin(RideTypeDescriptors), std::end(RideTypeDescriptors), [s](const auto& rtd) { return rtd.Name == s; });
    if (result == std::end(RideTypeDescriptors))
        return RIDE_TYPE_NULL;
    else
        return std::distance(std::begin(RideTypeDescriptors), result);
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

// Converts legacy sprite groups into OpenRCT2 sprite groups
void RideObject::ReadLegacySpriteGroups(CarEntry* vehicle, uint16_t spriteGroups)
{
    auto baseSpritePrecision = SpritePrecision::Sprites32;
    if (vehicle->flags & CAR_ENTRY_FLAG_USE_16_ROTATION_FRAMES)
        baseSpritePrecision = SpritePrecision::Sprites16;
    if (vehicle->flags & CAR_SPRITE_FLAG_USE_4_ROTATION_FRAMES)
        baseSpritePrecision = SpritePrecision::Sprites4;

    if (spriteGroups & CAR_SPRITE_FLAG_FLAT)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::SlopeFlat)].spritePrecision = baseSpritePrecision;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_GENTLE_SLOPES)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes12)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes25)].spritePrecision = baseSpritePrecision;
        if (vehicle->flags & CAR_ENTRY_FLAG_SPINNING_ADDITIONAL_FRAMES)
            vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes25)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_STEEP_SLOPES)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes42)].spritePrecision = SpritePrecision::Sprites8;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes60)].spritePrecision = baseSpritePrecision;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_VERTICAL_SLOPES)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes75)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes90)].spritePrecision = baseSpritePrecision;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::SlopesLoop)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::SlopeInverted)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_DIAGONAL_SLOPES)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes8)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes16)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes50)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_FLAT_BANKED)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::FlatBanked22)].spritePrecision = SpritePrecision::Sprites8;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::FlatBanked45)].spritePrecision = baseSpritePrecision;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_INLINE_TWISTS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::FlatBanked67)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::FlatBanked90)].spritePrecision = SpritePrecision::Sprites4;
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::InlineTwists)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_BANKED_TRANSITIONS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes12Banked22)].spritePrecision = baseSpritePrecision;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_DIAGONAL_GENTLE_SLOPE_BANKED_TRANSITIONS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes8Banked22)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TRANSITIONS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes25Banked22)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_GENTLE_SLOPE_BANKED_TURNS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes25Banked45)].spritePrecision = baseSpritePrecision;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_FLAT_TO_GENTLE_SLOPE_WHILE_BANKED_TRANSITIONS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Slopes12Banked45)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_CORKSCREWS)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::Corkscrews)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_RESTRAINT_ANIMATION)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::RestraintAnimation)].spritePrecision = SpritePrecision::Sprites4;
    }
    if (spriteGroups & CAR_SPRITE_FLAG_CURVED_LIFT_HILL)
    {
        vehicle->SpriteGroups[EnumValue(SpriteGroupType::CurvedLiftHill)].spritePrecision = baseSpritePrecision;
    }
}

uint8_t RideObject::GetDefaultClearance() const
{
    auto rideType = _legacyType.GetFirstNonNullRideType();
    const auto& rtd = GetRideTypeDescriptor(rideType);
    return rtd.Heights.ClearanceHeight;
}
