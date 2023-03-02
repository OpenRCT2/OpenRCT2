/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Editor.h"
#include "../Game.h"
#include "../GameState.h"
#include "../ParkImporter.h"
#include "../actions/WallPlaceAction.h"
#include "../audio/audio.h"
#include "../core/BitSet.hpp"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Fountain.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/PatrolArea.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../object/Object.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/EntryList.h"
#include "../ride/RideData.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrainManager.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TilePointerIndex.hpp"
#include "../world/Wall.h"
#include "RCT1.h"
#include "Tables.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

static constexpr const ObjectEntryIndex ObjectEntryIndexIgnore = 254;

using namespace OpenRCT2;

namespace RCT1
{
    class S4Importer final : public IParkImporter
    {
    private:
        std::string _s4Path;
        S4 _s4 = {};
        uint8_t _gameVersion = 0;
        uint8_t _parkValueConversionFactor = 0;
        bool _isScenario = false;

        // Lists of dynamic object entries
        RCT12::EntryList _rideEntries;
        RCT12::EntryList _smallSceneryEntries;
        RCT12::EntryList _largeSceneryEntries;
        RCT12::EntryList _wallEntries;
        RCT12::EntryList _pathEntries;
        RCT12::EntryList _pathAdditionEntries;
        RCT12::EntryList _sceneryGroupEntries;
        RCT12::EntryList _waterEntry;
        RCT12::EntryList _terrainSurfaceEntries;
        RCT12::EntryList _terrainEdgeEntries;
        RCT12::EntryList _footpathSurfaceEntries;
        RCT12::EntryList _footpathRailingsEntries;

        // Lookup tables for converting from RCT1 hard coded types to the new dynamic object entries
        ObjectEntryIndex _rideTypeToRideEntryMap[EnumValue(RideType::Count)]{};
        ObjectEntryIndex _vehicleTypeToRideEntryMap[RCT1_VEHICLE_TYPE_COUNT]{};
        ObjectEntryIndex _smallSceneryTypeToEntryMap[256]{};
        ObjectEntryIndex _largeSceneryTypeToEntryMap[256]{};
        ObjectEntryIndex _wallTypeToEntryMap[256]{};
        ObjectEntryIndex _pathTypeToEntryMap[24]{};
        ObjectEntryIndex _pathAdditionTypeToEntryMap[16]{};
        ObjectEntryIndex _sceneryThemeTypeToEntryMap[24]{};
        ObjectEntryIndex _terrainSurfaceTypeToEntryMap[16]{};
        ObjectEntryIndex _terrainEdgeTypeToEntryMap[16]{};
        ObjectEntryIndex _footpathSurfaceTypeToEntryMap[32]{};
        ObjectEntryIndex _footpathRailingsTypeToEntryMap[4]{};

        // Research
        BitSet<MAX_RIDE_OBJECTS> _researchRideEntryUsed{};
        BitSet<EnumValue(RideType::Count)> _researchRideTypeUsed{};

        // Scenario repository - used for determining scenario name
        IScenarioRepository* _scenarioRepository = GetScenarioRepository();

    public:
        ParkLoadResult Load(const u8string& path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::Equals(extension, ".sc4", true))
            {
                return LoadScenario(path);
            }
            if (String::Equals(extension, ".sv4", true))
            {
                return LoadSavedGame(path);
            }

            throw std::runtime_error("Invalid RCT1 park extension.");
        }

        ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, false, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, true, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadFromStream(
            IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck, const u8string& path) override
        {
            _s4 = *ReadAndDecodeS4(stream, isScenario);
            _s4Path = path;
            _isScenario = isScenario;
            _gameVersion = SawyerCodingDetectRCT1Version(_s4.GameVersion) & FILE_VERSION_MASK;

            // Only determine what objects we required to import this saved game
            InitialiseEntryMaps();
            CreateAvailableObjectMappings();
            return ParkLoadResult(GetRequiredObjects());
        }

        void Import() override
        {
            Initialise();

            CreateAvailableObjectMappings();

            ImportRides();
            ImportRideMeasurements();
            ImportSprites();
            ImportTileElements();
            ImportPeepSpawns();
            ImportFinance();
            ImportResearch();
            ImportParkName();
            ImportParkFlags();
            ImportClimate();
            ImportScenarioNameDetails();
            ImportScenarioObjective();
            ImportSavedView();
            FixLandOwnership();
            FixUrbanPark();
            CountBlockSections();
            SetDefaultNames();
            DetermineRideEntranceAndExitLocations();

            ResearchDetermineFirstOfType();

            CheatsReset();
            ClearRestrictedScenery();
            RestrictAllMiscScenery();
        }

        bool GetDetails(ScenarioIndexEntry* dst) override
        {
            *dst = {};

            SourceDescriptor desc;
            // If no entry is found, this is a custom scenario.
            bool isOfficial = ScenarioSources::TryGetById(_s4.ScenarioSlotIndex, &desc);

            dst->Category = desc.category;
            dst->SourceGame = ScenarioSource{ desc.source };
            dst->SourceIndex = desc.index;
            dst->ScenarioId = desc.id;

            dst->ObjectiveType = _s4.ScenarioObjectiveType;
            dst->ObjectiveArg1 = _s4.ScenarioObjectiveYears;
            // RCT1 used another way of calculating park value.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_PARK_VALUE_BY)
                dst->ObjectiveArg2 = CorrectRCT1ParkValue(_s4.ScenarioObjectiveCurrency);
            else
                dst->ObjectiveArg2 = _s4.ScenarioObjectiveCurrency;
            dst->ObjectiveArg3 = _s4.ScenarioObjectiveNumGuests;
            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST)
            {
                dst->ObjectiveArg3 = GetBuildTheBestRideId();
            }

            auto name = RCT2StringToUTF8(_s4.ScenarioName, RCT2LanguageId::EnglishUK);
            std::string details;

            // TryGetById won't set this property if the scenario is not recognised,
            // but localisation needs it.
            if (!isOfficial)
            {
                desc.title = name.c_str();
            }

            String::Set(dst->InternalName, sizeof(dst->InternalName), desc.title);

            StringId localisedStringIds[3];
            if (LanguageGetLocalisedScenarioStrings(desc.title, localisedStringIds))
            {
                if (localisedStringIds[0] != STR_NONE)
                {
                    name = String::ToStd(LanguageGetString(localisedStringIds[0]));
                }
                if (localisedStringIds[2] != STR_NONE)
                {
                    details = String::ToStd(LanguageGetString(localisedStringIds[2]));
                }
            }

            String::Set(dst->Name, sizeof(dst->Name), name.c_str());
            String::Set(dst->Details, sizeof(dst->Details), details.c_str());

