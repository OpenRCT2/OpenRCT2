#pragma once

#include "../common.h"
#include "../core/List.hpp"

extern "C"
{
    #include "../rct1.h"
}

/**
 * Class to import RollerCoaster Tycoon 1 scenarios (*.SC4) and saved games (*.SV4).
 */
class S4Importer
{
public:
    void LoadSavedGame(const utf8 * path);
    void LoadScenario(const utf8 * path);
    void Import();

private:
    const utf8 * _s4Path;
    rct1_s4      _s4;
    uint8        _gameVersion;

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
    uint8 _wallTypeToEntryMap[96];
    uint8 _sceneryThemeTypeToEntryMap[24];

    // Research
    uint8 _researchRideEntryUsed[128];
    uint8 _researchRideTypeUsed[128];

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
    void AddEntryForLargeScenery(uint8 largeSceneryType);
    void AddEntryForWall(uint8 wallType);
    void AddEntriesForSceneryTheme(uint8 sceneryThemeType);

    void LoadObjects();
    void LoadObjects(uint8 objectType, List<const char *> entries);

    void ImportMapElements();
    void ImportRides();
    void ImportRide(rct_ride * dst, rct1_ride * src);
    void ImportRideMeasurements();
    void ImportRideMeasurement(rct_ride_measurement * dst, rct_ride_measurement * src);
    void ImportPeepSpawns();
    void ImportMapAnimations();
    void ImportFinance();
    void ImportResearch();
    void InsertResearchVehicle(const rct1_research_item * researchItem, bool researched);
    void ImportParkName();
    void ImportParkFlags();
    void ImportScenarioNameDetails();
    void ImportScenarioObjective();
    void ImportSavedView();

    void ClearExtraTileEntries();
    void FixColours();
    void FixZ();
    void FixPaths();
    void FixWalls();
    void ConvertWall(int * type, int * colourA, int * colourB, int * colourC);
    void FixBanners();
    void ImportBanner(rct_banner * dst, rct_banner * src);
    void FixTerrain();
    void FixEntrancePositions();
    void FixMapElementEntryTypes();

    List<const char *> * GetEntryList(uint8 objectType);
    const rct1_research_item * GetResearchList(size_t * count);
    int GetSCNumber();
    const char * GetUserString(rct_string_id stringId);
};
