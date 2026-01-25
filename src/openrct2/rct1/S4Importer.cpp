/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "../audio/Audio.h"
#include "../core/BitSet.hpp"
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
#include "../entity/Fountain.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/PatrolArea.h"
#include "../entity/Peep.h"
#include "../entity/Staff.h"
#include "../localisation/Formatting.h"
#include "../management/Award.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/PeepAnimationsObject.h"
#include "../object/ScenarioMetaObject.h"
#include "../park/Legacy.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/CSStringConverter.h"
#include "../rct12/EntryList.h"
#include "../rct12/ScenarioPatcher.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/Track.h"
#include "../ride/Vehicle.h"
#include "../sawyer_coding/SawyerCoding.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioObjective.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../world/Climate.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TilePointerIndex.hpp"
#include "../world/Wall.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"
#include "RCT1.h"
#include "Tables.h"

#include <cassert>
#include <memory>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::SawyerCoding;

static constexpr ObjectEntryIndex ObjectEntryIndexIgnore = 254;

namespace OpenRCT2::RCT1
{
    constexpr uint8_t kDefaultParkValueConversionFactor = 100;

    class S4Importer final : public IParkImporter
    {
    private:
        std::string _s4Path;
        S4 _s4 = {};
        uint8_t _gameVersion = 0;
        uint8_t _parkValueConversionFactor = kDefaultParkValueConversionFactor;
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
        ObjectEntryIndex _rideTypeToRideEntryMap[EnumValue(RideType::count)]{};
        ObjectEntryIndex _vehicleTypeToRideEntryMap[EnumValue(VehicleType::count)]{};
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

        // Research
        BitSet<kMaxRideObjects> _researchRideEntryUsed{};
        BitSet<EnumValue(RideType::count)> _researchRideTypeUsed{};

        // Scenario repository - used for determining scenario name
        IScenarioRepository* _scenarioRepository = GetScenarioRepository();

    public:
        ParkLoadResult Load(const u8string& path, const bool skipObjectCheck) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::iequals(extension, ".sc4"))
            {
                return LoadScenario(path, skipObjectCheck);
            }
            if (String::iequals(extension, ".sv4"))
            {
                return LoadSavedGame(path, skipObjectCheck);
            }

