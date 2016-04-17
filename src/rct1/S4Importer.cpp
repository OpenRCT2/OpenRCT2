#include <Windows.h>

#include "../common.h"
#include "../core/List.hpp"
#include "../core/Util.hpp"
#include "import.h"

extern "C"
{
    #include "../rct1.h"
    #include "../util/sawyercoding.h"
}

/**
 * Class to import RollerCoaster Tycoon 1 scenarios (*.SC4) and saved games (*.SV4).
 */
class S4Importer
{
public:

private:
    rct1_s4 _s4;
    uint8   _gameVersion;

    // Lists of dynamic object entries
    List<const char *> _rideEntries;
    List<const char *> _smallSceneryEntries;
    List<const char *> _largeSceneryEntries;
    List<const char *> _wallEntries;
    List<const char *> _sceneryGroupEntries;

    // Lookup tables for converting from RCT1 hard coded types to the new dynamic object entries
    uint8 _rideTypeToRideEntryMap[96];
    uint8 _vehicleTypeToRideEntryMap[96];
    uint8 _smallSceneryTypeToEntryMap[96];
    uint8 _largeSceneryTypeToEntryMap[96];
    uint8 _wallSceneryTypeToEntryMap[96];
    uint8 _sceneryThemeTypeToEntryMap[24];

    void Initialise();

    /**
     * Scans the map and research list for all the object types used and builds lists and
     * lookup tables for converting from hard coded RCT1 object types to dynamic object entries.
     */
    void CreateAvailableObjectMappings();
    void AddAvailableEntriesFromResearchList();
    void AddAvailableEntriesFromMap();
    void AddAvailableEntriesFromRides();

    void AddEntryForRideType(uint8 rideType);
    void AddEntryForVehicleType(uint8 rideType, uint8 vehicleType);
    void AddEntryForSmallScenery(uint8 smallSceneryType);
    void AddEntriesForSceneryTheme(uint8 sceneryThemeType);

    const rct1_research_item * GetResearchList(size_t * count);
};

void S4Importer::Initialise()
{
    _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;

    Memory::Set(_rideTypeToRideEntryMap,     255, sizeof(_rideTypeToRideEntryMap));
    Memory::Set(_vehicleTypeToRideEntryMap,  255, sizeof(_vehicleTypeToRideEntryMap));
    Memory::Set(_smallSceneryTypeToEntryMap, 255, sizeof(_smallSceneryTypeToEntryMap));
    Memory::Set(_largeSceneryTypeToEntryMap, 255, sizeof(_largeSceneryTypeToEntryMap));
    Memory::Set(_wallSceneryTypeToEntryMap,  255, sizeof(_wallSceneryTypeToEntryMap));
    Memory::Set(_sceneryThemeTypeToEntryMap, 255, sizeof(_sceneryThemeTypeToEntryMap));
}

void S4Importer::CreateAvailableObjectMappings()
{
    AddAvailableEntriesFromResearchList();
    AddAvailableEntriesFromMap();
    AddAvailableEntriesFromRides();
}

void S4Importer::AddAvailableEntriesFromResearchList()
{
    size_t researchListCount;
    const rct1_research_item * researchList = GetResearchList(&researchListCount);
    for (size_t i = 0; i < researchListCount; i++)
    {
        const rct1_research_item * researchItem = &researchList[i];
        if (researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE ||
            researchItem->item == RCT1_RESEARCH_END)
        {
            break;
        }
        if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE)
        {
            continue;
        }

        switch (researchItem->category) {
        case RCT1_RESEARCH_CATEGORY_THEME:
            AddEntriesForSceneryTheme(researchItem->item);
            break;
        case RCT1_RESEARCH_CATEGORY_RIDE:
        {
            uint8 rideType = researchItem->item;

            // Add all vehicles for this ride type
            uint32 numVehicles = 0;
            for (size_t j = 0; j < researchListCount; j++)
            {
                const rct1_research_item *researchItem2 = &researchList[j];
                if (researchItem2->item == RCT1_RESEARCH_END_RESEARCHABLE ||
                    researchItem2->item == RCT1_RESEARCH_END_AVAILABLE)
                {
                    break;
                }

                if (researchItem2->category == RCT1_RESEARCH_CATEGORY_VEHICLE &&
                    researchItem2->related_ride == rideType)
                {
                    AddEntryForVehicleType(rideType, researchItem2->item);
                    numVehicles++;
                }
            }

            // If no vehicles found so just add the default for this ride
            if (numVehicles == 0)
            {
                AddEntryForRideType(rideType);
            }
            break;
        }
        }
    }
}

