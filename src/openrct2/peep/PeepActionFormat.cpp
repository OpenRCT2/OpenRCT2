/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepActionFormat.h"

#include "../entity/Peep.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"

namespace OpenRCT2
{
    constexpr auto kPeepActionToStringMapping = std::to_array<StringId>({
        STR_WALKING,                             // PeepActionDescriptionType::walking
        STR_SITTING,                             // PeepActionDescriptionType::sitting
        STR_LOOKING_AT_SCENERY,                  // PeepActionDescriptionType::watchingScenery
        STR_DROWNING,                            // PeepActionDescriptionType::drowning
        STR_SELECT_LOCATION,                     // PeepActionDescriptionType::pickedUp
        STR_HEADING_FOR,                         // PeepActionDescriptionType::headingFor
        STR_WATCHING_RIDE,                       // PeepActionDescriptionType::watchingRide
        STR_WATCHING_CONSTRUCTION_OF,            // PeepActionDescriptionType::watchingRideConstruction
        STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED, // PeepActionDescriptionType::watchingRideConstructionUnspecific
        STR_QUEUING_FOR,                         // PeepActionDescriptionType::queuingFor
        STR_ON_RIDE,                             // PeepActionDescriptionType::onRide
        STR_IN_RIDE,                             // PeepActionDescriptionType::inRide
        STR_AT_RIDE,                             // PeepActionDescriptionType::atShop
        STR_LEAVING_PARK,                        // PeepActionDescriptionType::leavingPark
        STR_SWEEPING_FOOTPATH,                   // PeepActionDescriptionType::sweepingFootpath
        STR_EMPTYING_LITTER_BIN,                 // PeepActionDescriptionType::emptyingBin
        STR_WATERING_GARDENS,                    // PeepActionDescriptionType::wateringGardens
        STR_MOWING_GRASS,                        // PeepActionDescriptionType::mowingGrass
        STR_HEADING_TO_RIDE_FOR_INSPECTION,      // PeepActionDescriptionType::headingToInspectRide
        STR_INSPECTING_RIDE,                     // PeepActionDescriptionType::inspectingRide
        STR_FIXING_RIDE,                         // PeepActionDescriptionType::fixingRide
        STR_ANSWERING_RADIO_CALL,                // PeepActionDescriptionType::answeringRadioCall
        STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL,   // PeepActionDescriptionType::respondingToBreakdownCall
    });
    static_assert(std::size(kPeepActionToStringMapping) == kNumPeepActionDescriptionTypes);

    void formatPeepActionTo(const Peep& peep, Formatter& ft)
    {
        auto description = peep.getActionDescription();
        auto stringId = kPeepActionToStringMapping[EnumValue(description.type)];
        ft.Add<StringId>(stringId);

        if (!description.rideId.IsNull())
        {
            const auto* ride = GetRide(description.rideId);
            if (ride != nullptr)
            {
                ride->formatNameTo(ft);
            }
        }
    }
} // namespace OpenRCT2
