/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma warning(disable : 4706) // assignment within conditional expression

#include "RideObject.h"

#include "../OpenRCT2.h"
#include "../audio/Audio.h"
#include "../core/EnumMap.hpp"
#include "../core/FlagHolder.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/Numerics.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
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

#include <iterator>
#include <unordered_map>

namespace OpenRCT2
{
    using namespace OpenRCT2::Entity::Yaw;
    using namespace OpenRCT2::Numerics;

    /*
     * The number of sprites in the sprite group is the specified precision multiplied by this number. General rule is any slope
     * or bank has its mirror included in the group:
     * - flat unbanked is 1
     * - flat banked is 2 (left/right)
     * - sloped unbanked is 2 (up/down)
     * - sloped & banked is 4 (left/right * up/down)
     * Exceptions:
     * - slopesLoop is 10 (5 slope angles * up/down)
     * - inlineTwists is 6 (3 bank angles * left/right)
     * - slopes25InlineTwists is 12 (3 bank angles * left/right * up/down)
     * - corkscrews is 20 (10 sprites for an entire corkscrew * left/right)
     * - restraints is 3
     * - curvedLiftHillUp and curvedLiftHillDown are 1 (normally would be combined, but aren't due to RCT2)
     */
    static const uint8_t SpriteGroupMultiplier[EnumValue(SpriteGroupType::Count)] = {
        1, 2, 2, 2, 2, 2, 2, 10, 1, 2, 2, 2, 2, 2, 2, 2, 6,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4, 20, 3, 1, 1,
    };
    static_assert(std::size(SpriteGroupMultiplier) == EnumValue(SpriteGroupType::Count));

    constexpr uint8_t DefaultSteamSpawnPosition[] = { 11, 22 };

    static const EnumMap<CarEntryAnimation> AnimationNameLookup{
        { "none", CarEntryAnimation::None },
        { "simpleVehicle", CarEntryAnimation::SimpleVehicle },
        { "steamLocomotive", CarEntryAnimation::SteamLocomotive },
        { "swanBoat", CarEntryAnimation::SwanBoat },
        { "monorailCycle", CarEntryAnimation::MonorailCycle },
        { "MultiDimension", CarEntryAnimation::MultiDimension },
        { "observationTower", CarEntryAnimation::ObservationTower },
        { "animalFlying", CarEntryAnimation::AnimalFlying },
    };

    constexpr auto NumLegacyAnimationTypes = 11;

    struct LegacyAnimationParameters
    {
        uint16_t Speed;
        uint8_t NumFrames;
        CarEntryAnimation Alias;
    };

    constexpr LegacyAnimationParameters VehicleEntryDefaultAnimation[] = {
        { 0, 1, CarEntryAnimation::None },                  // None
        { 1 << 12, 4, CarEntryAnimation::SteamLocomotive }, // Miniature Railway Locomotive
        { 1 << 10, 2, CarEntryAnimation::SwanBoat },        // Swan Boat
        { 1 << 11, 6, CarEntryAnimation::SimpleVehicle },   // Canoe
        { 1 << 11, 7, CarEntryAnimation::SimpleVehicle },   // Rowboat
        { 1 << 10, 2, CarEntryAnimation::SimpleVehicle },   // Water Tricycle
        { 0x3333, 8, CarEntryAnimation::ObservationTower }, // Observation Tower
        { 1 << 10, 4, CarEntryAnimation::SimpleVehicle },   // Mini Helicopter
        { 1 << 11, 4, CarEntryAnimation::MonorailCycle },   // Monorail Cycle
        { 0x3333, 8, CarEntryAnimation::MultiDimension },   // Multi Dimension Coaster
        { 24, 4, CarEntryAnimation::AnimalFlying },         // Animal Flying
    };
    static_assert(std::size(VehicleEntryDefaultAnimation) == NumLegacyAnimationTypes);

    static CarEntryAnimation GetAnimationTypeFromString(const std::string& s)
    {
        auto result = AnimationNameLookup.find(s);
        return (result != AnimationNameLookup.end()) ? result->second : CarEntryAnimation::None;
    }

    static LegacyAnimationParameters GetDefaultAnimationParameters(uint8_t legacyAnimationType)
    {
        if (legacyAnimationType >= NumLegacyAnimationTypes)
            return VehicleEntryDefaultAnimation[0];
        return VehicleEntryDefaultAnimation[legacyAnimationType];
    }

    static constexpr SpritePrecision PrecisionFromNumFrames(uint32_t numRotationFrames)
    {
        if (numRotationFrames == 0)
            return SpritePrecision::None;
        else
            return static_cast<SpritePrecision>(bitScanForward(numRotationFrames) + 1);
    }