void S4Importer::AddAvailableEntriesFromMap()
{
    size_t maxTiles = 128 * 128;
    size_t tileIndex = 0;
    rct_map_element * mapElement = _s4.map_elements;
    
    while (tileIndex < maxTiles)
    {
        switch (map_element_get_type(mapElement)) {
        case MAP_ELEMENT_TYPE_SCENERY:
            AddEntryForSmallScenery(mapElement->properties.scenery.type);
            break;
        }

        if (map_element_is_last_for_tile(mapElement++))
        {
            tileIndex++;
        }
    }
}

void S4Importer::AddAvailableEntriesFromRides()
{
    for (size_t i = 0; i < Util::CountOf(_s4.rides); i++)
    {
        rct1_ride * ride = &_s4.rides[i];

        // TODO might need to check if ride type has a vehicle type
        AddEntryForVehicleType(ride->type, ride->vehicle_type);
    }
}

void S4Importer::AddEntryForRideType(uint8 rideType)
{
    if (_rideTypeToRideEntryMap[rideType] == 255)
    {
        const char * entryName = RCT1::Tables::GetRideTypeObject(rideType);
        _rideEntries.Add(entryName);
        _rideTypeToRideEntryMap[rideType] = (uint8)_rideEntries.GetCount();
    }
}

void S4Importer::AddEntryForVehicleType(uint8 rideType, uint8 vehicleType)
{
    if (_vehicleTypeToRideEntryMap[rideType] == 255)
    {
        const char * entryName = RCT1::Tables::GetVehicleObject(vehicleType);

        uint8 rideEntryIndex = (uint8)_rideEntries.GetCount();
        _vehicleTypeToRideEntryMap[vehicleType] = rideEntryIndex;
        _rideEntries.Add(entryName);

        // Just overwrite this with the vehicle entry for now...
        _rideTypeToRideEntryMap[rideType] = rideEntryIndex;
    }
}

void S4Importer::AddEntryForSmallScenery(uint8 smallSceneryType)
{
    if (_smallSceneryTypeToEntryMap[smallSceneryType] == 255)
    {
        const char * entryName = RCT1::Tables::GetSmallSceneryObject(smallSceneryType);
        _smallSceneryEntries.Add(entryName);
        _smallSceneryTypeToEntryMap[smallSceneryType] = (uint8)_rideEntries.GetCount();
    }
}

void S4Importer::AddEntriesForSceneryTheme(uint8 sceneryThemeType)
{
    const char * entryName = RCT1::Tables::GetSceneryGroupObject(sceneryThemeType);

    _sceneryThemeTypeToEntryMap[sceneryThemeType] = (uint8)_sceneryGroupEntries.GetCount();
    _sceneryGroupEntries.Add(entryName);

    // TODO add entries for all scenery items belonging to scenery group
}

const rct1_research_item * S4Importer::GetResearchList(size_t * count)
{
    // Loopy Landscapes stores research items in a different place
    if (_gameVersion == FILE_VERSION_RCT1_LL)
    {
        *count = Util::CountOf(_s4.research_items_LL);
        return _s4.research_items_LL;
    }
    else
    {
        *count = Util::CountOf(_s4.research_items);
        return _s4.research_items;
    }
}

