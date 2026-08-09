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

#include "../audio/Audio.h"
#include "../core/EnumMap.hpp"
#include "../core/FlagHolder.hpp"
#include "../core/IStream.hpp"
#include "../core/Json.hpp"
#include "../core/Numerics.hpp"
#include "../drawing/Drawing.h"
#include "../entity/Yaw.hpp"
#include "../localisation/Language.h"
#include "../rct2/DATLimits.h"
#include "../rct2/RCT2.h"
#include "../ride/CarEntry.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../ride/Vehicle.h"
#include "ObjectRepository.h"

#include <iterator>

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
    static const uint8_t SpriteGroupMultiplier[EnumValue(SpriteGroupType::count)] = {
        1, 2, 2, 2, 2, 2, 2, 10, 1, 2, 2, 2, 2, 2, 2, 2, 6,  4, 4, 4,
        4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4, 20, 3, 1, 1,
    };
    static_assert(std::size(SpriteGroupMultiplier) == EnumValue(SpriteGroupType::count));

    constexpr uint8_t DefaultSteamSpawnPosition[] = { 11, 22 };

    static const EnumMap<CarEntryAnimation> AnimationNameLookup{
        { "none", CarEntryAnimation::none },
        { "simpleVehicle", CarEntryAnimation::simpleVehicle },
        { "steamLocomotive", CarEntryAnimation::steamLocomotive },
        { "swanBoat", CarEntryAnimation::swanBoat },
        { "monorailCycle", CarEntryAnimation::monorailCycle },
        { "MultiDimension", CarEntryAnimation::multiDimension },
        { "observationTower", CarEntryAnimation::observationTower },
        { "animalFlying", CarEntryAnimation::animalFlying },
    };

    constexpr auto NumLegacyAnimationTypes = 11;

    struct LegacyAnimationParameters
    {
        uint16_t Speed;
        uint8_t NumFrames;
        CarEntryAnimation Alias;
    };

    constexpr LegacyAnimationParameters VehicleEntryDefaultAnimation[] = {
        { 0, 1, CarEntryAnimation::none },                  // None
        { 1 << 12, 4, CarEntryAnimation::steamLocomotive }, // Miniature Railway Locomotive
        { 1 << 10, 2, CarEntryAnimation::swanBoat },        // Swan Boat
        { 1 << 11, 6, CarEntryAnimation::simpleVehicle },   // Canoe
        { 1 << 11, 7, CarEntryAnimation::simpleVehicle },   // Rowboat
        { 1 << 10, 2, CarEntryAnimation::simpleVehicle },   // Water Tricycle
        { 0x3333, 8, CarEntryAnimation::observationTower }, // Observation Tower
        { 1 << 10, 4, CarEntryAnimation::simpleVehicle },   // Mini Helicopter
        { 1 << 11, 4, CarEntryAnimation::monorailCycle },   // Monorail Cycle
        { 0x3333, 8, CarEntryAnimation::multiDimension },   // Multi Dimension Coaster
        { 24, 4, CarEntryAnimation::animalFlying },         // Animal Flying
    };
    static_assert(std::size(VehicleEntryDefaultAnimation) == NumLegacyAnimationTypes);

    static CarEntryAnimation GetAnimationTypeFromString(const std::string& s)
    {
        auto result = AnimationNameLookup.find(s);
        return (result != AnimationNameLookup.end()) ? result->second : CarEntryAnimation::none;
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
            return SpritePrecision::none;
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
        _legacyType.flags.holder = stream->ReadValue<uint32_t>();
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
                _legacyType.flags.set(RideEntryFlag::disableColourTab);
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
                _legacyType.Cars[i].guestLoadingWaypointSegments = stream->ReadValue<int8_t>() == 0 ? 0 : 4;
                if (_legacyType.ride_type[0] == RIDE_TYPE_ENTERPRISE)
                {
                    _legacyType.Cars[i].guestLoadingWaypointSegments = 8;
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
                _legacyType.Cars[i].guestLoadingWaypointSegments = 0;

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
            if (carEntry.groupEnabled(SpriteGroupType::slopeFlat))
            {
                // RCT2 calculates num_vertical_frames and num_horizontal_frames and overwrites these properties on the car
                // entry. Immediately afterwards, the two were multiplied in order to calculate base_num_frames and were never
                // used again. This has been changed to use the calculation results directly - num_vertical_frames and
                // num_horizontal_frames are no longer set on the car entry.
                // 0x6DE946
                carEntry.baseNumFrames = CalculateNumVerticalFrames(carEntry) * CalculateNumHorizontalFrames(carEntry);
                uint32_t baseImageId = currentCarImagesOffset;
                uint32_t imageIndex = baseImageId;
                carEntry.baseImageId = baseImageId;

                for (uint8_t spriteGroup = 0; spriteGroup < EnumValue(SpriteGroupType::count); spriteGroup++)
                {
                    if (carEntry.spriteGroups[spriteGroup].Enabled())
                    {
                        carEntry.spriteGroups[spriteGroup].imageId = imageIndex;
                        const auto spriteCount = carEntry.baseNumFrames
                            * carEntry.numRotationSprites(static_cast<SpriteGroupType>(spriteGroup))
                            * SpriteGroupMultiplier[spriteGroup];
                        imageIndex += spriteCount;
                    }
                }

                carEntry.numCarImages = imageIndex - currentCarImagesOffset;

                // Move the offset over this car's images. Including peeps
                currentCarImagesOffset = imageIndex + carEntry.numSeatingRows * carEntry.numCarImages;
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
                    carEntry.guestLoadingPositions = std::move(_peepLoadingPositions[i]);
                }

                if (!_peepLoadingWaypoints[i].empty())
                {
                    carEntry.guestLoadingWaypoints = std::move(_peepLoadingWaypoints[i]);
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
        car.tabRotationMask = stream->ReadValue<uint16_t>();
        stream->Seek(2 * 1, STREAM_SEEK_CURRENT);
        car.spacing = stream->ReadValue<uint32_t>();
        car.carMass = stream->ReadValue<uint16_t>();
        car.tabHeight = stream->ReadValue<int8_t>();
        car.numSeats = stream->ReadValue<uint8_t>();
        CarSpriteFlags carSpriteFlags;
        carSpriteFlags.holder = stream->ReadValue<uint16_t>();
        car.spriteWidth = stream->ReadValue<uint8_t>();
        car.spriteHeightNegative = stream->ReadValue<uint8_t>();
        car.spriteHeightPositive = stream->ReadValue<uint8_t>();
        auto legacyAnimation = stream->ReadValue<uint8_t>();
        car.flags.holder = stream->ReadValue<uint32_t>();
        // Implied in vanilla, but can be turned off in OpenRCT2.
        car.flags.set(CarEntryFlag::enableBodyColour);
        car.baseNumFrames = stream->ReadValue<uint16_t>();
        stream->Seek(15 * 4, STREAM_SEEK_CURRENT);
        car.numSeatingRows = stream->ReadValue<uint8_t>();
        car.spinningInertia = stream->ReadValue<uint8_t>();
        car.spinningFriction = stream->ReadValue<uint8_t>();
        car.frictionSoundId = stream->ReadValue<Audio::SoundId>();
        car.reversedCarIndex = stream->ReadValue<uint8_t>();
        car.soundRange = stream->ReadValue<SoundRange>();
        car.doubleSoundFrequency = stream->ReadValue<uint8_t>();
        car.poweredAcceleration = stream->ReadValue<uint8_t>();
        car.poweredMaxSpeed = stream->ReadValue<uint8_t>();
        car.paintStyle = stream->ReadValue<VehiclePaintStyle>();
        // Since this animation style may be replaced with a generic spinner system, make sure it gets limited to just our port.
        if (car.paintStyle == VehiclePaintStyle::spinningCars)
            car.paintStyle = VehiclePaintStyle::standard;
        car.effectVisual = stream->ReadValue<EffectVisual>();
        car.drawOrder = stream->ReadValue<uint8_t>();
        car.numVerticalFramesOverride = stream->ReadValue<uint8_t>();
        stream->Seek(4, STREAM_SEEK_CURRENT);

        // OpenRCT2-specific features below
        auto animationProperties = GetDefaultAnimationParameters(legacyAnimation);
        car.animation = animationProperties.Alias;
        car.animationSpeed = animationProperties.Speed;
        car.animationFrames = animationProperties.NumFrames;
        car.steamEffect.longitudinal = DefaultSteamSpawnPosition[0];
        car.steamEffect.vertical = DefaultSteamSpawnPosition[1];
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
            numVerticalFrames = carEntry.numVerticalFramesOverride;
        }
        else
        {
            if (!carEntry.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
            {
                if (carEntry.flags.has(CarEntryFlag::hasVehicleAnimation)
                    && carEntry.animation != CarEntryAnimation::observationTower)
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
                car.spriteGroups[EnumValue(SpriteGroupType::slopeFlat)].spritePrecision = SpritePrecision::sprites4;
                car.spriteWidth = 1;
                car.spriteHeightNegative = 1;
                car.spriteHeightPositive = 1;
                car.flags = { CarEntryFlag::hasSpinning };
                car.paintStyle = VehiclePaintStyle::flatRideOrCarRide;
                car.frictionSoundId = Audio::SoundId::null;
                car.soundRange = SoundRange::none;
                car.drawOrder = 6;

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
                    _legacyType.flags.set(RideEntryFlag::inverterShipSwingMode);
                }
                else if (swingMode == 2)
                {
                    _legacyType.flags.set(RideEntryFlag::inverterShipSwingMode);
                    _legacyType.flags.set(RideEntryFlag::magicCarpetSwingMode);
                }

                auto rotationMode = Json::GetNumber<int32_t>(properties["rotationMode"]);
                if (rotationMode == 1)
                {
                    _legacyType.flags.set(RideEntryFlag::hasTwistRotationType);
                }
                else if (rotationMode == 2)
                {
                    _legacyType.flags.set(RideEntryFlag::hasEnterpriseRotationType);
                }

                // ratingMultiplier was incorrectly called ratingMultipler pre-rename
                auto ratingMultiplier = properties["ratingMultiplier"].is_object() ? properties["ratingMultiplier"]
                                                                                   : properties["ratingMultipler"];

                if (ratingMultiplier.is_object())
                {
                    _legacyType.excitement_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["excitement"]);
                    _legacyType.intensity_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["intensity"]);
                    _legacyType.nausea_multiplier = Json::GetNumber<int8_t>(ratingMultiplier["nausea"]);
                }
            }

            _legacyType.BuildMenuPriority = Json::GetNumber<uint8_t>(properties["buildMenuPriority"]);
            _legacyType.flags |= Json::GetFlagHolder<RideEntryFlags, RideEntryFlag>(
                properties,
                {
                    { "noInversions", RideEntryFlag::noInversions },
                    { "noBanking", RideEntryFlag::noBankedTrack },
                    { "playDepartSound", RideEntryFlag::playDepartSound },
                    // Skipping "disallowWandering", no vehicle sets this flag.
                    { "playSplashSound", RideEntryFlag::playSplashSound },
                    { "playSplashSoundSlide", RideEntryFlag::coveredTrackIsWaterChannel },
                    { "hasShelter", RideEntryFlag::isACoveredRide },
                    { "limitAirTimeBonus", RideEntryFlag::limitAirTimeBonus },
                    { "disableBreakdown", RideEntryFlag::cannotBreakDown },
                    // Skipping noDoorsOverTrack, moved to ride groups.
                    { "noCollisionCrashes", RideEntryFlag::disableCollisionCrashes },
                    { "disablePainting", RideEntryFlag::disableColourTab },
                    { "riderControlsSpeed", RideEntryFlag::riderControlsSpeed },
                    { "hideEmptyTrains", RideEntryFlag::hideEmptyTrains },
                    { "noReverseOption", RideEntryFlag::noReverseOption },
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
            _legacyType.flags.set(RideEntryFlag::tabIconIsHalfScale);
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
        car.tabRotationMask = Json::GetNumber<uint16_t>(jCar["rotationFrameMask"]);
        car.spacing = Json::GetNumber<uint32_t>(jCar["spacing"]);
        car.carMass = Json::GetNumber<uint16_t>(jCar["mass"]);
        car.tabHeight = Json::GetNumber<int8_t>(jCar["tabOffset"]);
        car.numSeats = Json::GetNumber<uint8_t>(jCar["numSeats"]);
        if (Json::GetBoolean(jCar["seatsInPairs"], true) && car.numSeats > 1)
        {
            car.numSeats |= kVehicleSeatPairFlag;
        }

        car.spriteWidth = Json::GetNumber<uint8_t>(jCar["spriteWidth"]);
        car.spriteHeightNegative = Json::GetNumber<uint8_t>(jCar["spriteHeightNegative"]);
        car.spriteHeightPositive = Json::GetNumber<uint8_t>(jCar["spriteHeightPositive"]);
        car.baseNumFrames = Json::GetNumber<uint16_t>(jCar["baseNumFrames"]);
        car.numCarImages = Json::GetNumber<uint32_t>(jCar["numImages"]);
        car.numSeatingRows = Json::GetNumber<uint8_t>(jCar["numSeatRows"]);
        car.spinningInertia = Json::GetNumber<uint8_t>(jCar["spinningInertia"]);
        car.spinningFriction = Json::GetNumber<uint8_t>(jCar["spinningFriction"]);
        car.frictionSoundId = Json::GetEnum<Audio::SoundId>(jCar["frictionSoundId"], Audio::SoundId::null);
        car.reversedCarIndex = Json::GetNumber<uint8_t>(jCar["logFlumeReverserVehicleType"]);
        car.soundRange = Json::GetEnum<SoundRange>(jCar["soundRange"], SoundRange::none);
        car.doubleSoundFrequency = Json::GetNumber<uint8_t>(jCar["doubleSoundFrequency"]);
        car.poweredAcceleration = Json::GetNumber<uint8_t>(jCar["poweredAcceleration"]);
        car.poweredMaxSpeed = Json::GetNumber<uint8_t>(jCar["poweredMaxSpeed"]);
        car.paintStyle = Json::GetEnum<VehiclePaintStyle>(jCar["carVisual"], VehiclePaintStyle::standard);
        // Since this animation style may be replaced with a generic spinner system, make sure it gets limited to just our port.
        if (car.paintStyle == VehiclePaintStyle::spinningCars
            && (GetIdentifier() != "rct1.ride.spinning_cars"
                || _legacyType.ride_type[0] != RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER))
        {
            car.paintStyle = VehiclePaintStyle::standard;
        }
        car.effectVisual = Json::GetEnum<EffectVisual>(jCar["effectVisual"], EffectVisual::unknown1);
        car.drawOrder = Json::GetNumber<uint8_t>(jCar["drawOrder"]);
        car.numVerticalFramesOverride = Json::GetNumber<uint8_t>(jCar["numVerticalFramesOverride"]);

        auto jAnimation = jCar["animation"];
        if (jAnimation.is_object())
        {
            car.animation = GetAnimationTypeFromString(Json::GetString(jAnimation["animationType"]));
            car.animationSpeed = Json::GetNumber<uint16_t>(jAnimation["animationSpeed"]);
            car.animationFrames = Json::GetNumber<uint16_t>(jAnimation["animationFrames"]);
        }
        else
        {
            auto animationProperties = GetDefaultAnimationParameters(Json::GetNumber<uint8_t>(jAnimation));
            car.animation = animationProperties.Alias;
            car.animationSpeed = animationProperties.Speed;
            car.animationFrames = animationProperties.NumFrames;

            if (!jCar["animationSpeed"].is_null())
                car.animationSpeed = Json::GetNumber<uint16_t>(jCar["animationSpeed"]);
            if (!jCar["animationFrames"].is_null())
                car.animationFrames = Json::GetNumber<uint16_t>(jCar["animationFrames"]);
        }

        auto jSteamTranslation = jCar["steamPosition"];
        if (jSteamTranslation.is_object())
        {
            car.steamEffect.longitudinal = Json::GetNumber<int8_t>(
                jSteamTranslation["longitudinal"], DefaultSteamSpawnPosition[0]);
            car.steamEffect.vertical = Json::GetNumber<int8_t>(jSteamTranslation["vertical"], DefaultSteamSpawnPosition[1]);
        }
        else
        {
            car.steamEffect.longitudinal = DefaultSteamSpawnPosition[0];
            car.steamEffect.vertical = DefaultSteamSpawnPosition[1];
        }

        auto jLoadingPositions = jCar["loadingPositions"];
        if (jLoadingPositions.is_array())
        {
            for (auto& jPos : jLoadingPositions)
            {
                car.guestLoadingPositions.push_back(Json::GetNumber<int8_t>(jPos));
            }
        }
        else
        {
            auto jLoadingWaypoints = jCar["loadingWaypoints"];
            if (jLoadingWaypoints.is_array())
            {
                car.flags.set(CarEntryFlag::loadingWaypoints);
                car.guestLoadingWaypointSegments = Json::GetNumber<uint8_t>(jCar["numSegments"]);

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

                        car.guestLoadingWaypoints.push_back(std::move(entry));
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

                // Obsolete flag, used only by older custom content and now superseded by the 'spriteGroups' property
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
            for (uint8_t i = 0; i < EnumValue(SpriteGroupType::count); i++)
            {
                auto numRotationFrames = Json::GetNumber<uint8_t>(jRotationCount[kSpriteGroupNames[i]], 0);
                if (numRotationFrames != 0)
                {
                    if (!std::has_single_bit(numRotationFrames))
                    {
                        context->LogError(ObjectError::invalidProperty, "spriteGroups values must be powers of 2");
                        continue;
                    }
                    car.spriteGroups[i].spritePrecision = PrecisionFromNumFrames(numRotationFrames);
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
            if (!config.empty())
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
            if (!colours.empty())
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
        return GetRideTypeDescriptor(rideType).flags.has(RtdFlag::isShopOrFacility);
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
        auto baseSpritePrecision = SpritePrecision::sprites32;
        if (car.flags.has(CarEntryFlag::use16RotationFrames))
            baseSpritePrecision = SpritePrecision::sprites16;
        if (carSpriteFlags.has(CarSpriteFlag::has4RotationFrames))
            baseSpritePrecision = SpritePrecision::sprites4;

        if (carSpriteFlags.has(CarSpriteFlag::flat))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopeFlat)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopes))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes12)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::slopes25)].spritePrecision = baseSpritePrecision;
            if (car.flags.has(CarEntryFlag::hasSpinningCombinedWithNonSpinning))
                car.spriteGroups[EnumValue(SpriteGroupType::slopes25)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::steepSlopes))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes42)].spritePrecision = SpritePrecision::sprites8;
            car.spriteGroups[EnumValue(SpriteGroupType::slopes60)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::verticalSlopes))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes75)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::slopes90)].spritePrecision = baseSpritePrecision;
            car.spriteGroups[EnumValue(SpriteGroupType::slopesLoop)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::slopeInverted)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::diagonalSlopes))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes8)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::slopes16)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::slopes50)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatBanked))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::flatBanked22)].spritePrecision = SpritePrecision::sprites8;
            car.spriteGroups[EnumValue(SpriteGroupType::flatBanked45)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::inlineTwists))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::flatBanked67)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::flatBanked90)].spritePrecision = SpritePrecision::sprites4;
            car.spriteGroups[EnumValue(SpriteGroupType::inlineTwists)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatToGentleSlopeBankedTransitions))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes12Banked22)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::diagonalGentleSlopeBankedTransitions))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes8Banked22)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopeBankedTransitions))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes25Banked22)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::gentleSlopeBankedTurns))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes25Banked45)].spritePrecision = baseSpritePrecision;
        }
        if (carSpriteFlags.has(CarSpriteFlag::flatToGentleSlopeWhileBankedTransitions))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::slopes12Banked45)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::corkscrews))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::corkscrews)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::restraintAnimation))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::restraintAnimation)].spritePrecision = SpritePrecision::sprites4;
        }
        if (carSpriteFlags.has(CarSpriteFlag::curvedLiftHill))
        {
            car.spriteGroups[EnumValue(SpriteGroupType::curvedLiftHillUp)].spritePrecision = baseSpritePrecision;
        }
    }

    uint8_t RideObject::GetDefaultClearance() const
    {
        auto rideType = _legacyType.GetFirstNonNullRideType();
        const auto& rtd = GetRideTypeDescriptor(rideType);
        return rtd.Heights.ClearanceHeight;
    }
} // namespace OpenRCT2