    static void RideObjectUpdateRideType(RideObjectEntry& rideEntry)
    {
        for (auto i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
        {
            auto oldRideType = rideEntry.ride_type[i];
            if (oldRideType != kRideTypeNull)
            {
                rideEntry.ride_type[i] = RCT2::RCT2RideTypeToOpenRCT2RideType(oldRideType, rideEntry);
            }
        }
    }

    enum class CarSpriteFlag : uint8_t
    {
        flat,
        gentleSlopes,
        steepSlopes,
        verticalSlopes,
        diagonalSlopes,
        flatBanked,
        inlineTwists,
        flatToGentleSlopeBankedTransitions,
        diagonalGentleSlopeBankedTransitions,
        gentleSlopeBankedTransitions,
        gentleSlopeBankedTurns,
        flatToGentleSlopeWhileBankedTransitions,
        corkscrews,
        restraintAnimation,
        curvedLiftHill,
        has4RotationFrames,
    };
    using CarSpriteFlags = FlagHolder<uint32_t, CarSpriteFlag>;

    static void ReadLegacySpriteGroups(CarEntry& car, CarSpriteFlags carSpriteFlags);

    void RideObject::ReadLegacy(IReadObjectContext* context, IStream* stream)
    {
        _shouldLoadImages = context->ShouldLoadImages();

        stream->Seek(8, STREAM_SEEK_CURRENT);
        _legacyType.flags = stream->ReadValue<uint32_t>();
        for (auto& rideType : _legacyType.ride_type)
        {
            rideType = stream->ReadValue<uint8_t>();
            if (!RideTypeIsValid(rideType))
                rideType = kRideTypeNull;
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
            ReadLegacyCar(context, stream, carEntry);
        }
        stream->Seek(4, STREAM_SEEK_CURRENT);
        _legacyType.excitement_multiplier = stream->ReadValue<int8_t>();
        _legacyType.intensity_multiplier = stream->ReadValue<int8_t>();
        _legacyType.nausea_multiplier = stream->ReadValue<int8_t>();
        _legacyType.maxHeight = stream->ReadValue<uint8_t>();
        // Skipping a uint64_t for the enabled track pieces and two uint8_ts for the categories.
        stream->Seek(10, STREAM_SEEK_CURRENT);
        _legacyType.shop_item[0] = static_cast<ShopItem>(stream->ReadValue<uint8_t>());
        _legacyType.shop_item[1] = static_cast<ShopItem>(stream->ReadValue<uint8_t>());

        GetStringTable().Read(context, stream, ObjectStringID::name);
        GetStringTable().Read(context, stream, ObjectStringID::description);
        GetStringTable().Read(context, stream, ObjectStringID::capacity);

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

        if (isRideTypeShopOrFacility(_legacyType.ride_type[0]))
        {
            // This used to be hard-coded. JSON objects set this themselves.
            _presetColours.count = 1;
            _presetColours.list[0] = { Drawing::Colour::brightRed, Drawing::Colour::brightRed, Drawing::Colour::brightRed };

            if (_legacyType.ride_type[0] == RIDE_TYPE_FOOD_STALL || _legacyType.ride_type[0] == RIDE_TYPE_DRINK_STALL)
            {
                // In RCT2, no food or drink stall could be recoloured.
                _legacyType.flags |= RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB;
            }
        }

        // Read peep loading positions
        for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxCarTypesPerRideEntry; i++)
        {
            _peepLoadingWaypoints[i].clear();
            _peepLoadingPositions[i].clear();

            uint16_t numPeepLoadingPositions = stream->ReadValue<uint8_t>();
            if (numPeepLoadingPositions == 255)
            {
                numPeepLoadingPositions = stream->ReadValue<uint16_t>();
            }

            if (_legacyType.Cars[i].flags.has(CarEntryFlag::loadingWaypoints))
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
            context->LogError(ObjectError::invalidProperty, "Excitement multiplier too high.");
        }
        if (_legacyType.intensity_multiplier > 75)
        {
            context->LogError(ObjectError::invalidProperty, "Intensity multiplier too high.");
        }
        if (_legacyType.nausea_multiplier > 75)
        {
            context->LogError(ObjectError::invalidProperty, "Nausea multiplier too high.");
        }
        RideObjectUpdateRideType(_legacyType);
        _legacyType.Clearance = GetDefaultClearance();
    }

