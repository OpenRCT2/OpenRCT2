/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkImporter.h"

#include "Context.h"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "management/NewsItem.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "world/Sprite.h"

#include <memory>

void IParkImporter::ValidateGuestEntityIndexes(Guest& guest)
{
    if (guest.GuestNextInQueue != SPRITE_INDEX_NULL)
    {
        auto* next = TryGetEntity<Guest>(guest.GuestNextInQueue);
        if (next == nullptr)
        {
            log_warning("Next in queue incorrect for guest: %d. Resetting to null", guest.sprite_index);
            guest.GuestNextInQueue = SPRITE_INDEX_NULL;
        }
    }
}

void IParkImporter::ValidateVehicleEntityIndexes(Vehicle& vehicle)
{
    for (auto& peep : vehicle.peep)
    {
        if (peep != SPRITE_INDEX_NULL)
        {
            auto* guest = TryGetEntity<Guest>(peep);
            if (guest == nullptr)
            {
                log_warning("Vehicle %d has incorrect peeps loaded. Resetting to null", vehicle.sprite_index);
                peep = SPRITE_INDEX_NULL;
            }
        }
    }

    if (vehicle.prev_vehicle_on_ride != SPRITE_INDEX_NULL)
    {
        auto* prevVeh = TryGetEntity<Vehicle>(vehicle.prev_vehicle_on_ride);
        if (prevVeh == nullptr)
        {
            log_warning("Vehicle %d has incorrect previous vehicle on ride. Resetting to null", vehicle.sprite_index);
            vehicle.prev_vehicle_on_ride = SPRITE_INDEX_NULL;
        }
    }
    if (vehicle.next_vehicle_on_ride != SPRITE_INDEX_NULL)
    {
        auto* nextVeh = TryGetEntity<Vehicle>(vehicle.next_vehicle_on_ride);
        if (nextVeh == nullptr)
        {
            log_warning("Vehicle %d has incorrect next vehicle on ride. Resetting to null", vehicle.sprite_index);
            vehicle.next_vehicle_on_ride = SPRITE_INDEX_NULL;
        }
    }
    if (vehicle.next_vehicle_on_train != SPRITE_INDEX_NULL)
    {
        auto* nextVeh = TryGetEntity<Vehicle>(vehicle.next_vehicle_on_train);
        if (nextVeh == nullptr)
        {
            log_warning("Vehicle %d has incorrect next vehicle on train. Resetting to null", vehicle.sprite_index);
            vehicle.next_vehicle_on_train = SPRITE_INDEX_NULL;
        }
    }
}

void IParkImporter::ValidateRideEntityIndexes(Ride& ride)
{
    for (auto& vehicle : ride.vehicles)
    {
        if (vehicle != SPRITE_INDEX_NULL)
        {
            auto* veh = TryGetEntity<Vehicle>(vehicle);
            if (veh == nullptr)
            {
                log_warning("Ride %d has incorrect vehicle on ride. Resetting to null", ride.id);
                vehicle = SPRITE_INDEX_NULL;
            }
        }
    }

    for (auto& station : ride.stations)
    {
        if (station.Start.isNull())
        {
            continue;
        }
        if (station.LastPeepInQueue != SPRITE_INDEX_NULL)
        {
            auto* peep = TryGetEntity<Guest>(station.LastPeepInQueue);
            if (peep == nullptr)
            {
                log_warning("Last peep in queue incorrect for ride: %d. Resetting to null", ride.id);
                station.LastPeepInQueue = SPRITE_INDEX_NULL;
            }
        }
    }

    if (ride.mechanic != SPRITE_INDEX_NULL)
    {
        auto* peep = TryGetEntity<Staff>(ride.mechanic);
        if (peep == nullptr)
        {
            log_warning("Mechanic incorrect for ride: %d. Resetting to null", ride.id);
            ride.mechanic = SPRITE_INDEX_NULL;
        }
    }
    if (ride.type == RIDE_TYPE_SPIRAL_SLIDE && ride.slide_peep != SPRITE_INDEX_NULL)
    {
        auto* peep = TryGetEntity<Guest>(ride.slide_peep);
        if (peep == nullptr)
        {
            log_warning("Ride slide peep incorrect for ride: %d. Resetting to null", ride.id);
            ride.slide_peep = SPRITE_INDEX_NULL;
        }
    }
    if (ride.race_winner != SPRITE_INDEX_NULL)
    {
        auto* peep = TryGetEntity<Guest>(ride.race_winner);
        if (peep == nullptr)
        {
            log_warning("Ride race winner incorrect for ride: %d. Resetting to null", ride.id);
            ride.race_winner = SPRITE_INDEX_NULL;
        }
    }
}

void IParkImporter::ValidateNewsEntityIndexes(News::Item& item)
{
    if (item.Type == News::ItemType::Peep || item.Type == News::ItemType::PeepOnRide)
    {
        if (item.Assoc != SPRITE_INDEX_NULL)
        {
            auto* peep = TryGetEntity<Peep>(item.Assoc);
            if (peep == nullptr)
            {
                log_warning("News item has incorrect peep index %d. Resetting to null", item.Assoc);
            }
            item.Assoc = SPRITE_INDEX_NULL;
        }
    }
}

void IParkImporter::ValidateEntityIndexes()
{
    for (auto guest : EntityList<Guest>(EntityListId::Peep))
    {
        ValidateGuestEntityIndexes(*guest);
    }

    for (size_t i = 0; i < MAX_SPRITES; i++)
    {
        auto vehicle = TryGetEntity<Vehicle>(i);
        if (vehicle != nullptr)
        {
            ValidateVehicleEntityIndexes(*vehicle);
        }
    }

    for (auto& ride : GetRideManager())
    {
        ValidateRideEntityIndexes(ride);
    }

    for (auto i = 0; i < News::MaxItems; i++)
    {
        ValidateNewsEntityIndexes(gNewsItems[i]);
    }
}

namespace ParkImporter
{
    std::unique_ptr<IParkImporter> Create(const std::string& hintPath)
    {
        std::unique_ptr<IParkImporter> parkImporter;
        std::string extension = Path::GetExtension(hintPath);
        if (ExtensionIsRCT1(extension))
        {
            parkImporter = CreateS4();
        }
        else
        {
            auto context = OpenRCT2::GetContext();
            parkImporter = CreateS6(context->GetObjectRepository());
        }
        return parkImporter;
    }

    bool ExtensionIsRCT1(const std::string& extension)
    {
        return String::Equals(extension, ".sc4", true) || String::Equals(extension, ".sv4", true);
    }

    bool ExtensionIsScenario(const std::string& extension)
    {
        return String::Equals(extension, ".sc4", true) || String::Equals(extension, ".sc6", true)
            || String::Equals(extension, ".sea", true);
    }
} // namespace ParkImporter
