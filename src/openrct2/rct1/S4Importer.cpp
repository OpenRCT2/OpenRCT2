/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../Editor.h"
#include "../Game.h"
#include "../GameState.h"
#include "../ParkImporter.h"
#include "../actions/WallPlaceAction.h"
#include "../audio/audio.h"
#include "../core/BitSet.hpp"
#include "../core/Collections.hpp"
#include "../core/Console.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/FileStream.h"
#include "../core/Guard.hpp"
#include "../core/IStream.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/SawyerCoding.h"
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
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.Date.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../object/Object.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/PeepAnimationsObject.h"
#include "../object/ScenarioTextObject.h"
#include "../park/Legacy.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/CSStringConverter.h"
#include "../rct12/EntryList.h"
#include "../rct12/ScenarioPatcher.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrainManager.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Footpath.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TilePointerIndex.hpp"
#include "../world/Wall.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "RCT1.h"
#include "Tables.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <mutex>
#include <vector>

using namespace OpenRCT2;

static constexpr ObjectEntryIndex ObjectEntryIndexIgnore = 254;

namespace OpenRCT2::RCT1
{
    static std::mutex mtx;

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
        RCT12::EntryList _bannerEntries;
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
        ObjectEntryIndex _vehicleTypeToRideEntryMap[EnumValue(VehicleType::Count)]{};
        ObjectEntryIndex _smallSceneryTypeToEntryMap[256]{};
        ObjectEntryIndex _largeSceneryTypeToEntryMap[256]{};
        ObjectEntryIndex _wallTypeToEntryMap[256]{};
        ObjectEntryIndex _bannerTypeToEntryMap[9]{};
        ObjectEntryIndex _pathTypeToEntryMap[24]{};
        ObjectEntryIndex _pathAdditionTypeToEntryMap[16]{};
        ObjectEntryIndex _sceneryThemeTypeToEntryMap[24]{};
        ObjectEntryIndex _terrainSurfaceTypeToEntryMap[16]{};
        ObjectEntryIndex _terrainEdgeTypeToEntryMap[16]{};
        ObjectEntryIndex _footpathSurfaceTypeToEntryMap[32]{};
        ObjectEntryIndex _footpathRailingsTypeToEntryMap[4]{};

        // Research
        BitSet<kMaxRideObjects> _researchRideEntryUsed{};
        BitSet<EnumValue(RideType::Count)> _researchRideTypeUsed{};

        // Scenario repository - used for determining scenario name
        IScenarioRepository* _scenarioRepository = GetScenarioRepository();

    public:
        ParkLoadResult Load(const u8string& path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::iequals(extension, ".sc4"))
            {
                return LoadScenario(path);
            }
            if (String::iequals(extension, ".sv4"))
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
            _gameVersion = SawyerCoding::DetectRCT1Version(_s4.GameVersion) & FILE_VERSION_MASK;

