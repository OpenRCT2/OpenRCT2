/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../rct2/DATLimits.h"
#include "CarEntry.h"
#include "RideColour.h"
#include "RideTypes.h"
#include "ShopItem.h"
#include "VehicleColour.h"

#include <cstdint>

// Set to 255 on all tracked ride entries
static uint8_t constexpr NoFlatRideCars = 0xFF;
static ride_type_t constexpr RIDE_TYPE_NULL = 0xFF;

struct RideNaming
{
    StringId Name;
    StringId Description;
};

struct TrackColourPresetList
{
    uint8_t count;
    TrackColour list[256];
};

struct VehicleColourPresetList
{
    uint8_t count;
    VehicleColour list[256];
};

/**
 * Ride type structure.
 */
struct RideObjectEntry
{
    RideNaming naming;
    // The first three images are previews. They correspond to the ride_type[] array.
    uint32_t images_offset;
    uint32_t flags;
    ride_type_t ride_type[RCT2::ObjectLimits::MaxRideTypesPerRideEntry];
    uint8_t min_cars_in_train;
    uint8_t max_cars_in_train;
    uint8_t cars_per_flat_ride;
    // Number of cars that can't hold passengers
    uint8_t zero_cars;
    // The index to the vehicle type displayed in the vehicle tab.
    uint8_t TabCar;
    uint8_t DefaultCar;
    // Convert from first - fourth car to vehicle structure
    uint8_t FrontCar;
    uint8_t SecondCar;
    uint8_t RearCar;
    uint8_t ThirdCar;
    uint8_t BuildMenuPriority;
    CarEntry Cars[RCT2::ObjectLimits::MaxCarTypesPerRideEntry];
    VehicleColourPresetList* vehicle_preset_list;
    int8_t excitement_multiplier;
    int8_t intensity_multiplier;
    int8_t nausea_multiplier;
    uint8_t max_height;
    ShopItem shop_item[RCT2::ObjectLimits::MaxShopItemsPerRideEntry];
    StringId capacity;
    void* obj;

    const CarEntry* GetCar(size_t id) const
    {
        if (id < std::size(Cars))
        {
            return &Cars[id];
        }
        return nullptr;
    }

    const CarEntry* GetDefaultCar() const
    {
        return GetCar(DefaultCar);
    }

    ride_type_t GetFirstNonNullRideType() const
    {
        for (const auto& currentRideType : ride_type)
        {
            if (currentRideType != RIDE_TYPE_NULL)
                return currentRideType;
        }

        return RIDE_TYPE_NULL;
    }
};

RideNaming GetRideNaming(const ride_type_t rideType, const RideObjectEntry& rideEntry);
