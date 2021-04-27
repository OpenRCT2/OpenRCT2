/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"

#include <cstdint>

ObjectEntryIndex RCT2RideTypeToOpenRCT2RideType(uint8_t rct2RideType, const rct_ride_entry* rideEntry)
{
    switch (rct2RideType)
    {
        case RIDE_TYPE_CORKSCREW_ROLLER_COASTER:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_VERTICAL_LOOP)))
                return RIDE_TYPE_HYPERCOASTER;
            else
                return RIDE_TYPE_CORKSCREW_ROLLER_COASTER;
        case RIDE_TYPE_JUNIOR_ROLLER_COASTER:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RIDE_TYPE_CLASSIC_MINI_ROLLER_COASTER;
            else
                return RIDE_TYPE_JUNIOR_ROLLER_COASTER;
        case RIDE_TYPE_CAR_RIDE:
            if (rideEntry != nullptr && ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP))
                return RIDE_TYPE_MONSTER_TRUCKS;
            else
                return RIDE_TYPE_CAR_RIDE;
        case RIDE_TYPE_TWISTER_ROLLER_COASTER:
            if (rideEntry != nullptr && rideEntry->flags & RIDE_ENTRY_FLAG_NO_INVERSIONS)
                return RIDE_TYPE_HYPER_TWISTER;
            else
                return RIDE_TYPE_TWISTER_ROLLER_COASTER;
        case RIDE_TYPE_STEEL_WILD_MOUSE:
            if (rideEntry != nullptr && !(ride_entry_get_supported_track_pieces(rideEntry) & (1ULL << TRACK_SLOPE_STEEP)))
                return RIDE_TYPE_SPINNING_WILD_MOUSE;
            else
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

size_t GetRCT2StringBufferLen(const char* buffer, size_t maxBufferLen)
{
    constexpr char MULTIBYTE = static_cast<char>(255);
    size_t len = 0;
    for (size_t i = 0; i < maxBufferLen; i++)
    {
        auto ch = buffer[i];
        if (ch == MULTIBYTE)
        {
            i += 2;

            // Check if reading two more bytes exceeds max buffer len
            if (i < maxBufferLen)
            {
                len += 3;
            }
        }
        else if (ch == '\0')
        {
            break;
        }
        else
        {
            len++;
        }
    }
    return len;
}

uint8_t rct2_ride::GetMinCarsPerTrain() const
{
    return min_max_cars_per_train >> 4;
}

uint8_t rct2_ride::GetMaxCarsPerTrain() const
{
    return min_max_cars_per_train & 0xF;
}

void rct2_ride::SetMinCarsPerTrain(uint8_t newValue)
{
    min_max_cars_per_train &= ~0xF0;
    min_max_cars_per_train |= (newValue << 4);
}

void rct2_ride::SetMaxCarsPerTrain(uint8_t newValue)
{
    min_max_cars_per_train &= ~0x0F;
    min_max_cars_per_train |= newValue & 0x0F;
}

bool RCT2TrackTypeIsBooster(uint8_t rideType, uint16_t trackType)
{
    // Boosters share their ID with the Spinning Control track.
    return rideType != RIDE_TYPE_SPINNING_WILD_MOUSE && rideType != RIDE_TYPE_STEEL_WILD_MOUSE
        && trackType == TrackElemType::Booster;
}

track_type_t RCT2TrackTypeToOpenRCT2(RCT12TrackType origTrackType, uint8_t rideType)
{
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
        return RCT12FlatTrackTypeToOpenRCT2(origTrackType);
    if (origTrackType == TrackElemType::RotationControlToggleAlias && !RCT2TrackTypeIsBooster(rideType, origTrackType))
        return TrackElemType::RotationControlToggle;

    return origTrackType;
}

RCT12TrackType OpenRCT2TrackTypeToRCT2(track_type_t origTrackType)
{
    if (origTrackType == TrackElemType::RotationControlToggle)
        return TrackElemType::RotationControlToggleAlias;

    // This function is safe to run this way round.
    return OpenRCT2FlatTrackTypeToRCT12(origTrackType);
}

