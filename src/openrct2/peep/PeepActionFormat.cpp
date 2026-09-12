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
    struct MappingStrings
    {
        StringId singular;
        StringId group;
    };

    constexpr auto kPeepActionToStringMapping = std::to_array<MappingStrings>({
        { STR_WALKING, STR_GUESTS_WALKING },                                   // walking
        { STR_SITTING, STR_GUESTS_SITTING },                                   // sitting
        { STR_LOOKING_AT_SCENERY, STR_GUESTS_LOOKING_AT_SCENERY },             // watchingScenery
        { STR_DROWNING, STR_GUESTS_DROWNING },                                 // drowning
        { STR_SELECT_LOCATION, STR_GUESTS_PICKED_UP },                         // pickedUp
        { STR_HEADING_FOR, STR_GUESTS_HEADING_FOR },                           // headingFor
        { STR_WATCHING_RIDE, STR_GUESTS_WATCHING_RIDE },                       // watchingRide
        { STR_WATCHING_CONSTRUCTION_OF, STR_GUESTS_WATCHING_CONSTRUCTION_OF }, // watchingRideConstruction
        { STR_WATCHING_NEW_RIDE_BEING_CONSTRUCTED,
          STR_GUESTS_WATCHING_NEW_RIDE_BEING_CONSTRUCTED },        // watchingRideConstructionUnspecific
        { STR_QUEUING_FOR, STR_GUESTS_QUEUING_FOR },               // queuingFor
        { STR_ON_RIDE, STR_GUESTS_ON_RIDE },                       // onRide
        { STR_IN_RIDE, STR_GUESTS_IN_RIDE },                       // inRide
        { STR_AT_RIDE, STR_GUESTS_AT_RIDE },                       // atShop
        { STR_LEAVING_PARK, STR_GUESTS_LEAVING_PARK },             // leavingPark
        { STR_SWEEPING_FOOTPATH, kStringIdEmpty },                 // sweepingFootpath
        { STR_EMPTYING_LITTER_BIN, kStringIdEmpty },               // emptyingBin
        { STR_WATERING_GARDENS, kStringIdEmpty },                  // wateringGardens
        { STR_MOWING_GRASS, kStringIdEmpty },                      // mowingGrass
        { STR_HEADING_TO_RIDE_FOR_INSPECTION, kStringIdEmpty },    // headingToInspectRide
        { STR_INSPECTING_RIDE, kStringIdEmpty },                   // inspectingRide
        { STR_FIXING_RIDE, kStringIdEmpty },                       // fixingRide
        { STR_ANSWERING_RADIO_CALL, kStringIdEmpty },              // answeringRadioCall
        { STR_RESPONDING_TO_RIDE_BREAKDOWN_CALL, kStringIdEmpty }, // respondingToBreakdownCall
    });
    static_assert(std::size(kPeepActionToStringMapping) == kNumPeepActionDescriptionTypes);

    void formatPeepActionTo(const Peep& peep, Formatter& ft, bool asGroup)
    {
        auto description = peep.getActionDescription();
        auto stringIds = kPeepActionToStringMapping[EnumValue(description.type)];
        ft.Add<StringId>(asGroup ? stringIds.group : stringIds.singular);

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
