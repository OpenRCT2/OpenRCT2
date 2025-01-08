/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT2.h"

#include "../Context.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"

#include <cstdint>

namespace OpenRCT2::RCT2
{
    ObjectEntryIndex RCT2RideTypeToOpenRCT2RideType(uint8_t rct2RideType, const RideObjectEntry& rideEntry)
    {
        switch (rct2RideType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
                if (!RideEntryGetSupportedTrackPieces(rideEntry).get(EnumValue(TrackGroup::verticalLoop)))
                    return RIDE_TYPE_HYPERCOASTER;
                return RIDE_TYPE_CORKSCREW_ROLLER_COASTER;
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
                if (RideEntryGetSupportedTrackPieces(rideEntry).get(EnumValue(TrackGroup::slopeSteepDown)))
                    return RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER;
                return RIDE_TYPE_JUNIOR_ROLLER_COASTER;
            case RIDE_TYPE_CAR_RIDE:
                if (RideEntryGetSupportedTrackPieces(rideEntry).get(EnumValue(TrackGroup::slopeSteepDown)))
                    return RIDE_TYPE_MONSTER_TRUCKS;
                return RIDE_TYPE_CAR_RIDE;
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
                if (rideEntry.flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
                    return RIDE_TYPE_HYPER_TWISTER;
                return RIDE_TYPE_TWISTER_ROLLER_COASTER;
            case RIDE_TYPE_STEEL_WILD_MOUSE:
                if (!RideEntryGetSupportedTrackPieces(rideEntry).get(EnumValue(TrackGroup::slopeSteepDown)))
                    return RIDE_TYPE_SPINNING_WILD_MOUSE;
                return RIDE_TYPE_STEEL_WILD_MOUSE;

            default:
                return rct2RideType;
        }
    }

    bool RCT2RideTypeNeedsConversion(uint8_t rct2RideType)
    {
        switch (rct2RideType)
        {
            case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
            case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
            case RIDE_TYPE_CAR_RIDE:
            case RIDE_TYPE_TWISTER_ROLLER_COASTER:
            case RIDE_TYPE_STEEL_WILD_MOUSE:
                return true;

            default:
                return false;
        }
    }

    uint8_t OpenRCT2RideTypeToRCT2RideType(ObjectEntryIndex openrct2Type)
    {
        switch (openrct2Type)
        {
            case RIDE_TYPE_HYPERCOASTER:
                return RIDE_TYPE_CORKSCREW_ROLLER_COASTER;
            case RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER:
                return RIDE_TYPE_JUNIOR_ROLLER_COASTER;
            case RIDE_TYPE_MONSTER_TRUCKS:
                return RIDE_TYPE_CAR_RIDE;
            case RIDE_TYPE_HYPER_TWISTER:
                return RIDE_TYPE_TWISTER_ROLLER_COASTER;
            case RIDE_TYPE_SPINNING_WILD_MOUSE:
                return RIDE_TYPE_STEEL_WILD_MOUSE;

            default:
                return openrct2Type;
        }
    }

    uint8_t Ride::GetMinCarsPerTrain() const
    {
        return MinMaxCarsPerTrain >> 4;
    }

    uint8_t Ride::GetMaxCarsPerTrain() const
    {
        return MinMaxCarsPerTrain & 0xF;
    }

    OpenRCT2::TrackElemType RCT2TrackTypeToOpenRCT2(
        OpenRCT2::RCT12::TrackElemType origTrackType, ride_type_t rideType, bool isFlatRide)
    {
        auto originalClass = OriginalRideClass::Regular;
        if (rideType == RIDE_TYPE_STEEL_WILD_MOUSE || rideType == RIDE_TYPE_SPINNING_WILD_MOUSE)
            originalClass = OriginalRideClass::WildMouse;
        if (isFlatRide)
            originalClass = OriginalRideClass::FlatRide;

        return RCT2TrackTypeToOpenRCT2(origTrackType, originalClass);
    }

    OpenRCT2::TrackElemType RCT2TrackTypeToOpenRCT2(
        OpenRCT2::RCT12::TrackElemType origTrackType, OriginalRideClass originalClass)
    {
        switch (originalClass)
        {
            case OriginalRideClass::FlatRide:
                return RCT12FlatTrackTypeToOpenRCT2(origTrackType);
            case OriginalRideClass::WildMouse:
                // Boosters share their ID with the Spinning Control track.
                if (origTrackType == OpenRCT2::RCT12::TrackElemType::RotationControlToggleAlias)
                    return OpenRCT2::TrackElemType::RotationControlToggle;
                return static_cast<OpenRCT2::TrackElemType>(origTrackType);
            case OriginalRideClass::Regular:
            default:
                return static_cast<OpenRCT2::TrackElemType>(origTrackType);
        }
    }

    OpenRCT2::RCT12::TrackElemType OpenRCT2TrackTypeToRCT2(OpenRCT2::TrackElemType origTrackType)
    {
        if (origTrackType == TrackElemType::RotationControlToggle)
            return OpenRCT2::RCT12::TrackElemType::RotationControlToggleAlias;

        // This function is safe to run this way round.
        return OpenRCT2FlatTrackTypeToRCT12(origTrackType);
    }

    static FootpathMapping _footpathMappings[] = {
        // RCT2 mappings
        { "PATHASH ", "rct2.footpath_surface.ash", "rct2.footpath_surface.queue_yellow",
          "rct2.footpath_railings.bamboo_black" },
        { "PATHCRZY", "rct2.footpath_surface.crazy_paving", "rct2.footpath_surface.queue_yellow",
          "rct2.footpath_railings.concrete" },
        { "PATHDIRT", "rct2.footpath_surface.dirt", "rct2.footpath_surface.queue_yellow",
          "rct2.footpath_railings.bamboo_brown" },
        { "PATHSPCE", "rct2.footpath_surface.tarmac_red", "rct2.footpath_surface.queue_red", "rct2.footpath_railings.space" },
        { "ROAD    ", "rct2.footpath_surface.road", "rct2.footpath_surface.queue_blue", "rct2.footpath_railings.wood" },
        { "TARMACB ", "rct2.footpath_surface.tarmac_brown", "rct2.footpath_surface.queue_yellow",
          "rct2.footpath_railings.concrete" },
        { "TARMACG ", "rct2.footpath_surface.tarmac_green", "rct2.footpath_surface.queue_green",
          "rct2.footpath_railings.concrete_green" },
        { "TARMAC  ", "rct2.footpath_surface.tarmac", "rct2.footpath_surface.queue_blue", "rct2.footpath_railings.wood" },
        // Time Twister
        { "1920PATH", "rct2tt.footpath_surface.pavement", "rct2tt.footpath_surface.queue_pavement",
          "rct2tt.footpath_railings.pavement" },
        { "FUTRPATH", "rct2tt.footpath_surface.circuitboard", "rct2tt.footpath_surface.queue_circuitboard",
          "rct2tt.footpath_railings.circuitboard" },
        { "FUTRPAT2", "rct2tt.footpath_surface.circuitboard", "rct2tt.footpath_surface.queue_circuitboard",
          "rct2tt.footpath_railings.circuitboard_invisible" },
        { "JURRPATH", "rct2tt.footpath_surface.rocky", "rct2.footpath_surface.queue_yellow", "rct2tt.footpath_railings.rocky" },
        { "MEDIPATH", "rct2tt.footpath_surface.medieval", "rct2.footpath_surface.queue_yellow",
          "rct2tt.footpath_railings.medieval" },
        { "MYTHPATH", "rct2tt.footpath_surface.mosaic", "rct2.footpath_surface.queue_yellow",
          "rct2tt.footpath_railings.balustrade" },
        { "RANBPATH", "rct2tt.footpath_surface.rainbow", "rct2tt.footpath_surface.queue_rainbow",
          "rct2tt.footpath_railings.rainbow" },

        // RCT 1 mappings (for reverse lookup)
        { "PATHASH ", "rct1aa.footpath_surface.ash", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.bamboo_black" },
        { "PATHCRZY", "rct1.footpath_surface.crazy_paving", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.concrete" },
        { "PATHDIRT", "rct1.footpath_surface.dirt", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.bamboo_brown" },
        { "PATHSPCE", "rct1aa.footpath_surface.tarmac_red", "rct1aa.footpath_surface.queue_red",
          "rct1ll.footpath_railings.space" },
        { "TARMACB ", "rct1aa.footpath_surface.tarmac_brown", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.concrete" },
        { "TARMACG ", "rct1aa.footpath_surface.tarmac_green", "rct1aa.footpath_surface.queue_green",
          "rct2.footpath_railings.concrete_green" },
        { "TARMAC  ", "rct1.footpath_surface.tarmac", "rct1.footpath_surface.queue_blue", "rct2.footpath_railings.wood" },
        { "PATHCRZY", "rct1.footpath_surface.tiles_brown", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.concrete" },
        { "PATHCRZY", "rct1aa.footpath_surface.tiles_grey", "rct1.footpath_surface.queue_blue",
          "rct2.footpath_railings.concrete" },
        { "PATHCRZY", "rct1ll.footpath_surface.tiles_red", "rct1aa.footpath_surface.queue_red",
          "rct2.footpath_railings.concrete" },
        { "PATHCRZY", "rct1ll.footpath_surface.tiles_green", "rct1aa.footpath_surface.queue_green",
          "rct2.footpath_railings.concrete" },
        { "ROAD    ", "rct1.footpath_surface.road", "rct1.footpath_surface.queue_blue", "rct2.footpath_railings.wood" },

        // Custom path mapping
        { "PATHINVS", "openrct2.footpath_surface.invisible", "openrct2.footpath_surface.queue_invisible",
          "openrct2.footpath_railings.invisible" },
        { "PATHCLR ", "openrct2.footpath_surface.invisible", "openrct2.footpath_surface.queue_invisible",
          "openrct2.footpath_railings.invisible" },
        { "PAT25MKY", "openrct2.footpath_surface.invisible", "openrct2.footpath_surface.queue_invisible",
          "openrct2.footpath_railings.invisible" },
        { "JAINVSFP", "openrct2.footpath_surface.invisible", "openrct2.footpath_surface.queue_invisible",
          "openrct2.footpath_railings.invisible" },

        // Some test objects were temporarily released to the public
        { "#RCT1PTS", "rct1aa.footpath_surface.tiles_grey", "rct1.footpath_surface.queue_blue", "rct2.footpath_railings.wood" },
        { "#RCT1PTG", "rct1ll.footpath_surface.tiles_green", "rct1aa.footpath_surface.queue_green",
          "rct2.footpath_railings.wood" },
        { "#RCT1PTR", "rct1ll.footpath_surface.tiles_red", "rct1aa.footpath_surface.queue_red", "rct2.footpath_railings.wood" },
        { "#RCT1PTB", "rct1.footpath_surface.tiles_brown", "rct1aa.footpath_surface.queue_yellow",
          "rct2.footpath_railings.wood" },
    };

    const FootpathMapping* GetFootpathSurfaceId(const ObjectEntryDescriptor& desc, bool ideallyLoaded, bool isQueue)
    {
        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();

        auto name = desc.Entry.GetName();
        for (const auto& mapping : _footpathMappings)
        {
            if (mapping.Original == name)
            {
                if (ideallyLoaded)
                {
                    auto obj = objManager.GetLoadedObject(
                        ObjectEntryDescriptor(isQueue ? mapping.QueueSurface : mapping.NormalSurface));
                    if (obj == nullptr)
                        continue;
                }
                return &mapping;
            }
        }
        return nullptr;
    }

    std::optional<RCTObjectEntry> GetBestObjectEntryForSurface(std::string_view surface, std::string_view railings)
    {
        RCTObjectEntry result;
        std::memset(&result, 0, sizeof(result));

        result.SetType(ObjectType::Paths);

        auto foundMapping = false;
        for (const auto& mapping : _footpathMappings)
        {
            if (surface == mapping.NormalSurface || surface == mapping.QueueSurface)
            {
                if (railings == mapping.Railing)
                {
                    // Best match found
                    foundMapping = true;
                    result.SetName(mapping.Original);
                    break;
                }

                if (!foundMapping)
                {
                    // Found a mapping, but keep searching to see if there is a closer match
                    foundMapping = true;
                    result.SetName(mapping.Original);
                }
            }
        }

        if (foundMapping)
            return result;
        return {};
    }

    colour_t TD6SceneryElement::getTertiaryWallColour() const
    {
        return (Flags & 0xFC) >> 2;
    }
} // namespace OpenRCT2::RCT2
