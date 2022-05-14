/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../world/LargeScenery.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TilePointerIndex.hpp"
#include "../world/Wall.h"
#include "RCT1.h"
#include "Tables.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

static constexpr const ObjectEntryIndex OBJECT_ENTRY_INDEX_IGNORE = 254;

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
        ParkLoadResult Load(const utf8* path) override
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

        ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, false, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, true, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadFromStream(
            IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck, const utf8* path) override
        {
            _s4 = *ReadAndDecodeS4(stream, isScenario);
            _s4Path = path;
            _isScenario = isScenario;
            _gameVersion = sawyercoding_detect_rct1_version(_s4.game_version) & FILE_VERSION_MASK;

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
            determine_ride_entrance_and_exit_locations();

            map_count_remaining_land_rights();
            research_determine_first_of_type();

            CheatsReset();
            ClearRestrictedScenery();
            RestrictAllMiscScenery();
        }

        bool GetDetails(scenario_index_entry* dst) override
        {
            *dst = {};

            source_desc desc;
            // If no entry is found, this is a custom scenario.
            bool isOfficial = ScenarioSources::TryGetById(_s4.scenario_slot_index, &desc);

            dst->category = desc.category;
            dst->source_game = ScenarioSource{ desc.source };
            dst->source_index = desc.index;
            dst->sc_id = desc.id;

            dst->objective_type = _s4.scenario_objective_type;
            dst->objective_arg_1 = _s4.scenario_objective_years;
            // RCT1 used another way of calculating park value.
            if (_s4.scenario_objective_type == OBJECTIVE_PARK_VALUE_BY)
                dst->objective_arg_2 = CorrectRCT1ParkValue(_s4.scenario_objective_currency);
            else
                dst->objective_arg_2 = _s4.scenario_objective_currency;
            dst->objective_arg_3 = _s4.scenario_objective_num_guests;
            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.scenario_objective_type == OBJECTIVE_BUILD_THE_BEST)
            {
                dst->objective_arg_3 = GetBuildTheBestRideId();
            }

            auto name = rct2_to_utf8(_s4.scenario_name, RCT2LanguageId::EnglishUK);
            std::string details;

            // TryGetById won't set this property if the scenario is not recognised,
            // but localisation needs it.
            if (!isOfficial)
            {
                desc.title = name.c_str();
            }

            String::Set(dst->internal_name, sizeof(dst->internal_name), desc.title);

            rct_string_id localisedStringIds[3];
            if (language_get_localised_scenario_strings(desc.title, localisedStringIds))
            {
                if (localisedStringIds[0] != STR_NONE)
                {
                    name = String::ToStd(language_get_string(localisedStringIds[0]));
                }
                if (localisedStringIds[2] != STR_NONE)
                {
                    details = String::ToStd(language_get_string(localisedStringIds[2]));
                }
            }

            String::Set(dst->name, sizeof(dst->name), name.c_str());
            String::Set(dst->details, sizeof(dst->details), details.c_str());

            return true;
        }

        money32 CorrectRCT1ParkValue(money32 oldParkValue)
        {
            if (oldParkValue == MONEY32_UNDEFINED)
            {
                return MONEY32_UNDEFINED;
            }

            if (_parkValueConversionFactor == 0)
            {
                if (_s4.park_value != 0)
                {
                    // Use the ratio between the old and new park value to calcute the ratio to
                    // use for the park value history and the goal.
                    auto& park = GetContext()->GetGameState()->GetPark();
                    _parkValueConversionFactor = (park.CalculateParkValue() * 10) / _s4.park_value;
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
            int32_t fileType = sawyercoding_detect_file_type(data.get(), dataSize);
            if (isScenario && (fileType & FILE_VERSION_MASK) != FILE_VERSION_RCT1)
            {
                decodedSize = sawyercoding_decode_sc4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }
            else
            {
                decodedSize = sawyercoding_decode_sv4(data.get(), decodedData.get(), dataSize, sizeof(S4));
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

            uint16_t mapSize = _s4.map_size == 0 ? Limits::MaxMapSize : _s4.map_size;

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
            const scenario_index_entry* scenarioEntry = _scenarioRepository->GetByInternalName(_s4.scenario_name);
            if (scenarioEntry == nullptr)
            {
                return "";
            }

            return Path::GetFileName(scenarioEntry->path);
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

                if (researchItem->flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                {
                    if (researchItem->item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                    if (researchItem->item == RCT1_RESEARCH_END_AVAILABLE
                        || researchItem->item == RCT1_RESEARCH_END_RESEARCHABLE)
                    {
                        continue;
                    }
                }

                switch (researchItem->type)
                {
                    case RCT1_RESEARCH_TYPE_THEME:
                        AddEntriesForSceneryTheme(researchItem->item);
                        break;
                    case RCT1_RESEARCH_TYPE_RIDE:
                        AddEntryForRideType(static_cast<RideType>(researchItem->item));
                        break;
                    case RCT1_RESEARCH_TYPE_VEHICLE:
                        // For some bizarre reason, RCT1 research lists contain vehicles that aren't actually researched.
                        // Extra bizarrely, this does not seem to apply to Loopy Landscapes saves/scenarios.
                        if (rideTypeInResearch[researchItem->related_ride] || _gameVersion == FILE_VERSION_RCT1_LL)
                        {
                            AddEntryForVehicleType(static_cast<RideType>(researchItem->related_ride), researchItem->item);
                        }
                        break;
                }
            }
        }

        void AddAvailableEntriesFromMap()
        {
            size_t maxTiles = Limits::MaxMapSize * Limits::MaxMapSize;
            size_t tileIndex = 0;
            RCT12TileElement* tileElement = _s4.tile_elements;

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
            for (size_t i = 0; i < std::size(_s4.rides); i++)
            {
                auto ride = &_s4.rides[i];
                if (ride->type != RideType::Null)
                {
                    if (RCT1::RideTypeUsesVehicles(ride->type))
                        AddEntryForVehicleType(ride->type, ride->vehicle_type);
                    else
                        AddEntryForRideType(ride->type);
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
                        log_error("Cannot find object %s", objectName);
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
                entryName = RCT1::GetWaterObject(_s4.water_colour);
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
                _sceneryThemeTypeToEntryMap[sceneryThemeType] = OBJECT_ENTRY_INDEX_IGNORE;
            }
            else
            {
                auto entryName = RCT1::GetSceneryGroupObject(sceneryThemeType);
                if (_sceneryGroupEntries.GetCount() >= MAX_SCENERY_GROUP_OBJECTS)
                {
                    Console::WriteLine("Warning: More than %d (max scenery groups) in RCT1 park.", MAX_SCENERY_GROUP_OBJECTS);
                    Console::WriteLine("         [%s] scenery group not added.", entryName);
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
                if (_s4.rides[i].type != RideType::Null)
                {
                    const auto rideId = RideId::FromUnderlying(i);
                    ImportRide(GetOrAllocateRide(rideId), &_s4.rides[i], rideId);
                }
            }
        }

        void ImportRide(::Ride* dst, RCT1::Ride* src, RideId rideIndex)
        {
            *dst = {};
            dst->id = rideIndex;

            // This is a peculiarity of this exact version number, which only Heide-Park seems to use.
            if (_s4.game_version == 110018 && src->type == RideType::InvertedRollerCoaster)
            {
                dst->type = RIDE_TYPE_COMPACT_INVERTED_COASTER;
            }
            else
            {
                dst->type = RCT1::GetRideType(src->type, src->vehicle_type);
            }

            if (RCT1::RideTypeUsesVehicles(src->type))
            {
                dst->subtype = _vehicleTypeToRideEntryMap[src->vehicle_type];
            }
            else
            {
                dst->subtype = _rideTypeToRideEntryMap[EnumValue(src->type)];
            }

            rct_ride_entry* rideEntry = get_ride_entry(dst->subtype);
            // This can happen with hacked parks
            if (rideEntry == nullptr)
            {
                log_warning("Discarding ride with invalid ride entry");
                dst->type = RIDE_TYPE_NULL;
                return;
            }

            // Ride name
            if (is_user_string_id(src->name))
            {
                dst->custom_name = GetUserString(src->name);
            }

            dst->status = static_cast<RideStatus>(src->status);

            // Flags
            dst->lifecycle_flags = src->lifecycle_flags;
            // These flags were not in the base game
            if (_gameVersion == FILE_VERSION_RCT1)
            {
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_MUSIC;
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE;
                dst->lifecycle_flags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            }

            // Station
            if (src->overall_view.IsNull())
            {
                dst->overall_view.SetNull();
            }
            else
            {
                dst->overall_view = TileCoordsXY{ src->overall_view.x, src->overall_view.y }.ToCoordsXY();
            }

            for (StationIndex::UnderlyingType i = 0; i < Limits::MaxStationsPerRide; i++)
            {
                auto& dstStation = dst->GetStation(StationIndex::FromUnderlying(i));
                if (src->station_starts[i].IsNull())
                {
                    dstStation.Start.SetNull();
                }
                else
                {
                    auto tileStartLoc = TileCoordsXY{ src->station_starts[i].x, src->station_starts[i].y };
                    dstStation.Start = tileStartLoc.ToCoordsXY();
                }
                dstStation.SetBaseZ(src->station_height[i] * Limits::CoordsZStep);
                dstStation.Length = src->station_length[i];
                dstStation.Depart = src->station_light[i];

                dstStation.TrainAtStation = src->station_depart[i];

                // Direction is fixed later.
                if (src->entrance[i].IsNull())
                    dstStation.Entrance.SetNull();
                else
                    dstStation.Entrance = { src->entrance[i].x, src->entrance[i].y, src->station_height[i] / 2, 0 };

                if (src->exit[i].IsNull())
                    dstStation.Exit.SetNull();
                else
                    dstStation.Exit = { src->exit[i].x, src->exit[i].y, src->station_height[i] / 2, 0 };

                dstStation.QueueTime = src->queue_time[i];
                dstStation.LastPeepInQueue = EntityId::FromUnderlying(src->last_peep_in_queue[i]);
                dstStation.QueueLength = src->num_peeps_in_queue[i];

                dstStation.SegmentTime = src->time[i];
                dstStation.SegmentLength = src->length[i];
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

            dst->num_stations = src->num_stations;

            // Vehicle links (indexes converted later)
            for (int32_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->vehicles[i]);
            }
            for (int32_t i = Limits::MaxTrainsPerRide; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->num_vehicles = src->num_trains;
            dst->num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;
            dst->proposed_num_vehicles = src->num_trains;
            dst->max_trains = src->max_trains;
            dst->proposed_num_cars_per_train = src->num_cars_per_train + rideEntry->zero_cars;
            dst->special_track_elements = src->special_track_elements;
            dst->num_sheltered_sections = src->num_sheltered_sections;
            dst->sheltered_length = src->sheltered_length;

            // Operation
            dst->depart_flags = src->depart_flags;
            dst->min_waiting_time = src->min_waiting_time;
            dst->max_waiting_time = src->max_waiting_time;
            dst->operation_option = src->operation_option;
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
                    if (src->type == RideType::MerryGoRound || src->type == RideType::Dodgems)
                    {
                        if (src->depart_flags & RCT1_RIDE_DEPART_PLAY_MUSIC)
                        {
                            dst->depart_flags &= ~RCT1_RIDE_DEPART_PLAY_MUSIC;
                            dst->lifecycle_flags |= RIDE_LIFECYCLE_MUSIC;
                        }
                    }
                }
                else
                {
                    dst->music = src->music;
                }
            }

            if (src->operating_mode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                // Launched rides never passed through the station in RCT1.
                dst->mode = RideMode::PoweredLaunch;
            }
            else
            {
                dst->mode = static_cast<RideMode>(src->operating_mode);
            }

            SetRideColourScheme(dst, src);

            // Maintenance
            dst->build_date = static_cast<int32_t>(src->build_date);
            dst->inspection_interval = src->inspection_interval;
            dst->last_inspection = src->last_inspection;
            dst->reliability = src->reliability;
            dst->unreliability_factor = src->unreliability_factor;
            dst->downtime = src->downtime;
            dst->breakdown_reason = src->breakdown_reason;
            dst->mechanic_status = src->mechanic_status;
            dst->mechanic = EntityId::FromUnderlying(src->mechanic);
            dst->breakdown_reason_pending = src->breakdown_reason_pending;
            dst->inspection_station = StationIndex::FromUnderlying(src->inspection_station);
            dst->broken_car = src->broken_car;
            dst->broken_vehicle = src->broken_vehicle;

            // Measurement data
            dst->excitement = src->excitement;
            dst->intensity = src->intensity;
            dst->nausea = src->nausea;

            dst->max_speed = src->max_speed;
            dst->average_speed = src->average_speed;

            dst->max_positive_vertical_g = src->max_positive_vertical_g;
            dst->max_negative_vertical_g = src->max_negative_vertical_g;
            dst->max_lateral_g = src->max_lateral_g;
            dst->previous_lateral_g = src->previous_lateral_g;
            dst->previous_vertical_g = src->previous_vertical_g;
            dst->turn_count_banked = src->turn_count_banked;
            dst->turn_count_default = src->turn_count_default;
            dst->turn_count_sloped = src->turn_count_sloped;
            dst->drops = src->num_drops;
            dst->start_drop_height = src->start_drop_height / 2;
            dst->highest_drop_height = src->highest_drop_height / 2;
            if (dst->type == RIDE_TYPE_MINI_GOLF)
                dst->holes = src->num_inversions & 0x1F;
            else
                dst->inversions = src->num_inversions & 0x1F;
            dst->sheltered_eighths = src->num_inversions >> 5;
            dst->boat_hire_return_direction = src->boat_hire_return_direction;
            dst->boat_hire_return_position = { src->boat_hire_return_position.x, src->boat_hire_return_position.y };
            dst->chairlift_bullwheel_rotation = src->chairlift_bullwheel_rotation;
            for (int i = 0; i < 2; i++)
            {
                dst->ChairliftBullwheelLocation[i] = { src->chairlift_bullwheel_location[i].x,
                                                       src->chairlift_bullwheel_location[i].y,
                                                       src->chairlift_bullwheel_z[i] / 2 };
            }

            if (src->cur_test_track_location.IsNull())
            {
                dst->CurTestTrackLocation.SetNull();
            }
            else
            {
                dst->CurTestTrackLocation = { src->cur_test_track_location.x, src->cur_test_track_location.y,
                                              src->cur_test_track_z / 2 };
            }
            dst->testing_flags = src->testing_flags;
            dst->current_test_segment = src->current_test_segment;
            dst->current_test_station = StationIndex::GetNull();
            dst->average_speed_test_timeout = src->average_speed_test_timeout;
            dst->slide_in_use = src->slide_in_use;
            dst->slide_peep_t_shirt_colour = RCT1::GetColour(src->slide_peep_t_shirt_colour);
            dst->spiral_slide_progress = src->spiral_slide_progress;
            // Doubles as slide_peep
            dst->maze_tiles = src->maze_tiles;

            // Finance / customers
            dst->upkeep_cost = src->upkeep_cost;
            dst->price[0] = src->price;
            dst->price[1] = src->price_secondary;
            dst->income_per_hour = ToMoney64(src->income_per_hour);
            dst->total_customers = src->total_customers;
            dst->profit = ToMoney64(src->profit);
            dst->total_profit = ToMoney64(src->total_profit);
            dst->value = src->value;
            for (size_t i = 0; i < std::size(src->num_customers); i++)
            {
                dst->num_customers[i] = src->num_customers[i];
            }

            dst->satisfaction = src->satisfaction;
            dst->satisfaction_time_out = src->satisfaction_time_out;
            dst->satisfaction_next = src->satisfaction_next;
            dst->popularity = src->popularity;
            dst->popularity_next = src->popularity_next;
            dst->popularity_time_out = src->popularity_time_out;

            dst->num_riders = src->num_riders;

            dst->music_tune_id = 255;
        }

        void SetRideColourScheme(::Ride* dst, RCT1::Ride* src)
        {
            // Colours
            dst->colour_scheme_type = src->colour_scheme;

            const auto& rtd = GetRideTypeDescriptor(dst->type);
            switch (rtd.SetTrackColourMode)
            {
                case RCT1SetTrackColourMode::Maze:
                    // In RCT1 and AA, the maze was always hedges.
                    // LL has 4 types, like RCT2. For LL, only guard against invalid values.
                    if (_gameVersion < FILE_VERSION_RCT1_LL || src->track_colour_supports[0] > 3)
                        dst->track_colour[0].supports = MAZE_WALL_TYPE_HEDGE;
                    else
                        dst->track_colour[0].supports = src->track_colour_supports[0];
                    break;
                case RCT1SetTrackColourMode::Default:
                default:
                    if (_gameVersion == FILE_VERSION_RCT1)
                    {
                        dst->track_colour[0].main = RCT1::GetColour(src->track_primary_colour);
                        dst->track_colour[0].additional = RCT1::GetColour(src->track_secondary_colour);
                        dst->track_colour[0].supports = RCT1::GetColour(src->track_support_colour);

                        // Balloons were always blue in the original RCT.
                        if (src->type == RideType::BalloonStall)
                        {
                            dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
                        }
                        else if (src->type == RideType::RiverRapids)
                        {
                            dst->track_colour[0].main = COLOUR_WHITE;
                        }
                    }
                    else
                    {
                        for (int i = 0; i < Limits::NumColourSchemes; i++)
                        {
                            dst->track_colour[i].main = RCT1::GetColour(src->track_colour_main[i]);
                            dst->track_colour[i].additional = RCT1::GetColour(src->track_colour_additional[i]);
                            dst->track_colour[i].supports = RCT1::GetColour(src->track_colour_supports[i]);
                        }
                    }
                    break;
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
                    dst->entrance_style = src->entrance_style;
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
                    const auto colourSchemeCopyDescriptor = GetColourSchemeCopyDescriptor(src->vehicle_type);
                    if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Body = RCT1::GetColour(src->vehicle_colours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Body = RCT1::GetColour(src->vehicle_colours[i].trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Body = colourSchemeCopyDescriptor.colour1;
                    }

                    if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Trim = RCT1::GetColour(src->vehicle_colours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Trim = RCT1::GetColour(src->vehicle_colours[i].trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Trim = colourSchemeCopyDescriptor.colour2;
                    }

                    if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
                    {
                        dst->vehicle_colours[i].Tertiary = RCT1::GetColour(src->vehicle_colours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
                    {
                        dst->vehicle_colours[i].Tertiary = RCT1::GetColour(src->vehicle_colours[i].trim);
                    }
                    else
                    {
                        dst->vehicle_colours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                    }
                }
            }
        }

        void ImportRideMeasurements()
        {
            for (const auto& src : _s4.ride_measurements)
            {
                if (src.ride_index != RCT12_RIDE_ID_NULL)
                {
                    auto ride = get_ride(RCT12RideIdToOpenRCT2RideId(src.ride_index));
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
            dst.flags = src.flags;
            dst.last_use_tick = src.last_use_tick;
            dst.num_items = src.num_items;
            dst.current_item = src.current_item;
            dst.vehicle_index = src.vehicle_index;
            dst.current_station = StationIndex::FromUnderlying(src.current_station);
            for (size_t i = 0; i < std::size(src.velocity); i++)
            {
                dst.velocity[i] = src.velocity[i] / 2;
                dst.altitude[i] = src.altitude[i] / 2;
                dst.vertical[i] = src.vertical[i] / 2;
                dst.lateral[i] = src.lateral[i] / 2;
            }
        }

        void ImportEntity(const RCT12SpriteBase& src);
        template<typename T> void ImportEntity(const RCT12SpriteBase& src);

        void ImportSprites()
        {
            for (int i = 0; i < Limits::MaxEntities; i++)
            {
                ImportEntity(_s4.sprites[i].unknown);
            }
        }

        void SetVehicleColours(::Vehicle* dst, const RCT1::Vehicle* src)
        {
            const auto& srcRide = _s4.rides[src->ride];
            uint8_t vehicleTypeIndex = srcRide.vehicle_type;
            RCT1::VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                vehicleTypeIndex);

            // RCT1 had no third colour
            if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_1)
            {
                dst->colours.body_colour = RCT1::GetColour(src->colours.body_colour);
            }
            else if (colourSchemeCopyDescriptor.colour1 == COPY_COLOUR_2)
            {
                dst->colours.body_colour = RCT1::GetColour(src->colours.trim_colour);
            }
            else
            {
                dst->colours.body_colour = colourSchemeCopyDescriptor.colour1;
            }

            if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_1)
            {
                dst->colours.trim_colour = RCT1::GetColour(src->colours.body_colour);
            }
            else if (colourSchemeCopyDescriptor.colour2 == COPY_COLOUR_2)
            {
                dst->colours.trim_colour = RCT1::GetColour(src->colours.trim_colour);
            }
            else
            {
                dst->colours.trim_colour = colourSchemeCopyDescriptor.colour2;
            }

            if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_1)
            {
                dst->colours_extended = RCT1::GetColour(src->colours.body_colour);
            }
            else if (colourSchemeCopyDescriptor.colour3 == COPY_COLOUR_2)
            {
                dst->colours_extended = RCT1::GetColour(src->colours.trim_colour);
            }
            else
            {
                dst->colours_extended = colourSchemeCopyDescriptor.colour3;
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
            dst->SpriteType = RCT1::GetPeepSpriteType(src->sprite_type);
            dst->Action = static_cast<PeepActionType>(src->action);
            dst->SpecialSprite = src->special_sprite;
            dst->NextActionSpriteType = static_cast<PeepActionSpriteType>(src->next_action_sprite_type);
            dst->ActionSpriteImageOffset = src->action_sprite_image_offset;
            dst->WalkingFrameNum = src->no_action_frame_num;
            dst->ActionSpriteType = static_cast<PeepActionSpriteType>(src->action_sprite_type);
            dst->ActionFrame = src->action_frame;

            const rct_sprite_bounds* spriteBounds = &GetSpriteBounds(dst->SpriteType, dst->ActionSpriteType);
            dst->sprite_width = spriteBounds->sprite_width;
            dst->sprite_height_negative = spriteBounds->sprite_height_negative;
            dst->sprite_height_positive = spriteBounds->sprite_height_positive;

            dst->MoveTo({ src->x, src->y, src->z });

            dst->sprite_direction = src->sprite_direction;

            // Peep name
            if (is_user_string_id(src->name_string_idx))
            {
                dst->SetName(GetUserString(src->name_string_idx));
            }

            dst->State = static_cast<PeepState>(src->state);
            dst->SubState = src->sub_state;
            dst->NextLoc = { src->next_x, src->next_y, src->next_z * Limits::CoordsZStep };
            dst->NextFlags = src->next_flags;
            dst->Var37 = src->var_37;
            dst->StepProgress = src->step_progress;
            dst->TshirtColour = RCT1::GetColour(src->tshirt_colour);
            dst->TrousersColour = RCT1::GetColour(src->trousers_colour);
            dst->DestinationX = src->destination_x;
            dst->DestinationY = src->destination_y;
            dst->DestinationTolerance = src->destination_tolerance;
            dst->PeepDirection = src->direction;
            dst->Energy = src->energy;
            dst->EnergyTarget = src->energy_target;
            dst->Mass = src->mass;
            dst->WindowInvalidateFlags = 0;
            dst->CurrentRide = RCT12RideIdToOpenRCT2RideId(src->current_ride);
            dst->CurrentRideStation = StationIndex::FromUnderlying(src->current_ride_station);
            dst->CurrentTrain = src->current_train;
            dst->CurrentCar = src->current_car;
            dst->CurrentSeat = src->current_seat;
            dst->InteractionRideIndex = RCT12RideIdToOpenRCT2RideId(src->interaction_ride_index);
            dst->Id = src->id;
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
                if (_s4.patrol_areas[peepOffset + i] == 0)
                {
                    // No patrol for this area
                    continue;
                }

                // Loop over the bits of the uint8_t
                for (int32_t j = 0; j < 8; j++)
                {
                    int8_t bit = (_s4.patrol_areas[peepOffset + i] >> j) & 1;
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

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12SpriteBase* src)
        {
            dst->sprite_direction = src->sprite_direction;
            dst->sprite_width = src->sprite_width;
            dst->sprite_height_negative = src->sprite_height_negative;
            dst->sprite_height_positive = src->sprite_height_positive;
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
        }

        void ImportPeepSpawns()
        {
            gPeepSpawns.clear();
            for (size_t i = 0; i < Limits::MaxPeepSpawns; i++)
            {
                if (_s4.peep_spawn[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
                {
                    PeepSpawn spawn = { _s4.peep_spawn[i].x, _s4.peep_spawn[i].y, _s4.peep_spawn[i].z * 16,
                                        _s4.peep_spawn[i].direction };
                    gPeepSpawns.push_back(spawn);
                }
            }
        }

        void ImportFinance()
        {
            gParkEntranceFee = _s4.park_entrance_fee;
            gLandPrice = _s4.land_price;
            gConstructionRightsPrice = _s4.construction_rights_price;

            gCash = ToMoney64(_s4.cash);
            gBankLoan = ToMoney64(_s4.loan);
            gMaxBankLoan = ToMoney64(_s4.max_loan);
            // It's more like 1.33%, but we can only use integers. Can be fixed once we have our own save format.
            gBankLoanInterestRate = 1;
            gInitialCash = ToMoney64(_s4.cash);

            gCompanyValue = ToMoney64(_s4.company_value);
            gParkValue = ToMoney64(CorrectRCT1ParkValue(_s4.park_value));
            gCurrentProfit = ToMoney64(_s4.profit);

            for (size_t i = 0; i < Limits::FinanceGraphSize; i++)
            {
                gCashHistory[i] = ToMoney64(_s4.cash_history[i]);
                gParkValueHistory[i] = ToMoney64(CorrectRCT1ParkValue(_s4.park_value_history[i]));
                gWeeklyProfitHistory[i] = ToMoney64(_s4.weekly_profit_history[i]);
            }

            for (size_t i = 0; i < Limits::ExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::ExpenditureTypeCount; j++)
                {
                    gExpenditureTable[i][j] = ToMoney64(_s4.expenditure[i][j]);
                }
            }
            gCurrentExpenditure = ToMoney64(_s4.total_expenditure);

            gScenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s4.completed_company_value);
            gTotalAdmissions = _s4.num_admissions;
            gTotalIncomeFromAdmissions = ToMoney64(_s4.admission_total_income);

            // TODO marketing campaigns not working
            static_assert(
                std::numeric_limits<uint8_t>::max() > ADVERTISING_CAMPAIGN_COUNT,
                "Advertising enum bigger than capacity of iterator");
            for (uint8_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                if (_s4.marketing_status[i] & CAMPAIGN_ACTIVE_FLAG)
                {
                    MarketingCampaign campaign;
                    campaign.Type = i;
                    campaign.WeeksLeft = _s4.marketing_status[i] & ~CAMPAIGN_ACTIVE_FLAG;
                    if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                    {
                        campaign.RideId = RCT12RideIdToOpenRCT2RideId(_s4.marketing_assoc[i]);
                    }
                    else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        campaign.ShopItemType = ShopItem(_s4.marketing_assoc[i]);
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
                Limits::MaxMapSize, _s4.tile_elements, std::size(_s4.tile_elements));

            std::vector<TileElement> tileElements;
            const auto maxSize = _s4.map_size == 0 ? Limits::MaxMapSize : _s4.map_size;
            for (TileCoordsXY coords = { 0, 0 }; coords.y < MAXIMUM_MAP_SIZE_TECHNICAL; coords.y++)
            {
                for (coords.x = 0; coords.x < MAXIMUM_MAP_SIZE_TECHNICAL; coords.x++)
                {
                    auto tileAdded = false;
                    if (coords.x < maxSize && coords.y < maxSize)
                    {
                        // This is the equivalent of map_get_first_element_at(x, y), but on S4 data.
                        RCT12TileElement* srcElement = tilePointerIndex.GetFirstElementAt(coords);
                        do
                        {
                            if (srcElement->base_height == Limits::MaxElementHeight)
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

            dst->SetBaseZ(src->base_height * Limits::CoordsZStep);
            dst->SetClearanceZ(src->clearance_height * Limits::CoordsZStep);

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
                    const auto* ride = get_ride(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
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
                    if (rideType == RIDE_TYPE_MAZE)
                    {
                        dst2->SetMazeEntry(src2->GetMazeEntry());
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
                        auto baseZ = src->base_height * Limits::CoordsZStep;
                        auto clearanceZ = src->clearance_height * Limits::CoordsZStep;
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
                    if (index < std::size(_s4.banners))
                    {
                        auto srcBanner = &_s4.banners[index];
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

            research_reset_items();

            size_t researchListCount;
            const RCT1::ResearchItem* researchList = GetResearchList(&researchListCount);

            // Initialise the "seen" tables
            _researchRideEntryUsed.reset();
            _researchRideTypeUsed.reset();

            // The first six scenery groups are always available
            for (uint8_t i = 0; i < 6; i++)
            {
                research_insert_scenery_group_entry(i, true);
            }

            bool researched = true;
            auto rideTypeInResearch = GetRideTypesPresentInResearchList(researchList, researchListCount);
            std::vector<RCT1::ResearchItem> vehiclesWithMissingRideTypes;
            for (size_t i = 0; i < researchListCount; i++)
            {
                const auto& researchItem = researchList[i];
                if (researchItem.flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                {
                    if (researchItem.item == RCT1_RESEARCH_END_AVAILABLE)
                    {
                        researched = false;
                        continue;
                    }
                    // We don't import the random items yet.
                    else if (researchItem.item == RCT1_RESEARCH_END_RESEARCHABLE || researchItem.item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                }

                switch (researchItem.type)
                {
                    case RCT1_RESEARCH_TYPE_THEME:
                    {
                        uint8_t rct1SceneryTheme = researchItem.item;
                        auto sceneryGroupEntryIndex = _sceneryThemeTypeToEntryMap[rct1SceneryTheme];
                        if (sceneryGroupEntryIndex != OBJECT_ENTRY_INDEX_IGNORE
                            && sceneryGroupEntryIndex != OBJECT_ENTRY_INDEX_NULL)
                        {
                            research_insert_scenery_group_entry(sceneryGroupEntryIndex, researched);
                        }
                        break;
                    }
                    case RCT1_RESEARCH_TYPE_RIDE:
                    {
                        const auto rct1RideType = static_cast<RideType>(researchItem.item);
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
                                if (researchItem2.flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                                {
                                    if (researchItem2.item == RCT1_RESEARCH_END_RESEARCHABLE
                                        || researchItem2.item == RCT1_RESEARCH_END)
                                    {
                                        break;
                                    }

                                    continue;
                                }

                                if (researchItem2.type == RCT1_RESEARCH_TYPE_VEHICLE
                                    && static_cast<RideType>(researchItem2.related_ride) == rct1RideType)
                                {
                                    auto rideEntryIndex2 = _vehicleTypeToRideEntryMap[researchItem2.item];
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
                                research_insert_ride_entry(ownRideEntryIndex, researched);
                            }
                        }

                        break;
                    }
                    case RCT1_RESEARCH_TYPE_VEHICLE:
                    {
                        // Only add vehicle if the related ride has been seen, this to make sure that vehicles
                        // are researched only after the ride has been researched. Otherwise, remove them from the research
                        // list, so that they are automatically co-invented when their master ride is invented.
                        if (_researchRideTypeUsed[researchItem.related_ride])
                        {
                            InsertResearchVehicle(researchItem, researched);
                        }
                        else if (!rideTypeInResearch[researchItem.related_ride] && _gameVersion == FILE_VERSION_RCT1_LL)
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
            if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_ROLLERCOASTERS)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Rollercoaster);
            }
            if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_THRILL_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Thrill);
                activeResearchTypes |= EnumToFlag(ResearchCategory::Water);
            }
            if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Gentle);
                activeResearchTypes |= EnumToFlag(ResearchCategory::Transport);
            }
            if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_SHOPS)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::Shop);
            }
            if (_s4.research_priority & RCT1_RESEARCH_CATEGORY_SCENERY_THEMING)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::SceneryGroup);
            }
            gResearchPriorities = activeResearchTypes;
            gResearchFundingLevel = _s4.research_level;

            // This will mark items as researched/unresearched according to the research list.
            // This needs to be called before importing progress, as it will reset it.
            research_reset_current_item();

            // Research history
            gResearchProgress = _s4.research_progress;
            gResearchProgressStage = _s4.research_progress_stage;
            gResearchExpectedDay = _s4.next_research_expected_day;
            gResearchExpectedMonth = _s4.next_research_expected_month;

            if (_s4.last_research_flags == 0xFF)
            {
                gResearchLastItem = std::nullopt;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.last_research_item, _s4.last_research_type);
                gResearchLastItem = researchItem;
            }

            if (_s4.next_research_flags == 0xFF)
            {
                gResearchNextItem = std::nullopt;
                gResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                gResearchProgress = 0;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.next_research_item, _s4.next_research_type);
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
                if (researchItem.flags == RCT1_RESEARCH_FLAGS_SEPARATOR)
                {
                    if (researchItem.item == RCT1_RESEARCH_END_AVAILABLE || researchItem.item == RCT1_RESEARCH_END_RESEARCHABLE)
                    {
                        continue;
                    }
                    if (researchItem.item == RCT1_RESEARCH_END)
                    {
                        break;
                    }
                }

                if (researchItem.type == RCT1_RESEARCH_TYPE_RIDE)
                {
                    ret[researchItem.item] = true;
                }
            }

            return ret;
        }

        void InsertResearchVehicle(const ResearchItem& researchItem, bool researched)
        {
            uint8_t vehicle = researchItem.item;
            auto rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

            if (!_researchRideEntryUsed[rideEntryIndex])
            {
                _researchRideEntryUsed[rideEntryIndex] = true;
                research_insert_ride_entry(rideEntryIndex, researched);
            }
        }

        void ImportParkName()
        {
            std::string parkName = std::string(_s4.scenario_name);
            if (is_user_string_id(static_cast<rct_string_id>(_s4.park_name_string_index)))
            {
                std::string userString = GetUserString(_s4.park_name_string_index);
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
            gCurrentTicks = _s4.ticks;
            scenario_rand_seed(_s4.random_a, _s4.random_b);
            gDateMonthsElapsed = static_cast<int32_t>(_s4.month);
            gDateMonthTicks = _s4.day;

            // Park rating
            gParkRating = _s4.park_rating;

            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            park.ResetHistories();
            std::copy(std::begin(_s4.park_rating_history), std::end(_s4.park_rating_history), gParkRatingHistory);
            for (size_t i = 0; i < std::size(_s4.guests_in_park_history); i++)
            {
                if (_s4.guests_in_park_history[i] != RCT12ParkHistoryUndefined)
                {
                    gGuestsInParkHistory[i] = _s4.guests_in_park_history[i] * RCT12GuestsInParkHistoryFactor;
                }
            }

            // Awards
            auto& awards = GetAwards();
            for (auto& src : _s4.awards)
            {
                if (src.time != 0)
                {
                    awards.push_back(Award{ src.time, static_cast<AwardType>(src.type) });
                }
            }

            // Number of guests history
            std::fill(std::begin(gGuestsInParkHistory), std::end(gGuestsInParkHistory), std::numeric_limits<uint32_t>::max());
            for (size_t i = 0; i < std::size(_s4.guests_in_park_history); i++)
            {
                if (_s4.guests_in_park_history[i] != std::numeric_limits<uint8_t>::max())
                {
                    gGuestsInParkHistory[i] = _s4.guests_in_park_history[i] * 20;
                }
            }

            // News items
            for (size_t i = 0; i < Limits::MaxNewsItems; i++)
            {
                const rct12_news_item* src = &_s4.messages[i];
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
            gGuestInitialCash = _s4.guest_initial_cash;
            gGuestInitialHunger = _s4.guest_initial_hunger;
            gGuestInitialThirst = _s4.guest_initial_thirst;
            gGuestInitialHappiness = _s4.guest_initial_happiness;

            _guestGenerationProbability = _s4.guest_generation_probability;

            // Staff colours
            gStaffHandymanColour = RCT1::GetColour(_s4.handman_colour);
            gStaffMechanicColour = RCT1::GetColour(_s4.mechanic_colour);
            gStaffSecurityColour = RCT1::GetColour(_s4.security_guard_colour);

            // Flags
            gParkFlags = _s4.park_flags;
            gParkFlags &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;
            // Loopy Landscape parks can set a flag to lock the entry price to free.
            // If this flag is not set, the player can ask money for both rides and entry.
            if (!(_s4.park_flags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
            {
                gParkFlags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
            }

            gParkSize = _s4.park_size;
            gTotalRideValueForMoney = _s4.total_ride_value_for_money;
            gSamePriceThroughoutPark = 0;
            if (_gameVersion == FILE_VERSION_RCT1_LL)
            {
                gSamePriceThroughoutPark = _s4.same_price_throughout;
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
                    rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                    if (rideEntry != nullptr)
                    {
                        auto rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
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
                    rct_ride_entry* rideEntry = get_ride_entry(entryIndex);

                    if (rideEntry != nullptr)
                    {
                        auto rideType = ride_entry_get_first_non_null_ride_type(rideEntry);
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

                if (entryIndex != OBJECT_ENTRY_INDEX_IGNORE && entryIndex != OBJECT_ENTRY_INDEX_NULL)
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
            gClimate = ClimateType{ _s4.climate };
            gClimateUpdateTimer = _s4.climate_timer;
            gClimateCurrent.Temperature = _s4.temperature;
            gClimateCurrent.Weather = WeatherType{ _s4.weather };
            gClimateCurrent.WeatherEffect = WeatherEffectType::None;
            gClimateCurrent.WeatherGloom = _s4.weather_gloom;
            gClimateCurrent.Level = static_cast<WeatherLevel>(_s4.rain);
            gClimateNext.Temperature = _s4.target_temperature;
            gClimateNext.Weather = WeatherType{ _s4.target_weather };
            gClimateNext.WeatherEffect = WeatherEffectType::None;
            gClimateNext.WeatherGloom = _s4.target_weather_gloom;
            gClimateNext.Level = static_cast<WeatherLevel>(_s4.target_rain);
        }

        void ImportScenarioNameDetails()
        {
            std::string name = String::ToStd(_s4.scenario_name);
            std::string details;

            int32_t scNumber = _s4.scenario_slot_index;
            if (scNumber != -1)
            {
                source_desc sourceDesc;
                if (ScenarioSources::TryGetById(scNumber, &sourceDesc))
                {
                    rct_string_id localisedStringIds[3];
                    if (language_get_localised_scenario_strings(sourceDesc.title, localisedStringIds))
                    {
                        if (localisedStringIds[0] != STR_NONE)
                        {
                            name = String::ToStd(language_get_string(localisedStringIds[0]));
                        }
                        if (localisedStringIds[2] != STR_NONE)
                        {
                            details = String::ToStd(language_get_string(localisedStringIds[2]));
                        }
                    }
                }
            }

            gScenarioName = std::move(name);
            gScenarioDetails = std::move(details);
        }

        void ImportScenarioObjective()
        {
            gScenarioObjective.Type = _s4.scenario_objective_type;
            gScenarioObjective.Year = _s4.scenario_objective_years;
            gScenarioObjective.NumGuests = _s4.scenario_objective_num_guests;

            // RCT1 used a different way of calculating the park value.
            // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
            // we need to check the goal to avoid affecting scenarios like Volcania.
            if (_s4.scenario_objective_type == OBJECTIVE_PARK_VALUE_BY)
                gScenarioObjective.Currency = ToMoney64(CorrectRCT1ParkValue(_s4.scenario_objective_currency));
            else
                gScenarioObjective.Currency = ToMoney64(_s4.scenario_objective_currency);

            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.scenario_objective_type == OBJECTIVE_BUILD_THE_BEST)
                gScenarioObjective.RideId = GetBuildTheBestRideId();
        }

        void ImportSavedView()
        {
            gSavedView = ScreenCoordsXY{ _s4.view_x, _s4.view_y };
            gSavedViewZoom = ZoomLevel{ static_cast<int8_t>(_s4.view_zoom) };
            gSavedViewRotation = _s4.view_rotation;
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
            dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->type);

            dst->flags = 0;
            if (src->flags & BANNER_FLAG_NO_ENTRY)
            {
                dst->flags |= BANNER_FLAG_NO_ENTRY;
            }

            if (is_user_string_id(src->string_idx))
            {
                dst->text = GetUserString(src->string_idx);
            }

            dst->colour = RCT1::GetColour(src->colour);
            dst->text_colour = src->text_colour;
            dst->position.x = src->x;
            dst->position.y = src->y;
        }

        void FixEntrancePositions()
        {
            gParkEntrances.clear();
            tile_element_iterator it;
            tile_element_iterator_begin(&it);
            while (tile_element_iterator_next(&it) && gParkEntrances.size() < Limits::MaxParkEntrances)
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
                *count = std::size(_s4.research_items_LL);
                return _s4.research_items_LL;
            }

            *count = std::size(_s4.research_items);
            return _s4.research_items;
        }

        std::string GetUserString(rct_string_id stringId)
        {
            const auto originalString = _s4.string_table[(stringId - USER_STRING_START) % 1024];
            auto originalStringView = std::string_view(
                originalString, RCT2::GetRCT2StringBufferLen(originalString, USER_STRING_MAX_LENGTH));
            auto asUtf8 = rct2_to_utf8(originalStringView, RCT2LanguageId::EnglishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        void FixLandOwnership()
        {
            switch (_s4.scenario_slot_index)
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
            if (_s4.scenario_slot_index == SC_URBAN_PARK && _isScenario)
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
                auto ride = get_ride(merryGoRoundId);
                if (ride != nullptr)
                {
                    auto& station = ride->GetStation();
                    auto entranceCoords = station.Exit;
                    auto exitCoords = station.Entrance;
                    station.Entrance = entranceCoords;
                    station.Exit = exitCoords;

                    auto entranceElement = map_get_ride_exit_element_at(entranceCoords.ToCoordsXYZD(), false);
                    entranceElement->SetEntranceType(ENTRANCE_TYPE_RIDE_ENTRANCE);
                    auto exitElement = map_get_ride_entrance_element_at(exitCoords.ToCoordsXYZD(), false);
                    exitElement->SetEntranceType(ENTRANCE_TYPE_RIDE_EXIT);

                    // Trigger footpath update
                    footpath_queue_chain_reset();
                    footpath_connect_edges(
                        entranceCoords.ToCoordsXY(), reinterpret_cast<TileElement*>(entranceElement),
                        GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
                    footpath_update_queue_chains();
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
                    TileElement* tileElement = map_get_first_element_at(TileCoordsXY{ x, y });
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
                            auto ride = get_ride(rideIndex);
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
                if (researchList[i].flags == 0xFF)
                {
                    break;
                }

                if (researchList[i].type == RCT1_RESEARCH_TYPE_RIDE)
                {
                    return RCT1::GetRideType(static_cast<RideType>(researchList[i].item), 0);
                }
            }

            return RIDE_TYPE_NULL;
        }
    };

    // Very similar but not the same as S6Importer version (due to peeps)
    constexpr EntityType GetEntityTypeFromRCT1Sprite(const RCT12SpriteBase& src)
    {
        EntityType output = EntityType::Null;
        switch (src.sprite_identifier)
        {
            case RCT12SpriteIdentifier::Vehicle:
                output = EntityType::Vehicle;
                break;
            case RCT12SpriteIdentifier::Peep:
                if (RCT12PeepType(static_cast<const RCT1::Peep*>(&src)->type) == RCT12PeepType::Guest)
                {
                    output = EntityType::Guest;
                }
                else
                {
                    output = EntityType::Staff;
                }
                break;
            case RCT12SpriteIdentifier::Misc:

                switch (RCT12MiscEntityType(src.type))
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
            case RCT12SpriteIdentifier::Litter:
                output = EntityType::Litter;
                break;
            default:
                break;
        }
        return output;
    }

    template<> void S4Importer::ImportEntity<::Vehicle>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT1::Vehicle*>(&srcBase);
        const auto* ride = get_ride(RideId::FromUnderlying(src->ride));
        if (ride == nullptr)
            return;

        uint8_t vehicleEntryIndex = RCT1::GetVehicleSubEntryIndex(src->vehicle_type);

        dst->ride = RideId::FromUnderlying(src->ride);
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(ride->subtype);

        dst->vehicle_type = vehicleEntryIndex;
        dst->SubType = ::Vehicle::Type(src->type);
        dst->var_44 = src->var_44;
        dst->remaining_distance = src->remaining_distance;

        // Properties from vehicle entry
        dst->sprite_width = src->sprite_width;
        dst->sprite_height_negative = src->sprite_height_negative;
        dst->sprite_height_positive = src->sprite_height_positive;
        dst->sprite_direction = src->sprite_direction;

        dst->SpriteRect = ScreenRect(src->sprite_left, src->sprite_top, src->sprite_right, src->sprite_bottom);

        dst->mass = src->mass;
        dst->num_seats = src->num_seats;
        dst->speed = src->speed;
        dst->powered_acceleration = src->powered_acceleration;
        dst->brake_speed = src->brake_speed;

        dst->velocity = src->velocity;
        dst->acceleration = src->acceleration;
        dst->SwingSprite = src->SwingSprite;
        dst->SwingPosition = src->SwingPosition;
        dst->SwingSpeed = src->SwingSpeed;
        dst->restraints_position = src->restraints_position;
        dst->spin_sprite = src->spin_sprite;
        dst->sound_vector_factor = src->sound_vector_factor;
        dst->spin_speed = src->spin_speed;
        dst->sound2_flags = src->sound2_flags;
        dst->sound1_id = OpenRCT2::Audio::SoundId::Null;
        dst->sound2_id = OpenRCT2::Audio::SoundId::Null;
        dst->var_C0 = src->var_C0;
        dst->CollisionDetectionTimer = src->CollisionDetectionTimer;
        dst->animation_frame = src->animation_frame;
        dst->animationState = src->animationState;
        dst->NumLaps = src->NumLaps;
        dst->var_D3 = src->var_D3;
        dst->scream_sound_id = OpenRCT2::Audio::SoundId::Null;
        dst->Pitch = src->Pitch;
        dst->bank_rotation = src->bank_rotation;

        // Seat rotation was not in RCT1
        dst->target_seat_rotation = DEFAULT_SEAT_ROTATION;
        dst->seat_rotation = DEFAULT_SEAT_ROTATION;

        // Vehicle links (indexes converted later)
        dst->prev_vehicle_on_ride = EntityId::FromUnderlying(src->prev_vehicle_on_ride);
        dst->next_vehicle_on_ride = EntityId::FromUnderlying(src->next_vehicle_on_ride);
        dst->next_vehicle_on_train = EntityId::FromUnderlying(src->next_vehicle_on_train);

        // Guests (indexes converted later)
        for (int i = 0; i < 32; i++)
        {
            const auto spriteIndex = EntityId::FromUnderlying(src->peep[i]);
            dst->peep[i] = spriteIndex;
            if (!spriteIndex.IsNull())
            {
                dst->peep_tshirt_colours[i] = RCT1::GetColour(src->peep_tshirt_colours[i]);
            }
        }

        ::Vehicle::Status statusSrc = ::Vehicle::Status::MovingToEndOfStation;
        if (src->status <= static_cast<uint8_t>(::Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<::Vehicle::Status>(src->status);
        }
        dst->status = statusSrc;
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->TrackLocation = { src->track_x, src->track_y, src->track_z };
        dst->current_station = StationIndex::FromUnderlying(src->current_station);
        if (src->boat_location.IsNull() || ride->mode != RideMode::BoatHire || statusSrc != ::Vehicle::Status::TravellingBoat)
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            dst->SetTrackType(RCT1TrackTypeToOpenRCT2(src->GetTrackType(), ride->type));
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->boat_location.x, src->boat_location.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(0);
        }
        dst->track_progress = src->track_progress;
        dst->vertical_drop_countdown = src->vertical_drop_countdown;
        dst->sub_state = src->sub_state;
        dst->update_flags = src->update_flags;

        SetVehicleColours(dst, src);

        dst->mini_golf_current_animation = MiniGolfAnimation(src->mini_golf_current_animation);
        dst->mini_golf_flags = src->mini_golf_flags;

        dst->MoveTo({ src->x, src->y, src->z });

        dst->num_peeps = src->num_peeps;
        dst->next_free_seat = src->next_free_seat;
        dst->IsCrashedVehicle = src->flags & RCT12_SPRITE_FLAGS_IS_CRASHED_VEHICLE_SPRITE;
    }

    template<> void S4Importer::ImportEntity<Guest>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<Guest>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT1::Peep*>(&srcBase);
        ImportPeep(dst, src);

        dst->OutsideOfPark = static_cast<bool>(src->outside_of_park);
        dst->TimeToConsume = src->time_to_consume;
        dst->VandalismSeen = src->vandalism_seen;
        dst->UmbrellaColour = RCT1::GetColour(src->umbrella_colour);
        dst->HatColour = RCT1::GetColour(src->hat_colour);

        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->BalloonColour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->BalloonColour = RCT1::GetColour(src->balloon_colour);
        }
        dst->Happiness = src->happiness;
        dst->HappinessTarget = src->happiness_target;
        dst->Nausea = src->nausea;
        dst->NauseaTarget = src->nausea_target;
        dst->Hunger = src->hunger;
        dst->Thirst = src->thirst;
        dst->Toilet = src->toilet;
        dst->LitterCount = src->litter_count;
        dst->DisgustingCount = src->disgusting_count;
        dst->Intensity = static_cast<IntensityRange>(src->intensity);
        dst->NauseaTolerance = static_cast<PeepNauseaTolerance>(src->nausea_tolerance);
        dst->GuestTimeOnRide = src->time_on_ride;
        dst->DaysInQueue = src->days_in_queue;
        dst->CashInPocket = src->cash_in_pocket;
        dst->CashSpent = src->cash_spent;
        dst->ParkEntryTime = src->park_entry_time;
        dst->GuestNumRides = src->no_of_rides;
        dst->AmountOfDrinks = src->no_of_drinks;
        dst->AmountOfFood = src->no_of_food;
        dst->AmountOfSouvenirs = src->no_of_souvenirs;
        dst->PaidToEnter = src->paid_to_enter;
        dst->PaidOnRides = src->paid_on_rides;
        dst->PaidOnDrink = src->paid_on_drink;
        dst->PaidOnFood = src->paid_on_food;
        dst->PaidOnSouvenirs = src->paid_on_souvenirs;
        dst->VoucherRideId = RCT12RideIdToOpenRCT2RideId(src->voucher_arguments);
        dst->VoucherType = src->voucher_type;
        dst->SurroundingsThoughtTimeout = src->surroundings_thought_timeout;
        dst->Angriness = src->angriness;
        dst->TimeLost = src->time_lost;

        OpenRCT2::RideUse::GetHistory().Set(dst->sprite_index, RCT12GetRidesBeenOn(src));
        OpenRCT2::RideUse::GetTypeHistory().Set(dst->sprite_index, RCT12GetRideTypesBeenOn(src));

        dst->Photo1RideRef = RCT12RideIdToOpenRCT2RideId(src->photo1_ride_ref);

        for (size_t i = 0; i < std::size(src->thoughts); i++)
        {
            auto srcThought = &src->thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->type);
            if (srcThought->item == RCT12PeepThoughtItemNone)
                dstThought->item = PeepThoughtItemNone;
            else
                dstThought->item = srcThought->item;
            dstThought->freshness = srcThought->freshness;
            dstThought->fresh_timeout = srcThought->fresh_timeout;
        }

        dst->PreviousRide = RCT12RideIdToOpenRCT2RideId(src->previous_ride);
        dst->PreviousRideTimeOut = src->previous_ride_time_out;
        dst->GuestHeadingToRideId = RCT12RideIdToOpenRCT2RideId(src->guest_heading_to_ride_id);
        dst->GuestIsLostCountdown = src->peep_is_lost_countdown;
        dst->GuestNextInQueue = EntityId::FromUnderlying(src->next_in_queue);
        // Guests' favourite ride was only saved in LL.
        // Set it to N/A if the save comes from the original or AA.
        if (_gameVersion == FILE_VERSION_RCT1_LL)
        {
            dst->FavouriteRide = RCT12RideIdToOpenRCT2RideId(src->favourite_ride);
            dst->FavouriteRideRating = src->favourite_ride_rating;
        }
        else
        {
            dst->FavouriteRide = RideId::GetNull();
            dst->FavouriteRideRating = 0;
        }

        dst->SetItemFlags(src->GetItemFlags());

        if (dst->OutsideOfPark && dst->State != PeepState::LeavingPark)
        {
            increment_guests_heading_for_park();
        }
        else
        {
            increment_guests_in_park();
        }
    }

    template<> void S4Importer::ImportEntity<Staff>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<Staff>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT1::Peep*>(&srcBase);
        ImportPeep(dst, src);
        dst->AssignedStaffType = StaffType(src->staff_type);
        dst->MechanicTimeSinceCall = src->mechanic_time_since_call;
        dst->HireDate = src->park_entry_time;
        dst->StaffOrders = src->staff_orders;
        dst->StaffMowingTimeout = src->staff_mowing_timeout;
        dst->StaffLawnsMown = src->paid_to_enter;
        dst->StaffGardensWatered = src->paid_on_rides;
        dst->StaffLitterSwept = src->paid_on_food;
        dst->StaffBinsEmptied = src->paid_on_souvenirs;

        ImportStaffPatrolArea(dst, src->staff_id);
    }

    template<> void S4Importer::ImportEntity<Litter>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<Litter>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteLitter*>(&srcBase);
        ImportEntityCommonProperties(dst, src);

        dst->SubType = Litter::Type(src->type);
    }

    template<> void S4Importer::ImportEntity<SteamParticle>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<SteamParticle>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteSteamParticle*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
    }

    template<> void S4Importer::ImportEntity<MoneyEffect>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<MoneyEffect>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteMoneyEffect*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->move_delay;
        dst->NumMovements = src->num_movements;
        dst->Value = src->value;
        dst->OffsetX = src->offset_x;
        dst->Wiggle = src->wiggle;
    }

    template<> void S4Importer::ImportEntity<VehicleCrashParticle>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<VehicleCrashParticle>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteCrashedVehicleParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
    }

    template<> void S4Importer::ImportEntity<ExplosionCloud>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionCloud>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
    }

    template<> void S4Importer::ImportEntity<ExplosionFlare>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionFlare>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
    }

    template<> void S4Importer::ImportEntity<CrashSplashParticle>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<CrashSplashParticle>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
    }

    template<> void S4Importer::ImportEntity<JumpingFountain>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<JumpingFountain>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteJumpingFountain*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->FountainFlags = src->fountain_flags;
        dst->Iteration = src->iteration;
        dst->NumTicksAlive = src->num_ticks_alive;
        dst->frame = src->frame;
        dst->FountainType = JumpingFountainType::Water;
    }

    template<> void S4Importer::ImportEntity<Balloon>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<Balloon>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteBalloon*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->colour = COLOUR_LIGHT_BLUE;
        }
        else
        {
            dst->colour = RCT1::GetColour(src->colour);
        }
    }

    template<> void S4Importer::ImportEntity<Duck>(const RCT12SpriteBase& srcBase)
    {
        auto* dst = CreateEntityAt<Duck>(EntityId::FromUnderlying(srcBase.sprite_index));
        auto* src = static_cast<const RCT12SpriteDuck*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->frame;
        dst->state = static_cast<Duck::DuckState>(src->state);
    }

    void S4Importer::ImportEntity(const RCT12SpriteBase& src)
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

void load_from_sv4(const utf8* path)
{
    auto& objectMgr = GetContext()->GetObjectManager();
    auto s4Importer = std::make_unique<RCT1::S4Importer>();
    auto result = s4Importer->LoadSavedGame(path);
    objectMgr.LoadObjects(result.RequiredObjects);
    s4Importer->Import();
}

void load_from_sc4(const utf8* path)
{
    auto& objectMgr = GetContext()->GetObjectManager();
    auto s4Importer = std::make_unique<RCT1::S4Importer>();
    auto result = s4Importer->LoadScenario(path);
    objectMgr.LoadObjects(result.RequiredObjects);
    s4Importer->Import();
}