    void RideObject::Load()
    {
        GetStringTable().Sort();
        _legacyType.naming.Name = LanguageAllocateObjectString(GetName());
        _legacyType.naming.Description = LanguageAllocateObjectString(GetDescription());
        _legacyType.capacity = LanguageAllocateObjectString(GetCapacity());
        _legacyType.images_offset = LoadImages();
        _legacyType.vehicle_preset_list = &_presetColours;

        int32_t currentCarImagesOffset = _legacyType.images_offset + RCT2::ObjectLimits::kMaxRideTypesPerRideEntry;
        for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxCarTypesPerRideEntry; i++)
        {
            CarEntry& carEntry = _legacyType.Cars[i];
            if (carEntry.GroupEnabled(SpriteGroupType::SlopeFlat))
            {
                // RCT2 calculates num_vertical_frames and num_horizontal_frames and overwrites these properties on the car
                // entry. Immediately afterwards, the two were multiplied in order to calculate base_num_frames and were never
                // used again. This has been changed to use the calculation results directly - num_vertical_frames and
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

                // Move the offset over this car's images. Including peeps
                currentCarImagesOffset = imageIndex + carEntry.no_seating_rows * carEntry.NumCarImages;
                // 0x6DEB0D

                if (!carEntry.flags.has(CarEntryFlag::recalculateSpriteBounds))
                {
                    int32_t num_images = currentCarImagesOffset - baseImageId;
                    if (carEntry.flags.has(CarEntryFlag::spriteBoundsIncludeInvertedSet))
                    {
                        num_images *= 2;
                    }

                    if (_shouldLoadImages)
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
        UnloadImages();

        _legacyType.naming.Name = 0;
        _legacyType.naming.Description = 0;
        _legacyType.capacity = 0;
        _legacyType.images_offset = 0;
    }

    void RideObject::DrawPreview(
        Drawing::RenderTarget& rt, [[maybe_unused]] int32_t width, [[maybe_unused]] int32_t height) const
    {
        uint32_t imageId = _legacyType.images_offset;

        for (auto rideType : _legacyType.ride_type)
        {
            if (rideType != kRideTypeNull)
                break;

            imageId++;
        }

        GfxDrawSprite(rt, ImageId(imageId), { 0, 0 });
    }

    std::string RideObject::GetDescription() const
    {
        return GetString(ObjectStringID::description);
    }

    std::string RideObject::GetCapacity() const
    {
        return GetString(ObjectStringID::capacity);
    }

    ImageIndex RideObject::GetPreviewImage(ride_type_t type)
    {
        auto it = std::find(std::begin(_legacyType.ride_type), std::end(_legacyType.ride_type), type);
        if (it == std::end(_legacyType.ride_type))
        {
            return kImageIndexUndefined;
        }

        return _legacyType.images_offset + std::distance(std::begin(_legacyType.ride_type), it);
    }

    void RideObject::SetRepositoryItem(ObjectRepositoryItem* item) const
    {
        for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
        {
            item->RideInfo.RideType[i] = _legacyType.ride_type[i];
        }

        item->RideInfo.RideFlags = 0;
    }

    void RideObject::ReadLegacyCar([[maybe_unused]] IReadObjectContext* context, IStream* stream, CarEntry& car)
    {
        car.TabRotationMask = stream->ReadValue<uint16_t>();
        stream->Seek(2 * 1, STREAM_SEEK_CURRENT);
        car.spacing = stream->ReadValue<uint32_t>();
        car.car_mass = stream->ReadValue<uint16_t>();
        car.tab_height = stream->ReadValue<int8_t>();
        car.num_seats = stream->ReadValue<uint8_t>();
        CarSpriteFlags carSpriteFlags;
        carSpriteFlags.holder = stream->ReadValue<uint16_t>();
        car.spriteWidth = stream->ReadValue<uint8_t>();
        car.spriteHeightNegative = stream->ReadValue<uint8_t>();
        car.spriteHeightPositive = stream->ReadValue<uint8_t>();
        auto legacyAnimation = stream->ReadValue<uint8_t>();
        car.flags.holder = stream->ReadValue<uint32_t>();
        // Implied in vanilla, but can be turned off in OpenRCT2.
        car.flags.set(CarEntryFlag::enableBodyColour);
        car.base_num_frames = stream->ReadValue<uint16_t>();
        stream->Seek(15 * 4, STREAM_SEEK_CURRENT);
        car.no_seating_rows = stream->ReadValue<uint8_t>();
        car.spinning_inertia = stream->ReadValue<uint8_t>();
        car.spinning_friction = stream->ReadValue<uint8_t>();
        car.friction_sound_id = stream->ReadValue<Audio::SoundId>();
        car.ReversedCarIndex = stream->ReadValue<uint8_t>();
        car.soundRange = stream->ReadValue<SoundRange>();
        car.double_sound_frequency = stream->ReadValue<uint8_t>();
        car.powered_acceleration = stream->ReadValue<uint8_t>();
        car.powered_max_speed = stream->ReadValue<uint8_t>();
        car.PaintStyle = stream->ReadValue<uint8_t>();
        car.effect_visual = stream->ReadValue<uint8_t>();
        car.draw_order = stream->ReadValue<uint8_t>();
        car.num_vertical_frames_override = stream->ReadValue<uint8_t>();
        stream->Seek(4, STREAM_SEEK_CURRENT);

        // OpenRCT2-specific features below
        auto animationProperties = GetDefaultAnimationParameters(legacyAnimation);
        car.animation = animationProperties.Alias;
        car.AnimationSpeed = animationProperties.Speed;
        car.AnimationFrames = animationProperties.NumFrames;
        car.SteamEffect.Longitudinal = DefaultSteamSpawnPosition[0];
        car.SteamEffect.Vertical = DefaultSteamSpawnPosition[1];
        if (car.flags.has(CarEntryFlag::hasSpinning))
        {
            car.spinningNumFrames = 8;
        }
        if (car.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
        {
            car.spinningNumFrames = 32;
        }
        ReadLegacySpriteGroups(car, carSpriteFlags);
    }

    uint8_t RideObject::CalculateNumVerticalFrames(const CarEntry& carEntry)
    {
        // 0x6DE90B
        uint8_t numVerticalFrames;
        if (carEntry.flags.has(CarEntryFlag::overrideNumberOfVerticalFrames))
        {
            numVerticalFrames = carEntry.num_vertical_frames_override;
        }
        else
        {
            if (!carEntry.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
            {
                if (carEntry.flags.has(CarEntryFlag::hasVehicleAnimation)
                    && carEntry.animation != CarEntryAnimation::ObservationTower)
                {
                    if (!carEntry.flags.has(CarEntryFlag::hasDodgemInUseLights))
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
                numVerticalFrames = carEntry.spinningNumFrames;
            }
        }

        return numVerticalFrames;
    }

    uint8_t RideObject::CalculateNumHorizontalFrames(const CarEntry& carEntry)
    {
        uint8_t numHorizontalFrames;
        if (carEntry.flags.has(CarEntryFlag::hasSwinging))
        {
            if (!carEntry.flags.hasAny(CarEntryFlag::useSuspendedSwing, CarEntryFlag::useSlideSwing))
            {
                if (carEntry.flags.has(CarEntryFlag::useWoodenWildMouseSwing))
                {
                    numHorizontalFrames = 3;
                }
                else
                {
                    numHorizontalFrames = 5;
                }
            }
            else if (!carEntry.flags.hasAll(CarEntryFlag::useSuspendedSwing, CarEntryFlag::useSlideSwing))
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
        _shouldLoadImages = context->ShouldLoadImages();
        Guard::Assert(root.is_object(), "RideObject::ReadJson expects parameter root to be object");

        json_t properties = root["properties"];

        if (properties.is_object())
        {
            // This will convert a string to an array
            json_t rideTypes = Json::AsArray(properties["type"]);
            size_t numRideTypes = rideTypes.size();

            for (size_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
            {
                auto rideType = kRideTypeNull;

                if (i < numRideTypes)
                {
                    rideType = ParseRideType(Json::GetString(rideTypes[i]));

                    if (rideType == kRideTypeNull)
                    {
                        context->LogError(ObjectError::invalidProperty, "Unknown ride type");
                    }
                }

                _legacyType.ride_type[i] = rideType;
            }

            _legacyType.maxHeight = Json::GetNumber<uint8_t>(properties["maxHeight"]);
            _legacyType.Clearance = Json::GetNumber<uint8_t>(properties["clearance"], GetDefaultClearance());

            // This needs to be set for both shops/facilities _and_ regular rides.
            for (auto& item : _legacyType.shop_item)
            {
                item = ShopItem::none;
            }

            auto carColours = Json::AsArray(properties["carColours"]);
            _presetColours = ReadJsonCarColours(carColours);

            if (isRideTypeShopOrFacility(_legacyType.ride_type[0]))
            {
                // Standard car info for a shop
                auto& car = _legacyType.Cars[0];
                car.spacing = 544;
                car.SpriteGroups[EnumValue(SpriteGroupType::SlopeFlat)].spritePrecision = SpritePrecision::Sprites4;
                car.spriteWidth = 1;
                car.spriteHeightNegative = 1;
                car.spriteHeightPositive = 1;
                car.flags = { CarEntryFlag::hasSpinning };
                car.PaintStyle = VEHICLE_VISUAL_FLAT_RIDE_OR_CAR_RIDE;
                car.friction_sound_id = Audio::SoundId::null;
                car.soundRange = SoundRange::none;
                car.draw_order = 6;

                // Shop item
                auto rideSells = Json::AsArray(properties["sells"]);
                auto numShopItems = std::min(
                    static_cast<size_t>(RCT2::ObjectLimits::kMaxShopItemsPerRideEntry), rideSells.size());
                for (size_t i = 0; i < numShopItems; i++)
                {
                    auto shopItem = ParseShopItem(Json::GetString(rideSells[i]));
                    if (shopItem == ShopItem::none)
                    {
                        context->LogWarning(ObjectError::invalidProperty, "Unknown shop item");
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
                    _legacyType.flags |= RIDE_ENTRY_FLAG_INVERTER_SHIP_SWING_MODE;
                }
                else if (swingMode == 2)
                {
                    _legacyType.flags |= RIDE_ENTRY_FLAG_INVERTER_SHIP_SWING_MODE;
                    _legacyType.flags |= RIDE_ENTRY_FLAG_MAGIC_CARPET_SWING_MODE;
                }

                auto rotationMode = Json::GetNumber<int32_t>(properties["rotationMode"]);
                if (rotationMode == 1)
                {
                    _legacyType.flags |= RIDE_ENTRY_FLAG_TWIST_ROTATION_TYPE;
                }
                else if (rotationMode == 2)
                {
                    _legacyType.flags |= RIDE_ENTRY_FLAG_ENTERPRISE_ROTATION_TYPE;
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

        PopulateTablesFromJson(context, root);
    }

    void RideObject::ReadJsonVehicleInfo([[maybe_unused]] IReadObjectContext* context, json_t& properties)
    {
        Guard::Assert(properties.is_object(), "RideObject::ReadJsonVehicleInfo expects parameter properties to be object");

        _legacyType.min_cars_in_train = Json::GetNumber<uint8_t>(properties["minCarsPerTrain"], 1);
        _legacyType.max_cars_in_train = Json::GetNumber<uint8_t>(properties["maxCarsPerTrain"], 1);
        _legacyType.cars_per_flat_ride = Json::GetNumber<uint8_t>(properties["carsPerFlatRide"], kNoFlatRideCars);
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
            car.num_seats |= kVehicleSeatPairFlag;
        }

        car.spriteWidth = Json::GetNumber<uint8_t>(jCar["spriteWidth"]);
        car.spriteHeightNegative = Json::GetNumber<uint8_t>(jCar["spriteHeightNegative"]);
        car.spriteHeightPositive = Json::GetNumber<uint8_t>(jCar["spriteHeightPositive"]);
        car.base_num_frames = Json::GetNumber<uint16_t>(jCar["baseNumFrames"]);
        car.NumCarImages = Json::GetNumber<uint32_t>(jCar["numImages"]);
        car.no_seating_rows = Json::GetNumber<uint8_t>(jCar["numSeatRows"]);
        car.spinning_inertia = Json::GetNumber<uint8_t>(jCar["spinningInertia"]);
        car.spinning_friction = Json::GetNumber<uint8_t>(jCar["spinningFriction"]);
        car.friction_sound_id = Json::GetEnum<Audio::SoundId>(jCar["frictionSoundId"], Audio::SoundId::null);
        car.ReversedCarIndex = Json::GetNumber<uint8_t>(jCar["logFlumeReverserVehicleType"]);
        car.soundRange = Json::GetEnum<SoundRange>(jCar["soundRange"], SoundRange::none);
        car.double_sound_frequency = Json::GetNumber<uint8_t>(jCar["doubleSoundFrequency"]);
        car.powered_acceleration = Json::GetNumber<uint8_t>(jCar["poweredAcceleration"]);
        car.powered_max_speed = Json::GetNumber<uint8_t>(jCar["poweredMaxSpeed"]);
        car.PaintStyle = Json::GetNumber<uint8_t>(jCar["carVisual"]);
        car.effect_visual = Json::GetNumber<uint8_t>(jCar["effectVisual"], 1);
        car.draw_order = Json::GetNumber<uint8_t>(jCar["drawOrder"]);
        car.num_vertical_frames_override = Json::GetNumber<uint8_t>(jCar["numVerticalFramesOverride"]);

        auto jAnimation = jCar["animation"];
        if (jAnimation.is_object())
        {
            car.animation = GetAnimationTypeFromString(Json::GetString(jAnimation["animationType"]));
            car.AnimationSpeed = Json::GetNumber<uint16_t>(jAnimation["animationSpeed"]);
            car.AnimationFrames = Json::GetNumber<uint16_t>(jAnimation["animationFrames"]);
        }
        else
        {
            auto animationProperties = GetDefaultAnimationParameters(Json::GetNumber<uint8_t>(jAnimation));
            car.animation = animationProperties.Alias;
            car.AnimationSpeed = animationProperties.Speed;
            car.AnimationFrames = animationProperties.NumFrames;

            if (!jCar["animationSpeed"].is_null())
                car.AnimationSpeed = Json::GetNumber<uint16_t>(jCar["animationSpeed"]);
            if (!jCar["animationFrames"].is_null())
                car.AnimationFrames = Json::GetNumber<uint16_t>(jCar["animationFrames"]);
        }

        auto jSteamTranslation = jCar["steamPosition"];
        if (jSteamTranslation.is_object())
        {
            car.SteamEffect.Longitudinal = Json::GetNumber<int8_t>(
                jSteamTranslation["longitudinal"], DefaultSteamSpawnPosition[0]);
            car.SteamEffect.Vertical = Json::GetNumber<int8_t>(jSteamTranslation["vertical"], DefaultSteamSpawnPosition[1]);
        }
        else
        {
            car.SteamEffect.Longitudinal = DefaultSteamSpawnPosition[0];
            car.SteamEffect.Vertical = DefaultSteamSpawnPosition[1];
        }

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
                car.flags.set(CarEntryFlag::loadingWaypoints);
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
        car.spinningNumFrames = Json::GetNumber<uint8_t>(jCar["spinningNumFrames"]);

        car.flags |= Json::GetFlagHolder<CarEntryFlags, CarEntryFlag>(
            jCar,
            {
                { "isPoweredRideWithUnrestrictedGravity", CarEntryFlag::isPoweredRideWithUnrestrictedGravity },
                { "hasNoUpstopWheels", CarEntryFlag::hasNoUpstopWheels },
                { "hasNoUpstopWheelsBobsleigh", CarEntryFlag::hasNoUpstopWheelsBobsleigh },
                { "isMiniGolf", CarEntryFlag::isMiniGolf },
                { "isReverserBogie", CarEntryFlag::isReverserCoasterBogie },
                { "isReverserPassengerCar", CarEntryFlag::isReverserCoasterPassengerCar },
                { "hasInvertedSpriteSet", CarEntryFlag::hasInvertedSpriteSet },
                { "hasDodgemInUseLights", CarEntryFlag::hasDodgemInUseLights },
                { "hasAdditionalColour2", CarEntryFlag::enableTertiaryColour },
                { "recalculateSpriteBounds", CarEntryFlag::recalculateSpriteBounds },
                { "overrideNumberOfVerticalFrames", CarEntryFlag::overrideNumberOfVerticalFrames },
                { "spriteBoundsIncludeInvertedSet", CarEntryFlag::spriteBoundsIncludeInvertedSet },
                { "hasAdditionalSpinningFrames", CarEntryFlag::hasSpinningCombinedWithNonSpinning },
                { "isLift", CarEntryFlag::isLift },
                { "hasAdditionalColour1", CarEntryFlag::enableTrimColour },
                { "hasSwinging", CarEntryFlag::hasSwinging },
                { "hasSpinning", CarEntryFlag::hasSpinning },
                { "isPowered", CarEntryFlag::isPowered },
                { "hasScreamingRiders", CarEntryFlag::hasScreamingRiders },
                { "useSuspendedSwing", CarEntryFlag::useSuspendedSwing },
                { "useBoatHireCollisionDetection", CarEntryFlag::useBoatHireCollisionDetection },
                { "hasVehicleAnimation", CarEntryFlag::hasVehicleAnimation },
                { "hasRiderAnimation", CarEntryFlag::hasRiderAnimation },
                { "useWoodenWildMouseSwing", CarEntryFlag::useWoodenWildMouseSwing },
                { "useSlideSwing", CarEntryFlag::useSlideSwing },
                { "isChairlift", CarEntryFlag::isChairlift },
                { "isWaterRide", CarEntryFlag::isWaterRide },
                { "isGoKart", CarEntryFlag::isGoKart },
                { "useDodgemCarPlacement", CarEntryFlag::useDodgemCarPlacement },

                // Obsolete flag, only used on Boat Hire. Remaining usages have not yet been updated as of 2022-07-11.
                // TODO: Is this comment outdated?
                { "VEHICLE_ENTRY_FLAG_11", CarEntryFlag::use16RotationFrames },
            });
        if (Json::GetBoolean(jCar["hasBaseColour"], true))
            car.flags.set(CarEntryFlag::enableBodyColour);
        if (car.flags.has(CarEntryFlag::hasSpinning) && car.spinningNumFrames == 0)
        {
            car.spinningNumFrames = 8;
            if (car.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
            {
                car.spinningNumFrames = 32;
            }
        }

        // legacy sprite groups
        auto jFrames = jCar["frames"];
        if (jFrames.is_object())
        {
            auto carSpriteFlags = Json::GetFlagHolder<CarSpriteFlags, CarSpriteFlag>(
                jFrames,
                {
                    { "flat", CarSpriteFlag::flat },
                    { "gentleSlopes", CarSpriteFlag::gentleSlopes },
                    { "steepSlopes", CarSpriteFlag::steepSlopes },
                    { "verticalSlopes", CarSpriteFlag::verticalSlopes },
                    { "diagonalSlopes", CarSpriteFlag::diagonalSlopes },
                    { "flatBanked", CarSpriteFlag::flatBanked },
                    { "inlineTwists", CarSpriteFlag::inlineTwists },
                    { "flatToGentleSlopeBankedTransitions", CarSpriteFlag::flatToGentleSlopeBankedTransitions },
                    { "diagonalGentleSlopeBankedTransitions", CarSpriteFlag::diagonalGentleSlopeBankedTransitions },
                    { "gentleSlopeBankedTransitions", CarSpriteFlag::gentleSlopeBankedTransitions },
                    { "gentleSlopeBankedTurns", CarSpriteFlag::gentleSlopeBankedTurns },
                    { "flatToGentleSlopeWhileBankedTransitions", CarSpriteFlag::flatToGentleSlopeWhileBankedTransitions },
                    { "corkscrews", CarSpriteFlag::corkscrews },
                    { "restraintAnimation", CarSpriteFlag::restraintAnimation },
                    { "curvedLiftHill", CarSpriteFlag::curvedLiftHill },
                    { "VEHICLE_SPRITE_FLAG_15", CarSpriteFlag::has4RotationFrames },
                });
            ReadLegacySpriteGroups(car, carSpriteFlags);
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
                    if (!std::has_single_bit(numRotationFrames))
                    {
                        context->LogError(ObjectError::invalidProperty, "spriteGroups values must be powers of 2");
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
                carColour.Body = Drawing::colourFromString(Json::GetString(colours[0]));
                carColour.Trim = carColour.Body;
                carColour.Tertiary = carColour.Body;
                if (colours.size() >= 2)
                {
                    carColour.Trim = Drawing::colourFromString(Json::GetString(colours[1]));
                }
                if (colours.size() >= 3)
                {
                    carColour.Tertiary = Drawing::colourFromString(Json::GetString(colours[2]));
                }
            }
            config.push_back(carColour);
        }
        return config;
    }

    bool RideObject::isRideTypeShopOrFacility(ride_type_t rideType)
    {
        return GetRideTypeDescriptor(rideType).HasFlag(RtdFlag::isShopOrFacility);
    }

    ride_type_t RideObject::ParseRideType(const std::string& s)
    {
        auto result = std::find_if(
            std::begin(kRideTypeDescriptors), std::end(kRideTypeDescriptors), [s](const auto& rtd) { return rtd.Name == s; });
        if (result == std::end(kRideTypeDescriptors))
            return kRideTypeNull;
        else
            return std::distance(std::begin(kRideTypeDescriptors), result);
    }

    static const EnumMap<RideCategory> RideCategoryLookupTable{
        { "transport", RideCategory::transport },
        { "gentle", RideCategory::gentle },
        { "rollercoaster", RideCategory::rollerCoaster },
        { "thrill", RideCategory::thrill },
        { "water", RideCategory::water },
        { "stall", RideCategory::shop },
    };

    RideCategory RideObject::ParseRideCategory(const std::string& s)
    {
        auto result = RideCategoryLookupTable.find(s);
        return (result != RideCategoryLookupTable.end()) ? result->second : RideCategory::transport;
    }

    static const EnumMap<ShopItem> ShopItemLookupTable{
        { "burger", ShopItem::burger },
        { "chips", ShopItem::chips },
        { "ice_cream", ShopItem::iceCream },
        { "candyfloss", ShopItem::candyfloss },
        { "pizza", ShopItem::pizza },
        { "popcorn", ShopItem::popcorn },
        { "hot_dog", ShopItem::hotDog },
        { "tentacle", ShopItem::tentacle },
        { "toffee_apple", ShopItem::toffeeApple },
        { "doughnut", ShopItem::doughnut },
        { "chicken", ShopItem::chicken },
        { "pretzel", ShopItem::pretzel },
        { "funnel_cake", ShopItem::funnelCake },
        { "beef_noodles", ShopItem::beefNoodles },
        { "fried_rice_noodles", ShopItem::friedRiceNoodles },
        { "wonton_soup", ShopItem::wontonSoup },
        { "meatball_soup", ShopItem::meatballSoup },
        { "sub_sandwich", ShopItem::subSandwich },
        { "cookie", ShopItem::cookie },
        { "roast_sausage", ShopItem::roastSausage },
        { "drink", ShopItem::drink },
        { "coffee", ShopItem::coffee },
        { "lemonade", ShopItem::lemonade },
        { "chocolate", ShopItem::chocolate },
        { "iced_tea", ShopItem::icedTea },
        { "fruit_juice", ShopItem::fruitJuice },
        { "soybean_milk", ShopItem::soybeanMilk },
        { "sujeonggwa", ShopItem::sujeonggwa },
        { "balloon", ShopItem::balloon },
        { "toy", ShopItem::toy },
        { "map", ShopItem::map },
        { "photo", ShopItem::photo },
        { "umbrella", ShopItem::umbrella },
        { "voucher", ShopItem::voucher },
        { "hat", ShopItem::hat },
        { "tshirt", ShopItem::tShirt },
        { "sunglasses", ShopItem::sunglasses },
    };

    ShopItem RideObject::ParseShopItem(const std::string& s)
    {
        auto result = ShopItemLookupTable.find(s);
        return (result != ShopItemLookupTable.end()) ? result->second : ShopItem::none;
    }

    // Converts legacy sprite groups into OpenRCT2 sprite groups
    void ReadLegacySpriteGroups(CarEntry& car, CarSpriteFlags carSpriteFlags)
    {
        auto baseSpritePrecision = SpritePrecision::Sprites32;
        if (car.flags.has(CarEntryFlag::use16RotationFrames))
            baseSpritePrecision = SpritePrecision::Sprites16;
        if (carSpriteFlags.has(CarSpriteFlag::has4RotationFrames))
            baseSpritePrecision = SpritePrecision::Sprites4;

        if (carSpriteFlags.has(CarSpriteFlag::flat))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::SlopeFlat)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopes))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes12)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes25)].spritePrecision = baseSpritePrecision;
            if (car.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
                car.SpriteGroups[EnumValue(SpriteGroupType::Slopes25)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::steepSlopes))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes42)].spritePrecision = SpritePrecision::Sprites8;
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes60)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::verticalSlopes))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes75)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes90)].spritePrecision = baseSpritePrecision;
            car.SpriteGroups[EnumValue(SpriteGroupType::SlopesLoop)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::SlopeInverted)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::diagonalSlopes))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes8)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes16)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes50)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatBanked))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::FlatBanked22)].spritePrecision = SpritePrecision::Sprites8;
            car.SpriteGroups[EnumValue(SpriteGroupType::FlatBanked45)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::inlineTwists))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::FlatBanked67)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::FlatBanked90)].spritePrecision = SpritePrecision::Sprites4;
            car.SpriteGroups[EnumValue(SpriteGroupType::InlineTwists)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatToGentleSlopeBankedTransitions))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes12Banked22)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::diagonalGentleSlopeBankedTransitions))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes8Banked22)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopeBankedTransitions))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes25Banked22)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopeBankedTurns))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes25Banked45)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatToGentleSlopeWhileBankedTransitions))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Slopes12Banked45)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::corkscrews))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::Corkscrews)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::restraintAnimation))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::RestraintAnimation)].spritePrecision = SpritePrecision::Sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::curvedLiftHill))
        {
            car.SpriteGroups[EnumValue(SpriteGroupType::CurvedLiftHillUp)].spritePrecision = baseSpritePrecision;
        }
    }

    uint8_t RideObject::GetDefaultClearance() const
    {
        auto rideType = _legacyType.GetFirstNonNullRideType();
        const auto& rtd = GetRideTypeDescriptor(rideType);
        return rtd.Heights.ClearanceHeight;
    }
} // namespace OpenRCT2