static FootpathMapping _footpathMappings[] = {
    // RCT2 mappings
    { "PATHASH ", "rct2.pathsurface.ash", "rct2.pathsurface.queue.yellow", "rct2.railings.bambooblack" },
    { "PATHCRZY", "rct2.pathsurface.crazy", "rct2.pathsurface.queue.yellow", "rct2.railings.concrete" },
    { "PATHDIRT", "rct2.pathsurface.dirt", "rct2.pathsurface.queue.yellow", "rct2.railings.bamboobrown" },
    { "PATHSPCE", "rct2.pathsurface.space", "rct2.pathsurface.queue.red", "rct2.railings.space" },
    { "ROAD    ", "rct2.pathsurface.road", "rct2.pathsurface.queue.blue", "rct2.railings.wood" },
    { "TARMACB ", "rct2.pathsurface.tarmac.brown", "rct2.pathsurface.queue.yellow", "rct2.railings.concrete" },
    { "TARMACG ", "rct2.pathsurface.tarmac.green", "rct2.pathsurface.queue.green", "rct2.railings.concretegreen" },
    { "TARMAC  ", "rct2.pathsurface.tarmac", "rct2.pathsurface.queue.blue", "rct2.railings.wood" },
    // Time Twister
    { "1920PATH", "rct2.tt.pathsurface.pavement", "rct2.tt.pathsurface.queue.pavement", "rct2.tt.pathrailings.pavement" },
    { "FUTRPATH", "rct2.tt.pathsurface.circuitboard", "rct2.tt.pathsurface.queue.circuitboard",
      "rct2.tt.pathrailings.circuitboard" },
    { "FUTRPAT2", "rct2.tt.pathsurface.circuitboard", "rct2.tt.pathsurface.queue.circuitboard", "openrct2.railings.invisible" },
    { "JURRPATH", "rct2.tt.pathsurface.rocky", "rct2.pathsurface.queue.yellow", "rct2.tt.pathrailings.rocky" },
    { "MEDIPATH", "rct2.tt.pathsurface.medieval", "rct2.pathsurface.queue.yellow", "rct2.tt.pathrailings.medieval" },
    { "MYTHPATH", "rct2.tt.pathsurface.mosaic", "rct2.pathsurface.queue.yellow", "rct2.tt.pathrailings.balustrade" },
    { "RANBPATH", "rct2.tt.pathsurface.rainbow", "rct2.tt.pathsurface.queue.rainbow", "rct2.tt.pathrailings.rainbow" },

    // RCT 1 mappings (for reverse lookup)
    { "PATHASH ", "rct1.aa.pathsurface.ash", "rct1.aa.pathsurface.queue.yellow", "rct2.railings.bambooblack" },
    { "PATHCRZY", "rct1.pathsurface.crazy", "rct1.aa.pathsurface.queue.yellow", "rct2.railings.concrete" },
    { "PATHDIRT", "rct1.pathsurface.dirt", "rct1.aa.pathsurface.queue.yellow", "rct2.railings.bamboobrown" },
    { "PATHSPCE", "rct1.aa.pathsurface.space", "rct1.pathsurface.queue.red", "rct1.ll.railings.space" },
    { "TARMACB ", "rct1.aa.pathsurface.tarmac.brown", "rct1.aa.pathsurface.queue.yellow", "rct2.railings.concrete" },
    { "TARMACG ", "rct1.aa.pathsurface.tarmac.green", "rct1.aa.pathsurface.queue.green", "rct2.railings.concretegreen" },
    { "TARMAC  ", "rct1.pathsurface.tarmac", "rct1.pathsurface.queue.blue", "rct2.railings.wood" },
    { "PATHCRZY", "rct1.pathsurface.tile.brown", "rct1.aa.pathsurface.queue.yellow", "rct2.railings.concrete" },
    { "PATHCRZY", "rct1.aa.pathsurface.tile.grey", "rct1.pathsurface.queue.blue", "rct2.railings.concrete" },
    { "PATHCRZY", "rct1.ll.pathsurface.tile.red", "rct1.pathsurface.queue.red", "rct2.railings.concrete" },
    { "PATHCRZY", "rct1.ll.pathsurface.tile.green", "rct1.aa.pathsurface.queue.green", "rct2.railings.concrete" },
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

std::optional<rct_object_entry> GetBestObjectEntryForSurface(std::string_view surface, std::string_view railings)
{
    rct_object_entry result;
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
            else if (!foundMapping)
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