            return true;
        }

        money64 CorrectRCT1ParkValue(money32 oldParkValue)
        {
            if (oldParkValue == MONEY32_UNDEFINED)
            {
                return MONEY64_UNDEFINED;
            }

            if (_parkValueConversionFactor == 0)
            {
                if (_s4.ParkValue != 0)
                {
                    // Use the ratio between the old and new park value to calcute the ratio to
                    // use for the park value history and the goal.
                    auto& park = GetContext()->GetGameState()->GetPark();
                    _parkValueConversionFactor = (park.CalculateParkValue() * 10) / _s4.ParkValue;
                }
                else
                {
                    // In new games, the park value isn't set.
                    _parkValueConversionFactor = 100;
                }
            }

            return (oldParkValue * _parkValueConversionFactor) / 10;
        }

    private:
        std::unique_ptr<S4> ReadAndDecodeS4(IStream* stream, bool isScenario)
        {
            auto s4 = std::make_unique<S4>();
            size_t dataSize = stream->GetLength() - stream->GetPosition();
            auto data = stream->ReadArray<uint8_t>(dataSize);
            auto decodedData = std::make_unique<uint8_t[]>(sizeof(S4));

            size_t decodedSize;
            int32_t fileType = SawyerCodingDetectFileType(data.get(), dataSize);
            if (isScenario && (fileType & FILE_VERSION_MASK) != FILE_VERSION_RCT1)
            {
                decodedSize = SawyerCodingDecodeSC4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }
            else
            {
                decodedSize = SawyerCodingDecodeSV4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }

            if (decodedSize == sizeof(S4))
            {
                std::memcpy(s4.get(), decodedData.get(), sizeof(S4));
                return s4;
            }

            throw std::runtime_error("Unable to decode park.");
        }

        void Initialise()
        {
            // Avoid reusing the value used for last import
            _parkValueConversionFactor = 0;

            uint16_t mapSize = _s4.MapSize == 0 ? Limits::MaxMapSize : _s4.MapSize;

            gScenarioFileName = GetRCT1ScenarioName();

            // Do map initialisation, same kind of stuff done when loading scenario editor
            auto context = OpenRCT2::GetContext();
            context->GetGameState()->InitAll({ mapSize, mapSize });
            gEditorStep = EditorStep::ObjectSelection;
            gParkFlags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
            gScenarioCategory = SCENARIO_CATEGORY_OTHER;
        }

        std::string GetRCT1ScenarioName()
        {
            const ScenarioIndexEntry* scenarioEntry = _scenarioRepository->GetByInternalName(_s4.ScenarioName);
            if (scenarioEntry == nullptr)
            {
                return "";
            }

            return Path::GetFileName(scenarioEntry->Path);
        }

        void InitialiseEntryMaps()
        {
            std::fill(std::begin(_rideTypeToRideEntryMap), std::end(_rideTypeToRideEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_vehicleTypeToRideEntryMap), std::end(_vehicleTypeToRideEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_smallSceneryTypeToEntryMap), std::end(_smallSceneryTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_largeSceneryTypeToEntryMap), std::end(_largeSceneryTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_wallTypeToEntryMap), std::end(_wallTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathTypeToEntryMap), std::end(_pathTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathAdditionTypeToEntryMap), std::end(_pathAdditionTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_sceneryThemeTypeToEntryMap), std::end(_sceneryThemeTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(
                std::begin(_terrainSurfaceTypeToEntryMap), std::end(_terrainSurfaceTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_terrainEdgeTypeToEntryMap), std::end(_terrainEdgeTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(
                std::begin(_footpathSurfaceTypeToEntryMap), std::end(_footpathSurfaceTypeToEntryMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(
                std::begin(_footpathRailingsTypeToEntryMap), std::end(_footpathRailingsTypeToEntryMap),
                OBJECT_ENTRY_INDEX_NULL);
        }

        /**
         * Scans the map and research list for all the object types used and builds lists and
         * lookup tables for converting from hard coded RCT1 object types to dynamic object entries.
         */
        void CreateAvailableObjectMappings()
        {
            AddDefaultEntries();
            AddAvailableEntriesFromResearchList();
            AddAvailableEntriesFromMap();
            AddAvailableEntriesFromRides();
            AddAvailableEntriesFromSceneryGroups();
            AddEntryForWater();
        }

        void AddDefaultEntries()
        {
            // Add default scenery groups
            _sceneryGroupEntries.AddRange({
                "rct2.scenery_group.scgtrees",
                "rct2.scenery_group.scgshrub",
                "rct2.scenery_group.scggardn",
                "rct2.scenery_group.scgfence",
                "rct2.scenery_group.scgwalls",
                "rct2.scenery_group.scgpathx",
            });

            // Add default footpaths
            _footpathSurfaceEntries.AddRange(
                { "rct1.footpath_surface.tarmac", "rct1.footpath_surface.dirt", "rct1.footpath_surface.crazy_paving",
                  "rct1.footpath_surface.tiles_brown", "rct1aa.footpath_surface.ash", "rct1aa.footpath_surface.tarmac_green",
                  "rct1aa.footpath_surface.tarmac_brown", "rct1aa.footpath_surface.tiles_grey",
                  "rct1aa.footpath_surface.tarmac_red", "rct1ll.footpath_surface.tiles_green",
                  "rct1ll.footpath_surface.tiles_red", "rct1.footpath_surface.queue_blue", "rct1aa.footpath_surface.queue_red",
                  "rct1aa.footpath_surface.queue_yellow", "rct1aa.footpath_surface.queue_green" });

            _footpathRailingsEntries.AddRange({ "rct2.footpath_railings.wood", "rct1ll.footpath_railings.space",
                                                "rct1ll.footpath_railings.bamboo", "rct2.footpath_railings.concrete" });

            // Add default surfaces
            _terrainSurfaceEntries.AddRange(
                { "rct2.terrain_surface.grass", "rct2.terrain_surface.sand", "rct2.terrain_surface.dirt",
                  "rct2.terrain_surface.rock", "rct2.terrain_surface.martian", "rct2.terrain_surface.chequerboard",
                  "rct2.terrain_surface.grass_clumps", "rct2.terrain_surface.ice", "rct2.terrain_surface.grid_red",
                  "rct2.terrain_surface.grid_yellow", "rct2.terrain_surface.grid_purple", "rct2.terrain_surface.grid_green",
                  "rct2.terrain_surface.sand_red", "rct2.terrain_surface.sand_brown", "rct1aa.terrain_surface.roof_red",
                  "rct1ll.terrain_surface.roof_grey", "rct1ll.terrain_surface.rust", "rct1ll.terrain_surface.wood" });

            // Add default edges
            _terrainEdgeEntries.AddRange({ "rct2.terrain_edge.rock", "rct2.terrain_edge.wood_red",
                                           "rct2.terrain_edge.wood_black", "rct2.terrain_edge.ice", "rct1.terrain_edge.brick",
                                           "rct1.terrain_edge.iron", "rct1aa.terrain_edge.grey", "rct1aa.terrain_edge.yellow",
                                           "rct1aa.terrain_edge.red", "rct1ll.terrain_edge.purple", "rct1ll.terrain_edge.green",
                                           "rct1ll.terrain_edge.stone_brown", "rct1ll.terrain_edge.stone_grey",
                                           "rct1ll.terrain_edge.skyscraper_a", "rct1ll.terrain_edge.skyscraper_b" });
        }

        void AddAvailableEntriesFromResearchList()
        {
            size_t researchListCount;
            const ResearchItem* researchList = GetResearchList(&researchListCount);
            BitSet<EnumValue(RideType::Count)> rideTypeInResearch = GetRideTypesPresentInResearchList(
                researchList, researchListCount);
            for (size_t i = 0; i < researchListCount; i++)
            {
                const ResearchItem* researchItem = &researchList[i];

                if (researchItem->Flags == RCT1ResearchFlagsSeparator)
                {
                    if (researchItem->Item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                    if (researchItem->Item == RCT1_RESEARCH_END_AVAILABLE
                        || researchItem->Item == RCT1_RESEARCH_END_RESEARCHABLE)
                    {
                        continue;
                    }
                }

                switch (researchItem->Type)
                {
                    case RCT1_RESEARCH_TYPE_THEME:
                        AddEntriesForSceneryTheme(researchItem->Item);
                        break;
                    case RCT1_RESEARCH_TYPE_RIDE:
                        AddEntryForRideType(static_cast<RideType>(researchItem->Item));
                        break;
                    case RCT1_RESEARCH_TYPE_VEHICLE:
                        // For some bizarre reason, RCT1 research lists contain vehicles that aren't actually researched.
                        // Extra bizarrely, this does not seem to apply to Loopy Landscapes saves/scenarios.
                        if (rideTypeInResearch[researchItem->RelatedRide] || _gameVersion == FILE_VERSION_RCT1_LL)
                        {
                            AddEntryForVehicleType(static_cast<RideType>(researchItem->RelatedRide), researchItem->Item);
                        }
                        break;
                }
            }
        }

        void AddAvailableEntriesFromMap()
        {
            size_t maxTiles = Limits::MaxMapSize * Limits::MaxMapSize;
            size_t tileIndex = 0;
            RCT12TileElement* tileElement = _s4.TileElements;

            while (tileIndex < maxTiles)
            {
                switch (tileElement->GetType())
                {
                    case RCT12TileElementType::Surface:
                    {
                        auto surfaceEl = tileElement->AsSurface();
                        auto surfaceStyle = surfaceEl->GetSurfaceStyle();
                        auto edgeStyle = surfaceEl->GetEdgeStyle();
                        AddEntryForTerrainSurface(surfaceStyle);
                        AddEntryForTerrainEdge(edgeStyle);
                        break;
                    }
                    case RCT12TileElementType::Path:
                    {
                        uint8_t pathType = tileElement->AsPath()->GetRCT1PathType();
                        uint8_t pathAdditionsType = tileElement->AsPath()->GetAddition();
                        uint8_t footpathRailingsType = RCT1_PATH_SUPPORT_TYPE_TRUSS;
                        if (_gameVersion == FILE_VERSION_RCT1_LL)
                        {
                            footpathRailingsType = tileElement->AsPath()->GetRCT1SupportType();
                        }

                        AddEntryForPathAddition(pathAdditionsType);
                        AddEntryForPathSurface(pathType);
                        AddEntryForFootpathRailings(footpathRailingsType);
                        break;
                    }
                    case RCT12TileElementType::SmallScenery:
                        AddEntryForSmallScenery(tileElement->AsSmallScenery()->GetEntryIndex());
                        break;
                    case RCT12TileElementType::LargeScenery:
                        AddEntryForLargeScenery(tileElement->AsLargeScenery()->GetEntryIndex());
                        break;
                    case RCT12TileElementType::Wall:
                    {
                        for (int32_t edge = 0; edge < 4; edge++)
                        {
                            int32_t type = tileElement->AsWall()->GetRCT1WallType(edge);

                            if (type != -1)
                            {
                                AddEntryForWall(type);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }

                if ((tileElement++)->IsLastForTile())
                {
                    tileIndex++;
                }
            }
        }

        void AddAvailableEntriesFromRides()
        {
            for (size_t i = 0; i < std::size(_s4.Rides); i++)
            {
                auto ride = &_s4.Rides[i];
                if (ride->Type != RideType::Null)
                {
                    if (RCT1::RideTypeUsesVehicles(ride->Type))
                        AddEntryForVehicleType(ride->Type, ride->VehicleType);
                    else
                        AddEntryForRideType(ride->Type);
                }
            }
        }

        void AddAvailableEntriesFromSceneryGroups()
        {
            for (int32_t sceneryTheme = 0; sceneryTheme <= RCT1_SCENERY_THEME_PAGODA; sceneryTheme++)
            {
                if (sceneryTheme != 0 && _sceneryThemeTypeToEntryMap[sceneryTheme] == OBJECT_ENTRY_INDEX_NULL)
                    continue;

                auto objects = RCT1::GetSceneryObjects(sceneryTheme);
                for (auto objectName : objects)
                {
                    auto& objectRepository = OpenRCT2::GetContext()->GetObjectRepository();
                    auto foundObject = objectRepository.FindObject(objectName);
                    if (foundObject != nullptr)
                    {
                        auto objectType = foundObject->Type;
                        switch (objectType)
                        {
                            case ObjectType::SmallScenery:
                            case ObjectType::LargeScenery:
                            case ObjectType::Walls:
                            case ObjectType::Paths:
                            case ObjectType::PathBits:
                            {
                                RCT12::EntryList* entries = GetEntryList(objectType);

                                // Check if there are spare entries available
                                size_t maxEntries = static_cast<size_t>(object_entry_group_counts[EnumValue(objectType)]);
                                if (entries != nullptr && entries->GetCount() < maxEntries)
                                {
                                    entries->GetOrAddEntry(objectName);
                                }
                                break;
                            }
                            default:
                                // This switch processes only ObjectTypes valid for scenery
                                break;
                        }
                    }
                    else
                    {
                        LOG_ERROR("Cannot find object %s", objectName);
                    }
                }
            }
        }

        void AddEntryForWater()
        {
            std::string_view entryName;
            if (_gameVersion < FILE_VERSION_RCT1_LL)
            {
                entryName = RCT1::GetWaterObject(RCT1_WATER_CYAN);
            }
            else
            {
                entryName = RCT1::GetWaterObject(_s4.WaterColour);
            }
            _waterEntry.GetOrAddEntry(entryName);
        }

        void AddEntryForRideType(RideType rideType)
        {
            Guard::Assert(EnumValue(rideType) < std::size(_rideTypeToRideEntryMap));

            if (_rideTypeToRideEntryMap[EnumValue(rideType)] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto entryName = RCT1::GetRideTypeObject(rideType);
                if (!entryName.empty())
                {
                    auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                    _rideTypeToRideEntryMap[EnumValue(rideType)] = entryIndex;
                }
            }
        }

        void AddEntryForVehicleType(RideType rideType, uint8_t vehicleType)
        {
            Guard::Assert(EnumValue(rideType) < std::size(_rideTypeToRideEntryMap));

            if (_vehicleTypeToRideEntryMap[vehicleType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto entryName = RCT1::GetVehicleObject(vehicleType);
                if (!entryName.empty())
                {
                    auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                    _vehicleTypeToRideEntryMap[vehicleType] = entryIndex;

                    if (rideType != RideType::Null)
                        AddEntryForRideType(rideType);
                }
            }
        }

        void AddEntryForSmallScenery(ObjectEntryIndex smallSceneryType)
        {
            assert(smallSceneryType < std::size(_smallSceneryTypeToEntryMap));
            if (_smallSceneryTypeToEntryMap[smallSceneryType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
                auto entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

                _smallSceneryTypeToEntryMap[smallSceneryType] = entryIndex;
            }
        }

        void AddEntryForLargeScenery(ObjectEntryIndex largeSceneryType)
        {
            assert(largeSceneryType < std::size(_largeSceneryTypeToEntryMap));
            if (_largeSceneryTypeToEntryMap[largeSceneryType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
                auto entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

                _largeSceneryTypeToEntryMap[largeSceneryType] = entryIndex;
            }
        }

        void AddEntryForWall(ObjectEntryIndex wallType)
        {
            assert(wallType < std::size(_wallTypeToEntryMap));
            if (_wallTypeToEntryMap[wallType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto entryName = RCT1::GetWallObject(wallType);
                auto entryIndex = _wallEntries.GetOrAddEntry(entryName);

                _wallTypeToEntryMap[wallType] = entryIndex;
            }
        }

        void AddEntryForPathSurface(ObjectEntryIndex pathType)
        {
            assert(pathType < std::size(_footpathSurfaceTypeToEntryMap));
            if (_footpathSurfaceTypeToEntryMap[pathType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto identifier = RCT1::GetPathSurfaceObject(pathType);
                if (!identifier.empty())
                {
                    auto entryIndex = _footpathSurfaceEntries.GetOrAddEntry(identifier);
                    _footpathSurfaceTypeToEntryMap[pathType] = entryIndex;
                }
            }
        }

        void AddEntryForPathAddition(ObjectEntryIndex pathAdditionType)
        {
            if (pathAdditionType == RCT1_PATH_ADDITION_NONE)
                return;

            if (_pathAdditionTypeToEntryMap[pathAdditionType] == OBJECT_ENTRY_INDEX_NULL)
            {
                uint8_t normalisedPathAdditionType = RCT1::NormalisePathAddition(pathAdditionType);
                if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == OBJECT_ENTRY_INDEX_NULL)
                {
                    auto entryName = RCT1::GetPathAddtionObject(normalisedPathAdditionType);
                    auto entryIndex = _pathAdditionEntries.GetOrAddEntry(entryName);

                    _pathAdditionTypeToEntryMap[normalisedPathAdditionType] = entryIndex;
                }

                _pathAdditionTypeToEntryMap[pathAdditionType] = _pathAdditionTypeToEntryMap[normalisedPathAdditionType];
            }
        }

        void AddEntriesForSceneryTheme(ObjectEntryIndex sceneryThemeType)
        {
            if (sceneryThemeType == RCT1_SCENERY_THEME_GENERAL || sceneryThemeType == RCT1_SCENERY_THEME_JUMPING_FOUNTAINS
                || sceneryThemeType == RCT1_SCENERY_THEME_GARDEN_CLOCK)
            {
                _sceneryThemeTypeToEntryMap[sceneryThemeType] = ObjectEntryIndexIgnore;
            }
            else
            {
                auto entryName = RCT1::GetSceneryGroupObject(sceneryThemeType);
                if (_sceneryGroupEntries.GetCount() >= MAX_SCENERY_GROUP_OBJECTS)
                {
                    Console::WriteLine("Warning: More than %d (max scenery groups) in RCT1 park.", MAX_SCENERY_GROUP_OBJECTS);
                    std::string entryNameString = std::string(entryName);
                    Console::WriteLine("         [%s] scenery group not added.", entryNameString.c_str());
                }
                else
                {
                    auto entryIndex = _sceneryGroupEntries.GetOrAddEntry(entryName);
                    _sceneryThemeTypeToEntryMap[sceneryThemeType] = entryIndex;
                }
            }
        }

        void AddEntryForTerrainSurface(ObjectEntryIndex terrainSurfaceType)
        {
            assert(terrainSurfaceType < std::size(_terrainSurfaceTypeToEntryMap));
            if (_terrainSurfaceTypeToEntryMap[terrainSurfaceType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto identifier = RCT1::GetTerrainSurfaceObject(terrainSurfaceType);
                if (!identifier.empty())
                {
                    auto entryIndex = _terrainSurfaceEntries.GetOrAddEntry(identifier);
                    _terrainSurfaceTypeToEntryMap[terrainSurfaceType] = entryIndex;
                }
            }
        }

        void AddEntryForTerrainEdge(ObjectEntryIndex terrainEdgeType)
        {
            assert(terrainEdgeType < std::size(_terrainEdgeTypeToEntryMap));
            if (_terrainEdgeTypeToEntryMap[terrainEdgeType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto identifier = RCT1::GetTerrainEdgeObject(terrainEdgeType);
                if (!identifier.empty())
                {
                    auto entryIndex = _terrainEdgeEntries.GetOrAddEntry(identifier);
                    _terrainEdgeTypeToEntryMap[terrainEdgeType] = entryIndex;
                }
            }
        }

        void AddEntryForFootpathRailings(ObjectEntryIndex railingsType)
        {
            assert(railingsType < std::size(_footpathRailingsTypeToEntryMap));
            if (_footpathRailingsTypeToEntryMap[railingsType] == OBJECT_ENTRY_INDEX_NULL)
            {
                auto identifier = RCT1::GetFootpathRailingsObject(railingsType);
                if (!identifier.empty())
                {
                    auto entryIndex = _footpathRailingsEntries.GetOrAddEntry(identifier);
                    _footpathRailingsTypeToEntryMap[railingsType] = entryIndex;
                }
            }
        }

        void ImportRides()
        {
            for (int32_t i = 0; i < Limits::MaxRidesInPark; i++)
            {
                if (_s4.Rides[i].Type != RideType::Null)
                {
                    const auto rideId = RideId::FromUnderlying(i);
                    ImportRide(RideAllocateAtIndex(rideId), &_s4.Rides[i], rideId);
                }
            }
        }

        void ImportRide(::Ride* dst, RCT1::Ride* src, RideId rideIndex)
        {
            *dst = {};
            dst->id = rideIndex;

            // This is a peculiarity of this exact version number, which only Heide-Park seems to use.
            if (_s4.GameVersion == 110018 && src->Type == RideType::InvertedRollerCoaster)
            {
                dst->type = RIDE_TYPE_COMPACT_INVERTED_COASTER;
            }
            else
            {
                dst->type = RCT1::GetRideType(src->Type, src->VehicleType);
            }

            if (RCT1::RideTypeUsesVehicles(src->Type))
            {
                dst->subtype = _vehicleTypeToRideEntryMap[src->VehicleType];
            }
            else
            {
                dst->subtype = _rideTypeToRideEntryMap[EnumValue(src->Type)];
            }

            const auto* rideEntry = GetRideEntryByIndex(dst->subtype);
            // This can happen with hacked parks
            if (rideEntry == nullptr)
            {
                LOG_WARNING("Discarding ride with invalid ride entry");
                dst->type = RIDE_TYPE_NULL;
                return;
            }

            // Ride name
            if (IsUserStringID(src->Name))
            {
                dst->custom_name = GetUserString(src->Name);
            }

            dst->status = static_cast<RideStatus>(src->Status);

            // Flags
            dst->lifecycle_flags = src->LifecycleFlags;
            // These flags were not in the base game
            if (_gameVersion == FILE_VERSION_RCT1)
            {
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE;
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            }

            // Station
            if (src->OverallView.IsNull())
            {
                dst->overall_view.SetNull();
            }
            else
            {
                dst->overall_view = TileCoordsXY{ src->OverallView.x, src->OverallView.y }.ToCoordsXY();
            }

            for (StationIndex::UnderlyingType i = 0; i < Limits::MaxStationsPerRide; i++)
            {
                auto& dstStation = dst->GetStation(StationIndex::FromUnderlying(i));
                if (src->StationStarts[i].IsNull())
                {
                    dstStation.Start.SetNull();
                }
                else
                {
                    auto tileStartLoc = TileCoordsXY{ src->StationStarts[i].x, src->StationStarts[i].y };
                    dstStation.Start = tileStartLoc.ToCoordsXY();
                }
                dstStation.SetBaseZ(src->StationHeights[i] * Limits::CoordsZStep);
                dstStation.Length = src->StationLengths[i];
                dstStation.Depart = src->StationLights[i];

                dstStation.TrainAtStation = src->StationDeparts[i];

                // Direction is fixed later.
                if (src->Entrances[i].IsNull())
                    dstStation.Entrance.SetNull();
                else
                    dstStation.Entrance = { src->Entrances[i].x, src->Entrances[i].y, src->StationHeights[i] / 2, 0 };

                if (src->Exits[i].IsNull())
                    dstStation.Exit.SetNull();
                else
                    dstStation.Exit = { src->Exits[i].x, src->Exits[i].y, src->StationHeights[i] / 2, 0 };

                dstStation.QueueTime = src->QueueTime[i];
                dstStation.LastPeepInQueue = EntityId::FromUnderlying(src->LastPeepInQueue[i]);
                dstStation.QueueLength = src->NumPeepsInQueue[i];

                dstStation.SegmentTime = src->Time[i];
                dstStation.SegmentLength = src->Length[i];
            }
            // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
            for (int32_t i = Limits::MaxStationsPerRide; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
            {
                auto& dstStation = dst->GetStation(StationIndex::FromUnderlying(i));
                dstStation.Start.SetNull();
                dstStation.TrainAtStation = RideStation::NO_TRAIN;
                dstStation.Entrance.SetNull();
                dstStation.Exit.SetNull();
                dstStation.LastPeepInQueue = EntityId::GetNull();
            }

            dst->num_stations = src->NumStations;

            // Vehicle links (indexes converted later)
            for (int32_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->Vehicles[i]);
            }
            for (int32_t i = Limits::MaxTrainsPerRide; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->NumTrains = src->NumTrains;
            dst->num_cars_per_train = src->NumCarsPerTrain + rideEntry->zero_cars;
            dst->ProposedNumTrains = src->NumTrains;
            dst->max_trains = src->MaxTrains;
            dst->proposed_num_cars_per_train = src->NumCarsPerTrain + rideEntry->zero_cars;
            dst->special_track_elements = src->SpecialTrackElements;
            dst->num_sheltered_sections = src->NumShelteredSections;
            dst->sheltered_length = src->ShelteredLength;

            // Operation
            dst->depart_flags = src->DepartFlags;
            dst->min_waiting_time = src->MinWaitingTime;
            dst->max_waiting_time = src->MaxWaitingTime;
            dst->operation_option = src->OperationOption;
            dst->num_circuits = 1;
            dst->MinCarsPerTrain = rideEntry->min_cars_in_train;
            dst->MaxCarsPerTrain = rideEntry->max_cars_in_train;

            // RCT1 used 5mph / 8 km/h for every lift hill
            dst->lift_hill_speed = 5;

            dst->music = OBJECT_ENTRY_INDEX_NULL;
            if (GetRideTypeDescriptor(dst->type).HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
            {
                if (_gameVersion == FILE_VERSION_RCT1)
                {
                    // Original RCT had no music settings, take default style
                    auto style = GetStyleFromMusicIdentifier(GetRideTypeDescriptor(dst->type).DefaultMusic);
                    if (style.has_value())
                    {
                        dst->music = style.value();
                    }

                    // Only merry-go-round and dodgems had music and used
                    // the same flag as synchronise stations for the option to enable it
                    if (src->Type == RideType::MerryGoRound || src->Type == RideType::Dodgems)
                    {
                        if (src->DepartFlags & RCT1_RIDE_DEPART_PLAY_MUSIC)
                        {
                            dst->depart_flags &= ~RCT1_RIDE_DEPART_PLAY_MUSIC;
                            dst->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
                        }
                    }
                }
                else
                {
                    dst->music = src->Music;
                }
            }

            if (src->OperatingMode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                // Launched rides never passed through the station in RCT1.
                dst->mode = RideMode::PoweredLaunch;
            }
            else
            {
                dst->mode = static_cast<RideMode>(src->OperatingMode);
            }

            SetRideColourScheme(dst, src);

            // Maintenance
            dst->build_date = static_cast<int32_t>(src->BuildDate);
            dst->inspection_interval = src->InspectionInterval;
            dst->last_inspection = src->LastInspection;
            dst->reliability = src->Reliability;
            dst->unreliability_factor = src->UnreliabilityFactor;
            dst->downtime = src->Downtime;
            dst->breakdown_reason = src->BreakdownReason;
            dst->mechanic_status = src->MechanicStatus;
            dst->mechanic = EntityId::FromUnderlying(src->Mechanic);
            dst->breakdown_reason_pending = src->BreakdownReasonPending;
            dst->inspection_station = StationIndex::FromUnderlying(src->InspectionStation);
            dst->broken_car = src->BrokenCar;
            dst->broken_vehicle = src->BrokenVehicle;

            // Measurement data
            dst->excitement = src->Excitement;
            dst->intensity = src->Intensity;
            dst->nausea = src->Nausea;

            dst->max_speed = src->MaxSpeed;
            dst->average_speed = src->AverageSpeed;

            dst->max_positive_vertical_g = src->MaxPositiveVerticalG;
            dst->max_negative_vertical_g = src->MaxNegativeVerticalG;
            dst->max_lateral_g = src->MaxLateralG;
            dst->previous_lateral_g = src->PreviousLateralG;
            dst->previous_vertical_g = src->PreviousVerticalG;
            dst->turn_count_banked = src->TurnCountBanked;
            dst->turn_count_default = src->TurnCountDefault;
            dst->turn_count_sloped = src->TurnCountSloped;
            dst->drops = src->NumDrops;
            dst->start_drop_height = src->StartDropHeight / 2;
            dst->highest_drop_height = src->HighestDropHeight / 2;
            if (dst->type == RIDE_TYPE_MINI_GOLF)
                dst->holes = src->NumInversions & 0x1F;
            else
                dst->inversions = src->NumInversions & 0x1F;
            dst->sheltered_eighths = src->NumInversions >> 5;
            dst->boat_hire_return_direction = src->BoatHireReturnDirection;
            dst->boat_hire_return_position = { src->BoatHireReturnPosition.x, src->BoatHireReturnPosition.y };
            dst->chairlift_bullwheel_rotation = src->ChairliftBullwheelRotation;
            for (int i = 0; i < 2; i++)
            {
                dst->ChairliftBullwheelLocation[i] = { src->ChairliftBullwheelLocation[i].x,
                                                       src->ChairliftBullwheelLocation[i].y, src->ChairliftBullwheelZ[i] / 2 };
            }

            if (src->CurTestTrackLocation.IsNull())
            {
                dst->CurTestTrackLocation.SetNull();
            }
            else
            {
                dst->CurTestTrackLocation = { src->CurTestTrackLocation.x, src->CurTestTrackLocation.y,
                                              src->CurTestTrackZ / 2 };
            }
            dst->testing_flags = src->TestingFlags;
            dst->current_test_segment = src->CurrentTestSegment;
            dst->current_test_station = StationIndex::GetNull();
            dst->average_speed_test_timeout = src->AverageSpeedTestTimeout;
            dst->slide_in_use = src->SlideInUse;
            dst->slide_peep_t_shirt_colour = RCT1::GetColour(src->SlidePeepTshirtColour);
            dst->spiral_slide_progress = src->SpiralSlideProgress;
            // Doubles as slide_peep
            dst->maze_tiles = src->MazeTiles;

            // Finance / customers
            dst->upkeep_cost = ToMoney64(src->UpkeepCost);
            dst->price[0] = src->Price;
            dst->price[1] = src->PriceSecondary;
            dst->income_per_hour = ToMoney64(src->IncomePerHour);
            dst->total_customers = src->TotalCustomers;
            dst->profit = ToMoney64(src->Profit);
            dst->total_profit = ToMoney64(src->TotalProfit);
            dst->value = ToMoney64(src->Value);
            for (size_t i = 0; i < std::size(src->NumCustomers); i++)
            {
                dst->num_customers[i] = src->NumCustomers[i];
            }

            dst->satisfaction = src->Satisfaction;
            dst->satisfaction_time_out = src->SatisfactionTimeOut;
            dst->satisfaction_next = src->SatisfactionNext;
            dst->popularity = src->Popularity;
            dst->popularity_next = src->PopularityNext;
            dst->popularity_time_out = src->PopularityTimeOut;

            dst->num_riders = src->NumRiders;

            dst->music_tune_id = TUNE_ID_NULL;
        }

        void SetRideColourScheme(::Ride* dst, RCT1::Ride* src)
        {
            // Colours
            dst->colour_scheme_type = src->ColourScheme;
            if (_gameVersion == FILE_VERSION_RCT1)
            {
                dst->track_colour[0].main = RCT1::GetColour(src->TrackPrimaryColour);
                dst->track_colour[0].additional = RCT1::GetColour(src->TrackSecondaryColour);
                dst->track_colour[0].supports = RCT1::GetColour(src->TrackSupportColour);

                // Balloons were always blue in the original RCT.
                if (src->Type == RideType::BalloonStall)
                {
                    dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
                }
                else if (src->Type == RideType::RiverRapids)
                {
                    dst->track_colour[0].main = COLOUR_WHITE;
                }
            }
            else
            {
                for (int i = 0; i < Limits::NumColourSchemes; i++)
                {
                    dst->track_colour[i].main = RCT1::GetColour(src->TrackColourMain[i]);
                    dst->track_colour[i].additional = RCT1::GetColour(src->TrackColourAdditional[i]);
                    dst->track_colour[i].supports = RCT1::GetColour(src->TrackColourSupports[i]);
                }
            }

            dst->entrance_style = OBJECT_ENTRY_INDEX_NULL;
            if (dst->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
            {
                // Entrance styles were introduced with AA. They correspond directly with those in RCT2.
                if (_gameVersion == FILE_VERSION_RCT1)
                {
                    dst->entrance_style = 0; // plain entrance
                }
                else
                {
                    dst->entrance_style = src->EntranceStyle;
                }
            }

            if (_gameVersion < FILE_VERSION_RCT1_LL && dst->type == RIDE_TYPE_MERRY_GO_ROUND)
            {
                // The merry-go-round in pre-LL versions was always yellow with red
                dst->vehicle_colours[0].Body = COLOUR_YELLOW;
                dst->vehicle_colours[0].Trim = COLOUR_BRIGHT_RED;
            }
            else
            {
                for (int i = 0; i < Limits::MaxTrainsPerRide; i++)
                {
                    // RCT1 had no third colour
                    const auto colourSchemeCopyDescriptor = GetColourSchemeCopyDescriptor(src->VehicleType);
                    if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Body = RCT1::GetColour(src->VehicleColours[i].Body);
                    }
                    else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Body = RCT1::GetColour(src->VehicleColours[i].Trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Body = colourSchemeCopyDescriptor.colour1;
                    }

                    if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Trim = RCT1::GetColour(src->VehicleColours[i].Body);
                    }
                    else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Trim = RCT1::GetColour(src->VehicleColours[i].Trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Trim = colourSchemeCopyDescriptor.colour2;
                    }

                    if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Tertiary = RCT1::GetColour(src->VehicleColours[i].Body);
                    }
                    else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Tertiary = RCT1::GetColour(src->VehicleColours[i].Trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                    }
                }
            }

            // In RCT1 and AA, the maze was always hedges.
            // LL has 4 types, like RCT2. For LL, only guard against invalid values.
            if (src->Type == RideType::HedgeMaze)
            {
                if (_gameVersion < FILE_VERSION_RCT1_LL || src->TrackColourSupports[0] > 3)
                    dst->track_colour[0].supports = MAZE_WALL_TYPE_HEDGE;
                else
                    dst->track_colour[0].supports = src->TrackColourSupports[0];
            }
        }

        void ImportRideMeasurements()
        {
            for (const auto& src : _s4.RideMeasurements)
            {
                if (src.RideIndex != RCT12_RIDE_ID_NULL)
                {
                    auto ride = GetRide(RCT12RideIdToOpenRCT2RideId(src.RideIndex));
                    if (ride != nullptr)
                    {
                        ride->measurement = std::make_unique<RideMeasurement>();
                        ImportRideMeasurement(*ride->measurement, src);
                    }
                }
            }
        }

        void ImportRideMeasurement(RideMeasurement& dst, const RCT12RideMeasurement& src)
        {
            dst.flags = src.Flags;
            dst.last_use_tick = src.LastUseTick;
            dst.num_items = src.NumItems;
            dst.current_item = src.CurrentItem;
            dst.vehicle_index = src.VehicleIndex;
            dst.current_station = StationIndex::FromUnderlying(src.CurrentStation);
            for (size_t i = 0; i < std::size(src.Velocity); i++)
            {
                dst.velocity[i] = src.Velocity[i] / 2;
                dst.altitude[i] = src.Altitude[i] / 2;
                dst.vertical[i] = src.Vertical[i] / 2;
                dst.lateral[i] = src.Lateral[i] / 2;
            }
        }

        void ImportEntity(const RCT12EntityBase& src);
        template<typename T> void ImportEntity(const RCT12EntityBase& src);

        void ImportSprites()
        {
            for (int i = 0; i < Limits::MaxEntities; i++)
            {
                ImportEntity(_s4.Entities[i].Unknown);
            }
        }

        void SetVehicleColours(::Vehicle* dst, const RCT1::Vehicle* src)
        {
            const auto& srcRide = _s4.Rides[src->Ride];
            uint8_t vehicleTypeIndex = srcRide.VehicleType;
            RCT1::VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                vehicleTypeIndex);

            // RCT1 had no third colour
            if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
            {
                dst->colours.Body = RCT1::GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
            {
                dst->colours.Body = RCT1::GetColour(src->Colours.TrimColour);
            }
            else
            {
                dst->colours.Body = colourSchemeCopyDescriptor.colour1;
            }

            if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
            {
                dst->colours.Trim = RCT1::GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
            {
                dst->colours.Trim = RCT1::GetColour(src->Colours.TrimColour);
            }
            else
            {
                dst->colours.Trim = colourSchemeCopyDescriptor.colour2;
            }

            if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
            {
                dst->colours.Tertiary = RCT1::GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
            {
                dst->colours.Tertiary = RCT1::GetColour(src->Colours.TrimColour);
            }
            else
            {
                dst->colours.Tertiary = colourSchemeCopyDescriptor.colour3;
            }
        }

        void FixImportStaff()
        {
            // Only the individual patrol areas have been converted, so generate the combined patrol areas of each staff type
            UpdateConsolidatedPatrolAreas();
        }

        void ImportPeep(::Peep* dst, const RCT1::Peep* src)
        {
            // Peep vs. staff (including which kind)
            dst->SpriteType = RCT1::GetPeepSpriteType(src->SpriteType);
            dst->Action = static_cast<PeepActionType>(src->Action);
            dst->SpecialSprite = src->SpecialSprite;
            dst->NextActionSpriteType = static_cast<PeepActionSpriteType>(src->NextActionSpriteType);
            dst->ActionSpriteImageOffset = src->ActionSpriteImageOffset;
            dst->WalkingFrameNum = src->NoActionFrameNum;
            dst->ActionSpriteType = static_cast<PeepActionSpriteType>(src->ActionSpriteType);
            dst->ActionFrame = src->ActionFrame;

            const SpriteBounds* spriteBounds = &GetSpriteBounds(dst->SpriteType, dst->ActionSpriteType);
            dst->sprite_width = spriteBounds->sprite_width;
            dst->sprite_height_negative = spriteBounds->sprite_height_negative;
            dst->sprite_height_positive = spriteBounds->sprite_height_positive;

            dst->MoveTo({ src->x, src->y, src->z });

            dst->sprite_direction = src->EntityDirection;

            // Peep name
            if (IsUserStringID(src->NameStringID))
            {
                dst->SetName(GetUserString(src->NameStringID));
            }

            dst->State = static_cast<PeepState>(src->State);
            dst->SubState = src->SubState;
            dst->NextLoc = { src->NextX, src->NextY, src->NextZ * Limits::CoordsZStep };
            dst->NextFlags = src->NextFlags;
            dst->Var37 = src->Var37;
            dst->StepProgress = src->StepProgress;
            dst->TshirtColour = RCT1::GetColour(src->TshirtColour);
            dst->TrousersColour = RCT1::GetColour(src->TrousersColour);
            dst->DestinationX = src->DestinationX;
            dst->DestinationY = src->DestinationY;
            dst->DestinationTolerance = src->DestinationTolerance;
            dst->PeepDirection = src->Direction;
            dst->Energy = src->Energy;
            dst->EnergyTarget = src->EnergyTarget;
            dst->Mass = src->Mass;
            dst->WindowInvalidateFlags = 0;
            dst->CurrentRide = RCT12RideIdToOpenRCT2RideId(src->CurrentRide);
            dst->CurrentRideStation = StationIndex::FromUnderlying(src->CurrentRideStation);
            dst->CurrentTrain = src->CurrentTrain;
            dst->CurrentCar = src->CurrentCar;
            dst->CurrentSeat = src->CurrentSeat;
            dst->InteractionRideIndex = RCT12RideIdToOpenRCT2RideId(src->InteractionRideIndex);
            dst->PeepId = src->ID;
            dst->PathCheckOptimisation = 0;
            dst->PeepFlags = 0;
            dst->PathfindGoal.x = 0xFF;
            dst->PathfindGoal.y = 0xFF;
            dst->PathfindGoal.z = 0xFF;
            dst->PathfindGoal.direction = INVALID_DIRECTION;
        }

        void ImportStaffPatrolArea(Staff* staffmember, uint8_t staffId)
        {
            // TODO: It is likely that S4 files should have a staffmode check before setting
            // patrol areas. See S6 importer.

            // The patrol areas in RCT1 are encoded as follows, for coordinates x and y, separately for every staff member:
            // - Chop off the 7 lowest bits of the x and y coordinates, which leaves 5 bits per coordinate.
            //   This step also "produces" the 4x4 patrol squares.
            // - Append the two bitstrings to a 10-bit value like so: yyyyyxxxxx
            // - Use this 10-bit value as an index into an 8-bit array. The array is sized such that every 4x4 square
            //   used for the patrols on the map has a bit in that array. If a bit is 1, that square is part of the patrol.
            //   The correct bit position in that array is found like this: yyyyyxx|xxx
            //                                          index in the array ----^     ^--- bit position in the 8-bit value
            // We do the opposite in this function to recover the x and y values.

            int32_t peepOffset = staffId * Limits::PatrolAreaSize;
            for (int32_t i = 0; i < Limits::PatrolAreaSize; i++)
            {
                if (_s4.PatrolAreas[peepOffset + i] == 0)
                {
                    // No patrol for this area
                    continue;
                }

                // Loop over the bits of the uint8_t
                for (int32_t j = 0; j < 8; j++)
                {
                    int8_t bit = (_s4.PatrolAreas[peepOffset + i] >> j) & 1;
                    if (bit == 0)
                    {
                        // No patrol for this area
                        continue;
                    }
                    // val contains the 5 highest bits of both the x and y coordinates
                    int32_t val = j | (i << 3);
                    int32_t x = val & 0x1F;
                    x <<= 7;
                    int32_t y = val & 0x3E0;
                    y <<= 2;
                    staffmember->SetPatrolArea(
                        MapRange(x, y, x + (4 * COORDS_XY_STEP) - 1, y + (4 * COORDS_XY_STEP) - 1), true);
                }
            }
        }

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12EntityBase* src)
        {
            dst->sprite_direction = src->EntityDirection;
            dst->sprite_width = src->SpriteWidth;
            dst->sprite_height_negative = src->SpriteHeightNegative;
            dst->sprite_height_positive = src->SpriteHeightPositive;
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
        }

        void ImportPeepSpawns()
        {
            gPeepSpawns.clear();
            for (size_t i = 0; i < Limits::MaxPeepSpawns; i++)
            {
                if (_s4.PeepSpawn[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
                {
                    PeepSpawn spawn = { _s4.PeepSpawn[i].x, _s4.PeepSpawn[i].y, _s4.PeepSpawn[i].z * 16,
                                        _s4.PeepSpawn[i].direction };
                    gPeepSpawns.push_back(spawn);
                }
            }
        }

        void ImportFinance()
        {
            gParkEntranceFee = _s4.ParkEntranceFee;
            gLandPrice = ToMoney64(_s4.LandPrice);
            gConstructionRightsPrice = ToMoney64(_s4.ConstructionRightsPrice);

            gCash = ToMoney64(_s4.Cash);
            gBankLoan = ToMoney64(_s4.Loan);
            gMaxBankLoan = ToMoney64(_s4.MaxLoan);
            // It's more like 1.33%, but we can only use integers. Can be fixed once we have our own save format.
            gBankLoanInterestRate = 1;
            gInitialCash = ToMoney64(_s4.Cash);

            gCompanyValue = ToMoney64(_s4.CompanyValue);
            gParkValue = CorrectRCT1ParkValue(_s4.ParkValue);
            gCurrentProfit = ToMoney64(_s4.Profit);

            for (size_t i = 0; i < Limits::FinanceGraphSize; i++)
            {
                gCashHistory[i] = ToMoney64(_s4.CashHistory[i]);
                gParkValueHistory[i] = CorrectRCT1ParkValue(_s4.ParkValueHistory[i]);
                gWeeklyProfitHistory[i] = ToMoney64(_s4.WeeklyProfitHistory[i]);
            }

            for (size_t i = 0; i < Limits::ExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::ExpenditureTypeCount; j++)
                {
                    gExpenditureTable[i][j] = ToMoney64(_s4.Expenditure[i][j]);
                }
            }
            gCurrentExpenditure = ToMoney64(_s4.TotalExpenditure);

            gScenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s4.CompletedCompanyValue);
            gTotalAdmissions = _s4.NumAdmissions;
            gTotalIncomeFromAdmissions = ToMoney64(_s4.AdmissionTotalIncome);

            // TODO marketing campaigns not working
            static_assert(
                std::numeric_limits<uint8_t>::max() > ADVERTISING_CAMPAIGN_COUNT,
                "Advertising enum bigger than capacity of iterator");
            for (uint8_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                if (_s4.MarketingStatus[i] & CAMPAIGN_ACTIVE_FLAG)
                {
                    MarketingCampaign campaign;
                    campaign.Type = i;
                    campaign.WeeksLeft = _s4.MarketingStatus[i] & ~CAMPAIGN_ACTIVE_FLAG;
                    if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                    {
                        campaign.RideId = RCT12RideIdToOpenRCT2RideId(_s4.MarketingAssoc[i]);
                    }
                    else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        campaign.ShopItemType = ShopItem(_s4.MarketingAssoc[i]);
                    }
                    gMarketingCampaigns.push_back(campaign);
                }
            }
        }

        void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const RCT12::EntryList& entryList)
        {
            AppendRequiredObjects(objectList, objectType, entryList.GetEntries());
        }

        void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const std::vector<std::string>& objectNames)
        {
            for (const auto& objectName : objectNames)
            {
                auto descriptor = ObjectEntryDescriptor(objectName);
                descriptor.Type = objectType;
                objectList.Add(descriptor);
            }
        }

        ObjectList GetRequiredObjects()
        {
            ObjectList result;
            AppendRequiredObjects(result, ObjectType::Ride, _rideEntries);
            AppendRequiredObjects(result, ObjectType::SmallScenery, _smallSceneryEntries);
            AppendRequiredObjects(result, ObjectType::LargeScenery, _largeSceneryEntries);
            AppendRequiredObjects(result, ObjectType::Walls, _wallEntries);
            AppendRequiredObjects(result, ObjectType::Paths, _pathEntries);
            AppendRequiredObjects(result, ObjectType::PathBits, _pathAdditionEntries);
            AppendRequiredObjects(result, ObjectType::SceneryGroup, _sceneryGroupEntries);
            AppendRequiredObjects(
                result, ObjectType::Banners,
                std::vector<std::string>({
                    "rct2.footpath_banner.bn1",
                    "rct2.footpath_banner.bn2",
                    "rct2.footpath_banner.bn3",
                    "rct2.footpath_banner.bn4",
                    "rct2.footpath_banner.bn5",
                    "rct2.footpath_banner.bn6",
                    "rct2.footpath_banner.bn7",
                    "rct2.footpath_banner.bn8",
                    "rct2.footpath_banner.bn9",
                }));
            AppendRequiredObjects(result, ObjectType::ParkEntrance, std::vector<std::string>({ "rct2.park_entrance.pkent1" }));
            AppendRequiredObjects(result, ObjectType::Water, _waterEntry);
            AppendRequiredObjects(result, ObjectType::TerrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::TerrainEdge, _terrainEdgeEntries);
            AppendRequiredObjects(result, ObjectType::FootpathSurface, _footpathSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::FootpathRailings, _footpathRailingsEntries);
            RCT12AddDefaultObjects(result);
            return result;
        }

        void ImportTileElements()
        {
            gMapBaseZ = 7;

            // Build tile pointer cache (needed to get the first element at a certain location)
            auto tilePointerIndex = TilePointerIndex<RCT12TileElement>(
                Limits::MaxMapSize, _s4.TileElements, std::size(_s4.TileElements));

            std::vector<TileElement> tileElements;
            const auto maxSize = _s4.MapSize == 0 ? Limits::MaxMapSize : _s4.MapSize;
            for (TileCoordsXY coords = { 0, 0 }; coords.y < MAXIMUM_MAP_SIZE_TECHNICAL; coords.y++)
            {
                for (coords.x = 0; coords.x < MAXIMUM_MAP_SIZE_TECHNICAL; coords.x++)
                {
                    auto tileAdded = false;
                    if (coords.x < maxSize && coords.y < maxSize)
                    {
                        // This is the equivalent of MapGetFirstElementAt(x, y), but on S4 data.
                        RCT12TileElement* srcElement = tilePointerIndex.GetFirstElementAt(coords);
                        do
                        {
                            if (srcElement->BaseHeight == Limits::MaxElementHeight)
                                continue;

                            // Reserve 8 elements for import
                            auto originalSize = tileElements.size();
                            tileElements.resize(originalSize + 16);
                            auto dstElement = tileElements.data() + originalSize;
                            auto numAddedElements = ImportTileElement(dstElement, srcElement);
                            tileElements.resize(originalSize + numAddedElements);
                            tileAdded = true;
                        } while (!(srcElement++)->IsLastForTile());
                    }

                    if (!tileAdded)
                    {
                        // Add a default surface element, we always need at least one element per tile
                        auto& dstElement = tileElements.emplace_back();
                        dstElement.ClearAs(TileElementType::Surface);
                        dstElement.SetLastForTile(true);
                    }

                    // Set last element flag in case the original last element was never added
                    if (tileElements.size() > 0)
                    {
                        tileElements.back().SetLastForTile(true);
                    }
                }
            }

            SetTileElements(std::move(tileElements));
            FixEntrancePositions();
        }

        size_t ImportTileElement(TileElement* dst, const RCT12TileElement* src)
        {
            const auto rct12type = src->GetType();
            const auto tileElementType = ToOpenRCT2TileElementType(rct12type);
            dst->ClearAs(tileElementType);
            dst->SetDirection(src->GetDirection());

            // All saved in "flags"
            dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
            // Skipping IsGhost, which appears to use a different flag in RCT1.
            // This flag will be set by the caller.
            dst->SetLastForTile(false);

            dst->SetBaseZ(src->BaseHeight * Limits::CoordsZStep);
            dst->SetClearanceZ(src->ClearanceHeight * Limits::CoordsZStep);

            switch (tileElementType)
            {
                case TileElementType::Surface:
                {
                    auto dst2 = dst->AsSurface();
                    auto src2 = src->AsSurface();

                    auto surfaceStyle = _terrainSurfaceTypeToEntryMap[src2->GetSurfaceStyle()];
                    auto edgeStyle = _terrainEdgeTypeToEntryMap[src2->GetEdgeStyle()];

                    dst2->SetSlope(src2->GetSlope());
                    dst2->SetSurfaceStyle(surfaceStyle);
                    dst2->SetEdgeStyle(edgeStyle);
                    dst2->SetGrassLength(src2->GetGrassLength());
                    dst2->SetOwnership(src2->GetOwnership());
                    dst2->SetParkFences(src2->GetParkFences());
                    dst2->SetWaterHeight(src2->GetWaterHeight());
                    dst2->SetHasTrackThatNeedsWater(src2->HasTrackThatNeedsWater());

                    return 1;
                }
                case TileElementType::Path:
                {
                    auto dst2 = dst->AsPath();
                    auto src2 = src->AsPath();

                    dst2->SetQueueBannerDirection(src2->GetQueueBannerDirection());
                    dst2->SetSloped(src2->IsSloped());
                    dst2->SetSlopeDirection(src2->GetSlopeDirection());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetWide(src2->IsWide());
                    dst2->SetHasQueueBanner(src2->HasQueueBanner());
                    dst2->SetEdges(src2->GetEdges());
                    dst2->SetCorners(src2->GetCorners());
                    dst2->SetAddition(src2->GetAddition());
                    dst2->SetAdditionIsGhost(false);
                    dst2->SetAdditionStatus(src2->GetAdditionStatus());

                    // Type
                    uint8_t pathType = src2->GetRCT1PathType();
                    auto entryIndex = _footpathSurfaceTypeToEntryMap[pathType];

                    dst2->SetDirection(0);
                    dst2->SetIsBroken(false);
                    dst2->SetIsBlockedByVehicle(false);

                    dst2->SetLegacyPathEntryIndex(entryIndex);
                    dst2->SetShouldDrawPathOverSupports(true);
                    if (RCT1::PathIsQueue(pathType))
                    {
                        dst2->SetIsQueue(true);
                    }

                    uint8_t railingsType = RCT1_PATH_SUPPORT_TYPE_TRUSS;
                    if (_gameVersion == FILE_VERSION_RCT1_LL)
                    {
                        railingsType = src2->GetRCT1SupportType();
                    }
                    auto railingsEntryIndex = _footpathRailingsTypeToEntryMap[railingsType];
                    dst2->SetRailingsEntryIndex(railingsEntryIndex);

                    // Additions
                    ObjectEntryIndex additionType = dst2->GetAddition();
                    if (additionType != RCT1_PATH_ADDITION_NONE)
                    {
                        ObjectEntryIndex normalisedType = RCT1::NormalisePathAddition(additionType);
                        entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
                        if (additionType != normalisedType)
                        {
                            dst2->SetIsBroken(true);
                        }
                        dst2->SetAddition(entryIndex + 1);
                    }
                    return 1;
                }
                case TileElementType::Track:
                {
                    auto dst2 = dst->AsTrack();
                    auto src2 = src->AsTrack();
                    const auto* ride = GetRide(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    auto rideType = (ride != nullptr) ? ride->type : RIDE_TYPE_NULL;

                    dst2->SetTrackType(RCT1TrackTypeToOpenRCT2(src2->GetTrackType(), rideType));
                    dst2->SetRideType(rideType);
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetColourScheme(src2->GetColourScheme());
                    dst2->SetHasChain(src2->HasChain());
                    dst2->SetHasCableLift(false);
                    dst2->SetInverted(src2->IsInverted());
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetHasGreenLight(src2->HasGreenLight());
                    dst2->SetIsIndestructible(src2->IsIndestructible());
                    if (rideType == RIDE_TYPE_GHOST_TRAIN)
                    {
                        dst2->SetDoorAState(src2->GetDoorAState());
                        dst2->SetDoorBState(src2->GetDoorBState());
                    }
                    else
                    {
                        dst2->SetSeatRotation(DEFAULT_SEAT_ROTATION);
                    }
                    // Skipping IsHighlighted()

                    auto trackType = dst2->GetTrackType();
                    if (TrackTypeHasSpeedSetting(trackType))
                    {
                        dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                    }
                    else if (trackType == TrackElemType::OnRidePhoto)
                    {
                        dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                    }

                    // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                    const auto& rtd = GetRideTypeDescriptor(rideType);
                    if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
                    {
                        dst2->SetMazeEntry(src2->GetMazeEntry());
                    }

                    if (TrackTypeMustBeMadeInvisible(rideType, trackType))
                    {
                        dst->SetInvisible(true);
                    }

                    return 1;
                }
                case TileElementType::SmallScenery:
                {
                    auto dst2 = dst->AsSmallScenery();
                    auto src2 = src->AsSmallScenery();

                    auto entryIndex = _smallSceneryTypeToEntryMap[src2->GetEntryIndex()];
                    dst2->SetEntryIndex(entryIndex);
                    dst2->SetAge(src2->GetAge());
                    dst2->SetSceneryQuadrant(src2->GetSceneryQuadrant());
                    dst2->SetPrimaryColour(RCT1::GetColour(src2->GetPrimaryColour()));
                    if (src2->NeedsSupports())
                        dst2->SetNeedsSupports();

                    // Copied from [rct2: 0x006A2956]
                    switch (src2->GetEntryIndex())
                    {
                        case RCT1_SCENERY_GEOMETRIC_SCULPTURE_1:
                        case RCT1_SCENERY_GEOMETRIC_SCULPTURE_2:
                        case RCT1_SCENERY_GEOMETRIC_SCULPTURE_3:
                        case RCT1_SCENERY_GEOMETRIC_SCULPTURE_4:
                        case RCT1_SCENERY_GEOMETRIC_SCULPTURE_5:
                            dst2->SetSecondaryColour(COLOUR_WHITE);
                            break;
                        case RCT1_SCENERY_TULIPS_1:
                        case RCT1_SCENERY_TULIPS_2:
                            dst2->SetPrimaryColour(COLOUR_BRIGHT_RED);
                            dst2->SetSecondaryColour(COLOUR_YELLOW);
                            break;
                        case RCT1_SCENERY_SMALL_RED_GARDENS:
                            dst2->SetPrimaryColour(COLOUR_BRIGHT_RED);
                            break;
                    }

                    return 1;
                }
                case TileElementType::Entrance:
                {
                    auto dst2 = dst->AsEntrance();
                    auto src2 = src->AsEntrance();

                    dst2->SetEntranceType(src2->GetEntranceType());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());

                    if (src2->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                    {
                        auto pathType = src2->GetPathType();
                        if (pathType == 0)
                        {
                            pathType = RCT1_FOOTPATH_TYPE_TARMAC_GREY;
                        }
                        auto entryIndex = _footpathSurfaceTypeToEntryMap[pathType];
                        dst2->SetSurfaceEntryIndex(entryIndex);
                    }

                    return 1;
                }
                case TileElementType::Wall:
                {
                    auto src2 = src->AsWall();
                    auto slope = src2->GetRCT1Slope();
                    size_t numAddedElements = 0;

                    for (int32_t edge = 0; edge < 4; edge++)
                    {
                        int32_t type = src2->GetRCT1WallType(edge);
                        if (type == -1)
                            continue;

                        colour_t colourA = RCT1::GetColour(src2->GetRCT1WallColour());
                        colour_t colourB = COLOUR_BLACK;
                        colour_t colourC = COLOUR_BLACK;
                        ConvertWall(type, &colourA, &colourB);

                        type = _wallTypeToEntryMap[type];
                        auto baseZ = src->BaseHeight * Limits::CoordsZStep;
                        auto clearanceZ = src->ClearanceHeight * Limits::CoordsZStep;
                        auto edgeSlope = LandSlopeToWallSlope[slope][edge & 3];
                        if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
                        {
                            clearanceZ += LAND_HEIGHT_STEP;
                        }
                        if (edgeSlope & EDGE_SLOPE_ELEVATED)
                        {
                            edgeSlope &= ~EDGE_SLOPE_ELEVATED;
                            baseZ += LAND_HEIGHT_STEP;
                            clearanceZ += LAND_HEIGHT_STEP;
                        }

                        dst->SetType(TileElementType::Wall);
                        dst->SetDirection(edge);
                        dst->SetBaseZ(baseZ);
                        dst->SetClearanceZ(clearanceZ);
                        // Will be set later.
                        dst->SetLastForTile(false);

                        auto* wallElement = dst->AsWall();
                        wallElement->SetEntryIndex(type);
                        wallElement->SetPrimaryColour(colourA);
                        wallElement->SetSecondaryColour(colourB);
                        wallElement->SetTertiaryColour(colourC);
                        wallElement->SetBannerIndex(BannerIndex::GetNull());
                        wallElement->SetAcrossTrack(false);
                        wallElement->SetAnimationIsBackwards(false);
                        wallElement->SetSlope(edgeSlope);

                        dst++;
                        numAddedElements++;
                    }

                    return numAddedElements;
                }
                case TileElementType::LargeScenery:
                {
                    auto dst2 = dst->AsLargeScenery();
                    auto src2 = src->AsLargeScenery();

                    auto type = src2->GetEntryIndex();
                    dst2->SetEntryIndex(_largeSceneryTypeToEntryMap[type]);
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetPrimaryColour(RCT1::GetColour(src2->GetPrimaryColour()));
                    dst2->SetSecondaryColour(RCT1::GetColour(src2->GetSecondaryColour()));

                    return 1;
                }
                case TileElementType::Banner:
                {
                    auto dst2 = dst->AsBanner();
                    auto src2 = src->AsBanner();

                    dst2->SetPosition(src2->GetPosition());
                    dst2->SetAllowedEdges(src2->GetAllowedEdges());

                    auto index = src2->GetIndex();
                    if (index < std::size(_s4.Banners))
                    {
                        auto srcBanner = &_s4.Banners[index];
                        auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(index));
                        if (dstBanner == nullptr)
                        {
                            dst2->SetIndex(BannerIndex::GetNull());
                        }
                        else
                        {
                            ImportBanner(dstBanner, srcBanner);
                            dst2->SetIndex(BannerIndex::FromUnderlying(index));
                        }
                    }
                    else
                    {
                        dst2->SetIndex(BannerIndex::GetNull());
                    }
                    return 1;
                }
                default:
                    assert(false);
            }

            return 0;
        }

        void ImportResearch()
        {
            // All available objects must be loaded before this method is called as it
            // requires them to correctly insert objects into the research list

            ResearchResetItems();

            size_t researchListCount;
            const RCT1::ResearchItem* researchList = GetResearchList(&researchListCount);

            // Initialise the "seen" tables
            _researchRideEntryUsed.reset();
            _researchRideTypeUsed.reset();

            // The first six scenery groups are always available
            for (uint8_t i = 0; i < 6; i++)
            {
                ResearchInsertSceneryGroupEntry(i, true);
            }

            bool researched = true;
            auto rideTypeInResearch = GetRideTypesPresentInResearchList(researchList, researchListCount);
            std::vector<RCT1::ResearchItem> vehiclesWithMissingRideTypes;
            for (size_t i = 0; i < researchListCount; i++)
            {
                const auto& researchItem = researchList[i];
                if (researchItem.Flags == RCT1ResearchFlagsSeparator)
                {
                    if (researchItem.Item == RCT1_RESEARCH_END_AVAILABLE)
                    {
                        researched = false;
                        continue;
                    }
                    // We don't import the random items yet.
                    else if (researchItem.Item == RCT1_RESEARCH_END_RESEARCHABLE || researchItem.Item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                }

                switch (researchItem.Type)
                {
                    case RCT1_RESEARCH_TYPE_THEME:
                    {
                        uint8_t rct1SceneryTheme = researchItem.Item;
                        auto sceneryGroupEntryIndex = _sceneryThemeTypeToEntryMap[rct1SceneryTheme];
                        if (sceneryGroupEntryIndex != ObjectEntryIndexIgnore
                            && sceneryGroupEntryIndex != OBJECT_ENTRY_INDEX_NULL)
                        {
                            ResearchInsertSceneryGroupEntry(sceneryGroupEntryIndex, researched);
                        }
                        break;
                    }
                    case RCT1_RESEARCH_TYPE_RIDE:
                    {
                        const auto rct1RideType = static_cast<RideType>(researchItem.Item);
                        _researchRideTypeUsed[EnumValue(rct1RideType)] = true;

                        auto ownRideEntryIndex = _rideTypeToRideEntryMap[EnumValue(rct1RideType)];
                        Guard::Assert(
                            ownRideEntryIndex != OBJECT_ENTRY_INDEX_NULL, "ownRideEntryIndex was OBJECT_ENTRY_INDEX_NULL");

                        bool foundOwnType = false;
                        // If the ride type does not use vehicles, no point looking for them in the research list.
                        if (RCT1::RideTypeUsesVehicles(rct1RideType))
                        {
                            // Add all vehicles for this ride type that are researched or before this research item
                            for (size_t j = 0; j < researchListCount; j++)
                            {
                                const auto& researchItem2 = researchList[j];
                                if (researchItem2.Flags == RCT1ResearchFlagsSeparator)
                                {
                                    if (researchItem2.Item == RCT1_RESEARCH_END_RESEARCHABLE
                                        || researchItem2.Item == RCT1_RESEARCH_END)
                                    {
                                        break;
                                    }

                                    continue;
                                }

                                if (researchItem2.Type == RCT1_RESEARCH_TYPE_VEHICLE
                                    && static_cast<RideType>(researchItem2.RelatedRide) == rct1RideType)
                                {
                                    auto rideEntryIndex2 = _vehicleTypeToRideEntryMap[researchItem2.Item];
                                    bool isOwnType = (ownRideEntryIndex == rideEntryIndex2);
                                    if (isOwnType)
                                    {
                                        foundOwnType = true;
                                    }

                                    // Only add the vehicles that were listed before this ride, otherwise we might
                                    // change the research order
                                    if (j < i && (researched || isOwnType))
                                    {
                                        InsertResearchVehicle(researchItem2, researched);
                                    }
                                }
                            }
                        }

                        if (!foundOwnType)
                        {
                            if (!_researchRideEntryUsed[ownRideEntryIndex])
                            {
                                _researchRideEntryUsed[ownRideEntryIndex] = true;
                                ResearchInsertRideEntry(ownRideEntryIndex, researched);
                            }
                        }

                        break;
                    }
                    case RCT1_RESEARCH_TYPE_VEHICLE:
                    {
                        // Only add vehicle if the related ride has been seen, this to make sure that vehicles
                        // are researched only after the ride has been researched. Otherwise, remove them from the research
                        // list, so that they are automatically co-invented when their master ride is invented.
                        if (_researchRideTypeUsed[researchItem.RelatedRide])
                        {
                            InsertResearchVehicle(researchItem, researched);
                        }
                        else if (!rideTypeInResearch[researchItem.RelatedRide] && _gameVersion == FILE_VERSION_RCT1_LL)
                        {
                            vehiclesWithMissingRideTypes.push_back(researchItem);
                        }

                        break;
                    }
                    case RCT1_RESEARCH_TYPE_SPECIAL:
                        // Not supported
                        break;
                }
            }
            for (const auto& researchItem : vehiclesWithMissingRideTypes)
            {
                InsertResearchVehicle(researchItem, false);
            }

            // Research funding / priority
            uint8_t activeResearchTypes = 0;
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_ROLLERCOASTERS)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Rollercoaster);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_THRILL_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Thrill);
                activeResearchTypes |= EnumToFlag(ResearchCategory::Water);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Gentle);
                activeResearchTypes |= EnumToFlag(ResearchCategory::Transport);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_SHOPS)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Shop);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_SCENERY_THEMING)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::SceneryGroup);
            }
            gResearchPriorities = activeResearchTypes;
            gResearchFundingLevel = _s4.ResearchLevel;

            // This will mark items as researched/unresearched according to the research list.
            // This needs to be called before importing progress, as it will reset it.
            ResearchResetCurrentItem();

            // Research history
            gResearchProgress = _s4.ResearchProgress;
            gResearchProgressStage = _s4.ResearchProgressStage;
            gResearchExpectedDay = _s4.NextResearchExpectedDay;
            gResearchExpectedMonth = _s4.NextResearchExpectedMonth;

            if (_s4.LastResearchFlags == 0xFF)
            {
                gResearchLastItem = std::nullopt;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.LastResearchItem, _s4.LastResearchType);
                gResearchLastItem = researchItem;
            }

            if (_s4.NextResearchFlags == 0xFF)
            {
                gResearchNextItem = std::nullopt;
                gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                gResearchProgress = 0;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.NextResearchItem, _s4.NextResearchType);
                gResearchNextItem = researchItem;
            }
        }

        static BitSet<EnumValue(RideType::Count)> GetRideTypesPresentInResearchList(
            const RCT1::ResearchItem* researchList, size_t researchListCount)
        {
            BitSet<EnumValue(RideType::Count)> ret = {};

            for (size_t i = 0; i < researchListCount; i++)
            {
                const auto& researchItem = researchList[i];
                if (researchItem.Flags == RCT1ResearchFlagsSeparator)
                {
                    if (researchItem.Item == RCT1_RESEARCH_END_AVAILABLE || researchItem.Item == RCT1_RESEARCH_END_RESEARCHABLE)
                    {
                        continue;
                    }
                    if (researchItem.Item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                }

                if (researchItem.Type == RCT1_RESEARCH_TYPE_RIDE)
                {
                    ret[researchItem.Item] = true;
                }
            }

            return ret;
        }

        void InsertResearchVehicle(const ResearchItem& researchItem, bool researched)
        {
            uint8_t vehicle = researchItem.Item;
            auto rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

            if (!_researchRideEntryUsed[rideEntryIndex])
            {
                _researchRideEntryUsed[rideEntryIndex] = true;
                ResearchInsertRideEntry(rideEntryIndex, researched);
            }
        }

        void ImportParkName()
        {
            std::string parkName = std::string(_s4.ScenarioName);
            if (IsUserStringID(static_cast<StringId>(_s4.ParkNameStringIndex)))
            {
                std::string userString = GetUserString(_s4.ParkNameStringIndex);
                if (!userString.empty())
                {
                    parkName = std::move(userString);
                }
            }

            auto& park = GetContext()->GetGameState()->GetPark();
            park.Name = std::move(parkName);
        }

        void ImportParkFlags()
        {
            // Date and srand
            gCurrentTicks = _s4.Ticks;
            ScenarioRandSeed(_s4.RandomA, _s4.RandomB);
            gDateMonthsElapsed = static_cast<int32_t>(_s4.Month);
            gDateMonthTicks = _s4.Day;

            // Park rating
            gParkRating = _s4.ParkRating;

            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            park.ResetHistories();
            std::copy(std::begin(_s4.ParkRatingHistory), std::end(_s4.ParkRatingHistory), gParkRatingHistory);
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != RCT12ParkHistoryUndefined)
                {
                    gGuestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * RCT12GuestsInParkHistoryFactor;
                }
            }

            // Awards
            auto& awards = GetAwards();
            for (auto& src : _s4.Awards)
            {
                if (src.Time != 0)
                {
                    awards.push_back(Award{ src.Time, static_cast<AwardType>(src.Type) });
                }
            }

            // Number of guests history
            std::fill(std::begin(gGuestsInParkHistory), std::end(gGuestsInParkHistory), std::numeric_limits<uint32_t>::max());
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != std::numeric_limits<uint8_t>::max())
                {
                    gGuestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * 20;
                }
            }

            // News items
            for (size_t i = 0; i < Limits::MaxNewsItems; i++)
            {
                const RCT12NewsItem* src = &_s4.Messages[i];
                News::Item* dst = &gNewsItems[i];

                dst->Type = static_cast<News::ItemType>(src->Type);
                dst->Flags = src->Flags;
                dst->Ticks = src->Ticks;
                dst->MonthYear = src->MonthYear;
                dst->Day = src->Day;
                dst->Text = ConvertFormattedStringToOpenRCT2(std::string_view(src->Text, sizeof(src->Text)));

                if (dst->Type == News::ItemType::Research)
                {
                    uint8_t researchItem = src->Assoc & 0x000000FF;
                    uint8_t researchType = (src->Assoc & 0x00FF0000) >> 16;

                    ::ResearchItem tmpResearchItem = {};
                    ConvertResearchEntry(&tmpResearchItem, researchItem, researchType);
                    dst->Assoc = tmpResearchItem.rawValue;
                }
                else
                {
                    dst->Assoc = src->Assoc;
                }
            }

            // Initial guest status
            gGuestInitialCash = ToMoney64(_s4.GuestInitialCash);
            gGuestInitialHunger = _s4.GuestInitialHunger;
            gGuestInitialThirst = _s4.GuestInitialThirst;
            gGuestInitialHappiness = _s4.GuestInitialHappiness;

            _guestGenerationProbability = _s4.GuestGenerationProbability;

            // Staff colours
            gStaffHandymanColour = RCT1::GetColour(_s4.HandymanColour);
            gStaffMechanicColour = RCT1::GetColour(_s4.MechanicColour);
            gStaffSecurityColour = RCT1::GetColour(_s4.SecurityGuardColour);

            // Flags
            gParkFlags = _s4.ParkFlags;
            gParkFlags &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;
            gParkFlags |= PARK_FLAGS_RCT1_INTEREST;
            // Loopy Landscape parks can set a flag to lock the entry price to free.
            // If this flag is not set, the player can ask money for both rides and entry.
            if (!(_s4.ParkFlags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
            {
                gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
            }

            gParkSize = _s4.ParkSize;
            gTotalRideValueForMoney = _s4.TotalRideValueForMoney;
            gSamePriceThroughoutPark = 0;
            if (_gameVersion == FILE_VERSION_RCT1_LL)
            {
                gSamePriceThroughoutPark = _s4.SamePriceThroughout;
            }
        }

        void ConvertResearchEntry(::ResearchItem* dst, uint8_t srcItem, uint8_t srcType)
        {
            dst->SetNull();
            if (srcType == RCT1_RESEARCH_TYPE_RIDE)
            {
                auto entryIndex = _rideTypeToRideEntryMap[srcItem];

                if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
                {
                    const auto* rideEntry = GetRideEntryByIndex(entryIndex);

                    if (rideEntry != nullptr)
                    {
                        auto rideType = rideEntry->GetFirstNonNullRideType();
                        dst->entryIndex = entryIndex;
                        dst->baseRideType = rideType;
                        dst->type = Research::EntryType::Ride;
                        dst->flags = 0;
                        dst->category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                    }
                }
            }
            else if (srcType == RCT1_RESEARCH_TYPE_VEHICLE)
            {
                auto entryIndex = _vehicleTypeToRideEntryMap[srcItem];

                if (entryIndex != OBJECT_ENTRY_INDEX_NULL)
                {
                    const auto* rideEntry = GetRideEntryByIndex(entryIndex);

                    if (rideEntry != nullptr)
                    {
                        auto rideType = rideEntry->GetFirstNonNullRideType();
                        dst->entryIndex = entryIndex;
                        dst->baseRideType = rideType;
                        dst->type = Research::EntryType::Ride;
                        dst->flags = 0;
                        dst->category = GetRideTypeDescriptor(rideType).GetResearchCategory();
                    }
                }
            }
            else if (srcType == RCT1_RESEARCH_TYPE_THEME)
            {
                auto entryIndex = _sceneryThemeTypeToEntryMap[srcItem];

                if (entryIndex != ObjectEntryIndexIgnore && entryIndex != OBJECT_ENTRY_INDEX_NULL)
                {
                    dst->entryIndex = entryIndex;
                    dst->type = Research::EntryType::Scenery;
                    dst->category = ResearchCategory::SceneryGroup;
                    dst->baseRideType = 0;
                    dst->flags = 0;
                }
            }
        }

        void ImportClimate()
        {
            gClimate = ClimateType{ _s4.Climate };
            gClimateUpdateTimer = _s4.ClimateTimer;
            gClimateCurrent.Temperature = _s4.Temperature;
            gClimateCurrent.Weather = WeatherType{ _s4.Weather };
            gClimateCurrent.WeatherEffect = WeatherEffectType::None;
            gClimateCurrent.WeatherGloom = _s4.WeatherGloom;
            gClimateCurrent.Level = static_cast<WeatherLevel>(_s4.Rain);
            gClimateNext.Temperature = _s4.TargetTemperature;
            gClimateNext.Weather = WeatherType{ _s4.TargetWeather };
            gClimateNext.WeatherEffect = WeatherEffectType::None;
            gClimateNext.WeatherGloom = _s4.TargetWeatherGloom;
            gClimateNext.Level = static_cast<WeatherLevel>(_s4.TargetRain);
        }

        void ImportScenarioNameDetails()
        {
            std::string name = String::ToStd(_s4.ScenarioName);
            std::string details;

            int32_t scNumber = _s4.ScenarioSlotIndex;
            if (scNumber != -1)
            {
                SourceDescriptor sourceDesc;
                if (ScenarioSources::TryGetById(scNumber, &sourceDesc))
                {
                    StringId localisedStringIds[3];
                    if (LanguageGetLocalisedScenarioStrings(sourceDesc.title, localisedStringIds))
                    {
                        if (localisedStringIds[0] != STR_NONE)
                        {
                            name = String::ToStd(LanguageGetString(localisedStringIds[0]));
                        }
                        if (localisedStringIds[2] != STR_NONE)
                        {
                            details = String::ToStd(LanguageGetString(localisedStringIds[2]));
                        }
                    }
                }
            }

            gScenarioName = std::move(name);
            gScenarioDetails = std::move(details);
        }

        void ImportScenarioObjective()
        {
            gScenarioObjective.Type = _s4.ScenarioObjectiveType;
            gScenarioObjective.Year = _s4.ScenarioObjectiveYears;
            gScenarioObjective.NumGuests = _s4.ScenarioObjectiveNumGuests;

            // RCT1 used a different way of calculating the park value.
            // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
            // we need to check the goal to avoid affecting scenarios like Volcania.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_PARK_VALUE_BY)
                gScenarioObjective.Currency = CorrectRCT1ParkValue(_s4.ScenarioObjectiveCurrency);
            else
                gScenarioObjective.Currency = ToMoney64(_s4.ScenarioObjectiveCurrency);

            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST)
                gScenarioObjective.RideId = GetBuildTheBestRideId();
        }

        void ImportSavedView()
        {
            gSavedView = ScreenCoordsXY{ _s4.ViewX, _s4.ViewY };
            gSavedViewZoom = ZoomLevel{ static_cast<int8_t>(_s4.ViewZoom) };
            gSavedViewRotation = _s4.ViewRotation;
        }

        void ConvertWall(const int32_t& type, colour_t* colourA, colour_t* colourB)
        {
            switch (type)
            {
                case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE:
                    *colourA = COLOUR_DARK_BROWN;
                    break;
                case RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE:
                    *colourA = COLOUR_WHITE;
                    break;
                case RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE:
                    *colourA = COLOUR_SALMON_PINK;
                    break;
                case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW:
                    *colourA = COLOUR_DARK_BROWN;
                    break;
                case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_GATE:
                    *colourB = *colourA;
                    *colourA = COLOUR_DARK_BROWN;
                    break;
                case RCT1_WALL_TYPE_GLASS_SMOOTH:
                case RCT1_WALL_TYPE_GLASS_PANELS:
                    *colourB = COLOUR_WHITE;
                    break;
                case RCT1_WALL_TYPE_SMALL_GREY_CASTLE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW:
                case RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE:
                    *colourA = COLOUR_GREY;
                    break;
            }
        }

        void ImportBanner(Banner* dst, const RCT12Banner* src)
        {
            auto id = dst->id;

            *dst = {};
            dst->id = id;
            dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->Type);

            dst->flags = 0;
            if (src->Flags & BANNER_FLAG_NO_ENTRY)
            {
                dst->flags |= BANNER_FLAG_NO_ENTRY;
            }

            if (IsUserStringID(src->StringID))
            {
                dst->text = GetUserString(src->StringID);
            }

            dst->colour = RCT1::GetColour(src->Colour);
            dst->text_colour = src->TextColour;
            dst->position.x = src->x;
            dst->position.y = src->y;
        }

        void FixEntrancePositions()
        {
            gParkEntrances.clear();
            TileElementIterator it;
            TileElementIteratorBegin(&it);
            while (TileElementIteratorNext(&it) && gParkEntrances.size() < Limits::MaxParkEntrances)
            {
                TileElement* element = it.element;

                if (element->GetType() != TileElementType::Entrance)
                    continue;
                if (element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    continue;
                if ((element->AsEntrance()->GetSequenceIndex()) != 0)
                    continue;

                CoordsXYZD entrance = { TileCoordsXY(it.x, it.y).ToCoordsXY(), element->GetBaseZ(), element->GetDirection() };
                gParkEntrances.push_back(entrance);
            }
        }

        RCT12::EntryList* GetEntryList(ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectType::Ride:
                    return &_rideEntries;
                case ObjectType::SmallScenery:
                    return &_smallSceneryEntries;
                case ObjectType::LargeScenery:
                    return &_largeSceneryEntries;
                case ObjectType::Walls:
                    return &_wallEntries;
                case ObjectType::Paths:
                    return &_pathEntries;
                case ObjectType::PathBits:
                    return &_pathAdditionEntries;
                case ObjectType::SceneryGroup:
                    return &_sceneryGroupEntries;
                case ObjectType::Water:
                    return &_waterEntry;
                default:
                    // This switch processes only ObjectType for for Entries
                    break;
            }
            return nullptr;
        }

        const RCT1::ResearchItem* GetResearchList(size_t* count)
        {
            // Loopy Landscapes stores research items in a different place
            if (_gameVersion == FILE_VERSION_RCT1_LL)
            {
                *count = std::size(_s4.ResearchItemsLL);
                return _s4.ResearchItemsLL;
            }

            *count = std::size(_s4.ResearchItems);
            return _s4.ResearchItems;
        }

        std::string GetUserString(StringId stringId)
        {
            const auto originalString = _s4.StringTable[(stringId - USER_STRING_START) % 1024];
            auto originalStringView = std::string_view(
                originalString, RCT2::GetRCT2StringBufferLen(originalString, USER_STRING_MAX_LENGTH));
            auto asUtf8 = RCT2StringToUTF8(originalStringView, RCT2LanguageId::EnglishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        void FixLandOwnership()
        {
            switch (_s4.ScenarioSlotIndex)
            {
                case SC_DYNAMITE_DUNES:
                    FixLandOwnershipTiles({ { 97, 18 }, { 99, 19 }, { 83, 34 } });
                    break;
                case SC_LEAFY_LAKE:
                    FixLandOwnershipTiles({ { 49, 66 } });
                    break;
                case SC_KATIES_DREAMLAND:
                    FixLandOwnershipTiles({ { 74, 70 }, { 75, 70 }, { 76, 70 }, { 77, 73 }, { 80, 77 } });
                    break;
                case SC_POKEY_PARK:
                    FixLandOwnershipTiles({ { 64, 102 } });
                    break;
                case SC_MYSTIC_MOUNTAIN:
                    FixLandOwnershipTiles({ { 98, 69 }, { 98, 70 }, { 103, 64 }, { 53, 79 }, { 86, 93 }, { 87, 93 } });
                    break;
                case SC_PACIFIC_PYRAMIDS:
                    FixLandOwnershipTiles({ { 93, 105 }, { 63, 34 }, { 76, 25 }, { 85, 31 }, { 96, 47 }, { 96, 48 } });
                    break;
                case SC_UTOPIA_PARK:
                    FixLandOwnershipTiles({ { 85, 73 } });
                    break;
                case SC_URBAN_PARK:
                    FixLandOwnershipTiles({ { 64, 77 }, { 61, 66 }, { 61, 67 }, { 39, 20 } });
                    break;
            }
        }

        /**
         * In Urban Park, the entrance and exit of the merry-go-round are the wrong way round. This code fixes that.
         * To avoid messing up saves (in which this problem is most likely solved by the user), only carry out this
         * fix when loading from a scenario.
         */
        void FixUrbanPark()
        {
            if (_s4.ScenarioSlotIndex == SC_URBAN_PARK && _isScenario)
            {
                const auto merryGoRoundId = RideId::FromUnderlying(0);

                // First, make the queuing peep exit
                for (auto peep : EntityList<Guest>())
                {
                    if (peep->State == PeepState::QueuingFront && peep->CurrentRide == merryGoRoundId)
                    {
                        peep->RemoveFromQueue();
                        peep->SetState(PeepState::Falling);
                        break;
                    }
                }

                // Now, swap the entrance and exit.
                auto ride = GetRide(merryGoRoundId);
                if (ride != nullptr)
                {
                    auto& station = ride->GetStation();
                    auto entranceCoords = station.Exit;
                    auto exitCoords = station.Entrance;
                    station.Entrance = entranceCoords;
                    station.Exit = exitCoords;

                    auto entranceElement = MapGetRideExitElementAt(entranceCoords.ToCoordsXYZD(), false);
                    entranceElement->SetEntranceType(ENTRANCE_TYPE_RIDE_ENTRANCE);
                    auto exitElement = MapGetRideEntranceElementAt(exitCoords.ToCoordsXYZD(), false);
                    exitElement->SetEntranceType(ENTRANCE_TYPE_RIDE_EXIT);

                    // Trigger footpath update
                    FootpathQueueChainReset();
                    FootpathConnectEdges(
                        entranceCoords.ToCoordsXY(), reinterpret_cast<TileElement*>(entranceElement),
                        GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
                    FootpathUpdateQueueChains();
                }
            }
        }

        /**
         * Counts the block sections. The reason this iterates over the map is to avoid getting into infinite loops,
         * which can happen with hacked parks.
         */
        void CountBlockSections()
        {
            for (int32_t x = 0; x < Limits::MaxMapSize; x++)
            {
                for (int32_t y = 0; y < Limits::MaxMapSize; y++)
                {
                    TileElement* tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });
                    if (tileElement == nullptr)
                        continue;
                    do
                    {
                        if (tileElement->GetType() == TileElementType::Track)
                        {
                            // Lift hill tops are the only pieces present in RCT1 that can count as a block brake.
                            if (!tileElement->AsTrack()->HasChain())
                                continue;

                            auto trackType = tileElement->AsTrack()->GetTrackType();
                            switch (trackType)
                            {
                                case TrackElemType::Up25ToFlat:
                                case TrackElemType::Up60ToFlat:
                                case TrackElemType::DiagUp25ToFlat:
                                case TrackElemType::DiagUp60ToFlat:
                                    break;
                                default:
                                    continue;
                            }

                            RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
                            auto ride = GetRide(rideIndex);
                            if (ride != nullptr)
                            {
                                ride->num_block_brakes++;
                            }
                        }
                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }

        /**
         * This has to be done after importing tile elements, because it needs those to detect if a pre-existing ride
         * name should be considered reserved.
         */
        void SetDefaultNames()
        {
            for (auto& ride : GetRideManager())
            {
                if (ride.custom_name.empty())
                {
                    ride.SetNameToDefault();
                }
            }
        }

        ObjectEntryIndex GetBuildTheBestRideId()
        {
            size_t researchListCount;
            const RCT1::ResearchItem* researchList = GetResearchList(&researchListCount);
            for (size_t i = 0; i < researchListCount; i++)
            {
                if (researchList[i].Flags == 0xFF)
                {
                    break;
                }

                if (researchList[i].Type == RCT1_RESEARCH_TYPE_RIDE)
                {
                    return RCT1::GetRideType(static_cast<RideType>(researchList[i].Item), 0);
                }
            }

            return RIDE_TYPE_NULL;
        }
    };

    // Very similar but not the same as S6Importer version (due to peeps)
    constexpr EntityType GetEntityTypeFromRCT1Sprite(const RCT12EntityBase& src)
    {
        EntityType output = EntityType::Null;
        switch (src.EntityIdentifier)
        {
            case RCT12EntityIdentifier::Vehicle:
                output = EntityType::Vehicle;
                break;
            case RCT12EntityIdentifier::Peep:
            {
                const auto& peep = static_cast<const RCT1::Peep&>(src);
                if (peep.PeepType == RCT12PeepType::Guest)
                {
                    output = EntityType::Guest;
                }
                else
                {
                    output = EntityType::Staff;
                }
                break;
            }
            case RCT12EntityIdentifier::Misc:

                switch (RCT12MiscEntityType(src.Type))
                {
                    case RCT12MiscEntityType::SteamParticle:
                        output = EntityType::SteamParticle;
                        break;
                    case RCT12MiscEntityType::MoneyEffect:
                        output = EntityType::MoneyEffect;
                        break;
                    case RCT12MiscEntityType::CrashedVehicleParticle:
                        output = EntityType::CrashedVehicleParticle;
                        break;
                    case RCT12MiscEntityType::ExplosionCloud:
                        output = EntityType::ExplosionCloud;
                        break;
                    case RCT12MiscEntityType::CrashSplash:
                        output = EntityType::CrashSplash;
                        break;
                    case RCT12MiscEntityType::ExplosionFlare:
                        output = EntityType::ExplosionFlare;
                        break;
                    case RCT12MiscEntityType::JumpingFountainWater:
                    case RCT12MiscEntityType::JumpingFountainSnow:
                        output = EntityType::JumpingFountain;
                        break;
                    case RCT12MiscEntityType::Balloon:
                        output = EntityType::Balloon;
                        break;
                    case RCT12MiscEntityType::Duck:
                        output = EntityType::Duck;
                        break;
                    default:
                        break;
                }
                break;
            case RCT12EntityIdentifier::Litter:
                output = EntityType::Litter;
                break;
            default:
                break;
        }
        return output;
    }

    template<> void S4Importer::ImportEntity<::Vehicle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT1::Vehicle*>(&srcBase);
        const auto* ride = GetRide(RideId::FromUnderlying(src->Ride));
        if (ride == nullptr)
            return;

        const auto& rct1Ride = _s4.Rides[src->Ride];
        uint8_t vehicleEntryIndex = RCT1::GetVehicleSubEntryIndex(rct1Ride.VehicleType, src->CarType);

        dst->ride = RideId::FromUnderlying(src->Ride);
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(ride->subtype);

        dst->vehicle_type = vehicleEntryIndex;
        dst->SubType = ::Vehicle::Type(src->Type);
        dst->var_44 = src->Var44;
        dst->remaining_distance = src->RemainingDistance;

        // Properties from vehicle entry
        dst->sprite_width = src->SpriteWidth;
        dst->sprite_height_negative = src->SpriteHeightNegative;
        dst->sprite_height_positive = src->SpriteHeightPositive;
        dst->sprite_direction = src->EntityDirection;

        dst->SpriteRect = ScreenRect(src->SpriteLeft, src->SpriteTop, src->SpriteRight, src->SpriteBottom);

        dst->mass = src->Mass;
        dst->num_seats = src->NumSeats;
        dst->speed = src->Speed;
        dst->powered_acceleration = src->PoweredAcceleration;
        dst->brake_speed = src->BrakeSpeed;

        dst->velocity = src->Velocity;
        dst->acceleration = src->Acceleration;
        dst->SwingSprite = src->SwingSprite;
        dst->SwingPosition = src->SwingPosition;
        dst->SwingSpeed = src->SwingSpeed;
        dst->restraints_position = src->RestraintsPosition;
        dst->spin_sprite = src->SpinSprite;
        dst->sound_vector_factor = src->SoundVectorFactor;
        dst->spin_speed = src->SpinSpeed;
        dst->sound2_flags = src->Sound2Flags;
        dst->sound1_id = OpenRCT2::Audio::SoundId::Null;
        dst->sound2_id = OpenRCT2::Audio::SoundId::Null;
        dst->var_C0 = src->VarC0;
        dst->CollisionDetectionTimer = src->CollisionDetectionTimer;
        dst->animation_frame = src->AnimationFrame;
        dst->animationState = src->AnimationState;
        dst->NumLaps = src->NumLaps;
        dst->var_D3 = src->VarD3;
        dst->scream_sound_id = OpenRCT2::Audio::SoundId::Null;
        dst->Pitch = src->Pitch;
        dst->bank_rotation = src->BankRotation;

        // Seat rotation was not in RCT1
        dst->target_seat_rotation = DEFAULT_SEAT_ROTATION;
        dst->seat_rotation = DEFAULT_SEAT_ROTATION;

        // Vehicle links (indexes converted later)
        dst->prev_vehicle_on_ride = EntityId::FromUnderlying(src->PrevVehicleOnRide);
        dst->next_vehicle_on_ride = EntityId::FromUnderlying(src->NextVehicleOnRide);
        dst->next_vehicle_on_train = EntityId::FromUnderlying(src->NextVehicleOnTrain);

        // Guests (indexes converted later)
        for (int i = 0; i < 32; i++)
        {
            const auto spriteIndex = EntityId::FromUnderlying(src->Peep[i]);
            dst->peep[i] = spriteIndex;
            if (!spriteIndex.IsNull())
            {
                dst->peep_tshirt_colours[i] = RCT1::GetColour(src->PeepTshirtColours[i]);
            }
        }

        ::Vehicle::Status statusSrc = ::Vehicle::Status::MovingToEndOfStation;
        if (src->Status <= static_cast<uint8_t>(::Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<::Vehicle::Status>(src->Status);
        }
        dst->status = statusSrc;
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->TrackLocation = { src->TrackX, src->TrackY, src->TrackZ };
        dst->current_station = StationIndex::FromUnderlying(src->CurrentStation);
        if (src->BoatLocation.IsNull() || ride->mode != RideMode::BoatHire || statusSrc != ::Vehicle::Status::TravellingBoat)
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            dst->SetTrackType(RCT1TrackTypeToOpenRCT2(src->GetTrackType(), ride->type));
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->BoatLocation.x, src->BoatLocation.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(0);
        }
        dst->track_progress = src->TrackProgress;
        dst->vertical_drop_countdown = src->VerticalDropCountdown;
        dst->sub_state = src->SubState;
        dst->Flags = src->UpdateFlags;

        SetVehicleColours(dst, src);

        dst->mini_golf_current_animation = MiniGolfAnimation(src->MiniGolfCurrentAnimation);
        dst->mini_golf_flags = src->MiniGolfFlags;

        dst->MoveTo({ src->x, src->y, src->z });

        dst->num_peeps = src->NumPeeps;
        dst->next_free_seat = src->NextFreeSeat;
        if (src->Flags & RCT12_ENTITY_FLAGS_IS_CRASHED_VEHICLE_ENTITY)
        {
            dst->SetFlag(VehicleFlags::Crashed);
        }
    }

    template<> void S4Importer::ImportEntity<Guest>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Guest>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT1::Peep*>(&srcBase);
        ImportPeep(dst, src);

        dst->OutsideOfPark = static_cast<bool>(src->OutsideOfPark);
        dst->TimeToConsume = src->TimeToConsume;
        dst->VandalismSeen = src->VandalismSeen;
        dst->UmbrellaColour = RCT1::GetColour(src->UmbrellaColour);
        dst->HatColour = RCT1::GetColour(src->HatColour);

        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->BalloonColour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->BalloonColour = RCT1::GetColour(src->BalloonColour);
        }
        dst->Happiness = src->Happiness;
        dst->HappinessTarget = src->HappinessTarget;
        dst->Nausea = src->Nausea;
        dst->NauseaTarget = src->NauseaTarget;
        dst->Hunger = src->Hunger;
        dst->Thirst = src->Thirst;
        dst->Toilet = src->Toilet;
        dst->LitterCount = src->LitterCount;
        dst->DisgustingCount = src->DisgustingCount;
        dst->Intensity = static_cast<IntensityRange>(src->Intensity);
        dst->NauseaTolerance = static_cast<PeepNauseaTolerance>(src->NauseaTolerance);
        dst->GuestTimeOnRide = src->TimeOnRide;
        dst->DaysInQueue = src->DaysInQueue;
        dst->CashInPocket = src->CashInPocket;
        dst->CashSpent = src->CashSpent;
        dst->ParkEntryTime = src->ParkEntryTime;
        dst->GuestNumRides = src->NumRides;
        dst->AmountOfDrinks = src->NumDrinks;
        dst->AmountOfFood = src->NumFood;
        dst->AmountOfSouvenirs = src->NumSouvenirs;
        dst->PaidToEnter = src->PaidToEnter;
        dst->PaidOnRides = src->PaidOnRides;
        dst->PaidOnDrink = src->PaidOnDrink;
        dst->PaidOnFood = src->PaidOnFood;
        dst->PaidOnSouvenirs = src->PaidOnSouvenirs;
        dst->VoucherRideId = RCT12RideIdToOpenRCT2RideId(src->VoucherArguments);
        dst->VoucherType = src->VoucherType;
        dst->SurroundingsThoughtTimeout = src->SurroundingsThoughtTimeout;
        dst->Angriness = src->Angriness;
        dst->TimeLost = src->TimeLost;

        OpenRCT2::RideUse::GetHistory().Set(dst->Id, RCT12GetRidesBeenOn(src));
        OpenRCT2::RideUse::GetTypeHistory().Set(dst->Id, RCT12GetRideTypesBeenOn(src));

        dst->Photo1RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo1RideRef);

        for (size_t i = 0; i < std::size(src->Thoughts); i++)
        {
            auto srcThought = &src->Thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->Type);
            if (srcThought->Item == RCT12PeepThoughtItemNone)
                dstThought->item = PeepThoughtItemNone;
            else
                dstThought->item = srcThought->Item;
            dstThought->freshness = srcThought->Freshness;
            dstThought->fresh_timeout = srcThought->FreshTimeout;
        }

        dst->PreviousRide = RCT12RideIdToOpenRCT2RideId(src->PreviousRide);
        dst->PreviousRideTimeOut = src->PreviousRideTimeOut;
        dst->GuestHeadingToRideId = RCT12RideIdToOpenRCT2RideId(src->GuestHeadingToRideID);
        dst->GuestIsLostCountdown = src->PeepIsLostCountdown;
        dst->GuestNextInQueue = EntityId::FromUnderlying(src->NextInQueue);
        // Guests' favourite ride was only saved in LL.
        // Set it to N/A if the save comes from the original or AA.
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            dst->FavouriteRide = RCT12RideIdToOpenRCT2RideId(src->FavouriteRide);
            dst->FavouriteRideRating = src->FavouriteRideRating;
        }
        else
        {
            dst->FavouriteRide = RideId::GetNull();
            dst->FavouriteRideRating = 0;
        }

        dst->SetItemFlags(src->GetItemFlags());

        if (dst->OutsideOfPark && dst->State != PeepState::LeavingPark)
        {
            IncrementGuestsHeadingForPark();
        }
        else
        {
            IncrementGuestsInPark();
        }
    }

    template<> void S4Importer::ImportEntity<Staff>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Staff>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT1::Peep*>(&srcBase);
        ImportPeep(dst, src);
        dst->AssignedStaffType = StaffType(src->StaffType);
        dst->MechanicTimeSinceCall = src->MechanicTimeSinceCall;
        dst->HireDate = src->ParkEntryTime;
        dst->StaffOrders = src->StaffOrders;
        dst->StaffMowingTimeout = src->StaffMowingTimeout;
        dst->StaffLawnsMown = src->PaidToEnter;
        dst->StaffGardensWatered = src->PaidOnRides;
        dst->StaffLitterSwept = src->PaidOnFood;
        dst->StaffBinsEmptied = src->PaidOnSouvenirs;

        ImportStaffPatrolArea(dst, src->StaffID);
    }

    template<> void S4Importer::ImportEntity<Litter>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Litter>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityLitter*>(&srcBase);
        ImportEntityCommonProperties(dst, src);

        dst->SubType = Litter::Type(src->Type);
        dst->creationTick = src->CreationTick;
    }

    template<> void S4Importer::ImportEntity<SteamParticle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<SteamParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntitySteamParticle*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_move = src->TimeToMove;
    }

    template<> void S4Importer::ImportEntity<MoneyEffect>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<MoneyEffect>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityMoneyEffect*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->MoveDelay;
        dst->NumMovements = src->NumMovements;
        dst->Vertical = src->Vertical;
        dst->Value = src->Value;
        dst->OffsetX = src->OffsetX;
        dst->Wiggle = src->Wiggle;
    }

    template<> void S4Importer::ImportEntity<VehicleCrashParticle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<VehicleCrashParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityCrashedVehicleParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_live = src->TimeToLive;
        dst->colour[0] = RCT1::GetColour(src->Colour[0]);
        dst->colour[1] = RCT1::GetColour(src->Colour[1]);
        dst->crashed_sprite_base = src->CrashedEntityBase;
        dst->velocity_x = src->VelocityX;
        dst->velocity_y = src->VelocityY;
        dst->velocity_z = src->VelocityZ;
        dst->acceleration_x = src->AccelerationX;
        dst->acceleration_y = src->AccelerationY;
        dst->acceleration_z = src->AccelerationZ;
    }

    template<> void S4Importer::ImportEntity<ExplosionCloud>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionCloud>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S4Importer::ImportEntity<ExplosionFlare>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionFlare>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S4Importer::ImportEntity<CrashSplashParticle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<CrashSplashParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S4Importer::ImportEntity<JumpingFountain>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<JumpingFountain>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityJumpingFountain*>(&srcBase);

        auto fountainType = JumpingFountainType::Water;
        if (RCT12MiscEntityType(src->Type) == RCT12MiscEntityType::JumpingFountainSnow)
            fountainType = JumpingFountainType::Snow;

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->FountainType = fountainType;
        dst->NumTicksAlive = src->NumTicksAlive;
        dst->FountainFlags = src->FountainFlags;
        dst->TargetX = src->TargetX;
        dst->TargetY = src->TargetY;
        dst->Iteration = src->Iteration;
    }

    template<> void S4Importer::ImportEntity<Balloon>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Balloon>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityBalloon*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->popped = src->Popped;
        dst->time_to_move = src->TimeToMove;
        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->colour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->colour = RCT1::GetColour(src->Colour);
        }
    }

    template<> void S4Importer::ImportEntity<Duck>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Duck>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityDuck*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->target_x = src->TargetX;
        dst->target_y = src->TargetY;
        dst->state = static_cast<Duck::DuckState>(src->State);
    }

    void S4Importer::ImportEntity(const RCT12EntityBase& src)
    {
        switch (GetEntityTypeFromRCT1Sprite(src))
        {
            case EntityType::Vehicle:
                ImportEntity<::Vehicle>(src);
                break;
            case EntityType::Guest:
                ImportEntity<Guest>(src);
                break;
            case EntityType::Staff:
                ImportEntity<Staff>(src);
                break;
            case EntityType::SteamParticle:
                ImportEntity<SteamParticle>(src);
                break;
            case EntityType::MoneyEffect:
                ImportEntity<MoneyEffect>(src);
                break;
            case EntityType::CrashedVehicleParticle:
                ImportEntity<VehicleCrashParticle>(src);
                break;
            case EntityType::ExplosionCloud:
                ImportEntity<ExplosionCloud>(src);
                break;
            case EntityType::ExplosionFlare:
                ImportEntity<ExplosionFlare>(src);
                break;
            case EntityType::CrashSplash:
                ImportEntity<CrashSplashParticle>(src);
                break;
            case EntityType::JumpingFountain:
                ImportEntity<JumpingFountain>(src);
                break;
            case EntityType::Balloon:
                ImportEntity<Balloon>(src);
                break;
            case EntityType::Duck:
                ImportEntity<Duck>(src);
                break;
            case EntityType::Litter:
                ImportEntity<Litter>(src);
                break;
            default:
                // Null elements do not need imported
                break;
        }
    }
} // namespace RCT1

std::unique_ptr<IParkImporter> ParkImporter::CreateS4()
{
    return std::make_unique<RCT1::S4Importer>();
}