            throw std::runtime_error("Invalid RCT1 park extension.");
        }

        ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FileMode::open);
            auto result = LoadFromStream(&fs, false, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FileMode::open);
            auto result = LoadFromStream(&fs, true, skipObjectCheck, path);
            return result;
        }

        ParkLoadResult LoadFromStream(
            IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck, const u8string& path) override
        {
            _s4 = *ReadAndDecodeS4(stream, isScenario);
            _s4Path = path;
            _isScenario = isScenario;
            _gameVersion = DetectRCT1Version(_s4.GameVersion) & FILE_VERSION_MASK;

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
            ImportEntities(gameState);
            ImportTileElements(gameState);
            ImportMapAnimations();
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
            SetDefaultNames(gameState);
            DetermineRideEntranceAndExitLocations();

            ResearchDetermineFirstOfType();

            CheatsReset();
            ClearRestrictedScenery();
            RestrictAllMiscScenery();
        }

        bool PopulateIndexEntry(ScenarioIndexEntry* dst) override
        {
            *dst = {};

            SourceDescriptor desc;
            // If no entry is found, this is a custom scenario.
            bool isOfficial = ScenarioSources::TryGetById(_s4.ScenarioSlotIndex, &desc);

            // Perform an additional name check if this is detected to be a competition scenario
            if (isOfficial && desc.category == Scenario::Category::competitions)
                isOfficial = ScenarioSources::TryGetByName(_s4.ScenarioName, &desc);

            dst->Category = desc.category;
            dst->SourceGame = ScenarioSource{ desc.source };
            dst->SourceIndex = desc.index;
            dst->ScenarioId = desc.id;

            dst->ObjectiveType = _s4.ScenarioObjectiveType;
            dst->ObjectiveArg1 = _s4.ScenarioObjectiveYears;
            // RCT1 used another way of calculating park value.
            if (_s4.ScenarioObjectiveType == Scenario::ObjectiveType::parkValueBy)
                dst->ObjectiveArg2 = CorrectRCT1ParkValue(_s4.ScenarioObjectiveCurrency);
            else
                dst->ObjectiveArg2 = _s4.ScenarioObjectiveCurrency;
            dst->ObjectiveArg3 = _s4.ScenarioObjectiveNumGuests;
            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.ScenarioObjectiveType == Scenario::ObjectiveType::buildTheBest)
            {
                dst->ObjectiveArg3 = GetBuildTheBestRideId();
            }

            std::string name = RCT2StringToUTF8(_s4.ScenarioName, RCT2LanguageId::englishUK);
            std::string details;

            // TryGetById won't set this property if the scenario is not recognised,
            // but localisation needs it.
            if (!isOfficial)
            {
                desc.title = name.c_str();
            }

            dst->InternalName = desc.title;

            if (!desc.textObjectId.empty())
            {
                auto& objManager = GetContext()->GetObjectManager();

                // Load the one specified
                if (auto obj = objManager.LoadTempObject(desc.textObjectId, false); obj != nullptr)
                {
                    auto& textObject = reinterpret_cast<ScenarioMetaObject&>(*obj);
                    name = textObject.GetScenarioName();
                    details = textObject.GetScenarioDetails();
                }
            }

            dst->Name = name;
            dst->Details = details;

            return true;
        }

        ParkPreview GetParkPreview() override
        {
            return {};
        }

    private:
        uint8_t calculateParkValueConversionFactor(const Park::ParkData& park, const GameState_t& gameState)
        {
            if (_s4.ParkValue == 0)
                return kDefaultParkValueConversionFactor;

            // Use the ratio between the old and new park value to calculate the ratio to
            // use for the park value history and the goal.
            return (Park::CalculateParkValue(park, gameState) * 10) / _s4.ParkValue;
        }

        money64 CorrectRCT1ParkValue(money32 oldParkValue)
        {
            if (oldParkValue == kMoney32Undefined)
            {
                return kMoney64Undefined;
            }

            assert(_parkValueConversionFactor != 0);

            return (oldParkValue * _parkValueConversionFactor) / 10;
        }

        std::unique_ptr<S4> ReadAndDecodeS4(IStream* stream, bool isScenario)
        {
            auto s4 = std::make_unique<S4>();
            size_t dataSize = stream->GetLength() - stream->GetPosition();
            auto data = stream->ReadArray<uint8_t>(dataSize);
            auto decodedData = std::make_unique<uint8_t[]>(sizeof(S4));

            size_t decodedSize;
            int32_t fileType = DetectFileType(data.get(), dataSize);
            if (isScenario && (fileType & FILE_VERSION_MASK) != FILE_VERSION_RCT1)
            {
                decodedSize = DecodeSC4(data.get(), decodedData.get(), dataSize, sizeof(S4));
            }
            else
            {
                decodedSize = DecodeSV4(data.get(), decodedData.get(), dataSize, sizeof(S4));
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
            _parkValueConversionFactor = kDefaultParkValueConversionFactor;

            uint16_t mapSize = _s4.MapSize == 0 ? Limits::kMaxMapSize : _s4.MapSize;

            gameState.scenarioFileName = GetRCT1ScenarioName();

            // Do map initialisation, same kind of stuff done when loading scenario editor
            gameStateInitAll(gameState, { mapSize, mapSize });
            gameState.editorStep = EditorStep::ObjectSelection;
            gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
            gameState.scenarioOptions.category = Scenario::Category::other;
        }

        std::string GetRCT1ScenarioName()
        {
            const ScenarioIndexEntry* scenarioEntry = _scenarioRepository->GetByInternalName(_s4.ScenarioName);
            if (scenarioEntry == nullptr)
            {
                return {};
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
            _sceneryGroupEntries.AddRange(
                {
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

            // All four are always available. By using the same order as RCT1, we don’t need to map the indices later on.
            _footpathRailingsEntries.AddRange(
                {
                    "rct2.footpath_railings.wood",     // RCT1_PATH_SUPPORT_TYPE_TRUSS
                    "rct2.footpath_railings.concrete", // RCT1_PATH_SUPPORT_TYPE_COATED_WOOD
                    "rct1ll.footpath_railings.space",  // RCT1_PATH_SUPPORT_TYPE_SPACE
                    "rct1ll.footpath_railings.bamboo", // RCT1_PATH_SUPPORT_TYPE_BAMBOO
                });

            // Add default surfaces
            _terrainSurfaceEntries.AddRange(
                { "rct2.terrain_surface.grass", "rct2.terrain_surface.sand", "rct2.terrain_surface.dirt",
                  "rct2.terrain_surface.rock", "rct2.terrain_surface.martian", "rct2.terrain_surface.chequerboard",
                  "rct2.terrain_surface.grass_clumps", "rct2.terrain_surface.ice", "rct2.terrain_surface.grid_red",
                  "rct2.terrain_surface.grid_yellow", "rct2.terrain_surface.grid_purple", "rct2.terrain_surface.grid_green",
                  "rct2.terrain_surface.sand_red", "rct2.terrain_surface.sand_brown", "rct1aa.terrain_surface.roof_red",
                  "rct1ll.terrain_surface.roof_grey", "rct1ll.terrain_surface.rust", "rct1ll.terrain_surface.wood" });

            // Add default edges
            _terrainEdgeEntries.AddRange(
                { "rct2.terrain_edge.rock", "rct2.terrain_edge.wood_red", "rct2.terrain_edge.wood_black",
                  "rct2.terrain_edge.ice", "rct1.terrain_edge.brick", "rct1.terrain_edge.iron", "rct1aa.terrain_edge.grey",
                  "rct1aa.terrain_edge.yellow", "rct1aa.terrain_edge.red", "rct1ll.terrain_edge.purple",
                  "rct1ll.terrain_edge.green", "rct1ll.terrain_edge.stone_brown", "rct1ll.terrain_edge.stone_grey",
                  "rct1ll.terrain_edge.skyscraper_a", "rct1ll.terrain_edge.skyscraper_b" });
        }

        void AddAvailableEntriesFromResearchList()
        {
            size_t researchListCount;
            const ResearchItem* researchList = GetResearchList(&researchListCount);
            BitSet<EnumValue(RideType::count)> rideTypeInResearch = GetRideTypesPresentInResearchList(
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
                    case RCT12TileElementType::surface:
                    {
                        auto surfaceEl = tileElement->AsSurface();
                        auto surfaceStyle = surfaceEl->GetSurfaceStyle();
                        auto edgeStyle = surfaceEl->GetEdgeStyle();
                        AddEntryForTerrainSurface(surfaceStyle);
                        AddEntryForTerrainEdge(edgeStyle);
                        break;
                    }
                    case RCT12TileElementType::path:
                    {
                        uint8_t pathType = tileElement->AsPath()->GetRCT1PathType();
                        uint8_t pathAdditionsType = tileElement->AsPath()->GetAddition();

                        AddEntryForPathAddition(pathAdditionsType);
                        AddEntryForPathSurface(pathType);
                        break;
                    }
                    case RCT12TileElementType::smallScenery:
                        AddEntryForSmallScenery(tileElement->AsSmallScenery()->GetEntryIndex());
                        break;
                    case RCT12TileElementType::largeScenery:
                        AddEntryForLargeScenery(tileElement->AsLargeScenery()->GetEntryIndex());
                        break;
                    case RCT12TileElementType::wall:
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
                if (ride->type != RideType::null)
                {
                    if (RideTypeUsesVehicles(ride->type))
                        AddEntryForVehicleType(ride->type, ride->vehicleType);
                    else
                        AddEntryForRideType(ride->type);
                }
            }
        }

        void AddAvailableEntriesFromSceneryGroups()
        {
            for (int32_t sceneryTheme = 0; sceneryTheme <= RCT1_SCENERY_THEME_PAGODA; sceneryTheme++)
            {
                if (sceneryTheme != 0 && _sceneryThemeTypeToEntryMap[sceneryTheme] == kObjectEntryIndexNull)
                    continue;

                auto objects = GetSceneryObjects(sceneryTheme);
                for (auto objectName : objects)
                {
                    auto& objectRepository = GetContext()->GetObjectRepository();
                    auto foundObject = objectRepository.FindObject(objectName);
                    if (foundObject != nullptr)
                    {
                        auto objectType = foundObject->Type;
                        switch (objectType)
                        {
                            case ObjectType::smallScenery:
                            case ObjectType::largeScenery:
                            case ObjectType::walls:
                            case ObjectType::banners:
                            case ObjectType::pathAdditions:
                            {
                                RCT12::EntryList* entries = GetEntryList(objectType);

                                // Check if there are spare entries available
                                size_t maxEntries = getObjectEntryGroupCount(objectType);
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
                if (type == BannerType::null)
                    continue;

                AddEntryForBanner(type);
            }
        }

        void AddEntryForWater()
        {
            std::string_view entryName;
            if (_gameVersion < FILE_VERSION_RCT1_LL)
            {
                entryName = GetWaterObject(RCT1_WATER_CYAN);
            }
            else
            {
                entryName = GetWaterObject(_s4.WaterColour);
            }
            _waterEntry.GetOrAddEntry(entryName);
        }

        void AddEntryForRideType(RideType rideType)
        {
            Guard::Assert(EnumValue(rideType) < std::size(_rideTypeToRideEntryMap));

            if (_rideTypeToRideEntryMap[EnumValue(rideType)] == kObjectEntryIndexNull)
            {
                auto entryName = GetRideTypeObject(rideType, _gameVersion == FILE_VERSION_RCT1_LL);
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
                auto entryName = GetVehicleObject(vehicleType);
                if (!entryName.empty())
                {
                    auto entryIndex = _rideEntries.GetOrAddEntry(entryName);
                    _vehicleTypeToRideEntryMap[EnumValue(vehicleType)] = entryIndex;

                    if (rideType != RideType::null)
                        AddEntryForRideType(rideType);
                }
            }
        }

        void AddEntryForSmallScenery(ObjectEntryIndex smallSceneryType)
        {
            assert(smallSceneryType < std::size(_smallSceneryTypeToEntryMap));
            if (_smallSceneryTypeToEntryMap[smallSceneryType] == kObjectEntryIndexNull)
            {
                auto entryName = GetSmallSceneryObject(smallSceneryType);
                auto entryIndex = _smallSceneryEntries.GetOrAddEntry(entryName);

                _smallSceneryTypeToEntryMap[smallSceneryType] = entryIndex;
            }
        }

        void AddEntryForLargeScenery(ObjectEntryIndex largeSceneryType)
        {
            assert(largeSceneryType < std::size(_largeSceneryTypeToEntryMap));
            if (_largeSceneryTypeToEntryMap[largeSceneryType] == kObjectEntryIndexNull)
            {
                auto entryName = GetLargeSceneryObject(largeSceneryType);
                auto entryIndex = _largeSceneryEntries.GetOrAddEntry(entryName);

                _largeSceneryTypeToEntryMap[largeSceneryType] = entryIndex;
            }
        }

        void AddEntryForWall(ObjectEntryIndex wallType)
        {
            assert(wallType < std::size(_wallTypeToEntryMap));
            if (_wallTypeToEntryMap[wallType] == kObjectEntryIndexNull)
            {
                auto entryName = GetWallObject(wallType);
                auto entryIndex = _wallEntries.GetOrAddEntry(entryName);

                _wallTypeToEntryMap[wallType] = entryIndex;
            }
        }

        void AddEntryForBanner(BannerType bannerType)
        {
            assert(EnumValue(bannerType) < std::size(_bannerTypeToEntryMap));
            if (_bannerTypeToEntryMap[EnumValue(bannerType)] == kObjectEntryIndexNull)
            {
                auto entryName = GetBannerObject(bannerType);
                auto entryIndex = _bannerEntries.GetOrAddEntry(entryName);

                _bannerTypeToEntryMap[EnumValue(bannerType)] = entryIndex;
            }
        }

        void AddEntryForPathSurface(ObjectEntryIndex pathType)
        {
            assert(pathType < std::size(_footpathSurfaceTypeToEntryMap));
            if (_footpathSurfaceTypeToEntryMap[pathType] == kObjectEntryIndexNull)
            {
                auto identifier = GetPathSurfaceObject(pathType);
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
                uint8_t normalisedPathAdditionType = NormalisePathAddition(pathAdditionType);
                if (_pathAdditionTypeToEntryMap[normalisedPathAdditionType] == kObjectEntryIndexNull)
                {
                    auto entryName = GetPathAddtionObject(normalisedPathAdditionType);
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
                auto entryName = GetSceneryGroupObject(sceneryThemeType);
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
                auto identifier = GetTerrainSurfaceObject(terrainSurfaceType);
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
                auto identifier = GetTerrainEdgeObject(terrainEdgeType);
                if (!identifier.empty())
                {
                    auto entryIndex = _terrainEdgeEntries.GetOrAddEntry(identifier);
                    _terrainEdgeTypeToEntryMap[terrainEdgeType] = entryIndex;
                }
            }
        }

        void ImportRides()
        {
            for (int32_t i = 0; i < Limits::kMaxRidesInPark; i++)
            {
                if (_s4.Rides[i].type != RideType::null)
                {
                    const auto rideId = RideId::FromUnderlying(i);
                    ImportRide(RideAllocateAtIndex(rideId), &_s4.Rides[i], rideId);
                }
            }
        }

        void ImportRide(::Ride* dst, Ride* src, RideId rideIndex)
        {
            *dst = {};
            dst->id = rideIndex;

            // This is a peculiarity of this exact version number, which only Heide-Park seems to use.
            if (_s4.GameVersion == 110018 && src->type == RideType::invertedRollerCoaster)
            {
                dst->type = RIDE_TYPE_COMPACT_INVERTED_COASTER;
            }
            else
            {
                dst->type = GetRideType(src->type, src->vehicleType);
            }

            if (RideTypeUsesVehicles(src->type))
            {
                dst->subtype = _vehicleTypeToRideEntryMap[EnumValue(src->vehicleType)];
            }
            else
            {
                dst->subtype = _rideTypeToRideEntryMap[EnumValue(src->type)];
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
            if (IsUserStringID(src->name))
            {
                dst->customName = GetUserString(src->name);
            }

            dst->status = static_cast<RideStatus>(src->status);

            // Flags
            dst->lifecycleFlags = src->lifecycleFlags;
            // These flags were not in the base game
            if (_gameVersion == FILE_VERSION_RCT1)
            {
                dst->lifecycleFlags &= ~RIDE_LIFECYCLE_MUSIC;
                dst->lifecycleFlags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE;
                dst->lifecycleFlags &= ~RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK;
            }
            if (VehicleTypeIsReversed(src->vehicleType))
            {
                dst->lifecycleFlags |= RIDE_LIFECYCLE_REVERSED_TRAINS;
            }

            // Station
            if (src->overallView.IsNull())
            {
                dst->overallView.SetNull();
            }
            else
            {
                dst->overallView = TileCoordsXY{ src->overallView.x, src->overallView.y }.ToCoordsXY();
            }

            for (StationIndex::UnderlyingType i = 0; i < Limits::kMaxStationsPerRide; i++)
            {
                auto& dstStation = dst->getStation(StationIndex::FromUnderlying(i));
                if (src->stationStarts[i].IsNull())
                {
                    dstStation.Start.SetNull();
                }
                else
                {
                    auto tileStartLoc = TileCoordsXY{ src->stationStarts[i].x, src->stationStarts[i].y };
                    dstStation.Start = tileStartLoc.ToCoordsXY();
                }
                dstStation.SetBaseZ(src->stationHeights[i] * Limits::kCoordsZStep);
                dstStation.Length = src->stationLengths[i];
                dstStation.Depart = src->stationLights[i];

                dstStation.TrainAtStation = src->stationDeparts[i];

                // Direction is fixed later.
                if (src->entrances[i].IsNull())
                    dstStation.Entrance.SetNull();
                else
                    dstStation.Entrance = { src->entrances[i].x, src->entrances[i].y, src->stationHeights[i] / 2, 0 };

                if (src->exits[i].IsNull())
                    dstStation.Exit.SetNull();
                else
                    dstStation.Exit = { src->exits[i].x, src->exits[i].y, src->stationHeights[i] / 2, 0 };

                dstStation.QueueTime = src->queueTime[i];
                dstStation.LastPeepInQueue = EntityId::FromUnderlying(src->lastPeepInQueue[i]);
                dstStation.QueueLength = src->numPeepsInQueue[i];

                dstStation.SegmentTime = src->time[i];
                dstStation.SegmentLength = src->length[i];
            }
            // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
            for (int32_t i = Limits::kMaxStationsPerRide; i < OpenRCT2::Limits::kMaxStationsPerRide; i++)
            {
                auto& dstStation = dst->getStation(StationIndex::FromUnderlying(i));
                dstStation.Start.SetNull();
                dstStation.TrainAtStation = RideStation::kNoTrain;
                dstStation.Entrance.SetNull();
                dstStation.Exit.SetNull();
                dstStation.LastPeepInQueue = EntityId::GetNull();
            }

            dst->numStations = src->numStations;

            // Vehicle links (indexes converted later)
            for (int32_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->vehicles[i]);
            }
            for (int32_t i = Limits::kMaxTrainsPerRide; i <= OpenRCT2::Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->numTrains = src->numTrains;
            dst->numCarsPerTrain = src->numCarsPerTrain + rideEntry->zero_cars;
            dst->proposedNumTrains = src->numTrains;
            dst->maxTrains = src->maxTrains;
            dst->proposedNumCarsPerTrain = src->numCarsPerTrain + rideEntry->zero_cars;
            auto split = splitCombinedHelicesAndSpecialElements(src->specialTrackElements);
            dst->numHelices = split.first;
            dst->specialTrackElements = split.second;
            dst->numShelteredSections = src->numShelteredSections;
            dst->shelteredLength = src->shelteredLength;

            // Operation
            dst->departFlags = src->departFlags;
            dst->minWaitingTime = src->minWaitingTime;
            dst->maxWaitingTime = src->maxWaitingTime;
            dst->operationOption = src->operationOption;
            dst->numCircuits = 1;
            dst->minCarsPerTrain = rideEntry->min_cars_in_train;
            dst->maxCarsPerTrain = rideEntry->max_cars_in_train;

            // RCT1 used 5mph / 8 km/h for every lift hill
            dst->liftHillSpeed = 5;

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
                    if (src->type == RideType::merryGoRound || src->type == RideType::dodgems)
                    {
                        if (src->departFlags & RCT1_RIDE_DEPART_PLAY_MUSIC)
                        {
                            dst->departFlags &= ~RCT1_RIDE_DEPART_PLAY_MUSIC;
                            dst->lifecycleFlags |= RIDE_LIFECYCLE_MUSIC;
                        }
                    }
                }
                else
                {
                    dst->music = src->music;
                }
            }

            if (src->operatingMode == RCT1_RIDE_MODE_POWERED_LAUNCH)
            {
                // Launched rides never passed through the station in RCT1.
                dst->mode = RideMode::poweredLaunch;
            }
            else
            {
                dst->mode = static_cast<RideMode>(src->operatingMode);
            }

            SetRideColourScheme(dst, src);

            // Maintenance
            dst->buildDate = static_cast<int32_t>(src->buildDate);
            auto clampedInspectionInterval = std::clamp<uint8_t>(src->inspectionInterval, 0, EnumValue(RideInspection::never));
            dst->inspectionInterval = static_cast<RideInspection>(clampedInspectionInterval);
            dst->lastInspection = src->lastInspection;
            dst->reliability = src->reliability;
            dst->unreliabilityFactor = src->unreliabilityFactor;
            dst->downtime = src->downtime;
            dst->breakdownReason = src->breakdownReason;
            dst->mechanicStatus = src->mechanicStatus;
            dst->mechanic = EntityId::FromUnderlying(src->mechanic);
            dst->breakdownReasonPending = src->breakdownReasonPending;
            dst->inspectionStation = StationIndex::FromUnderlying(src->inspectionStation);
            dst->brokenCar = src->brokenCar;
            dst->brokenTrain = src->brokenTrain;

            // Measurement data
            dst->ratings = src->ratings;

            dst->maxSpeed = src->maxSpeed;
            dst->averageSpeed = src->averageSpeed;

            dst->maxPositiveVerticalG = src->maxPositiveVerticalG;
            dst->maxNegativeVerticalG = src->maxNegativeVerticalG;
            dst->maxLateralG = src->maxLateralG;
            dst->previousLateralG = src->previousLateralG;
            dst->previousVerticalG = src->previousVerticalG;
            dst->turnCountBanked = src->turnCountBanked;
            dst->turnCountDefault = src->turnCountDefault;
            dst->turnCountSloped = src->turnCountSloped;

            auto splitDropsLifts = splitCombinedNumDropsPoweredLifts(src->numDrops);
            dst->numDrops = splitDropsLifts.first;
            dst->numPoweredLifts = splitDropsLifts.second;

            dst->startDropHeight = src->startDropHeight / 2;
            dst->highestDropHeight = src->highestDropHeight / 2;
            if (src->type == RideType::miniatureGolf)
                dst->numHoles = src->numInversions & kRCT12InversionAndHoleMask;
            else
                dst->numInversions = src->numInversions & kRCT12InversionAndHoleMask;
            dst->shelteredEighths = src->numInversions >> 5;
            dst->boatHireReturnDirection = src->boatHireReturnDirection;
            dst->boatHireReturnPosition = { src->boatHireReturnPosition.x, src->boatHireReturnPosition.y };
            dst->chairliftBullwheelRotation = src->chairliftBullwheelRotation;
            for (int i = 0; i < 2; i++)
            {
                dst->chairliftBullwheelLocation[i] = { src->chairliftBullwheelLocation[i].x,
                                                       src->chairliftBullwheelLocation[i].y, src->chairliftBullwheelZ[i] / 2 };
            }

            if (src->curTestTrackLocation.IsNull())
            {
                dst->curTestTrackLocation.SetNull();
            }
            else
            {
                dst->curTestTrackLocation = { src->curTestTrackLocation.x, src->curTestTrackLocation.y,
                                              src->curTestTrackZ / 2 };
            }
            dst->testingFlags.holder = src->testingFlags;
            dst->currentTestSegment = src->currentTestSegment;
            dst->currentTestStation = StationIndex::GetNull();
            dst->averageSpeedTestTimeout = src->averageSpeedTestTimeout;
            dst->slideInUse = src->slideInUse;
            dst->slidePeepTShirtColour = GetColour(src->slidePeepTshirtColour);
            dst->spiralSlideProgress = src->spiralSlideProgress;
            // Doubles as slidePeep
            dst->mazeTiles = src->mazeTiles;

            // Finance / customers
            dst->upkeepCost = ToMoney64(src->upkeepCost);
            dst->price[0] = src->price;
            dst->price[1] = src->priceSecondary;
            dst->incomePerHour = ToMoney64(src->incomePerHour);
            dst->totalCustomers = src->totalCustomers;
            dst->profit = ToMoney64(src->profit);
            dst->totalProfit = ToMoney64(src->totalProfit);
            dst->value = ToMoney64(src->value);
            for (size_t i = 0; i < std::size(src->numCustomers); i++)
            {
                dst->numCustomers[i] = src->numCustomers[i];
            }

            dst->satisfaction = src->satisfaction;
            dst->satisfactionTimeout = src->satisfactionTimeOut;
            dst->satisfactionNext = src->satisfactionNext;
            dst->popularity = src->popularity;
            dst->popularityNext = src->popularityNext;
            dst->popularityTimeout = src->popularityTimeOut;

            dst->numRiders = src->numRiders;

            dst->musicTuneId = kTuneIDNull;
        }

        void SetRideColourScheme(::Ride* dst, Ride* src)
        {
            // Colours
            dst->vehicleColourSettings = src->vehicleColourSettings;
            if (_gameVersion == FILE_VERSION_RCT1)
            {
                dst->trackColours[0].main = GetColour(src->trackPrimaryColour);
                dst->trackColours[0].additional = GetColour(src->trackSecondaryColour);
                dst->trackColours[0].supports = GetColour(src->trackSupportColour);

                // Balloons were always blue in the original RCT.
                if (src->type == RideType::balloonStall)
                {
                    dst->trackColours[0].main = Drawing::Colour::lightBlue;
                }
                else if (src->type == RideType::riverRapids)
                {
                    dst->trackColours[0].main = Drawing::Colour::white;
                }
            }
            else
            {
                for (int i = 0; i < Limits::kNumColourSchemes; i++)
                {
                    dst->trackColours[i].main = GetColour(src->trackColourMain[i]);
                    dst->trackColours[i].additional = GetColour(src->trackColourAdditional[i]);
                    dst->trackColours[i].supports = GetColour(src->trackColourSupports[i]);
                }
            }

            dst->entranceStyle = kObjectEntryIndexNull;
            if (dst->getRideTypeDescriptor().HasFlag(RtdFlag::hasEntranceAndExit))
            {
                // Entrance styles were introduced with AA. They correspond directly with those in RCT2.
                if (_gameVersion == FILE_VERSION_RCT1)
                {
                    dst->entranceStyle = 0; // plain entrance
                }
                else
                {
                    dst->entranceStyle = src->entranceStyle;
                }
            }

            if (_gameVersion < FILE_VERSION_RCT1_LL && src->type == RideType::merryGoRound)
            {
                // The merry-go-round in pre-LL versions was always yellow with red
                dst->vehicleColours[0].Body = Drawing::Colour::yellow;
                dst->vehicleColours[0].Trim = Drawing::Colour::brightRed;
            }
            else
            {
                for (int i = 0; i < Limits::kMaxTrainsPerRide; i++)
                {
                    // RCT1 had no third colour
                    const auto colourSchemeCopyDescriptor = GetColourSchemeCopyDescriptor(src->vehicleType);
                    if (colourSchemeCopyDescriptor.colour1 == kCopyColour1)
                    {
                        dst->vehicleColours[i].Body = GetColour(src->vehicleColours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour1 == kCopyColour2)
                    {
                        dst->vehicleColours[i].Body = GetColour(src->vehicleColours[i].trim);
                    }
                    else
                    {
                        dst->vehicleColours[i].Body = colourSchemeCopyDescriptor.colour1;
                    }

                    if (colourSchemeCopyDescriptor.colour2 == kCopyColour1)
                    {
                        dst->vehicleColours[i].Trim = GetColour(src->vehicleColours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour2 == kCopyColour2)
                    {
                        dst->vehicleColours[i].Trim = GetColour(src->vehicleColours[i].trim);
                    }
                    else
                    {
                        dst->vehicleColours[i].Trim = colourSchemeCopyDescriptor.colour2;
                    }

                    if (colourSchemeCopyDescriptor.colour3 == kCopyColour1)
                    {
                        dst->vehicleColours[i].Tertiary = GetColour(src->vehicleColours[i].body);
                    }
                    else if (colourSchemeCopyDescriptor.colour3 == kCopyColour2)
                    {
                        dst->vehicleColours[i].Tertiary = GetColour(src->vehicleColours[i].trim);
                    }
                    else
                    {
                        dst->vehicleColours[i].Tertiary = colourSchemeCopyDescriptor.colour3;
                    }
                }
            }

            // In RCT1 and AA, the maze was always hedges.
            // LL has 4 types, like RCT2. For LL, only guard against invalid values.
            if (src->type == RideType::hedgeMaze)
            {
                if (_gameVersion < FILE_VERSION_RCT1_LL || src->trackColourSupports[0] > 3)
                    dst->trackColours[0].supports = static_cast<Drawing::Colour>(hedges);
                else
                    dst->trackColours[0].supports = static_cast<Drawing::Colour>(src->trackColourSupports[0]);
            }
        }

        void ImportRideMeasurements()
        {
            for (const auto& src : _s4.RideMeasurements)
            {
                if (src.RideIndex != kRCT12RideIdNull)
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
            dst.flags.holder = src.Flags;
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

        void ImportEntity(GameState_t& gameState, const RCT12EntityBase& src);
        template<typename T>
        void ImportEntity(GameState_t& gameState, const RCT12EntityBase& src);

        void ImportEntities(GameState_t& gameState)
        {
            for (int i = 0; i < Limits::kMaxEntities; i++)
            {
                ImportEntity(gameState, _s4.Entities[i].Unknown);
            }
        }

        void ResetPeepSpriteBounds([[maybe_unused]] GameState_t& gameState)
        {
            // TODO: Entities are currently read from the global state, change this once entities are stored
            // in the passed gameState.
            auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::guest);
            for (auto* peep : EntityList<Guest>())
            {
                const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
                peep->SpriteData.Width = spriteBounds.spriteWidth;
                peep->SpriteData.HeightMin = spriteBounds.spriteHeightNegative;
                peep->SpriteData.HeightMax = spriteBounds.spriteHeightPositive;
            }

            auto& objManager = GetContext()->GetObjectManager();
            for (auto* peep : EntityList<Staff>())
            {
                animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);
                const auto& spriteBounds = animObj->GetSpriteBounds(peep->AnimationGroup, peep->AnimationType);
                peep->SpriteData.Width = spriteBounds.spriteWidth;
                peep->SpriteData.HeightMin = spriteBounds.spriteHeightNegative;
                peep->SpriteData.HeightMax = spriteBounds.spriteHeightPositive;
            }
        }

        void SetVehicleColours(::Vehicle* dst, const Vehicle* src)
        {
            const auto& srcRide = _s4.Rides[src->Ride];
            VehicleColourSchemeCopyDescriptor colourSchemeCopyDescriptor = GetColourSchemeCopyDescriptor(srcRide.vehicleType);

            // RCT1 had no third colour
            if (colourSchemeCopyDescriptor.colour1 == kCopyColour1)
            {
                dst->colours.Body = GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour1 == kCopyColour2)
            {
                dst->colours.Body = GetColour(src->Colours.TrimColour);
            }
            else
            {
                dst->colours.Body = colourSchemeCopyDescriptor.colour1;
            }

            if (colourSchemeCopyDescriptor.colour2 == kCopyColour1)
            {
                dst->colours.Trim = GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour2 == kCopyColour2)
            {
                dst->colours.Trim = GetColour(src->Colours.TrimColour);
            }
            else
            {
                dst->colours.Trim = colourSchemeCopyDescriptor.colour2;
            }

            if (colourSchemeCopyDescriptor.colour3 == kCopyColour1)
            {
                dst->colours.Tertiary = GetColour(src->Colours.BodyColour);
            }
            else if (colourSchemeCopyDescriptor.colour3 == kCopyColour2)
            {
                dst->colours.Tertiary = GetColour(src->Colours.TrimColour);
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

        void ImportPeep(::Peep* dst, const Peep* src)
        {
            dst->AnimationObjectIndex = kObjectEntryIndexNull;
            auto rct12AnimGroup = GetPeepAnimationGroup(src->AnimationGroup);
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
            dst->TshirtColour = GetColour(src->TshirtColour);
            dst->TrousersColour = GetColour(src->TrousersColour);
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
            dst->PathfindGoal.direction = kInvalidDirection;
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

        void ImportMapAnimations()
        {
            for (const auto& mapAnimation : std::span(_s4.MapAnimations, _s4.NumMapAnimations))
            {
                switch (mapAnimation.Type)
                {
                    case kRCT12MapAnimationTypeOnRidePhoto:
                        MapAnimations::CreateTemporary(
                            { mapAnimation.x, mapAnimation.y, (mapAnimation.BaseZ / 2) * kCoordsZStep },
                            MapAnimations::TemporaryType::onRidePhoto);
                        break;
                    case kRCT12MapAnimationTypeLandEdgeDoor:
                        MapAnimations::CreateTemporary(
                            { mapAnimation.x, mapAnimation.y, (mapAnimation.BaseZ / 2) * kCoordsZStep },
                            MapAnimations::TemporaryType::landEdgeDoor);
                        break;
                }
            }
        }

        void ImportPeepSpawns(GameState_t& gameState)
        {
            gameState.peepSpawns.clear();
            for (size_t i = 0; i < Limits::kMaxPeepSpawns; i++)
            {
                if (_s4.PeepSpawn[i].x != kRCT12PeepSpawnUndefined)
                {
                    PeepSpawn spawn = { _s4.PeepSpawn[i].x, _s4.PeepSpawn[i].y, _s4.PeepSpawn[i].z * 16,
                                        _s4.PeepSpawn[i].direction };
                    gameState.peepSpawns.push_back(spawn);
                }
            }
        }

        void ImportFinance(GameState_t& gameState)
        {
            auto& park = gameState.park;

            park.entranceFee = _s4.ParkEntranceFee;
            gameState.scenarioOptions.landPrice = ToMoney64(_s4.LandPrice);
            gameState.scenarioOptions.constructionRightsPrice = ToMoney64(_s4.ConstructionRightsPrice);

            park.cash = ToMoney64(_s4.Cash);
            park.bankLoan = ToMoney64(_s4.Loan);
            park.maxBankLoan = ToMoney64(_s4.MaxLoan);
            // It's more like 1.33%, but we can only use integers. Can be fixed once we have our own save format.
            park.bankLoanInterestRate = 1;
            gameState.scenarioOptions.initialCash = ToMoney64(_s4.Cash);

            park.companyValue = ToMoney64(_s4.CompanyValue);
            park.value = CorrectRCT1ParkValue(_s4.ParkValue);
            park.currentProfit = ToMoney64(_s4.Profit);

            // With park value known, we can recalculate the conversion factor
            _parkValueConversionFactor = calculateParkValueConversionFactor(park, gameState);

            for (size_t i = 0; i < Limits::kFinanceGraphSize; i++)
            {
                park.cashHistory[i] = ToMoney64(_s4.CashHistory[i]);
                park.valueHistory[i] = CorrectRCT1ParkValue(_s4.ParkValueHistory[i]);
                park.weeklyProfitHistory[i] = ToMoney64(_s4.WeeklyProfitHistory[i]);
            }

            for (size_t i = 0; i < Limits::kExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::kExpenditureTypeCount; j++)
                {
                    park.expenditureTable[i][j] = ToMoney64(_s4.Expenditure[i][j]);
                }
            }
            park.currentExpenditure = ToMoney64(_s4.TotalExpenditure);

            gameState.scenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s4.CompletedCompanyValue);
            park.totalAdmissions = _s4.NumAdmissions;
            park.totalIncomeFromAdmissions = ToMoney64(_s4.AdmissionTotalIncome);

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
                    park.marketingCampaigns.push_back(campaign);
                }
            }
        }

        ObjectList GetRequiredObjects()
        {
            ObjectList result;
            AppendRequiredObjects(result, ObjectType::ride, _rideEntries);
            AppendRequiredObjects(result, ObjectType::smallScenery, _smallSceneryEntries);
            AppendRequiredObjects(result, ObjectType::largeScenery, _largeSceneryEntries);
            AppendRequiredObjects(result, ObjectType::walls, _wallEntries);
            AppendRequiredObjects(result, ObjectType::paths, _pathEntries);
            AppendRequiredObjects(result, ObjectType::pathAdditions, _pathAdditionEntries);
            AppendRequiredObjects(result, ObjectType::sceneryGroup, _sceneryGroupEntries);
            AppendRequiredObjects(result, ObjectType::banners, _bannerEntries);
            AppendRequiredObjects(
                result, ObjectType::parkEntrance, std::vector<std::string_view>({ "rct2.park_entrance.pkent1" }));
            AppendRequiredObjects(result, ObjectType::water, _waterEntry);
            AppendRequiredObjects(result, ObjectType::terrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::terrainEdge, _terrainEdgeEntries);
            AppendRequiredObjects(result, ObjectType::footpathSurface, _footpathSurfaceEntries);
            AppendRequiredObjects(result, ObjectType::footpathRailings, _footpathRailingsEntries);
            AppendRequiredObjects(result, ObjectType::peepNames, std::vector<std::string_view>({ "rct2.peep_names.original" }));
            AppendRequiredObjects(result, ObjectType::station, kDefaultStationStyles);
            RCT12AddDefaultMusic(result);

            // Normalise the name to make the scenario as recognisable as possible
            auto normalisedName = ScenarioSources::NormaliseName(_s4.ScenarioName);

            // Infer what scenario meta object to use, if any
            SourceDescriptor desc;
            if (ScenarioSources::TryGetByName(normalisedName.c_str(), &desc) && !desc.textObjectId.empty())
                AppendRequiredObjects(result, ObjectType::scenarioMeta, std::vector<std::string_view>({ desc.textObjectId }));

            // Add all legacy peep animation objects
            auto animObjects = GetLegacyPeepAnimationObjects();
            AppendRequiredObjects(result, ObjectType::peepAnimations, animObjects);

            auto climateObjId = GetClimateObjectIdFromLegacyClimateType(_s4.Climate);
            AppendRequiredObjects(result, ObjectType::climate, std::vector({ climateObjId }));

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
            FixSupportsOnGroundPath(gameState);
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
                    if (PathIsQueue(pathType))
                    {
                        dst2->SetIsQueue(true);
                    }

                    uint8_t railingsType = RCT1_PATH_SUPPORT_TYPE_TRUSS;
                    if (_gameVersion == FILE_VERSION_RCT1_LL)
                    {
                        railingsType = src2->GetRCT1SupportType();
                    }
                    // All types are already loaded, in the same order as RCT1.
                    dst2->SetRailingsEntryIndex(railingsType);

                    // Additions
                    ObjectEntryIndex additionType = src2->GetAddition();
                    if (additionType != RCT1_PATH_ADDITION_NONE)
                    {
                        ObjectEntryIndex normalisedType = NormalisePathAddition(additionType);
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
                    auto rct1RideType = _s4.Rides[src2->GetRideIndex()].type;

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
                    if (rct1RideType == RideType::ghostTrain)
                    {
                        dst2->SetDoorAState(src2->GetDoorAState());
                        dst2->SetDoorBState(src2->GetDoorBState());
                    }
                    else
                    {
                        dst2->SetSeatRotation(kDefaultSeatRotation);
                    }
                    // Skipping IsHighlighted()

                    auto trackType = dst2->GetTrackType();
                    // Brakes import as closed to preserve legacy behaviour
                    dst2->SetBrakeClosed(trackType == TrackElemType::brakes);
                    if (TrackTypeHasSpeedSetting(trackType))
                    {
                        dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                    }
                    else if (trackType == TrackElemType::onRidePhoto)
                    {
                        dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                    }

                    // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                    if (rct1RideType == RideType::hedgeMaze)
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
                    dst2->SetPrimaryColour(GetColour(EnumValue(src2->GetPrimaryColour())));
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
                            dst2->SetSecondaryColour(Drawing::Colour::white);
                            break;
                        case RCT1_SCENERY_TULIPS_1:
                        case RCT1_SCENERY_TULIPS_2:
                            dst2->SetPrimaryColour(Drawing::Colour::brightRed);
                            dst2->SetSecondaryColour(Drawing::Colour::yellow);
                            break;
                        case RCT1_SCENERY_SMALL_RED_GARDENS:
                            dst2->SetPrimaryColour(Drawing::Colour::brightRed);
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

                        Drawing::Colour colourA = GetColour(EnumValue(src2->GetRCT1WallColour()));
                        Drawing::Colour colourB = Drawing::Colour::black;
                        Drawing::Colour colourC = Drawing::Colour::black;
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
                    dst2->SetPrimaryColour(GetColour(EnumValue(src2->GetPrimaryColour())));
                    dst2->SetSecondaryColour(GetColour(EnumValue(src2->GetSecondaryColour())));

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
            const ResearchItem* researchList = GetResearchList(&researchListCount);

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
            std::vector<ResearchItem> vehiclesWithMissingRideTypes;
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
                        if (RideTypeUsesVehicles(rct1RideType))
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
                activeResearchTypes |= EnumToFlag(ResearchCategory::rollercoaster);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_THRILL_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::thrill);
                activeResearchTypes |= EnumToFlag(ResearchCategory::water);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_GENTLE_TRANSPORT_RIDES)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::gentle);
                activeResearchTypes |= EnumToFlag(ResearchCategory::transport);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_SHOPS)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::shop);
            }
            if (_s4.ResearchPriority & RCT1_RESEARCH_CATEGORY_SCENERY_THEMING)
            {
                activeResearchTypes |= EnumToFlag(ResearchCategory::sceneryGroup);
            }
            gameState.researchPriorities = activeResearchTypes;
            gameState.researchFundingLevel = _s4.ResearchLevel;

            // This will mark items as researched/unresearched according to the research list.
            // This needs to be called before importing progress, as it will reset it.
            ResearchResetCurrentItem();

            // Research history
            gameState.researchProgress = _s4.ResearchProgress;
            gameState.researchProgressStage = _s4.ResearchProgressStage;
            gameState.researchExpectedDay = _s4.NextResearchExpectedDay;
            gameState.researchExpectedMonth = _s4.NextResearchExpectedMonth;

            if (_s4.LastResearchFlags == 0xFF)
            {
                gameState.researchLastItem = std::nullopt;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.LastResearchItem, _s4.LastResearchType);
                gameState.researchLastItem = researchItem;
            }

            if (_s4.NextResearchFlags == 0xFF)
            {
                gameState.researchNextItem = std::nullopt;
                gameState.researchProgressStage = RESEARCH_STAGE_INITIAL_RESEARCH;
                gameState.researchProgress = 0;
            }
            else
            {
                ::ResearchItem researchItem = {};
                ConvertResearchEntry(&researchItem, _s4.NextResearchItem, _s4.NextResearchType);
                gameState.researchNextItem = researchItem;
            }
        }

        static BitSet<EnumValue(RideType::count)> GetRideTypesPresentInResearchList(
            const ResearchItem* researchList, size_t researchListCount)
        {
            BitSet<EnumValue(RideType::count)> ret = {};

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

            auto& park = gameState.park;
            park.name = std::move(parkName);
        }

        std::vector<News::Item> convertNewsQueue(std::span<const RCT12NewsItem> queue)
        {
            std::vector<News::Item> output{};

            for (const auto& src : queue)
            {
                News::Item dst{};

                if (src.Type == 0)
                    break;

                dst.type = static_cast<News::ItemType>(src.Type);
                dst.flags = src.Flags;
                dst.ticks = src.Ticks;
                dst.monthYear = src.MonthYear;
                dst.day = src.Day;
                dst.text = ConvertFormattedStringToOpenRCT2(std::string_view(src.Text, sizeof(src.Text)));

                if (dst.type == News::ItemType::research)
                {
                    uint8_t researchItem = src.Assoc & 0x000000FF;
                    uint8_t researchType = (src.Assoc & 0x00FF0000) >> 16;

                    ::ResearchItem tmpResearchItem = {};
                    ConvertResearchEntry(&tmpResearchItem, researchItem, researchType);
                    dst.assoc = tmpResearchItem.rawValue;
                }
                else
                {
                    dst.assoc = src.Assoc;
                }

                output.emplace_back(dst);
            }

            return output;
        }

        void ImportParkFlags(GameState_t& gameState)
        {
            // Date and srand
            gameState.currentTicks = _s4.Ticks;
            ScenarioRandSeed(_s4.RandomA, _s4.RandomB);
            gameState.date = Date{ _s4.Month, _s4.Day };

            auto& park = gameState.park;

            // Park rating
            park.rating = _s4.ParkRating;

            Park::ResetHistories(park);
            for (size_t i = 0; i < std::size(_s4.ParkRatingHistory); i++)
            {
                if (_s4.ParkRatingHistory[i] != kRCT12ParkHistoryUndefined)
                {
                    park.ratingHistory[i] = _s4.ParkRatingHistory[i] * kRCT12ParkRatingHistoryFactor;
                }
            }
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != kRCT12ParkHistoryUndefined)
                {
                    park.guestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * kRCT12GuestsInParkHistoryFactor;
                }
            }

            // Awards
            auto& currentAwards = park.currentAwards;
            for (auto& src : _s4.Awards)
            {
                if (src.Time != 0)
                {
                    currentAwards.push_back(Award{ src.Time, static_cast<AwardType>(src.Type) });
                }
            }

            // Number of guests history
            std::fill(
                std::begin(park.guestsInParkHistory), std::end(park.guestsInParkHistory), std::numeric_limits<uint32_t>::max());
            for (size_t i = 0; i < std::size(_s4.GuestsInParkHistory); i++)
            {
                if (_s4.GuestsInParkHistory[i] != std::numeric_limits<uint8_t>::max())
                {
                    park.guestsInParkHistory[i] = _s4.GuestsInParkHistory[i] * 20;
                }
            }

            // News items
            auto recentMessages = convertNewsQueue(_s4.recentMessages);
            auto archivedMessages = convertNewsQueue(_s4.archivedMessages);
            News::importNewsItems(gameState, recentMessages, archivedMessages);

            // Initial guest status
            gameState.scenarioOptions.guestInitialCash = ToMoney64(_s4.GuestInitialCash);
            gameState.scenarioOptions.guestInitialHunger = _s4.GuestInitialHunger;
            gameState.scenarioOptions.guestInitialThirst = _s4.GuestInitialThirst;
            gameState.scenarioOptions.guestInitialHappiness = _s4.GuestInitialHappiness;

            park.guestGenerationProbability = _s4.GuestGenerationProbability;

            // Staff colours
            park.staffHandymanColour = GetColour(_s4.HandymanColour);
            park.staffMechanicColour = GetColour(_s4.MechanicColour);
            park.staffSecurityColour = GetColour(_s4.SecurityGuardColour);

            // Flags
            park.flags = _s4.ParkFlags;
            park.flags &= ~PARK_FLAGS_ANTI_CHEAT_DEPRECATED;
            park.flags |= PARK_FLAGS_RCT1_INTEREST;
            // Loopy Landscape parks can set a flag to lock the entry price to free.
            // If this flag is not set, the player can ask money for both rides and entry.
            if (!(_s4.ParkFlags & RCT1_PARK_FLAGS_PARK_ENTRY_LOCKED_AT_FREE))
            {
                park.flags |= PARK_FLAGS_UNLOCK_ALL_PRICES;
            }

            park.size = _s4.ParkSize;
            park.totalRideValueForMoney = _s4.TotalRideValueForMoney;
            park.samePriceThroughoutPark = 0;
            if (_gameVersion == FILE_VERSION_RCT1_LL)
            {
                park.samePriceThroughoutPark = _s4.SamePriceThroughout;
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
                        dst->type = Research::EntryType::ride;
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
                        dst->type = Research::EntryType::ride;
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
                    dst->type = Research::EntryType::scenery;
                    dst->category = ResearchCategory::sceneryGroup;
                    dst->baseRideType = 0;
                    dst->flags = 0;
                }
            }
        }

        void ImportClimate(GameState_t& gameState)
        {
            gameState.weatherUpdateTimer = _s4.WeatherUpdateTimer;
            gameState.weatherCurrent = {
                .weatherType = WeatherType{ _s4.Weather },
                .temperature = static_cast<int8_t>(_s4.Temperature),
                .weatherEffect = WeatherEffectType::None,
                .weatherGloom = _s4.WeatherGloom,
                .level = static_cast<WeatherLevel>(_s4.Rain),
            };
            gameState.weatherNext = {
                .weatherType = WeatherType{ _s4.TargetWeather },
                .temperature = static_cast<int8_t>(_s4.TargetTemperature),
                .weatherEffect = WeatherEffectType::None,
                .weatherGloom = _s4.TargetWeatherGloom,
                .level = static_cast<WeatherLevel>(_s4.TargetRain),
            };
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
                // If no entry is found, this is a custom scenario
                bool isOfficial = ScenarioSources::TryGetById(_s4.ScenarioSlotIndex, &desc);

                // Perform an additional name check if this is detected to be a competition scenario
                if (isOfficial && desc.category == Scenario::Category::competitions)
                    isOfficial = ScenarioSources::TryGetByName(_s4.ScenarioName, &desc);

                if (isOfficial && !desc.textObjectId.empty())
                {
                    auto& objManager = GetContext()->GetObjectManager();

                    if (auto obj = objManager.LoadTempObject(desc.textObjectId, false); obj != nullptr)
                    {
                        auto& textObject = reinterpret_cast<ScenarioMetaObject&>(*obj);
                        name = textObject.GetScenarioName();
                        parkName = textObject.GetParkName();
                        details = textObject.GetScenarioDetails();
                    }
                }
            }

            gameState.scenarioOptions.name = std::move(name);
            gameState.scenarioOptions.details = std::move(details);
            if (_isScenario && !parkName.empty())
            {
                auto& park = gameState.park;
                park.name = std::move(parkName);
            }
        }

        void ImportScenarioObjective(GameState_t& gameState)
        {
            gameState.scenarioOptions.objective.Type = _s4.ScenarioObjectiveType;
            gameState.scenarioOptions.objective.Year = _s4.ScenarioObjectiveYears;
            gameState.scenarioOptions.objective.NumGuests = _s4.ScenarioObjectiveNumGuests;

            // RCT1 used a different way of calculating the park value.
            // This is corrected here, but since scenario_objective_currency doubles as minimum excitement rating,
            // we need to check the goal to avoid affecting scenarios like Volcania.
            if (_s4.ScenarioObjectiveType == Scenario::ObjectiveType::parkValueBy)
                gameState.scenarioOptions.objective.Currency = CorrectRCT1ParkValue(_s4.ScenarioObjectiveCurrency);
            else
                gameState.scenarioOptions.objective.Currency = ToMoney64(_s4.ScenarioObjectiveCurrency);

            // This does not seem to be saved in the objective arguments, so look up the ID from the available rides instead.
            if (_s4.ScenarioObjectiveType == Scenario::ObjectiveType::buildTheBest)
                gameState.scenarioOptions.objective.RideId = GetBuildTheBestRideId();
        }

        void ImportSavedView(GameState_t& gameState)
        {
            gameState.savedView = ScreenCoordsXY{ _s4.ViewX, _s4.ViewY };
            gameState.savedViewZoom = ZoomLevel{ static_cast<int8_t>(_s4.ViewZoom) };
            gameState.savedViewRotation = _s4.ViewRotation;
        }

        void ConvertWall(const int32_t& type, Drawing::Colour* colourA, Drawing::Colour* colourB)
        {
            switch (type)
            {
                case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE:
                    *colourA = Drawing::Colour::darkBrown;
                    break;
                case RCT1_WALL_TYPE_WHITE_WOODEN_PANEL_FENCE:
                    *colourA = Drawing::Colour::white;
                    break;
                case RCT1_WALL_TYPE_RED_WOODEN_PANEL_FENCE:
                    *colourA = Drawing::Colour::salmonPink;
                    break;
                case RCT1_WALL_TYPE_WOODEN_PANEL_FENCE_WITH_SNOW:
                    *colourA = Drawing::Colour::darkBrown;
                    break;
                case RCT1_WALL_TYPE_GLASS_SMOOTH:
                case RCT1_WALL_TYPE_GLASS_PANELS:
                    *colourB = Drawing::Colour::white;
                    break;
                case RCT1_WALL_TYPE_SMALL_GREY_CASTLE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_CROSS:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_GATE:
                case RCT1_WALL_TYPE_LARGE_GREY_CASTLE_WINDOW:
                case RCT1_WALL_TYPE_MEDIUM_GREY_CASTLE:
                    *colourA = Drawing::Colour::grey;
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

            dst->flags = {};
            if (src->flags.has(BannerFlag::noEntry))
            {
                dst->flags.set(BannerFlag::noEntry);
            }

            if (IsUserStringID(src->StringID))
            {
                dst->text = GetUserString(src->StringID);
            }

            dst->colour = GetColour(src->Colour);
            dst->textColour = src->textColour;
            dst->position.x = src->x;
            dst->position.y = src->y;
        }

        void FixEntrancePositions(GameState_t& gameState)
        {
            auto& park = gameState.park;
            park.entrances.clear();

            TileElementIterator it;
            TileElementIteratorBegin(&it);
            while (TileElementIteratorNext(&it) && park.entrances.size() < Limits::kMaxParkEntrances)
            {
                TileElement* element = it.element;

                if (element->GetType() != TileElementType::Entrance)
                    continue;
                if (element->AsEntrance()->GetEntranceType() != ENTRANCE_TYPE_PARK_ENTRANCE)
                    continue;
                if ((element->AsEntrance()->GetSequenceIndex()) != 0)
                    continue;

                CoordsXYZD entrance = { TileCoordsXY(it.x, it.y).ToCoordsXY(), element->GetBaseZ(), element->GetDirection() };
                park.entrances.push_back(entrance);
            }
        }

        void FixSupportsOnGroundPath(GameState_t& gameState)
        {
            TileElementIterator it;
            TileElementIteratorBegin(&it);
            while (TileElementIteratorNext(&it))
            {
                if (it.element->GetType() != TileElementType::Path)
                    continue;

                auto* pathElement = it.element->AsPath();
                if (pathElement->IsSloped())
                    continue;

                if (pathElement->IsQueue())
                    continue;

                auto* surface = MapGetSurfaceElementAt(TileCoordsXY(it.x, it.y));
                if (surface == nullptr)
                    continue;

                if (surface->GetSlope() != kTileSlopeFlat)
                    continue;

                if (surface->GetBaseZ() != pathElement->GetBaseZ())
                    continue;

                // RCT1 would always draw supports around a path if it was flat on the ground.
                // In RCT2, this depends on the support type of the path, even though that isn’t even visible in this case.
                // As such, always import footpath that is on the ground with box supports.
                pathElement->SetRailingsEntryIndex(RCT1_PATH_SUPPORT_TYPE_TRUSS);
            }
        }

        RCT12::EntryList* GetEntryList(ObjectType objectType)
        {
            switch (objectType)
            {
                case ObjectType::ride:
                    return &_rideEntries;
                case ObjectType::smallScenery:
                    return &_smallSceneryEntries;
                case ObjectType::largeScenery:
                    return &_largeSceneryEntries;
                case ObjectType::walls:
                    return &_wallEntries;
                case ObjectType::banners:
                    return &_bannerEntries;
                case ObjectType::paths:
                    return &_pathEntries;
                case ObjectType::pathAdditions:
                    return &_pathAdditionEntries;
                case ObjectType::sceneryGroup:
                    return &_sceneryGroupEntries;
                case ObjectType::water:
                    return &_waterEntry;
                default:
                    // This switch processes only ObjectType for for Entries
                    break;
            }
            return nullptr;
        }

        const ResearchItem* GetResearchList(size_t* count)
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
            auto asUtf8 = RCT2StringToUTF8(originalStringView, RCT2LanguageId::englishUK);
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

            gameState.nextGuestNumber = nextGuestNumber + 1;
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
                                case TrackElemType::up25ToFlat:
                                case TrackElemType::up60ToFlat:
                                case TrackElemType::diagUp25ToFlat:
                                case TrackElemType::diagUp60ToFlat:
                                    break;
                                default:
                                    continue;
                            }

                            RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
                            auto ride = GetRide(rideIndex);
                            if (ride != nullptr)
                            {
                                ride->numBlockBrakes++;
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
        void SetDefaultNames(GameState_t& gameState)
        {
            for (auto& ride : RideManager(gameState))
            {
                if (ride.customName.empty())
                {
                    ride.setNameToDefault();
                }
            }
        }

        ObjectEntryIndex GetBuildTheBestRideId()
        {
            size_t researchListCount;
            const ResearchItem* researchList = GetResearchList(&researchListCount);
            for (size_t i = 0; i < researchListCount; i++)
            {
                if (researchList[i].Flags == 0xFF)
                {
                    break;
                }

                if (researchList[i].Type == RCT1_RESEARCH_TYPE_RIDE)
                {
                    return GetRideType(static_cast<RideType>(researchList[i].Item), static_cast<VehicleType>(0));
                }
            }

            return kRideTypeNull;
        }
    };

    // Very similar but not the same as S6Importer version (due to peeps)
    constexpr EntityType GetEntityTypeFromRCT1Sprite(const RCT12EntityBase& src)
    {
        EntityType output = EntityType::null;
        switch (src.EntityIdentifier)
        {
            case RCT12EntityIdentifier::vehicle:
                output = EntityType::vehicle;
                break;
            case RCT12EntityIdentifier::peep:
            {
                const auto& peep = static_cast<const Peep&>(src);
                if (peep.PeepType == RCT12PeepType::guest)
                {
                    output = EntityType::guest;
                }
                else
                {
                    output = EntityType::staff;
                }
                break;
            }
            case RCT12EntityIdentifier::misc:

                switch (RCT12MiscEntityType(src.Type))
                {
                    case RCT12MiscEntityType::steamParticle:
                        output = EntityType::steamParticle;
                        break;
                    case RCT12MiscEntityType::moneyEffect:
                        output = EntityType::moneyEffect;
                        break;
                    case RCT12MiscEntityType::crashedVehicleParticle:
                        output = EntityType::crashedVehicleParticle;
                        break;
                    case RCT12MiscEntityType::explosionCloud:
                        output = EntityType::explosionCloud;
                        break;
                    case RCT12MiscEntityType::crashSplash:
                        output = EntityType::crashSplash;
                        break;
                    case RCT12MiscEntityType::explosionFlare:
                        output = EntityType::explosionFlare;
                        break;
                    case RCT12MiscEntityType::jumpingFountainWater:
                    case RCT12MiscEntityType::jumpingFountainSnow:
                        output = EntityType::jumpingFountain;
                        break;
                    case RCT12MiscEntityType::balloon:
                        output = EntityType::balloon;
                        break;
                    case RCT12MiscEntityType::duck:
                        output = EntityType::duck;
                        break;
                    default:
                        break;
                }
                break;
            case RCT12EntityIdentifier::litter:
                output = EntityType::litter;
                break;
            default:
                break;
        }
        return output;
    }

    template<>
    void S4Importer::ImportEntity<::Vehicle>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const Vehicle*>(&srcBase);
        const auto* ride = GetRide(RideId::FromUnderlying(src->Ride));
        if (ride == nullptr)
            return;

        const auto& rct1Ride = _s4.Rides[src->Ride];
        uint8_t vehicleEntryIndex = GetVehicleSubEntryIndex(rct1Ride.vehicleType, src->CarType);

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
        dst->dopplerShift = src->SoundVectorFactor;
        dst->spin_speed = src->SpinSpeed;
        dst->sound2_flags = src->Sound2Flags;
        dst->sound1_id = Audio::SoundId::null;
        dst->sound2_id = Audio::SoundId::null;
        dst->var_C0 = src->VarC0;
        dst->CollisionDetectionTimer = src->CollisionDetectionTimer;
        dst->animation_frame = src->AnimationFrame;
        dst->animationState = src->AnimationState;
        dst->NumLaps = src->NumLaps;
        dst->var_D3 = src->VarD3;
        dst->scream_sound_id = Audio::SoundId::null;
        dst->pitch = src->pitch;
        dst->roll = src->roll;

        // Seat rotation was not in RCT1
        dst->target_seat_rotation = kDefaultSeatRotation;
        dst->seat_rotation = kDefaultSeatRotation;

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
                dst->peep_tshirt_colours[i] = GetColour(src->PeepTshirtColours[i]);
            }
        }

        ::Vehicle::Status statusSrc = ::Vehicle::Status::movingToEndOfStation;
        if (src->Status <= static_cast<uint8_t>(::Vehicle::Status::stoppedByBlockBrakes))
        {
            statusSrc = static_cast<::Vehicle::Status>(src->Status);
        }
        dst->status = statusSrc;
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->TrackLocation = { src->TrackX, src->TrackY, src->TrackZ };
        dst->current_station = StationIndex::FromUnderlying(src->CurrentStation);
        if (src->BoatLocation.IsNull() || ride->mode != RideMode::boatHire || statusSrc != ::Vehicle::Status::travellingBoat)
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            dst->SetTrackType(RCT1TrackTypeToOpenRCT2(src->GetTrackType(), ride->type));
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->BoatLocation.x, src->BoatLocation.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(TrackElemType::flat);
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

        if (VehicleTypeIsReversed(rct1Ride.vehicleType))
        {
            dst->SetFlag(VehicleFlags::CarIsReversed);
        }
    }

    template<>
    void S4Importer::ImportEntity<Guest>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<Guest>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const Peep*>(&srcBase);
        ImportPeep(dst, src);

        dst->OutsideOfPark = static_cast<bool>(src->OutsideOfPark);
        dst->TimeToConsume = src->TimeToConsume;
        dst->VandalismSeen = src->VandalismSeen;

        // Balloons were always blue in RCT1 without AA/LL, umbrellas always red
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->UmbrellaColour = Drawing::Colour::brightRed;
            dst->BalloonColour = Drawing::Colour::lightBlue;
        }
        else
        {
            dst->UmbrellaColour = GetColour(src->UmbrellaColour);
            dst->BalloonColour = GetColour(src->BalloonColour);
        }
        dst->HatColour = GetColour(src->HatColour);

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

        RideUse::GetHistory().Set(dst->Id, RCT12GetRidesBeenOn(src));
        RideUse::GetTypeHistory().Set(dst->Id, RCT12GetRideTypesBeenOn(src));

        dst->Photo1RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo1RideRef);
        dst->PeepFlags = src->getPeepFlags(_gameVersion == FILE_VERSION_RCT1_LL);

        for (size_t i = 0; i < std::size(src->Thoughts); i++)
        {
            auto srcThought = &src->Thoughts[i];
            auto dstThought = &dst->Thoughts[i];
            dstThought->type = static_cast<PeepThoughtType>(srcThought->Type);
            if (srcThought->Item == kRCT12PeepThoughtItemNone)
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

        dst->SetItemFlags(src->GetItemFlags(_gameVersion == FILE_VERSION_RCT1));
    }

    template<>
    void S4Importer::ImportEntity<Staff>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<Staff>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const Peep*>(&srcBase);
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
    void S4Importer::ImportEntity<Litter>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<Litter>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityLitter*>(&srcBase);
        ImportEntityCommonProperties(dst, src);

        dst->SubType = Litter::Type(src->Type);
        dst->creationTick = src->CreationTick;
    }

    template<>
    void S4Importer::ImportEntity<SteamParticle>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<SteamParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntitySteamParticle*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_move = src->TimeToMove;
    }

    template<>
    void S4Importer::ImportEntity<MoneyEffect>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<MoneyEffect>(EntityId::FromUnderlying(srcBase.EntityIndex));
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
    void S4Importer::ImportEntity<VehicleCrashParticle>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<VehicleCrashParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityCrashedVehicleParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_live = src->TimeToLive;
        dst->colour[0] = GetColour(src->Colour[0]);
        dst->colour[1] = GetColour(src->Colour[1]);
        dst->crashed_sprite_base = src->CrashedEntityBase;
        dst->velocity_x = src->VelocityX;
        dst->velocity_y = src->VelocityY;
        dst->velocity_z = src->VelocityZ;
        dst->acceleration_x = src->AccelerationX;
        dst->acceleration_y = src->AccelerationY;
        dst->acceleration_z = src->AccelerationZ;
    }

    template<>
    void S4Importer::ImportEntity<ExplosionCloud>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<ExplosionCloud>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<ExplosionFlare>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<ExplosionFlare>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<CrashSplashParticle>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<CrashSplashParticle>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityParticle*>(&srcBase);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S4Importer::ImportEntity<JumpingFountain>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<JumpingFountain>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityJumpingFountain*>(&srcBase);

        auto fountainType = JumpingFountainType::Water;
        if (RCT12MiscEntityType(src->Type) == RCT12MiscEntityType::jumpingFountainSnow)
            fountainType = JumpingFountainType::Snow;

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->FountainType = fountainType;
        dst->NumTicksAlive = src->NumTicksAlive;
        dst->fountainFlags = src->fountainFlags;
        dst->TargetX = src->TargetX;
        dst->TargetY = src->TargetY;
        dst->Iteration = src->Iteration;
    }

    template<>
    void S4Importer::ImportEntity<Balloon>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<Balloon>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityBalloon*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->popped = src->Popped;
        dst->time_to_move = src->TimeToMove;
        // Balloons were always blue in RCT1 without AA/LL
        if (_gameVersion == FILE_VERSION_RCT1)
        {
            dst->colour = Drawing::Colour::lightBlue;
        }
        else
        {
            dst->colour = GetColour(src->Colour);
        }
    }

    template<>
    void S4Importer::ImportEntity<Duck>(GameState_t& gameState, const RCT12EntityBase& srcBase)
    {
        auto* dst = getGameState().entities.CreateEntityAt<Duck>(EntityId::FromUnderlying(srcBase.EntityIndex));
        auto* src = static_cast<const RCT12EntityDuck*>(&srcBase);

        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->target_x = src->TargetX;
        dst->target_y = src->TargetY;
        dst->state = static_cast<Duck::DuckState>(src->State);
    }

    void S4Importer::ImportEntity(GameState_t& gameState, const RCT12EntityBase& src)
    {
        switch (GetEntityTypeFromRCT1Sprite(src))
        {
            case EntityType::vehicle:
                ImportEntity<::Vehicle>(gameState, src);
                break;
            case EntityType::guest:
                ImportEntity<Guest>(gameState, src);
                break;
            case EntityType::staff:
                ImportEntity<Staff>(gameState, src);
                break;
            case EntityType::steamParticle:
                ImportEntity<SteamParticle>(gameState, src);
                break;
            case EntityType::moneyEffect:
                ImportEntity<MoneyEffect>(gameState, src);
                break;
            case EntityType::crashedVehicleParticle:
                ImportEntity<VehicleCrashParticle>(gameState, src);
                break;
            case EntityType::explosionCloud:
                ImportEntity<ExplosionCloud>(gameState, src);
                break;
            case EntityType::explosionFlare:
                ImportEntity<ExplosionFlare>(gameState, src);
                break;
            case EntityType::crashSplash:
                ImportEntity<CrashSplashParticle>(gameState, src);
                break;
            case EntityType::jumpingFountain:
                ImportEntity<JumpingFountain>(gameState, src);
                break;
            case EntityType::balloon:
                ImportEntity<Balloon>(gameState, src);
                break;
            case EntityType::duck:
                ImportEntity<Duck>(gameState, src);
                break;
            case EntityType::litter:
                ImportEntity<Litter>(gameState, src);
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