            // Only determine what objects we required to import this saved game
            InitialiseEntryMaps();
            CreateAvailableObjectMappings();
            return ParkLoadResult(GetRequiredObjects());
        }

        void Import(GameState_t& gameState) override
        {
            Initialise(gameState);

            ImportRides();
            ImportRideMeasurements();
            ImportEntities();
            ImportTileElements(gameState);
            ImportPeepSpawns(gameState);
            ImportFinance(gameState);
            ImportResearch(gameState);
            ImportParkName(gameState);
            ImportParkFlags(gameState);
            ImportClimate(gameState);
            ImportScenarioNameDetails(gameState);
            ImportScenarioObjective(gameState);
            ImportSavedView(gameState);

            ConvertPeepAnimationTypeToObjects(gameState);
            ResetPeepSpriteBounds(gameState);

            if (_isScenario)
            {
                RCT12::FetchAndApplyScenarioPatch(_s4Path);
            }
            FixNextGuestNumber(gameState);
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

            std::string name = RCT2StringToUTF8(_s4.ScenarioName, RCT2LanguageId::EnglishUK);
            std::string details;

            // TryGetById won't set this property if the scenario is not recognised,
            // but localisation needs it.
            if (!isOfficial)
            {
                desc.title = name.c_str();
            }

            String::set(dst->InternalName, sizeof(dst->InternalName), desc.title);

            if (!desc.textObjectId.empty())
            {
                auto& objManager = GetContext()->GetObjectManager();

                // Ensure only one thread talks to the object manager at a time
                std::lock_guard lock(mtx);

                // Unload loaded scenario text object, if any.
                if (auto* obj = objManager.GetLoadedObject(ObjectType::ScenarioText, 0); obj != nullptr)
                    objManager.UnloadObjects({ obj->GetDescriptor() });

                // Load the one specified
                if (auto* obj = objManager.LoadObject(desc.textObjectId); obj != nullptr)
                {
                    auto* textObject = reinterpret_cast<ScenarioTextObject*>(obj);
                    name = textObject->GetScenarioName();
                    details = textObject->GetScenarioDetails();
                }
            }

            String::set(dst->Name, sizeof(dst->Name), name.c_str());
            String::set(dst->Details, sizeof(dst->Details), details.c_str());

            return true;
        }

        money64 CorrectRCT1ParkValue(money32 oldParkValue)
        {
            if (oldParkValue == kMoney32Undefined)
            {
                return kMoney64Undefined;
            }

            if (_parkValueConversionFactor == 0)
            {
                if (_s4.ParkValue != 0)
                {
                    // Use the ratio between the old and new park value to calcute the ratio to
                    // use for the park value history and the goal.
                    _parkValueConversionFactor = (Park::CalculateParkValue() * 10) / _s4.ParkValue;
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
            int32_t fileType = SawyerCoding::DetectFileType(data.get(), dataSize);
            if (isScenario && (fileType & FILE_VERSION_MASK) != FILE_VERSION_RCT1)
            {
                decodedSize = SawyerCoding::DecodeSC4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }
            else
            {
                decodedSize = SawyerCoding::DecodeSV4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }

            if (decodedSize == sizeof(S4))
            {
                std::memcpy(s4.get(), decodedData.get(), sizeof(S4));
                return s4;
            }

            throw std::runtime_error("Unable to decode park.");
        }

        void Initialise(GameState_t& gameState)
        {
            // Avoid reusing the value used for last import
            _parkValueConversionFactor = 0;

            uint16_t mapSize = _s4.MapSize == 0 ? Limits::kMaxMapSize : _s4.MapSize;

            gameState.ScenarioFileName = GetRCT1ScenarioName();

            // Do map initialisation, same kind of stuff done when loading scenario editor
            gameStateInitAll(gameState, { mapSize, mapSize });
            gameState.EditorStep = EditorStep::ObjectSelection;
            gameState.Park.Flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
            gameState.ScenarioCategory = SCENARIO_CATEGORY_OTHER;
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
            std::fill(std::begin(_rideTypeToRideEntryMap), std::end(_rideTypeToRideEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_vehicleTypeToRideEntryMap), std::end(_vehicleTypeToRideEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_smallSceneryTypeToEntryMap), std::end(_smallSceneryTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_largeSceneryTypeToEntryMap), std::end(_largeSceneryTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_wallTypeToEntryMap), std::end(_wallTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_bannerTypeToEntryMap), std::end(_bannerTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_pathTypeToEntryMap), std::end(_pathTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_pathAdditionTypeToEntryMap), std::end(_pathAdditionTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_sceneryThemeTypeToEntryMap), std::end(_sceneryThemeTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(
                std::begin(_terrainSurfaceTypeToEntryMap), std::end(_terrainSurfaceTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(std::begin(_terrainEdgeTypeToEntryMap), std::end(_terrainEdgeTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(
                std::begin(_footpathSurfaceTypeToEntryMap), std::end(_footpathSurfaceTypeToEntryMap), kObjectEntryIndexNull);
            std::fill(
                std::begin(_footpathRailingsTypeToEntryMap), std::end(_footpathRailingsTypeToEntryMap), kObjectEntryIndexNull);
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
            AddAvailableEntriesFromBannerList();
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
                        if (rideTypeInResearch[researchItem->RelatedRide])
                        {
                            AddEntryForVehicleType(
                                static_cast<RideType>(researchItem->RelatedRide), static_cast<VehicleType>(researchItem->Item));
                        }
                        break;
                }
            }
        }

        void AddAvailableEntriesFromMap()
        {
            size_t maxTiles = Limits::kMaxMapSize * Limits::kMaxMapSize;
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
                if (sceneryTheme != 0 && _sceneryThemeTypeToEntryMap[sceneryTheme] == kObjectEntryIndexNull)
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
                            case ObjectType::Banners:
                            case ObjectType::PathAdditions:
                            {
                                RCT12::EntryList* entries = GetEntryList(objectType);

                                // Check if there are spare entries available
                                size_t maxEntries = static_cast<size_t>(getObjectEntryGroupCount(objectType));
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

        void AddAvailableEntriesFromBannerList()
        {
            for (size_t i = 0; i < std::size(_s4.Banners); i++)
            {
                auto& banner = _s4.Banners[i];
                auto type = static_cast<BannerType>(banner.Type);
                if (type == BannerType::Null)
                    continue;

                AddEntryForBanner(type);
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

            if (_rideTypeToRideEntryMap[EnumValue(rideType)] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetRideTypeObject(rideType, _gameVersion == FILE_VERSION_RCT1_LL);
                if (!entryName.empty())
                {
                    auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                    _rideTypeToRideEntryMap[EnumValue(rideType)] = entryIndex;
                }
            }
        }

        void AddEntryForVehicleType(RideType rideType, VehicleType vehicleType)
        {
            Guard::Assert(EnumValue(rideType) < std::size(_rideTypeToRideEntryMap));

            if (_vehicleTypeToRideEntryMap[EnumValue(vehicleType)] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetVehicleObject(vehicleType);
                if (!entryName.empty())
                {
                    auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                    _vehicleTypeToRideEntryMap[EnumValue(vehicleType)] = entryIndex;

                    if (rideType != RideType::Null)
                        AddEntryForRideType(rideType);
                }
            }
        }

        void AddEntryForSmallScenery(ObjectEntryIndex smallSceneryType)
        {
            assert(smallSceneryType < std::size(_smallSceneryTypeToEntryMap));
            if (_smallSceneryTypeToEntryMap[smallSceneryType] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetSmallSceneryObject(smallSceneryType);
                auto entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

                _smallSceneryTypeToEntryMap[smallSceneryType] = entryIndex;
            }
        }

        void AddEntryForLargeScenery(ObjectEntryIndex largeSceneryType)
        {
            assert(largeSceneryType < std::size(_largeSceneryTypeToEntryMap));
            if (_largeSceneryTypeToEntryMap[largeSceneryType] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetLargeSceneryObject(largeSceneryType);
                auto entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

                _largeSceneryTypeToEntryMap[largeSceneryType] = entryIndex;
            }
        }

        void AddEntryForWall(ObjectEntryIndex wallType)
        {
            assert(wallType < std::size(_wallTypeToEntryMap));
            if (_wallTypeToEntryMap[wallType] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetWallObject(wallType);
                auto entryIndex = _wallEntries.GetOrAddEntry(entryName);

                _wallTypeToEntryMap[wallType] = entryIndex;
            }
        }

        void AddEntryForBanner(BannerType bannerType)
        {
            assert(EnumValue(bannerType) < std::size(_bannerTypeToEntryMap));
            if (_bannerTypeToEntryMap[EnumValue(bannerType)] == kObjectEntryIndexNull)
            {
                auto entryName = RCT1::GetBannerObject(bannerType);
                auto entryIndex = _bannerEntries.GetOrAddEntry(entryName);

                _bannerTypeToEntryMap[EnumValue(bannerType)] = entryIndex;
            }
        }

        void AddEntryForPathSurface(ObjectEntryIndex pathType)
        {
            assert(pathType < std::size(_footpathSurfaceTypeToEntryMap));
            if (_footpathSurfaceTypeToEntryMap[pathType] == kObjectEntryIndexNull)
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

            if (_pathAdditionTypeToEntryMap[pathAdditionType] == kObjectEntryIndexNull)
            {
                uint8_t normalisedPathAdditionType = RCT1::NormalisePathAddition(pathAdditionType);
                if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == kObjectEntryIndexNull)
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
                if (_sceneryGroupEntries.GetCount() >= kMaxSceneryGroupObjects)
                {
                    Console::WriteLine("Warning: More than %d (max scenery groups) in RCT1 park.", kMaxSceneryGroupObjects);
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
            if (_terrainSurfaceTypeToEntryMap[terrainSurfaceType] == kObjectEntryIndexNull)
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
            if (_terrainEdgeTypeToEntryMap[terrainEdgeType] == kObjectEntryIndexNull)
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
            if (_footpathRailingsTypeToEntryMap[railingsType] == kObjectEntryIndexNull)
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
            for (int32_t i = 0; i < Limits::kMaxRidesInPark; i++)
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
                dst->subtype = _vehicleTypeToRideEntryMap[EnumValue(src->VehicleType)];
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
                dst->type = kRideTypeNull;
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
            if (VehicleTypeIsReversed(src->VehicleType))
            {
                dst->lifecycle_flags |= RIDE_LIFECYCLE_REVERSED_TRAINS;
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

            for (StationIndex::UnderlyingType i = 0; i < Limits::kMaxStationsPerRide; i++)
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
                dstStation.SetBaseZ(src->StationHeights[i] * Limits::kCoordsZStep);
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
            for (int32_t i = Limits::kMaxStationsPerRide; i < OpenRCT2::Limits::kMaxStationsPerRide; i++)
            {
                auto& dstStation = dst->GetStation(StationIndex::FromUnderlying(i));
                dstStation.Start.SetNull();
                dstStation.TrainAtStation = RideStation::kNoTrain;
                dstStation.Entrance.SetNull();
                dstStation.Exit.SetNull();
                dstStation.LastPeepInQueue = EntityId::GetNull();
            }

            dst->num_stations = src->NumStations;

            // Vehicle links (indexes converted later)
            for (int32_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->Vehicles[i]);
            }
            for (int32_t i = Limits::kMaxTrainsPerRide; i <= OpenRCT2::Limits::kMaxTrainsPerRide; i++)
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

            dst->music = kObjectEntryIndexNull;
            if (GetRideTypeDescriptor(dst->type).HasFlag(RtdFlag::allowMusic))
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
            dst->ratings = src->ratings;

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
            dst->dropsPoweredLifts = src->NumDrops;
            dst->start_drop_height = src->StartDropHeight / 2;
            dst->highest_drop_height = src->HighestDropHeight / 2;
            if (src->Type == RideType::MiniatureGolf)
                dst->holes = src->NumInversions & kRCT12InversionAndHoleMask;
            else
                dst->inversions = src->NumInversions & kRCT12InversionAndHoleMask;
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

            dst->music_tune_id = kTuneIDNull;
        }

        void SetRideColourScheme(::Ride* dst, RCT1::Ride* src)
        {
            // Colours
            dst->vehicleColourSettings = src->vehicleColourSettings;
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
                for (int i = 0; i < Limits::kNumColourSchemes; i++)
                {
                    dst->track_colour[i].main = RCT1::GetColour(src->TrackColourMain[i]);
                    dst->track_colour[i].additional = RCT1::GetColour(src->TrackColourAdditional[i]);
                    dst->track_colour[i].supports = RCT1::GetColour(src->TrackColourSupports[i]);
                }
            }

            dst->entrance_style = kObjectEntryIndexNull;
            if (dst->GetRideTypeDescriptor().HasFlag(RtdFlag::hasEntranceAndExit))
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

            if (_gameVersion < FILE_VERSION_RCT1_LL && src->Type == RideType::MerryGoRound)
            {
                // The merry-go-round in pre-LL versions was always yellow with red
                dst->vehicle_colours[0].Body = COLOUR_YELLOW;
                dst->vehicle_colours[0].Trim = COLOUR_BRIGHT_RED;
            }
            else
            {
                for (int i = 0; i < Limits::kMaxTrainsPerRide; i++)
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
        template<typename T>
        void ImportEntity(const RCT12EntityBase& src);

        void ImportEntities()
        {
            for (int i = 0; i < Limits::kMaxEntities; i++)
            {
                ImportEntity(_s4.Entities[i].Unknown);
            }
        }

        void ResetPeepSpriteBounds([[maybe_unused]] GameState_t& gameState)
        {
            // TODO: Entities are currently read from the global state, change this once entities are stored
            // in the passed gameState.
            auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::Guest);
            for (auto* peep : EntityList<Guest>())
            {
                const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
                peep->SpriteData.Width = spriteBounds.sprite_width;
                peep->SpriteData.HeightMin = spriteBounds.sprite_height_negative;
                peep->SpriteData.HeightMax = spriteBounds.sprite_height_positive;
            }

            auto& objManager = GetContext()->GetObjectManager();
            for (auto* peep : EntityList<Staff>())
            {
                animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);
                const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
                peep->SpriteData.Width = spriteBounds.sprite_width;
                peep->SpriteData.HeightMin = spriteBounds.sprite_height_negative;
                peep->SpriteData.HeightMax = spriteBounds.sprite_height_positive;
            }
        }

        void SetVehicleColours(::Vehicle* dst, const RCT1::Vehicle* src)
        {
            const auto& srcRide = _s4.Rides[src->Ride];
            RCT1::VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = RCT1::GetColourSchemeCopyDescriptor(
                srcRide.VehicleType);

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
            dst->AnimationObjectIndex = kObjectEntryIndexNull;
            auto rct12AnimGroup = RCT1::GetPeepAnimationGroup(src->AnimationGroup);
            dst->AnimationGroup = static_cast<::PeepAnimationGroup>(rct12AnimGroup);

            dst->Action = static_cast<PeepActionType>(src->Action);
            dst->SpecialSprite = src->SpecialSprite;
            dst->NextAnimationType = static_cast<PeepAnimationType>(src->NextAnimationType);
            dst->AnimationImageIdOffset = src->AnimationImageIdOffset;
            dst->WalkingAnimationFrameNum = src->NoActionFrameNum;
            dst->AnimationType = static_cast<PeepAnimationType>(src->AnimationType);
            dst->AnimationFrameNum = src->AnimationFrameNum;

            // Peep sprite bounds used to be set here. These are now set in a finishing step after loading peep anim objects.

            dst->MoveTo({ src->x, src->y, src->z });

            dst->Orientation = src->EntityDirection;

            // Peep name
            if (IsUserStringID(src->NameStringID))
            {
                dst->SetName(GetUserString(src->NameStringID));
            }

            dst->State = static_cast<PeepState>(src->State);
            dst->SubState = src->SubState;
            dst->NextLoc = { src->NextX, src->NextY, src->NextZ * Limits::kCoordsZStep };
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

            int32_t peepOffset = staffId * Limits::kPatrolAreaSize;
            for (int32_t i = 0; i < Limits::kPatrolAreaSize; i++)
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
                    staffmember->SetPatrolArea(MapRange(x, y, x + (4 * kCoordsXYStep) - 1, y + (4 * kCoordsXYStep) - 1), true);
                }
            }
        }

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12EntityBase* src)
        {
            dst->Orientation = src->EntityDirection;
            dst->SpriteData.Width = src->SpriteWidth;
            dst->SpriteData.HeightMin = src->SpriteHeightNegative;
            dst->SpriteData.HeightMax = src->SpriteHeightPositive;
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
        }

        void ImportPeepSpawns(GameState_t& gameState)
        {
            gameState.PeepSpawns.clear();
            for (size_t i = 0; i < Limits::kMaxPeepSpawns; i++)
            {
                if (_s4.PeepSpawn[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
                {
                    PeepSpawn spawn = { _s4.PeepSpawn[i].x, _s4.PeepSpawn[i].y, _s4.PeepSpawn[i].z * 16,
                                        _s4.PeepSpawn[i].direction };
                    gameState.PeepSpawns.push_back(spawn);
                }
            }
        }

        void ImportFinance(GameState_t& gameState)
        {
            gameState.Park.EntranceFee = _s4.ParkEntranceFee;
            gameState.LandPrice = ToMoney64(_s4.LandPrice);
            gameState.ConstructionRightsPrice = ToMoney64(_s4.ConstructionRightsPrice);

            gameState.Cash = ToMoney64(_s4.Cash);
            gameState.BankLoan = ToMoney64(_s4.Loan);
            gameState.MaxBankLoan = ToMoney64(_s4.MaxLoan);
            // It's more like 1.33%, but we can only use integers. Can be fixed once we have our own save format.
            gameState.BankLoanInterestRate = 1;
            gameState.InitialCash = ToMoney64(_s4.Cash);

            gameState.CompanyValue = ToMoney64(_s4.CompanyValue);
            gameState.Park.Value = CorrectRCT1ParkValue(_s4.ParkValue);
            gameState.CurrentProfit = ToMoney64(_s4.Profit);

            for (size_t i = 0; i < Limits::kFinanceGraphSize; i++)
            {
                gameState.CashHistory[i] = ToMoney64(_s4.CashHistory[i]);
                gameState.Park.ValueHistory[i] = CorrectRCT1ParkValue(_s4.ParkValueHistory[i]);
                gameState.WeeklyProfitHistory[i] = ToMoney64(_s4.WeeklyProfitHistory[i]);
            }

            for (size_t i = 0; i < Limits::kExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::kExpenditureTypeCount; j++)
                {
                    gameState.ExpenditureTable[i][j] = ToMoney64(_s4.Expenditure[i][j]);
                }
            }
            gameState.CurrentExpenditure = ToMoney64(_s4.TotalExpenditure);

            gameState.ScenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s4.CompletedCompanyValue);
            gameState.TotalAdmissions = _s4.NumAdmissions;
            gameState.TotalIncomeFromAdmissions = ToMoney64(_s4.AdmissionTotalIncome);

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
                    gameState.MarketingCampaigns.push_back(campaign);
                }
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
            AppendRequiredObjects(result, ObjectType::PathAdditions, _pathAdditionEntries);
            AppendRequiredObjects(result, ObjectType::SceneryGroup, _sceneryGroupEntries);
            AppendRequiredObjects(result, ObjectType::Banners, _bannerEntries);
            AppendRequiredObjects(
                result, ObjectType::ParkEntrance, std::vector<std::string_view>({ "rct2.park_entrance.pkent1" }));
            AppendRequiredObjects(result, ObjectType::Water, _waterEntry);
            AppendRequiredObjects(result, ObjectType::TerrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::TerrainEdge, _terrainEdgeEntries);
            AppendRequiredObjects(result, ObjectType::FootpathSurface, _footpathSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::FootpathRailings, _footpathRailingsEntries);
            AppendRequiredObjects(result, ObjectType::PeepNames, std::vector<std::string_view>({ "rct2.peep_names.original" }));
            RCT12AddDefaultObjects(result);

            // Normalise the name to make the scenario as recognisable as possible
            auto normalisedName = ScenarioSources::NormaliseName(_s4.ScenarioName);

            // Infer what scenario text object to use, if any
            SourceDescriptor desc;
            if (ScenarioSources::TryGetByName(normalisedName.c_str(), &desc) && !desc.textObjectId.empty())
                AppendRequiredObjects(result, ObjectType::ScenarioText, std::vector<std::string_view>({ desc.textObjectId }));

            // Add all legacy peep animation objects
            auto animObjects = GetLegacyPeepAnimationObjects(result);
            AppendRequiredObjects(result, ObjectType::PeepAnimations, animObjects);

            return result;
        }

        void ImportTileElements(GameState_t& gameState)
        {
            // Build tile pointer cache (needed to get the first element at a certain location)
            auto tilePointerIndex = TilePointerIndex<RCT12TileElement>(
                Limits::kMaxMapSize, _s4.TileElements, std::size(_s4.TileElements));

            std::vector<TileElement> tileElements;
            const auto maxSize = _s4.MapSize == 0 ? Limits::kMaxMapSize : _s4.MapSize;
            for (TileCoordsXY coords = { 0, 0 }; coords.y < kMaximumMapSizeTechnical; coords.y++)
            {
                for (coords.x = 0; coords.x < kMaximumMapSizeTechnical; coords.x++)
                {
                    auto tileAdded = false;
                    if (coords.x < maxSize && coords.y < maxSize)
                    {
                        // This is the equivalent of MapGetFirstElementAt(x, y), but on S4 data.
                        RCT12TileElement* srcElement = tilePointerIndex.GetFirstElementAt(coords);
                        do
                        {
                            if (srcElement->BaseHeight == Limits::kMaxElementHeight)
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

            SetTileElements(gameState, std::move(tileElements));
            FixEntrancePositions(gameState);
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

            dst->SetBaseZ(src->BaseHeight * Limits::kCoordsZStep);
            dst->SetClearanceZ(src->ClearanceHeight * Limits::kCoordsZStep);

            switch (tileElementType)
            {
                case TileElementType::Surface:
                {
                    auto dst2 = dst->AsSurface();
                    auto src2 = src->AsSurface();

                    auto surfaceStyle = _terrainSurfaceTypeToEntryMap[src2->GetSurfaceStyle()];
                    auto edgeStyle = _terrainEdgeTypeToEntryMap[src2->GetEdgeStyle()];

                    dst2->SetSlope(src2->GetSlope());
                    dst2->SetSurfaceObjectIndex(surfaceStyle);
                    dst2->SetEdgeObjectIndex(edgeStyle);
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
                    dst2->SetAddition(0);
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
                    ObjectEntryIndex additionType = src2->GetAddition();
                    if (additionType != RCT1_PATH_ADDITION_NONE)
                    {
                        ObjectEntryIndex normalisedType = RCT1::NormalisePathAddition(additionType);
                        entryIndex = _pathAdditionTypeToEntryMap[normalisedType];
                        if (additionType != normalisedType)
                        {
                            dst2->SetIsBroken(true);
                        }
                        dst2->SetAdditionEntryIndex(entryIndex);
                    }
                    return 1;
                }
                case TileElementType::Track:
                {
                    auto dst2 = dst->AsTrack();
                    auto src2 = src->AsTrack();
                    const auto* ride = GetRide(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    auto rideType = (ride != nullptr) ? ride->type : kRideTypeNull;
                    auto rct1RideType = _s4.Rides[src2->GetRideIndex()].Type;

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
                    if (rct1RideType == RideType::GhostTrain)
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
                    // Brakes import as closed to preserve legacy behaviour
                    dst2->SetBrakeClosed(trackType == TrackElemType::Brakes);
                    if (TrackTypeHasSpeedSetting(trackType))
                    {
                        dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                    }
                    else if (trackType == TrackElemType::OnRidePhoto)
                    {
                        dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                    }

                    // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                    if (rct1RideType == RideType::HedgeMaze)
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
                        auto baseZ = src->BaseHeight * Limits::kCoordsZStep;
                        auto clearanceZ = src->ClearanceHeight * Limits::kCoordsZStep;
                        auto edgeSlope = GetWallSlopeFromEdgeSlope(slope, edge & 3);
                        if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
                        {
                            clearanceZ += kLandHeightStep;
                        }
                        if (edgeSlope & EDGE_SLOPE_ELEVATED)
                        {
                            edgeSlope &= ~EDGE_SLOPE_ELEVATED;
                            baseZ += kLandHeightStep;
                            clearanceZ += kLandHeightStep;
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

        void ImportResearch(GameState_t& gameState)
        {
            // All available objects must be loaded before this method is called as it
            // requires them to correctly insert objects into the research list

            ResearchResetItems(gameState);

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
                        if (sceneryGroupEntryIndex != ObjectEntryIndexIgnore && sceneryGroupEntryIndex != kObjectEntryIndexNull)
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
                            ownRideEntryIndex != kObjectEntryIndexNull, "ownRideEntryIndex was kObjectEntryIndexNull");

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
            gameState.ResearchPriorities = activeResearchTypes;
            gameState.ResearchFundingLevel = _s4.ResearchLevel;

            // This will mark items as researched/unresearched according to the research list.
            // This needs to be called before importing progress, as it will reset it.
            ResearchResetCurrentItem();

            // Research history
            gameState.ResearchProgress = _s4.ResearchProgress;
            gameState.ResearchProgressStage = _s4.ResearchProgressStage;
            gameState.ResearchExpectedDay = _s4.NextResearchExpectedDay;
            gameState.ResearchExpectedMonth = _s4.NextResearchExpectedMonth;

            if (_s4.LastResearchFlags == 0xFF)
            {
                gameState.ResearchLastItem = std::nullopt;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.LastResearchItem, _s4.LastResearchType);
                gameState.ResearchLastItem = researchItem;
            }

            if (_s4.NextResearchFlags == 0xFF)
            {
                gameState.ResearchNextItem = std::nullopt;
                gameState.ResearchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                gameState.ResearchProgress = 0;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.NextResearchItem, _s4.NextResearchType);
                gameState.ResearchNextItem = researchItem;
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
            // RCT1 research sometimes contain vehicles that aren’t actually researched.
            // In such cases, `_vehicleTypeToRideEntryMap` will return kObjectEntryIndexNull. This is expected.
            auto rideEntryIndex = _vehicleTypeToRideEntryMap[vehicle];

            if (rideEntryIndex < std::size(_researchRideEntryUsed) && !_researchRideEntryUsed[rideEntryIndex])
            {
                _researchRideEntryUsed[rideEntryIndex] = true;
                ResearchInsertRideEntry(rideEntryIndex, researched);
            }
        }

        void ImportParkName(GameState_t& gameState)
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

            auto& park = gameState.Park;
            park.Name = std::move(parkName);
        }

        void ImportParkFlags(GameState_t& gameState)
        {
            // Date and srand
            gameState.CurrentTicks = _s4.Ticks;
            ScenarioRandSeed(_s4.RandomA, _s4.RandomB);
            gameState.Date = Date{ _s4.Month, _s4.Day };

            // Park rating
            gameState.Park.Rating = _s4.ParkRating;

            Park::ResetHistories(gameState);
            for (size_t i = 0; i < std::size(_s4.ParkRatingHistory); i++)
            {
                if (_s4.ParkRatingHistory[i] != RCT12ParkHistoryUndefined)
                {
                    gameState.Park.RatingHistory[i] = _s4.ParkRatingHistory[i] * RCT12ParkRatingHistoryFactor;
                }
            }
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != RCT12ParkHistoryUndefined)
                {
                    gameState.GuestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * RCT12GuestsInParkHistoryFactor;
                }
            }

            // Awards
            auto& currentAwards = gameState.CurrentAwards;
            for (auto& src : _s4.Awards)
            {
                if (src.Time != 0)
                {
                    currentAwards.push_back(Award{ src.Time, static_cast<AwardType>(src.Type) });
                }
            }

            // Number of guests history
            std::fill(
                std::begin(gameState.GuestsInParkHistory), std::end(gameState.GuestsInParkHistory),
                std::numeric_limits<uint32_t>::max());
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != std::numeric_limits<uint8_t>::max())
                {
                    gameState.GuestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * 20;
                }
            }

            // News items
            for (size_t i = 0; i < Limits::MaxNewsItems; i++)
            {
                const RCT12NewsItem* src = &_s4.Messages[i];
                News::Item* dst = &gameState.NewsItems[i];

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
            gameState.GuestInitialCash = ToMoney64(_s4.GuestInitialCash);
            gameState.GuestInitialHunger = _s4.GuestInitialHunger;
            gameState.GuestInitialThirst = _s4.GuestInitialThirst;
            gameState.GuestInitialHappiness = _s4.GuestInitialHappiness;

            gameState.GuestGenerationProbability = _s4.GuestGenerationProbability;

            // Staff colours
            gameState.StaffHandymanColour = RCT1::GetColour(_s4.HandymanColour);
            gameState.StaffMechanicColour = RCT1::GetColour(_s4.MechanicColour);
            gameState.StaffSecurityColour = RCT1::GetColour(_s4.SecurityGuardColour);

            // Flags
            gameState.Park.Flags = _s4.ParkFlags;
            gameState.Park.Flags &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;
            gameState.Park.Flags |= PARK_FLAGS_RCT1_INTEREST;
            // Loopy Landscape parks can set a flag to lock the entry price to free.
            // If this flag is not set, the player can ask money for both rides and entry.
            if (!(_s4.ParkFlags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
            {
                gameState.Park.Flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
            }

            gameState.Park.Size = _s4.ParkSize;
            gameState.TotalRideValueForMoney = _s4.TotalRideValueForMoney;
            gameState.SamePriceThroughoutPark = 0;
            if (_gameVersion == FILE_VERSION_RCT1_LL)
            {
                gameState.SamePriceThroughoutPark = _s4.SamePriceThroughout;
            }
        }

        void ConvertResearchEntry(::ResearchItem* dst, uint8_t srcItem, uint8_t srcType)
        {
            dst->SetNull();
            if (srcType == RCT1_RESEARCH_TYPE_RIDE)
            {
                auto entryIndex = _rideTypeToRideEntryMap[srcItem];

                if (entryIndex != kObjectEntryIndexNull)
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

                if (entryIndex != kObjectEntryIndexNull)
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

                if (entryIndex != ObjectEntryIndexIgnore && entryIndex != kObjectEntryIndexNull)
                {
                    dst->entryIndex = entryIndex;
                    dst->type = Research::EntryType::Scenery;
                    dst->category = ResearchCategory::SceneryGroup;
                    dst->baseRideType = 0;
                    dst->flags = 0;
                }
            }
        }

        void ImportClimate(GameState_t& gameState)
        {
            gameState.Climate = ClimateType{ _s4.Climate };
            gameState.ClimateUpdateTimer = _s4.ClimateTimer;
            gameState.ClimateCurrent.Temperature = _s4.Temperature;
            gameState.ClimateCurrent.Weather = WeatherType{ _s4.Weather };
            gameState.ClimateCurrent.WeatherEffect = WeatherEffectType::None;
            gameState.ClimateCurrent.WeatherGloom = _s4.WeatherGloom;
            gameState.ClimateCurrent.Level = static_cast<WeatherLevel>(_s4.Rain);
            gameState.ClimateNext.Temperature = _s4.TargetTemperature;
            gameState.ClimateNext.Weather = WeatherType{ _s4.TargetWeather };
            gameState.ClimateNext.WeatherEffect = WeatherEffectType::None;
            gameState.ClimateNext.WeatherGloom = _s4.TargetWeatherGloom;
            gameState.ClimateNext.Level = static_cast<WeatherLevel>(_s4.TargetRain);
        }

        void ImportScenarioNameDetails(GameState_t& gameState)
        {
            std::string name = String::toStd(_s4.ScenarioName);
            std::string parkName;
            std::string details;

            int32_t scNumber = _s4.ScenarioSlotIndex;
            if (scNumber != -1)
            {
                SourceDescriptor desc;
                if (ScenarioSources::TryGetById(scNumber, &desc) && !desc.textObjectId.empty())
                {
                    auto& objManager = GetContext()->GetObjectManager();

                    // Ensure only one thread talks to the object manager at a time
                    std::lock_guard lock(mtx);

                    // Unload loaded scenario text object, if any.
                    if (auto* obj = objManager.GetLoadedObject(ObjectType::ScenarioText, 0); obj != nullptr)
                        objManager.UnloadObjects({ obj->GetDescriptor() });

                    // Load the one specified
                    if (auto* obj = objManager.LoadObject(desc.textObjectId); obj != nullptr)
                    {
                        auto* textObject = reinterpret_cast<ScenarioTextObject*>(obj);
                        name = textObject->GetScenarioName();
                        parkName = textObject->GetParkName();
                        details = textObject->GetScenarioDetails();
                    }
                }
            }

            gameState.ScenarioName = std::move(name);
            gameState.ScenarioDetails = std::move(details);
            if (_isScenario && !parkName.empty())
            {
                auto& park = GetGameState().Park;
                park.Name = std::move(parkName);
            }
        }

        void ImportScenarioObjective(GameState_t& gameState)
        {
            gameState.ScenarioObjective.Type = _s4.ScenarioObjectiveType;
            gameState.ScenarioObjective.Year = _s4.ScenarioObjectiveYears;
            gameState.ScenarioObjective.NumGuests = _s4.ScenarioObjectiveNumGuests;

            // RCT1 used a different way of calculating the park value.
            // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
            // we need to check the goal to avoid affecting scenarios like Volcania.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_PARK_VALUE_BY)
                gameState.ScenarioObjective.Currency = CorrectRCT1ParkValue(_s4.ScenarioObjectiveCurrency);
            else
                gameState.ScenarioObjective.Currency = ToMoney64(_s4.ScenarioObjectiveCurrency);

            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.ScenarioObjectiveType == OBJECTIVE_BUILD_THE_BEST)
                gameState.ScenarioObjective.RideId = GetBuildTheBestRideId();
        }

        void ImportSavedView(GameState_t& gameState)
        {
            gameState.SavedView = ScreenCoordsXY{ _s4.ViewX, _s4.ViewY };
            gameState.SavedViewZoom = ZoomLevel{ static_cast<int8_t>(_s4.ViewZoom) };
            gameState.SavedViewRotation = _s4.ViewRotation;
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
            auto type = RCTEntryIndexToOpenRCT2EntryIndex(src->Type);
            if (type < std::size(_bannerTypeToEntryMap))
                type = _bannerTypeToEntryMap[type];
            else
                type = kObjectEntryIndexNull;
            dst->type = type;

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

        void FixEntrancePositions(GameState_t& gameState)
        {
            gameState.Park.Entrances.clear();
            TileElementIterator it;
            TileElementIteratorBegin(&it);
            while (TileElementIteratorNext(&it) && gameState.Park.Entrances.size() < Limits::kMaxParkEntrances)
            {
                TileElement* element = it.element;

                if (element->GetType() != TileElementType::Entrance)
                    continue;
                if (element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    continue;
                if ((element->AsEntrance()->GetSequenceIndex()) != 0)
                    continue;

                CoordsXYZD entrance = { TileCoordsXY(it.x, it.y).ToCoordsXY(), element->GetBaseZ(), element->GetDirection() };
                gameState.Park.Entrances.push_back(entrance);
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
                case ObjectType::Banners:
                    return &_bannerEntries;
                case ObjectType::Paths:
                    return &_pathEntries;
                case ObjectType::PathAdditions:
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
            const auto originalString = _s4.StringTable[stringId % 1024];
            auto originalStringView = std::string_view(
                originalString, RCT12::GetRCTStringBufferLen(originalString, kUserStringMaxLength));
            auto asUtf8 = RCT2StringToUTF8(originalStringView, RCT2LanguageId::EnglishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        void FixNextGuestNumber(GameState_t& gameState)
        {
            // In RCT1, the next guest number is not saved, so we have to calculate it.
            // This is done by finding the highest guest number in the park, and adding 1.
            uint32_t nextGuestNumber = 0;

            // TODO: Entities are currently read from the global state, change this once entities are stored
            // in the passed gameState.
            for (auto peep : EntityList<Guest>())
            {
                nextGuestNumber = std::max(nextGuestNumber, peep->PeepId);
            }

            gameState.NextGuestNumber = nextGuestNumber + 1;
        }

        /**
         * Counts the block sections. The reason this iterates over the map is to avoid getting into infinite loops,
         * which can happen with hacked parks.
         */
        void CountBlockSections()
        {
            for (int32_t x = 0; x < Limits::kMaxMapSize; x++)
            {
                for (int32_t y = 0; y < Limits::kMaxMapSize; y++)
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
                    return RCT1::GetRideType(static_cast<RideType>(researchList[i].Item), static_cast<VehicleType>(0));
                }
            }

            return kRideTypeNull;
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

    template<>
    void S4Importer::ImportEntity<::Vehicle>(const RCT12EntityBase& srcBase)
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
        dst->SpriteData.Width = src->SpriteWidth;
        dst->SpriteData.HeightMin = src->SpriteHeightNegative;
        dst->SpriteData.HeightMax = src->SpriteHeightPositive;
        dst->Orientation = src->EntityDirection;

        dst->SpriteData.SpriteRect = ScreenRect(src->SpriteLeft, src->SpriteTop, src->SpriteRight, src->SpriteBottom);

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
            dst->SetTrackType(OpenRCT2::TrackElemType::Flat);
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
        dst->BlockBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;

        if (VehicleTypeIsReversed(rct1Ride.VehicleType))
        {
            dst->SetFlag(VehicleFlags::CarIsReversed);
        }
    }

    template<>
    void S4Importer::ImportEntity<Guest>(const RCT12EntityBase& srcBase)
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
                dstThought->item = kPeepThoughtItemNone;
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
    }

    template<>
    void S4Importer::ImportEntity<Staff>(const RCT12EntityBase& srcBase)
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

    template<>
    void S4Importer::ImportEntity<Litter>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<Litter>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityLitter*>(&srcBase);
        ImportEntityCommonProperties(dst, src);

        dst->SubType = Litter::Type(src->Type);
        dst->creationTick = src->CreationTick;
    }

    template<>
    void S4Importer::ImportEntity<SteamParticle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<SteamParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntitySteamParticle*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_move = src->TimeToMove;
    }

    template<>
    void S4Importer::ImportEntity<MoneyEffect>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<MoneyEffect>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityMoneyEffect*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->MoveDelay;
        dst->NumMovements = src->NumMovements;
        dst->GuestPurchase = src->Vertical;
        dst->Value = src->Value;
        dst->OffsetX = src->OffsetX;
        dst->Wiggle = src->Wiggle;
    }

    template<>
    void S4Importer::ImportEntity<VehicleCrashParticle>(const RCT12EntityBase& srcBase)
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

    template<>
    void S4Importer::ImportEntity<ExplosionCloud>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionCloud>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<ExplosionFlare>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<ExplosionFlare>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<CrashSplashParticle>(const RCT12EntityBase& srcBase)
    {
        auto* dst = CreateEntityAt<CrashSplashParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<JumpingFountain>(const RCT12EntityBase& srcBase)
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

    template<>
    void S4Importer::ImportEntity<Balloon>(const RCT12EntityBase& srcBase)
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

    template<>
    void S4Importer::ImportEntity<Duck>(const RCT12EntityBase& srcBase)
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
} // namespace OpenRCT2::RCT1

std::unique_ptr<IParkImporter> ParkImporter::CreateS4()
{
    return std::make_unique<RCT1::S4Importer>();
}
