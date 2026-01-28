/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkFile.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../Editor.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../Version.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/Crypt.h"
#include "../core/DataSerialiser.h"
#include "../core/File.h"
#include "../core/OrcaStream.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../drawing/Drawing.h"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Fountain.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../localisation/Formatter.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/RideUseSystem.h"
#include "../rct2/RCT2.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/WindowManager.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Legacy.h"
#include "ParkPreview.h"

#include <cassert>
#include <cstdint>
#include <ctime>
#include <numeric>
#include <optional>
#include <string_view>
#include <vector>

using namespace OpenRCT2;

namespace OpenRCT2
{
    namespace ParkFileChunkType
    {
        // clang-format off
//      constexpr uint32_t RESERVED_0           = 0x00;
        constexpr uint32_t AUTHORING            = 0x01;
        constexpr uint32_t OBJECTS              = 0x02;
        constexpr uint32_t SCENARIO             = 0x03;
        constexpr uint32_t GENERAL              = 0x04;
        constexpr uint32_t CLIMATE              = 0x05;
        constexpr uint32_t PARK                 = 0x06;
//      constexpr uint32_t HISTORY              = 0x07;
        constexpr uint32_t RESEARCH             = 0x08;
        constexpr uint32_t NOTIFICATIONS        = 0x09;
        constexpr uint32_t INTERFACE            = 0x20;
        constexpr uint32_t TILES                = 0x30;
        constexpr uint32_t ENTITIES             = 0x31;
        constexpr uint32_t RIDES                = 0x32;
        constexpr uint32_t BANNERS              = 0x33;
//      constexpr uint32_t STAFF                = 0x35;
        constexpr uint32_t CHEATS               = 0x36;
        constexpr uint32_t RESTRICTED_OBJECTS   = 0x37;
        constexpr uint32_t PLUGIN_STORAGE       = 0x38;
        constexpr uint32_t PREVIEW              = 0x39;
        constexpr uint32_t PACKED_OBJECTS       = 0x80;
        // clang-format on
    } // namespace ParkFileChunkType

    class ParkFile
    {
    public:
        ObjectList RequiredObjects;
        std::vector<const ObjectRepositoryItem*> ExportObjectsList;
        bool OmitTracklessRides{};

    private:
        std::unique_ptr<OrcaStream> _os;
        ObjectEntryIndex _pathToSurfaceMap[kMaxPathObjects];
        ObjectEntryIndex _pathToQueueSurfaceMap[kMaxPathObjects];
        ObjectEntryIndex _pathToRailingsMap[kMaxPathObjects];

        void ThrowIfIncompatibleVersion()
        {
            const auto& header = _os->getHeader();
            if (/*header.targetVersion < kParkFileMinSupportedVersion || */ header.minVersion > kParkFileCurrentVersion)
            {
                throw UnsupportedVersionException(header.minVersion, header.targetVersion);
            }
        }

    public:
        bool IsSemiCompatibleVersion(uint32_t& minVersion, uint32_t& targetVersion)
        {
            const auto& header = _os->getHeader();
            minVersion = header.minVersion;
            targetVersion = header.targetVersion;
            return targetVersion > kParkFileCurrentVersion;
        }

        void Load(const std::string_view path, const bool skipObjectCheck)
        {
            FileStream fs(path, FileMode::open);
            Load(fs, skipObjectCheck);
        }

        void Load(IStream& stream, const bool skipObjectCheck)
        {
            _os = std::make_unique<OrcaStream>(stream, OrcaStream::Mode::reading);
            ThrowIfIncompatibleVersion();

            RequiredObjects = {};
            if (!skipObjectCheck)
            {
                ReadWriteObjectsChunk(*_os);
                ReadWritePackedObjectsChunk(*_os);
            }
        }

        void Import(GameState_t& gameState)
        {
            auto& os = *_os;
            ReadWriteTilesChunk(gameState, os);
            ReadWriteBannersChunk(gameState, os);
            ReadWriteRidesChunk(gameState, os);
            ReadWriteEntitiesChunk(gameState, os);
            ReadWriteScenarioChunk(gameState, os);
            ReadWriteGeneralChunk(gameState, os);
            ReadWriteParkChunk(gameState, os);
            ReadWriteClimateChunk(gameState, os);
            ReadWriteResearchChunk(gameState, os);
            ReadWriteNotificationsChunk(gameState, os);
            ReadWriteInterfaceChunk(gameState, os);
            ReadWriteCheatsChunk(gameState, os);
            ReadWriteRestrictedObjectsChunk(gameState, os);
            ReadWritePluginStorageChunk(gameState, os);
            if (os.getHeader().targetVersion < 0x4)
            {
                UpdateTrackElementsRideType();
            }

            // Initial cash will eventually be removed
            gameState.scenarioOptions.initialCash = gameState.park.cash;
        }

        void Save(GameState_t& gameState, IStream& stream, int16_t compressionLevel)
        {
            OrcaStream os(stream, OrcaStream::Mode::writing, compressionLevel);

            auto& header = os.getHeader();
            header.magic = kParkFileMagic;
            header.targetVersion = kParkFileCurrentVersion;
            header.minVersion = kParkFileMinVersion;

            ReadWriteAuthoringChunk(os);
            ReadWriteObjectsChunk(os);
            ReadWriteTilesChunk(gameState, os);
            ReadWriteBannersChunk(gameState, os);
            ReadWriteRidesChunk(gameState, os);
            ReadWriteEntitiesChunk(gameState, os);
            ReadWriteScenarioChunk(gameState, os);
            ReadWriteGeneralChunk(gameState, os);
            ReadWriteParkChunk(gameState, os);
            ReadWriteClimateChunk(gameState, os);
            ReadWriteResearchChunk(gameState, os);
            ReadWriteNotificationsChunk(gameState, os);
            ReadWriteInterfaceChunk(gameState, os);
            ReadWriteCheatsChunk(gameState, os);
            ReadWriteRestrictedObjectsChunk(gameState, os);
            ReadWritePluginStorageChunk(gameState, os);
            ReadWritePreviewChunk(gameState, os);
            ReadWritePackedObjectsChunk(os);
        }

        void Save(GameState_t& gameState, const std::string_view path, int16_t compressionLevel)
        {
            FileStream fs(path, FileMode::write);
            Save(gameState, fs, compressionLevel);
        }

        ScenarioIndexEntry ReadScenarioChunk()
        {
            ScenarioIndexEntry entry{};
            auto& os = *_os;
            os.readWriteChunk(ParkFileChunkType::SCENARIO, [&entry](OrcaStream::ChunkStream& cs) {
                entry.Category = cs.read<Scenario::Category>();

                std::string name;
                ReadWriteStringTable(cs, name, "en-GB");
                entry.Name = name;
                entry.InternalName = name;

                std::string parkName;
                ReadWriteStringTable(cs, parkName, "en-GB");

                std::string scenarioDetails;
                ReadWriteStringTable(cs, scenarioDetails, "en-GB");
                entry.Details = scenarioDetails;

                // wrong order is intentional here due to ReadWriteScenarioChunk writing guests first
                entry.ObjectiveType = cs.read<Scenario::ObjectiveType>();
                entry.ObjectiveArg1 = cs.read<uint8_t>();
                entry.ObjectiveArg3 = cs.read<uint16_t>();
                entry.ObjectiveArg2 = cs.read<int32_t>();

                entry.SourceGame = ScenarioSource::Other;
            });
            return entry;
        }

        ParkPreview ReadPreviewChunk()
        {
            ParkPreview preview{};
            auto& os = *_os;
            os.readWriteChunk(ParkFileChunkType::PREVIEW, [&preview](OrcaStream::ChunkStream& cs) {
                cs.readWrite(preview.parkName);
                cs.readWrite(preview.parkRating);
                cs.readWrite(preview.year);
                cs.readWrite(preview.month);
                cs.readWrite(preview.day);
                cs.readWrite(preview.parkUsesMoney);
                cs.readWrite(preview.cash);
                cs.readWrite(preview.numRides);
                cs.readWrite(preview.numGuests);

                cs.readWriteVector(preview.images, [&cs](PreviewImage& image) {
                    cs.readWrite(image.type);
                    cs.readWrite(image.width);
                    cs.readWrite(image.height);
                    cs.readWriteArray(image.pixels, [&cs](Drawing::PaletteIndex& pixel) {
                        cs.readWrite(pixel);
                        return true;
                    });
                });
            });
            return preview;
        }

    private:
        static uint8_t GetMinCarsPerTrain(uint8_t value)
        {
            return value >> 4;
        }

        static uint8_t GetMaxCarsPerTrain(uint8_t value)
        {
            return value & 0xF;
        }

        void ReadWriteAuthoringChunk(OrcaStream& os)
        {
            // Write-only for now
            if (os.getMode() == OrcaStream::Mode::writing)
            {
                os.readWriteChunk(ParkFileChunkType::AUTHORING, [](OrcaStream::ChunkStream& cs) {
                    cs.write(std::string_view(gVersionInfoFull));
                    std::vector<std::string> authors;
                    cs.readWriteVector(authors, [](std::string& s) {});
                    cs.write(std::string_view());                  // custom notes that can be attached to the save
                    cs.write(static_cast<uint64_t>(std::time(0))); // date started
                    cs.write(static_cast<uint64_t>(std::time(0))); // date modified
                });
            }
        }

        void ReadWriteObjectsChunk(OrcaStream& os)
        {
            static constexpr uint8_t DESCRIPTOR_NONE = 0;
            static constexpr uint8_t DESCRIPTOR_DAT = 1;
            static constexpr uint8_t DESCRIPTOR_JSON = 2;

            if (os.getMode() == OrcaStream::Mode::reading)
            {
                std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), kObjectEntryIndexNull);
                std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), kObjectEntryIndexNull);
                std::fill(std::begin(_pathToRailingsMap), std::end(_pathToRailingsMap), kObjectEntryIndexNull);
                auto* pathToSurfaceMap = _pathToSurfaceMap;
                auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
                auto* pathToRailingsMap = _pathToRailingsMap;
                const auto version = os.getHeader().targetVersion;

                ObjectList requiredObjects;
                struct LegacyFootpathMapping
                {
                    ObjectEntryIndex originalEntryIndex;
                    const RCT2::FootpathMapping& mapping;
                };
                std::vector<LegacyFootpathMapping> legacyPathMappings;
                os.readWriteChunk(
                    ParkFileChunkType::OBJECTS, [&requiredObjects, version, &legacyPathMappings](OrcaStream::ChunkStream& cs) {
                        auto numSubLists = cs.read<uint16_t>();
                        for (size_t i = 0; i < numSubLists; i++)
                        {
                            auto objectType = static_cast<ObjectType>(cs.read<uint16_t>());
                            auto subListSize = static_cast<ObjectEntryIndex>(cs.read<uint32_t>());
                            for (ObjectEntryIndex j = 0; j < subListSize; j++)
                            {
                                auto kind = cs.read<uint8_t>();

                                switch (kind)
                                {
                                    case DESCRIPTOR_NONE:
                                        break;
                                    case DESCRIPTOR_DAT:
                                    {
                                        RCTObjectEntry datEntry;
                                        cs.read(&datEntry, sizeof(datEntry));
                                        ObjectEntryDescriptor desc(datEntry);
                                        if (version < kFixedObsoleteFootpathsVersion && datEntry.GetType() == ObjectType::paths)
                                        {
                                            auto footpathMapping = GetFootpathMapping(desc);
                                            if (footpathMapping != nullptr)
                                            {
                                                legacyPathMappings.push_back(LegacyFootpathMapping{ j, *footpathMapping });
                                                continue;
                                            }
                                        }

                                        requiredObjects.SetObject(j, desc);
                                        break;
                                    }
                                    case DESCRIPTOR_JSON:
                                    {
                                        ObjectEntryDescriptor desc;
                                        desc.Type = objectType;
                                        auto identifier = cs.read<std::string>();
                                        if (version <= 7)
                                        {
                                            auto newIdentifier = MapToNewObjectIdentifier(identifier);
                                            if (!newIdentifier.empty())
                                            {
                                                identifier = newIdentifier;
                                            }
                                        }
                                        else if (version <= 12)
                                        {
                                            if (identifier == "openrct2.ride.rmct1")
                                            {
                                                identifier = "openrct2.ride.hybrid_coaster";
                                            }
                                            else if (identifier == "openrct2.ride.rmct2")
                                            {
                                                identifier = "openrct2.ride.single_rail_coaster";
                                            }
                                        }
                                        else if (version <= 14)
                                        {
                                            if (identifier == "openrct2.ride.alp1")
                                            {
                                                identifier = "openrct2.ride.alpine_coaster";
                                            }
                                        }
                                        desc.Identifier = identifier;
                                        desc.Version = VersionTuple(cs.read<std::string>());

                                        if (version <= 2)
                                        {
                                            auto footpathMapping = GetFootpathMapping(desc);
                                            if (footpathMapping != nullptr)
                                            {
                                                legacyPathMappings.push_back(LegacyFootpathMapping{ j, *footpathMapping });

                                                continue;
                                            }
                                        }

                                        requiredObjects.SetObject(j, desc);
                                        break;
                                    }
                                    default:
                                        throw std::runtime_error("Unknown object descriptor kind.");
                                }
                            }
                        }
                    });

                for (const auto& mapping : legacyPathMappings)
                {
                    UpdateFootpathsFromMapping(
                        pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap, requiredObjects, mapping.originalEntryIndex,
                        &mapping.mapping);
                }

                if (version < kPeepNamesObjectsVersion)
                {
                    AppendRequiredObjects(
                        requiredObjects, ObjectType::peepNames, std::vector<std::string_view>({ "rct2.peep_names.original" }));
                }

                if (version < kPeepAnimationObjectsVersion)
                {
                    auto animObjects = GetLegacyPeepAnimationObjects();
                    AppendRequiredObjects(requiredObjects, ObjectType::peepAnimations, animObjects);
                }

                if (version < kClimateObjectsVersion)
                {
                    RCT12::ClimateType legacyClimate{};
                    os.readWriteChunk(ParkFileChunkType::CLIMATE, [&legacyClimate](OrcaStream::ChunkStream& cs) {
                        cs.readWrite(legacyClimate);
                    });

                    auto climateObjId = GetClimateObjectIdFromLegacyClimateType(legacyClimate);
                    AppendRequiredObjects(requiredObjects, ObjectType::climate, std::vector({ climateObjId }));
                }

                RequiredObjects = std::move(requiredObjects);
            }
            else
            {
                os.readWriteChunk(ParkFileChunkType::OBJECTS, [](OrcaStream::ChunkStream& cs) {
                    auto& objManager = GetContext()->GetObjectManager();
                    auto objectList = objManager.GetLoadedObjects();

                    // Write number of object sub lists
                    cs.write(static_cast<uint16_t>(getTransientObjectTypes().size()));
                    for (auto objectType : getTransientObjectTypes())
                    {
                        // Write sub list
                        const auto& list = objectList.GetList(objectType);
                        cs.write(static_cast<uint16_t>(objectType));
                        cs.write(static_cast<uint32_t>(list.size()));
                        for (const auto& entry : list)
                        {
                            if (entry.HasValue())
                            {
                                if (entry.Generation == ObjectGeneration::JSON)
                                {
                                    cs.write(DESCRIPTOR_JSON);
                                    cs.write(entry.Identifier);
                                    cs.write(VersionString(entry.Version));
                                }
                                else
                                {
                                    cs.write(DESCRIPTOR_DAT);
                                    cs.write(&entry.Entry, sizeof(RCTObjectEntry));
                                }
                            }
                            else
                            {
                                cs.write(DESCRIPTOR_NONE);
                            }
                        }
                    }
                });
            }
        }

        void ReadWriteScenarioChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::SCENARIO, [&gameState, &os](OrcaStream::ChunkStream& cs) {
                cs.readWrite(gameState.scenarioOptions.category);
                ReadWriteStringTable(cs, gameState.scenarioOptions.name, "en-GB");
                ReadWriteStringTable(cs, gameState.park.name, "en-GB");
                ReadWriteStringTable(cs, gameState.scenarioOptions.details, "en-GB");

                cs.readWrite(gameState.scenarioOptions.objective.Type);
                cs.readWrite(gameState.scenarioOptions.objective.Year);
                cs.readWrite(gameState.scenarioOptions.objective.NumGuests);
                cs.readWrite(gameState.scenarioOptions.objective.Currency);

                cs.readWrite(gameState.scenarioParkRatingWarningDays);

                cs.readWrite(gameState.scenarioCompletedCompanyValue);
                if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined
                    || gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                {
                    cs.write("");
                }
                else
                {
                    cs.readWrite(gameState.scenarioCompletedBy);
                }

                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    auto earlyCompletion = cs.read<bool>();
                    if (Network::GetMode() == Network::Mode::client)
                    {
                        gAllowEarlyCompletionInNetworkPlay = earlyCompletion;
                    }
                }
                else
                {
                    cs.write(AllowEarlyCompletion());
                }

                if (os.getHeader().targetVersion >= 1)
                {
                    cs.readWrite(gameState.scenarioFileName);
                }
            });
        }

        void ReadWritePreviewChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::PREVIEW, [&gameState](OrcaStream::ChunkStream& cs) {
                auto preview = generatePreviewFromGameState(gameState);

                cs.readWrite(preview.parkName);
                cs.readWrite(preview.parkRating);
                cs.readWrite(preview.year);
                cs.readWrite(preview.month);
                cs.readWrite(preview.day);
                cs.readWrite(preview.parkUsesMoney);
                cs.readWrite(preview.cash);
                cs.readWrite(preview.numRides);
                cs.readWrite(preview.numGuests);

                cs.readWriteVector(preview.images, [&cs](PreviewImage& image) {
                    cs.readWrite(image.type);
                    cs.readWrite(image.width);
                    cs.readWrite(image.height);
                    cs.readWriteArray(image.pixels, [&cs](Drawing::PaletteIndex& pixel) {
                        cs.readWrite(pixel);
                        return true;
                    });
                });
            });
        }

        void ReadWriteGeneralChunk(GameState_t& gameState, OrcaStream& os)
        {
            const auto version = os.getHeader().targetVersion;
            auto found = os.readWriteChunk(ParkFileChunkType::GENERAL, [&](OrcaStream::ChunkStream& cs) {
                // Only GAME_PAUSED_NORMAL from gGamePaused is relevant.
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    const uint8_t isPaused = cs.read<uint8_t>();
                    gGamePaused &= ~GAME_PAUSED_NORMAL;
                    gGamePaused |= (isPaused & GAME_PAUSED_NORMAL);
                }
                else
                {
                    const uint8_t isPaused = (gGamePaused & GAME_PAUSED_NORMAL);
                    cs.write(isPaused);
                }
                cs.readWrite(gameState.currentTicks);
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    uint16_t monthTicks;
                    uint32_t monthsElapsed;
                    cs.readWrite(monthTicks);
                    cs.readWrite(monthsElapsed);
                    gameState.date = Date{ monthsElapsed, monthTicks };
                }
                else
                {
                    auto& date = GetDate();
                    cs.write(date.GetMonthTicks());
                    cs.write(date.GetMonthsElapsed());
                }

                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    uint32_t s0{}, s1{};
                    cs.readWrite(s0);
                    cs.readWrite(s1);
                    Random::RCT2::Seed s{ s0, s1 };
                    gameState.scenarioRand.seed(s);
                }
                else
                {
                    auto randState = gameState.scenarioRand.state();
                    cs.write(randState.s0);
                    cs.write(randState.s1);
                }

                cs.readWrite(gameState.scenarioOptions.guestInitialHappiness);
                if (version <= 18)
                {
                    money16 tempGuestInitialCash{};
                    cs.readWrite(tempGuestInitialCash);
                    gameState.scenarioOptions.guestInitialCash = ToMoney64(tempGuestInitialCash);
                }
                else
                {
                    cs.readWrite(gameState.scenarioOptions.guestInitialCash);
                }
                cs.readWrite(gameState.scenarioOptions.guestInitialHunger);
                cs.readWrite(gameState.scenarioOptions.guestInitialThirst);

                cs.readWrite(gameState.nextGuestNumber);
                cs.readWriteVector(gameState.peepSpawns, [&cs](PeepSpawn& spawn) {
                    cs.readWrite(spawn.x);
                    cs.readWrite(spawn.y);
                    cs.readWrite(spawn.z);
                    cs.readWrite(spawn.direction);
                });

                if (version <= 18)
                {
                    money16 tempLandPrice{};
                    money16 tempConstructionRightPrice{};
                    cs.readWrite(tempLandPrice);
                    cs.readWrite(tempConstructionRightPrice);
                    gameState.scenarioOptions.landPrice = ToMoney64(tempLandPrice);
                    gameState.scenarioOptions.constructionRightsPrice = ToMoney64(tempConstructionRightPrice);
                }
                else
                {
                    cs.readWrite(gameState.scenarioOptions.landPrice);
                    cs.readWrite(gameState.scenarioOptions.constructionRightsPrice);
                }
                cs.readWrite(gameState.grassSceneryTileLoopPosition);
                cs.readWrite(gameState.widePathTileLoopPosition);

                auto& rideRatings = gameState.rideRatingUpdateStates;
                if (os.getHeader().targetVersion >= 21)
                {
                    cs.readWriteArray(rideRatings, [this, &cs](RideRating::UpdateState& calcData) {
                        ReadWriteRideRatingCalculationData(cs, calcData);
                        return true;
                    });
                }
                else
                {
                    // Only single state was stored prior to version 20.
                    if (os.getMode() == OrcaStream::Mode::reading)
                    {
                        // Since we read only one state ensure the rest is reset.
                        RideRating::ResetUpdateStates();
                    }
                    auto& rideRatingUpdateState = rideRatings[0];
                    ReadWriteRideRatingCalculationData(cs, rideRatingUpdateState);
                }

                if (os.getHeader().targetVersion >= 14)
                {
                    cs.readWrite(gIsAutosave);
                }
            });
            if (!found)
            {
                throw std::runtime_error("No general chunk found.");
            }
        }

        void ReadWriteRideRatingCalculationData(OrcaStream::ChunkStream& cs, RideRating::UpdateState& calcData)
        {
            cs.readWrite(calcData.AmountOfBrakes);
            cs.readWrite(calcData.Proximity);
            cs.readWrite(calcData.ProximityStart);
            cs.readWrite(calcData.CurrentRide);
            cs.readWrite(calcData.State);
            cs.readWrite(calcData.ProximityTrackType);
            cs.readWrite(calcData.ProximityBaseHeight);
            cs.readWrite(calcData.ProximityTotal);
            cs.readWriteArray(calcData.ProximityScores, [&cs](uint16_t& value) {
                cs.readWrite(value);
                return true;
            });
            cs.readWrite(calcData.AmountOfBrakes);
            cs.readWrite(calcData.AmountOfReversers);
            cs.readWrite(calcData.StationFlags);
        }

        void ReadWriteInterfaceChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::INTERFACE, [&gameState](OrcaStream::ChunkStream& cs) {
                cs.readWrite(gameState.savedView.x);
                cs.readWrite(gameState.savedView.y);
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    auto savedZoomlevel = static_cast<ZoomLevel>(cs.read<int8_t>());
                    gameState.savedViewZoom = std::clamp(savedZoomlevel, ZoomLevel::min(), ZoomLevel::max());
                }
                else
                {
                    cs.write(static_cast<int8_t>(gameState.savedViewZoom));
                }
                cs.readWrite(gameState.savedViewRotation);
                cs.readWrite(gameState.lastEntranceStyle);
                cs.readWrite(gameState.editorStep);
            });
        }

        void ReadWriteCheatsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::CHEATS, [](OrcaStream::ChunkStream& cs) {
                DataSerialiser ds(cs.getMode() == OrcaStream::Mode::writing, cs.getStream());
                CheatsSerialise(ds);
            });
        }

        void ReadWriteRestrictedObjectsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::RESTRICTED_OBJECTS, [](OrcaStream::ChunkStream& cs) {
                auto& restrictedScenery = GetRestrictedScenery();

                // We are want to support all object types in the future, so convert scenery type
                // to object type when we write the list
                cs.readWriteVector(restrictedScenery, [&cs](ScenerySelection& item) {
                    if (cs.getMode() == OrcaStream::Mode::reading)
                    {
                        item.SceneryType = GetSceneryTypeFromObjectType(static_cast<ObjectType>(cs.read<uint16_t>()));
                        item.EntryIndex = cs.read<ObjectEntryIndex>();
                    }
                    else
                    {
                        cs.write(static_cast<uint16_t>(GetObjectTypeFromSceneryType(item.SceneryType)));
                        cs.write(item.EntryIndex);
                    }
                });
            });
        }

        void ReadWritePluginStorageChunk(GameState_t& gameState, OrcaStream& os)
        {
            if (os.getMode() == OrcaStream::Mode::writing)
            {
#ifdef ENABLE_SCRIPTING
                // Dump the plugin storage to JSON (stored in park)
                auto& scriptEngine = GetContext()->GetScriptEngine();
                gameState.pluginStorage = scriptEngine.GetParkStorageAsJSON();
#endif
                if (gameState.pluginStorage.empty() || gameState.pluginStorage == "{}")
                {
                    // Don't write the chunk if there is no plugin storage
                    return;
                }
            }

            os.readWriteChunk(ParkFileChunkType::PLUGIN_STORAGE, [&gameState](OrcaStream::ChunkStream& cs) {
                cs.readWrite(gameState.pluginStorage);
            });

            if (os.getMode() == OrcaStream::Mode::reading)
            {
#ifdef ENABLE_SCRIPTING
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.SetParkStorageFromJSON(gameState.pluginStorage);
#endif
            }
        }

        void ReadWritePackedObjectsChunk(OrcaStream& os)
        {
            static constexpr uint8_t DESCRIPTOR_DAT = 0;
            static constexpr uint8_t DESCRIPTOR_PARKOBJ = 1;

            if (os.getMode() == OrcaStream::Mode::writing && ExportObjectsList.size() == 0)
            {
                // Do not emit chunk if there are no packed objects
                return;
            }

            os.readWriteChunk(ParkFileChunkType::PACKED_OBJECTS, [this](OrcaStream::ChunkStream& cs) {
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    auto& objRepository = GetContext()->GetObjectRepository();
                    auto numObjects = cs.read<uint32_t>();
                    for (uint32_t i = 0; i < numObjects; i++)
                    {
                        auto type = cs.read<uint8_t>();
                        if (type == DESCRIPTOR_DAT)
                        {
                            RCTObjectEntry entry;
                            cs.read(&entry, sizeof(entry));
                            auto size = cs.read<uint32_t>();
                            std::vector<uint8_t> data;
                            data.resize(size);
                            cs.read(data.data(), data.size());

                            auto legacyIdentifier = entry.GetName();
                            if (objRepository.FindObjectLegacy(legacyIdentifier) == nullptr)
                            {
                                objRepository.AddObjectFromFile(
                                    ObjectGeneration::DAT, legacyIdentifier, data.data(), data.size());
                            }
                        }
                        else if (type == DESCRIPTOR_PARKOBJ)
                        {
                            auto identifier = cs.read<std::string>();
                            auto size = cs.read<uint32_t>();
                            std::vector<uint8_t> data;
                            data.resize(size);
                            cs.read(data.data(), data.size());
                            if (objRepository.FindObject(identifier) == nullptr)
                            {
                                objRepository.AddObjectFromFile(ObjectGeneration::JSON, identifier, data.data(), data.size());
                            }
                        }
                        else
                        {
                            throw std::runtime_error("Unsupported packed object");
                        }
                    }
                }
                else
                {
                    auto& stream = cs.getStream();
                    auto countPosition = stream.GetPosition();

                    // Write placeholder count, update later
                    uint32_t count = 0;
                    cs.write(count);

                    // Write objects
                    for (const auto* ori : ExportObjectsList)
                    {
                        auto extension = Path::GetExtension(ori->Path);
                        if (String::iequals(extension, ".dat"))
                        {
                            cs.write(DESCRIPTOR_DAT);
                            cs.write(&ori->ObjectEntry, sizeof(RCTObjectEntry));
                        }
                        else if (String::iequals(extension, ".parkobj"))
                        {
                            cs.write(DESCRIPTOR_PARKOBJ);
                            cs.write(ori->Identifier);
                        }
                        else
                        {
                            Console::WriteLine("%s not packed: unsupported extension.", ori->Identifier.c_str());
                            continue;
                        }

                        auto data = File::ReadAllBytes(ori->Path);
                        cs.write<uint32_t>(static_cast<uint32_t>(data.size()));
                        cs.write(data.data(), data.size());
                        count++;
                    }

                    auto backupPosition = stream.GetPosition();
                    stream.SetPosition(countPosition);
                    cs.write(count);
                    stream.SetPosition(backupPosition);
                }
            });
        }

        void ReadWriteClimateChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::CLIMATE, [&os, &gameState](OrcaStream::ChunkStream& cs) {
                auto version = os.getHeader().targetVersion;
                if (version < kClimateObjectsVersion)
                {
                    // Legacy climate is converted elsewhere, so we can skip it here.
                    cs.ignore<RCT12::ClimateType>();
                }

                cs.readWrite(gameState.weatherUpdateTimer);

                for (auto* cl : { &gameState.weatherCurrent, &gameState.weatherNext })
                {
                    cs.readWrite(cl->weatherType);
                    cs.readWrite(cl->temperature);
                    cs.readWrite(cl->weatherEffect);
                    cs.readWrite(cl->weatherGloom);
                    cs.readWrite(cl->level);
                }
            });
        }

        void ReadWriteParkChunk(GameState_t& gameState, OrcaStream& os)
        {
            // TODO: load/save all parks
            auto& park = gameState.park;

            os.readWriteChunk(
                ParkFileChunkType::PARK, [version = os.getHeader().targetVersion, &park](OrcaStream::ChunkStream& cs) {
                    cs.readWrite(park.name);
                    cs.readWrite(park.cash);
                    cs.readWrite(park.bankLoan);
                    cs.readWrite(park.maxBankLoan);
                    cs.readWrite(park.bankLoanInterestRate);
                    cs.readWrite(park.flags);
                    if (version <= 18)
                    {
                        money16 tempParkEntranceFee{};
                        cs.readWrite(tempParkEntranceFee);
                        park.entranceFee = ToMoney64(tempParkEntranceFee);
                    }
                    else
                    {
                        cs.readWrite(park.entranceFee);
                    }

                    cs.readWrite(park.staffHandymanColour);
                    cs.readWrite(park.staffMechanicColour);
                    cs.readWrite(park.staffSecurityColour);
                    cs.readWrite(park.samePriceThroughoutPark);

                    // Finances
                    if (cs.getMode() == OrcaStream::Mode::reading)
                    {
                        auto numMonths = std::min<uint32_t>(kExpenditureTableMonthCount, cs.read<uint32_t>());
                        auto numTypes = std::min<uint32_t>(static_cast<uint32_t>(ExpenditureType::count), cs.read<uint32_t>());
                        for (uint32_t i = 0; i < numMonths; i++)
                        {
                            for (uint32_t j = 0; j < numTypes; j++)
                            {
                                park.expenditureTable[i][j] = cs.read<money64>();
                            }
                        }
                    }
                    else
                    {
                        auto numMonths = static_cast<uint32_t>(kExpenditureTableMonthCount);
                        auto numTypes = static_cast<uint32_t>(ExpenditureType::count);

                        cs.write(numMonths);
                        cs.write(numTypes);
                        for (uint32_t i = 0; i < numMonths; i++)
                        {
                            for (uint32_t j = 0; j < numTypes; j++)
                            {
                                cs.write(park.expenditureTable[i][j]);
                            }
                        }
                    }
                    cs.readWrite(park.historicalProfit);

                    // Marketing
                    cs.readWriteVector(park.marketingCampaigns, [&cs](MarketingCampaign& campaign) {
                        cs.readWrite(campaign.Type);
                        cs.readWrite(campaign.WeeksLeft);
                        cs.readWrite(campaign.flags.holder);
                        cs.readWrite(campaign.RideId);
                    });

                    // Awards
                    auto& currentAwards = park.currentAwards;
                    if (version <= 6)
                    {
                        Award awards[RCT2::Limits::kMaxAwards]{};
                        cs.readWriteArray(awards, [&cs, &currentAwards](Award& award) {
                            if (award.Time != 0)
                            {
                                cs.readWrite(award.Time);
                                cs.readWrite(award.Type);
                                currentAwards.push_back(award);
                                return true;
                            }

                            return false;
                        });
                    }
                    else
                    {
                        cs.readWriteVector(currentAwards, [&cs](Award& award) {
                            cs.readWrite(award.Time);
                            cs.readWrite(award.Type);
                        });
                    }
                    cs.readWrite(park.value);
                    cs.readWrite(park.companyValue);
                    cs.readWrite(park.size);
                    cs.readWrite(park.numGuestsInPark);
                    cs.readWrite(park.numGuestsHeadingForPark);
                    cs.readWrite(park.rating);
                    cs.readWrite(park.ratingCasualtyPenalty);
                    cs.readWrite(park.currentExpenditure);
                    cs.readWrite(park.currentProfit);
                    cs.readWrite(park.weeklyProfitAverageDividend);
                    cs.readWrite(park.weeklyProfitAverageDivisor);
                    cs.readWrite(park.totalAdmissions);
                    cs.readWrite(park.totalIncomeFromAdmissions);
                    if (version <= 16)
                    {
                        money16 legacyTotalRideValueForMoney = 0;
                        cs.readWrite(legacyTotalRideValueForMoney);
                        park.totalRideValueForMoney = legacyTotalRideValueForMoney;
                    }
                    else
                    {
                        cs.readWrite(park.totalRideValueForMoney);
                    }
                    cs.readWrite(park.numGuestsInParkLastWeek);
                    cs.readWrite(park.guestChangeModifier);
                    cs.readWrite(park.guestGenerationProbability);
                    cs.readWrite(park.suggestedGuestMaximum);

                    cs.readWriteArray(park.peepWarningThrottle, [&cs](uint8_t& value) {
                        cs.readWrite(value);
                        return true;
                    });

                    if (version < k16BitParkHistoryVersion)
                    {
                        if (cs.getMode() == OrcaStream::Mode::reading)
                        {
                            uint8_t smallHistory[kParkRatingHistorySize];
                            cs.readWriteArray(smallHistory, [&cs](uint8_t& value) {
                                cs.readWrite(value);
                                return true;
                            });
                            for (int i = 0; i < kParkRatingHistorySize; i++)
                            {
                                if (smallHistory[i] == kRCT12ParkHistoryUndefined)
                                    park.ratingHistory[i] = kParkRatingHistoryUndefined;
                                else
                                {
                                    park.ratingHistory[i] = static_cast<uint16_t>(
                                        smallHistory[i] * kRCT12ParkRatingHistoryFactor);
                                }
                            }
                        }
                        else
                        {
                            uint8_t smallHistory[kParkRatingHistorySize];
                            for (int i = 0; i < kParkRatingHistorySize; i++)
                            {
                                if (park.ratingHistory[i] == kParkRatingHistoryUndefined)
                                    smallHistory[i] = kRCT12ParkHistoryUndefined;
                                else
                                {
                                    smallHistory[i] = static_cast<uint8_t>(
                                        park.ratingHistory[i] / kRCT12ParkRatingHistoryFactor);
                                }
                            }
                            cs.readWriteArray(smallHistory, [&cs](uint8_t& value) {
                                cs.readWrite(value);
                                return true;
                            });
                        }
                    }
                    else
                    {
                        cs.readWriteArray(park.ratingHistory, [&cs](uint16_t& value) {
                            cs.readWrite(value);
                            return true;
                        });
                    }

                    cs.readWriteArray(park.guestsInParkHistory, [&cs](uint32_t& value) {
                        cs.readWrite(value);
                        return true;
                    });

                    cs.readWriteArray(park.cashHistory, [&cs](money64& value) {
                        cs.readWrite(value);
                        return true;
                    });
                    cs.readWriteArray(park.weeklyProfitHistory, [&cs](money64& value) {
                        cs.readWrite(value);
                        return true;
                    });
                    cs.readWriteArray(park.valueHistory, [&cs](money64& value) {
                        cs.readWrite(value);
                        return true;
                    });
                });
        }

        void ReadWriteResearchChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::RESEARCH, [&gameState](OrcaStream::ChunkStream& cs) {
                // Research status
                cs.readWrite(gameState.researchFundingLevel);
                cs.readWrite(gameState.researchPriorities);
                cs.readWrite(gameState.researchProgressStage);
                cs.readWrite(gameState.researchProgress);
                cs.readWrite(gameState.researchExpectedMonth);
                cs.readWrite(gameState.researchExpectedDay);
                ReadWriteResearchItem(cs, gameState.researchLastItem);
                ReadWriteResearchItem(cs, gameState.researchNextItem);

                // Invention list
                cs.readWriteVector(
                    gameState.researchItemsUninvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
                cs.readWriteVector(
                    gameState.researchItemsInvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
            });
        }

        static void ReadWriteResearchItem(OrcaStream::ChunkStream& cs, std::optional<ResearchItem>& item)
        {
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                auto hasValue = cs.read<bool>();
                if (hasValue)
                {
                    ResearchItem placeholder;
                    ReadWriteResearchItem(cs, placeholder);
                    item = placeholder;
                }
            }
            else
            {
                if (item)
                {
                    cs.write<bool>(true);
                    ReadWriteResearchItem(cs, *item);
                }
                else
                {
                    cs.write<bool>(false);
                }
            }
        }

        static void ReadWriteResearchItem(OrcaStream::ChunkStream& cs, ResearchItem& item)
        {
            cs.readWrite(item.type);
            cs.readWrite(item.baseRideType);
            cs.readWrite(item.entryIndex);
            cs.readWrite(item.flags);
            cs.readWrite(item.category);
        }

        void ReadWriteNotificationsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::NOTIFICATIONS, [&gameState](OrcaStream::ChunkStream& cs) {
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    std::vector<News::Item> recent;
                    cs.readWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    std::vector<News::Item> archived;
                    cs.readWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    News::importNewsItems(gameState, recent, archived);
                }
                else
                {
                    std::vector<News::Item> recent(
                        std::begin(gameState.newsItems.GetRecent()), std::end(gameState.newsItems.GetRecent()));
                    cs.readWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    std::vector<News::Item> archived(
                        std::begin(gameState.newsItems.GetArchived()), std::end(gameState.newsItems.GetArchived()));
                    cs.readWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });
                }
            });
        }

        static void ReadWriteNewsItem(OrcaStream::ChunkStream& cs, News::Item& item)
        {
            cs.readWrite(item.type);
            cs.readWrite(item.flags);
            cs.readWrite(item.assoc);
            cs.readWrite(item.ticks);
            cs.readWrite(item.monthYear);
            cs.readWrite(item.day);
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                auto s = cs.read<std::string>();
                item.text = s;
            }
            else
            {
                cs.write(std::string_view(item.text));
            }
        }

        void ReadWriteTilesChunk(GameState_t& gameState, OrcaStream& os)
        {
            auto* pathToSurfaceMap = _pathToSurfaceMap;
            auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
            auto* pathToRailingsMap = _pathToRailingsMap;

            auto found = os.readWriteChunk(
                ParkFileChunkType::TILES,
                [pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap, &os, &gameState](OrcaStream::ChunkStream& cs) {
                    cs.readWrite(gameState.mapSize.x);
                    cs.readWrite(gameState.mapSize.y);

                    if (cs.getMode() == OrcaStream::Mode::reading)
                    {
                        gameStateInitAll(gameState, gameState.mapSize);

                        auto numElements = cs.read<uint32_t>();

                        std::vector<TileElement> tileElements;
                        tileElements.resize(numElements);
                        cs.read(tileElements.data(), tileElements.size() * sizeof(TileElement));
                        SetTileElements(gameState, std::move(tileElements));
                        {
                            TileElementIterator it;
                            TileElementIteratorBegin(&it);
                            while (TileElementIteratorNext(&it))
                            {
                                if (it.element->GetType() == TileElementType::Path)
                                {
                                    auto* pathElement = it.element->AsPath();
                                    if (pathElement->HasLegacyPathEntry())
                                    {
                                        auto pathEntryIndex = pathElement->GetLegacyPathEntryIndex();
                                        if (pathToRailingsMap[pathEntryIndex] != kObjectEntryIndexNull)
                                        {
                                            if (pathElement->IsQueue())
                                                pathElement->SetSurfaceEntryIndex(pathToQueueSurfaceMap[pathEntryIndex]);
                                            else
                                                pathElement->SetSurfaceEntryIndex(pathToSurfaceMap[pathEntryIndex]);

                                            pathElement->SetRailingsEntryIndex(pathToRailingsMap[pathEntryIndex]);
                                        }
                                    }
                                }
                                else if (it.element->GetType() == TileElementType::Track)
                                {
                                    auto* trackElement = it.element->AsTrack();
                                    auto trackType = trackElement->GetTrackType();
                                    if (TrackTypeMustBeMadeInvisible(
                                            trackElement->GetRideType(), trackType, os.getHeader().targetVersion))
                                    {
                                        it.element->SetInvisible(true);
                                    }
                                    if (os.getHeader().targetVersion < kBlockBrakeImprovementsVersion)
                                    {
                                        if (trackType == TrackElemType::brakes)
                                            trackElement->SetBrakeClosed(true);
                                        if (trackType == TrackElemType::blockBrakes)
                                            trackElement->SetBrakeBoosterSpeed(kRCT2DefaultBlockBrakeSpeed);
                                    }
                                }
                                else if (
                                    it.element->GetType() == TileElementType::SmallScenery && os.getHeader().targetVersion < 23)
                                {
                                    auto* sceneryElement = it.element->AsSmallScenery();
                                    // Previous formats stored the needs supports flag in the primary colour
                                    // We have moved it into a flags field to support extended colour sets
                                    bool needsSupports = EnumValue(sceneryElement->GetPrimaryColour())
                                        & kRCT12SmallSceneryElementNeedsSupportsFlag;
                                    if (needsSupports)
                                    {
                                        const auto valueWithoutFlag = EnumValue(sceneryElement->GetPrimaryColour())
                                            & ~kRCT12SmallSceneryElementNeedsSupportsFlag;
                                        sceneryElement->SetPrimaryColour(static_cast<Drawing::Colour>(valueWithoutFlag));
                                        sceneryElement->SetNeedsSupports();
                                    }
                                }
                            }
                        }
                        ParkEntranceUpdateLocations();
                    }
                    else
                    {
                        auto tileElements = GetReorganisedTileElementsWithoutGhosts();
                        cs.write(static_cast<uint32_t>(tileElements.size()));
                        cs.write(tileElements.data(), tileElements.size() * sizeof(TileElement));
                    }
                });
            if (!found)
            {
                throw std::runtime_error("No tiles chunk found.");
            }
        }

        void UpdateTrackElementsRideType()
        {
            auto& gameState = getGameState();
            for (int32_t y = 0; y < gameState.mapSize.y; y++)
            {
                for (int32_t x = 0; x < gameState.mapSize.x; x++)
                {
                    TileElement* tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });
                    if (tileElement == nullptr)
                        continue;
                    do
                    {
                        if (tileElement->GetType() != TileElementType::Track)
                            continue;

                        auto* trackElement = tileElement->AsTrack();
                        const auto* ride = GetRide(trackElement->GetRideIndex());
                        if (ride != nullptr)
                        {
                            trackElement->SetRideType(ride->type);
                        }

                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }

        void ReadWriteBannersChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.readWriteChunk(ParkFileChunkType::BANNERS, [&os](OrcaStream::ChunkStream& cs) {
                auto version = os.getHeader().targetVersion;
                if (cs.getMode() == OrcaStream::Mode::writing)
                {
                    auto numBanners = GetNumBanners();
                    cs.write(static_cast<uint32_t>(numBanners));

                    [[maybe_unused]] size_t numWritten = 0;
                    for (BannerIndex::UnderlyingType i = 0; i < kMaxBanners; i++)
                    {
                        auto banner = GetBanner(BannerIndex::FromUnderlying(i));
                        if (banner != nullptr)
                        {
                            ReadWriteBanner(version, cs, *banner);
                            numWritten++;
                        }
                    }

                    assert(numWritten == numBanners);
                }
                else if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    if (version == 0)
                    {
                        std::vector<Banner> banners;
                        cs.readWriteVector(banners, [version, &cs](Banner& banner) { ReadWriteBanner(version, cs, banner); });
                        for (BannerIndex::UnderlyingType i = 0; i < banners.size(); i++)
                        {
                            auto bannerIndex = BannerIndex::FromUnderlying(i);
                            auto banner = GetOrCreateBanner(bannerIndex);
                            if (banner != nullptr)
                            {
                                *banner = std::move(banners[i]);
                                banner->id = bannerIndex;
                            }
                        }
                    }
                    else
                    {
                        auto numBanners = cs.read<uint32_t>();
                        for (size_t i = 0; i < numBanners; i++)
                        {
                            Banner readBanner;
                            ReadWriteBanner(version, cs, readBanner);

                            auto banner = GetOrCreateBanner(readBanner.id);
                            if (banner == nullptr)
                            {
                                throw std::runtime_error("Invalid banner index");
                            }
                            else
                            {
                                *banner = std::move(readBanner);
                            }
                        }
                    }
                }
            });
        }

        static void ReadWriteBanner(uint32_t version, OrcaStream::ChunkStream& cs, Banner& banner)
        {
            if (version >= 1)
            {
                cs.readWrite(banner.id);
            }
            cs.readWrite(banner.type);
            cs.readWrite(banner.flags.holder);
            cs.readWrite(banner.text);
            cs.readWrite(banner.colour);
            cs.readWrite(banner.rideIndex);
            cs.readWrite(banner.textColour);
            cs.readWrite(banner.position.x);
            cs.readWrite(banner.position.y);
        }

        void ReadWriteRidesChunk(GameState_t& gameState, OrcaStream& os)
        {
            const auto version = os.getHeader().targetVersion;
            os.readWriteChunk(ParkFileChunkType::RIDES, [this, &version, &os, &gameState](OrcaStream::ChunkStream& cs) {
                std::vector<RideId> rideIds;
                if (cs.getMode() == OrcaStream::Mode::reading)
                {
                    RideInitAll();
                }
                else
                {
                    if (OmitTracklessRides)
                    {
                        auto tracklessRides = GetTracklessRides();
                        for (const auto& ride : RideManager(gameState))
                        {
                            auto it = std::find(tracklessRides.begin(), tracklessRides.end(), ride.id);
                            if (it == tracklessRides.end())
                            {
                                rideIds.push_back(ride.id);
                            }
                        }
                    }
                    else
                    {
                        for (const auto& ride : RideManager(gameState))
                        {
                            rideIds.push_back(ride.id);
                        }
                    }
                }
                cs.readWriteVector(rideIds, [&cs, &version, &os](RideId& rideId) {
                    // Ride ID
                    cs.readWrite(rideId);

                    auto& ride = [&]() -> Ride& {
                        if (cs.getMode() == OrcaStream::Mode::writing)
                            return *GetRide(rideId);
                        else
                            return *RideAllocateAtIndex(rideId);
                    }();

                    // Status
                    cs.readWrite(ride.type);
                    cs.readWrite(ride.subtype);
                    cs.readWrite(ride.mode);
                    cs.readWrite(ride.status);
                    cs.readWrite(ride.departFlags);
                    cs.readWrite(ride.lifecycleFlags);

                    // Meta
                    cs.readWrite(ride.customName);
                    cs.readWrite(ride.defaultNameNumber);

                    if (version <= 18)
                    {
                        money16 prices[2] = {};
                        cs.readWriteArray(prices, [&cs](money16& price) {
                            cs.readWrite(price);
                            return true;
                        });
                        ride.price[0] = prices[0];
                        ride.price[1] = prices[1];
                    }
                    else
                    {
                        cs.readWriteArray(ride.price, [&cs](money64& price) {
                            cs.readWrite(price);
                            return true;
                        });
                    }

                    // Colours
                    cs.readWrite(ride.entranceStyle);
                    cs.readWrite(ride.vehicleColourSettings);
                    cs.readWriteArray(ride.trackColours, [&cs](TrackColour& tc) {
                        cs.readWrite(tc.main);
                        cs.readWrite(tc.additional);
                        cs.readWrite(tc.supports);
                        return true;
                    });

                    cs.readWriteArray(ride.vehicleColours, [&cs](VehicleColour& vc) {
                        cs.readWrite(vc.Body);
                        cs.readWrite(vc.Trim);
                        cs.readWrite(vc.Tertiary);
                        return true;
                    });

                    // Stations
                    cs.readWrite(ride.numStations);
                    cs.readWriteArray(ride.getStations(), [&cs](RideStation& station) {
                        cs.readWrite(station.Start);
                        cs.readWrite(station.Height);
                        cs.readWrite(station.Length);
                        cs.readWrite(station.Depart);
                        cs.readWrite(station.TrainAtStation);
                        cs.readWrite(station.Entrance);
                        cs.readWrite(station.Exit);
                        cs.readWrite(station.SegmentLength);
                        cs.readWrite(station.SegmentTime);
                        cs.readWrite(station.QueueTime);
                        cs.readWrite(station.QueueLength);
                        cs.readWrite(station.LastPeepInQueue);
                        return true;
                    });

                    cs.readWrite(ride.overallView.x);
                    cs.readWrite(ride.overallView.y);

                    // Vehicles
                    cs.readWrite(ride.numTrains);
                    cs.readWrite(ride.numCarsPerTrain);
                    cs.readWrite(ride.proposedNumTrains);
                    cs.readWrite(ride.proposedNumCarsPerTrain);
                    cs.readWrite(ride.maxTrains);
                    if (version < 0x5)
                    {
                        uint8_t value;
                        cs.readWrite(value);
                        ride.minCarsPerTrain = GetMinCarsPerTrain(value);
                        ride.maxCarsPerTrain = GetMaxCarsPerTrain(value);
                    }
                    else
                    {
                        cs.readWrite(ride.minCarsPerTrain);
                        cs.readWrite(ride.maxCarsPerTrain);
                    }

                    cs.readWrite(ride.minWaitingTime);
                    cs.readWrite(ride.maxWaitingTime);
                    cs.readWriteArray(ride.vehicles, [&cs](EntityId& v) {
                        cs.readWrite(v);
                        return true;
                    });

                    // Operation
                    cs.readWrite(ride.operationOption);
                    cs.readWrite(ride.liftHillSpeed);
                    cs.readWrite(ride.numCircuits);

                    // Special
                    cs.readWrite(ride.boatHireReturnDirection);
                    cs.readWrite(ride.boatHireReturnPosition);
                    cs.readWrite(ride.chairliftBullwheelLocation[0]);
                    cs.readWrite(ride.chairliftBullwheelLocation[1]);
                    cs.readWrite(ride.chairliftBullwheelRotation);
                    cs.readWrite(ride.slideInUse);
                    cs.readWrite(ride.slidePeep);
                    cs.readWrite(ride.slidePeepTShirtColour);
                    cs.readWrite(ride.spiralSlideProgress);
                    cs.readWrite(ride.raceWinner);
                    cs.readWrite(ride.cableLift);
                    cs.readWrite(ride.cableLiftLoc);

                    // Stats
                    if (cs.getMode() == OrcaStream::Mode::reading)
                    {
                        auto hasMeasurement = cs.read<uint8_t>();
                        if (hasMeasurement != 0)
                        {
                            ride.measurement = std::make_unique<RideMeasurement>();
                            ReadWriteRideMeasurement(cs, *ride.measurement);
                        }
                    }
                    else
                    {
                        if (ride.measurement == nullptr)
                        {
                            cs.write<uint8_t>(0);
                        }
                        else
                        {
                            cs.write<uint8_t>(1);
                            ReadWriteRideMeasurement(cs, *ride.measurement);
                        }
                    }

                    if (os.getHeader().targetVersion < kHigherInversionsHolesHelicesStatsVersion)
                    {
                        uint8_t combinedValue = cs.read<uint8_t>();
                        auto split = splitCombinedHelicesAndSpecialElements(combinedValue);
                        ride.numHelices = split.first;
                        ride.specialTrackElements = split.second;
                    }
                    else
                    {
                        cs.readWrite(ride.numHelices);
                        cs.readWrite(ride.specialTrackElements.holder);
                    }

                    cs.readWrite(ride.maxSpeed);
                    cs.readWrite(ride.averageSpeed);
                    cs.readWrite(ride.currentTestSegment);
                    cs.readWrite(ride.averageSpeedTestTimeout);

                    cs.readWrite(ride.maxPositiveVerticalG);
                    cs.readWrite(ride.maxNegativeVerticalG);
                    cs.readWrite(ride.maxLateralG);
                    cs.readWrite(ride.previousVerticalG);
                    cs.readWrite(ride.previousLateralG);

                    cs.readWrite(ride.testingFlags.holder);
                    cs.readWrite(ride.curTestTrackLocation);

                    cs.readWrite(ride.turnCountDefault);
                    cs.readWrite(ride.turnCountBanked);
                    cs.readWrite(ride.turnCountSloped);

                    if (version < kHigherInversionsHolesHelicesStatsVersion)
                    {
                        uint16_t numInversions = cs.read<uint16_t>();
                        ride.numInversions = numInversions;
                        auto combinedValue = cs.read<uint8_t>();
                        auto split = splitCombinedNumDropsPoweredLifts(combinedValue);
                        ride.numDrops = split.first;
                        ride.numPoweredLifts = split.second;
                    }
                    else
                    {
                        cs.readWrite(ride.numInversions);
                        cs.readWrite(ride.numDrops);
                        cs.readWrite(ride.numPoweredLifts);
                    }
                    cs.readWrite(ride.startDropHeight);
                    cs.readWrite(ride.highestDropHeight);
                    cs.readWrite(ride.shelteredLength);
                    cs.readWrite(ride.var11C);
                    cs.readWrite(ride.numShelteredSections);
                    if (version >= kInversionsHolesShelteredEightsSplit)
                    {
                        cs.readWrite(ride.shelteredEighths);
                        if (version < kHigherInversionsHolesHelicesStatsVersion)
                        {
                            auto numHoles = cs.read<uint16_t>();
                            ride.numHoles = numHoles;
                        }
                        else
                        {
                            cs.readWrite(ride.numHoles);
                        }
                    }
                    cs.readWrite(ride.currentTestStation);
                    cs.readWrite(ride.numBlockBrakes);
                    cs.readWrite(ride.totalAirTime);

                    cs.readWrite(ride.ratings.excitement);
                    cs.readWrite(ride.ratings.intensity);
                    cs.readWrite(ride.ratings.nausea);

                    if (version <= 18)
                    {
                        uint16_t tempRideValue{};
                        cs.readWrite(tempRideValue);
                        if (tempRideValue == 0xFFFFu)
                        {
                            ride.value = kRideValueUndefined;
                        }
                        else
                        {
                            ride.value = ToMoney64(tempRideValue);
                        }
                    }
                    else if (version == 19)
                    {
                        money64 tempRideValue{};
                        cs.readWrite(tempRideValue);
                        if (tempRideValue == 0xFFFFu)
                        {
                            ride.value = kRideValueUndefined;
                        }
                        else
                        {
                            ride.value = tempRideValue;
                        }
                    }
                    else
                    {
                        cs.readWrite(ride.value);
                    }

                    cs.readWrite(ride.numRiders);
                    cs.readWrite(ride.buildDate);

                    if (version <= 18)
                    {
                        money16 tempUpkeepCost{};
                        cs.readWrite(tempUpkeepCost);
                        ride.upkeepCost = ToMoney64(tempUpkeepCost);
                    }
                    else
                    {
                        cs.readWrite(ride.upkeepCost);
                    }

                    cs.readWrite(ride.curNumCustomers);
                    cs.readWrite(ride.numCustomersTimeout);

                    cs.readWriteArray(ride.numCustomers, [&cs](uint16_t& v) {
                        cs.readWrite(v);
                        return true;
                    });

                    cs.readWrite(ride.totalCustomers);
                    cs.readWrite(ride.totalProfit);
                    cs.readWrite(ride.popularity);
                    cs.readWrite(ride.popularityTimeout);
                    cs.readWrite(ride.popularityNext);
                    cs.readWrite(ride.guestsFavourite);
                    cs.readWrite(ride.numPrimaryItemsSold);
                    cs.readWrite(ride.numSecondaryItemsSold);
                    cs.readWrite(ride.incomePerHour);
                    cs.readWrite(ride.profit);
                    cs.readWrite(ride.satisfaction);
                    cs.readWrite(ride.satisfactionTimeout);
                    cs.readWrite(ride.satisfactionNext);

                    // Breakdown
                    cs.readWrite(ride.breakdownReasonPending);
                    cs.readWrite(ride.mechanicStatus);
                    cs.readWrite(ride.mechanic);
                    cs.readWrite(ride.inspectionStation);
                    cs.readWrite(ride.brokenTrain);
                    cs.readWrite(ride.brokenCar);
                    cs.readWrite(ride.breakdownReason);
                    cs.readWrite(ride.reliabilitySubvalue);
                    cs.readWrite(ride.reliabilityPercentage);
                    cs.readWrite(ride.unreliabilityFactor);
                    cs.readWrite(ride.downtime);
                    cs.readWrite(ride.inspectionInterval);
                    cs.readWrite(ride.lastInspection);

                    cs.readWriteArray(ride.downtimeHistory, [&cs](uint8_t& v) {
                        cs.readWrite(v);
                        return true;
                    });

                    cs.readWrite(ride.breakdownSoundModifier);
                    cs.readWrite(ride.notFixedTimeout);
                    cs.readWrite(ride.lastCrashType);
                    cs.readWrite(ride.connectedMessageThrottle);

                    cs.readWrite(ride.vehicleChangeTimeout);

                    cs.readWrite(ride.currentIssues);
                    cs.readWrite(ride.lastIssueTime);

                    // Music
                    cs.readWrite(ride.music);
                    cs.readWrite(ride.musicTuneId);
                    cs.readWrite(ride.musicPosition);
                    return true;
                });
            });
        }

        static void ReadWriteRideMeasurement(OrcaStream::ChunkStream& cs, RideMeasurement& measurement)
        {
            cs.readWrite(measurement.flags.holder);
            cs.readWrite(measurement.last_use_tick);
            cs.readWrite(measurement.num_items);
            cs.readWrite(measurement.current_item);
            cs.readWrite(measurement.vehicle_index);
            cs.readWrite(measurement.current_station);
            for (size_t i = 0; i < measurement.num_items; i++)
            {
                cs.readWrite(measurement.vertical[i]);
                cs.readWrite(measurement.lateral[i]);
                cs.readWrite(measurement.velocity[i]);
                cs.readWrite(measurement.altitude[i]);
            }
        }

        template<typename T>
        static void ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, T& entity);

        static void ReadWriteEntityCommon(OrcaStream::ChunkStream& cs, EntityBase& entity)
        {
            cs.readWrite(entity.Id);
            cs.readWrite(entity.SpriteData.HeightMin);
            cs.readWrite(entity.x);
            cs.readWrite(entity.y);
            cs.readWrite(entity.z);
            cs.readWrite(entity.SpriteData.Width);
            cs.readWrite(entity.SpriteData.HeightMax);
            cs.readWrite(entity.Orientation);
        }

        static std::vector<ObjectEntryIndex> LegacyGetRideTypesBeenOn(const std::array<uint8_t, 16>& srcArray)
        {
            std::vector<ObjectEntryIndex> ridesTypesBeenOn;
            for (ObjectEntryIndex i = 0; i < RCT2::Limits::kMaxRideObjects; i++)
            {
                if (srcArray[i / 8] & (1 << (i % 8)))
                {
                    ridesTypesBeenOn.push_back(i);
                }
            }
            return ridesTypesBeenOn;
        }
        static std::vector<RideId> LegacyGetRidesBeenOn(const std::array<uint8_t, 32>& srcArray)
        {
            std::vector<RideId> ridesBeenOn;
            for (uint16_t i = 0; i < RCT2::Limits::kMaxRidesInPark; i++)
            {
                if (srcArray[i / 8] & (1 << (i % 8)))
                {
                    ridesBeenOn.push_back(RideId::FromUnderlying(i));
                }
            }
            return ridesBeenOn;
        }

        static void ReadWritePeep(OrcaStream& os, OrcaStream::ChunkStream& cs, Peep& entity)
        {
            auto version = os.getHeader().targetVersion;

            ReadWriteEntityCommon(cs, entity);

            auto guest = entity.As<Guest>();
            auto staff = entity.As<Staff>();

            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                auto name = cs.read<std::string>();
                entity.SetName(name);
            }
            else
            {
                cs.write(static_cast<const char*>(entity.Name));
            }

            cs.readWrite(entity.NextLoc);
            cs.readWrite(entity.NextFlags);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->OutsideOfPark);
                }
                else
                {
                    cs.ignore<bool>();
                }
            }

            cs.readWrite(entity.State);
            cs.readWrite(entity.SubState);

            if (version >= kPeepAnimationObjectsVersion)
                cs.readWrite(entity.AnimationObjectIndex);
            else
                entity.AnimationObjectIndex = kObjectEntryIndexNull;

            cs.readWrite(entity.AnimationGroup);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->GuestNumRides);
                }
                else
                {
                    cs.readWrite(staff->AssignedStaffType);
                }
            }

            cs.readWrite(entity.TshirtColour);
            cs.readWrite(entity.TrousersColour);
            cs.readWrite(entity.DestinationX);
            cs.readWrite(entity.DestinationY);
            cs.readWrite(entity.DestinationTolerance);
            cs.readWrite(entity.Var37);
            cs.readWrite(entity.Energy);
            cs.readWrite(entity.EnergyTarget);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->Happiness);
                    cs.readWrite(guest->HappinessTarget);
                    cs.readWrite(guest->Nausea);
                    cs.readWrite(guest->NauseaTarget);
                    cs.readWrite(guest->Hunger);
                    cs.readWrite(guest->Thirst);
                    cs.readWrite(guest->Toilet);
                }
                else
                {
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                }
            }

            cs.readWrite(entity.Mass);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->TimeToConsume);
                }
                else
                {
                    uint8_t temp{};
                    cs.readWrite(temp);
                }
            }

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    if (cs.getMode() == OrcaStream::Mode::reading)
                    {
                        guest->Intensity = IntensityRange(cs.read<uint8_t>());
                    }
                    else
                    {
                        cs.write(static_cast<uint8_t>(guest->Intensity));
                    }
                    cs.readWrite(guest->NauseaTolerance);
                }
                else
                {
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                }
            }

            cs.readWrite(entity.WindowInvalidateFlags);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    money16 tempPaidOnDrink{};
                    cs.readWrite(tempPaidOnDrink);
                    guest->PaidOnDrink = ToMoney64(tempPaidOnDrink);
                    std::array<uint8_t, 16> rideTypeBeenOn;
                    cs.readWriteArray(rideTypeBeenOn, [&cs](uint8_t& rideType) {
                        cs.readWrite(rideType);
                        return true;
                    });
                    RideUse::GetTypeHistory().Set(guest->Id, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
                    cs.readWrite(guest->ItemFlags);
                    cs.readWrite(guest->Photo2RideRef);
                    cs.readWrite(guest->Photo3RideRef);
                    cs.readWrite(guest->Photo4RideRef);
                }
                else
                {
                    cs.ignore<money16>();

                    std::vector<uint8_t> temp;
                    cs.readWriteVector(temp, [&cs](uint8_t& rideType) {
                        cs.readWrite(rideType);
                        return true;
                    });
                    cs.ignore<uint64_t>();
                    cs.ignore<RideId>();
                    cs.ignore<RideId>();
                    cs.ignore<RideId>();
                }
            }

            cs.readWrite(entity.CurrentRide);
            cs.readWrite(entity.CurrentRideStation);
            cs.readWrite(entity.CurrentTrain);
            cs.readWrite(entity.TimeToSitdown);
            cs.readWrite(entity.SpecialSprite);
            cs.readWrite(entity.AnimationType);
            cs.readWrite(entity.NextAnimationType);
            cs.readWrite(entity.AnimationImageIdOffset);
            cs.readWrite(entity.Action);
            cs.readWrite(entity.AnimationFrameNum);
            cs.readWrite(entity.StepProgress);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->GuestNextInQueue);
                }
                else
                {
                    cs.readWrite(staff->MechanicTimeSinceCall);
                }
            }

            cs.readWrite(entity.PeepDirection);
            cs.readWrite(entity.InteractionRideIndex);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->TimeInQueue);
                    std::array<uint8_t, 32> ridesBeenOn;
                    cs.readWriteArray(ridesBeenOn, [&cs](uint8_t& rideType) {
                        cs.readWrite(rideType);
                        return true;
                    });
                    RideUse::GetHistory().Set(guest->Id, LegacyGetRidesBeenOn(ridesBeenOn));
                }
                else
                {
                    cs.ignore<uint16_t>();

                    std::vector<uint8_t> ridesBeenOn;
                    cs.readWriteVector(ridesBeenOn, [&cs](uint8_t& rideId) {
                        cs.readWrite(rideId);
                        return true;
                    });
                }
            }

            cs.readWrite(entity.PeepId);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    money32 tempCashInPocket{};
                    money32 tempCashSpent{};
                    cs.readWrite(tempCashInPocket);
                    cs.readWrite(tempCashSpent);
                    guest->CashInPocket = ToMoney64(tempCashInPocket);
                    guest->CashSpent = ToMoney64(tempCashSpent);

                    cs.readWrite(guest->ParkEntryTime);
                    cs.readWrite(guest->RejoinQueueTimeout);
                    cs.readWrite(guest->PreviousRide);
                    cs.readWrite(guest->PreviousRideTimeOut);
                    cs.readWriteArray(guest->Thoughts, [&cs](PeepThought& thought) {
                        cs.readWrite(thought.type);

                        uint8_t item{};
                        cs.readWrite(item);
                        if (item == 255)
                        {
                            thought.item = kPeepThoughtItemNone;
                        }
                        else
                        {
                            thought.item = item;
                        }

                        cs.readWrite(thought.freshness);
                        cs.readWrite(thought.fresh_timeout);
                        return true;
                    });
                }
                else
                {
                    cs.ignore<money32>();
                    cs.ignore<money32>();
                    cs.readWrite(staff->HireDate);
                    cs.ignore<int8_t>();
                    cs.ignore<RideId>();
                    cs.ignore<uint16_t>();

                    std::vector<PeepThought> temp;
                    cs.readWriteVector(temp, [&cs](PeepThought& thought) {
                        cs.readWrite(thought.type);
                        cs.readWrite(thought.item);
                        cs.readWrite(thought.freshness);
                        cs.readWrite(thought.fresh_timeout);
                        return true;
                    });
                }
            }

            cs.readWrite(entity.PathCheckOptimisation);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->GuestHeadingToRideId);
                    cs.readWrite(guest->GuestIsLostCountdown);
                    cs.readWrite(guest->Photo1RideRef);
                }
                else
                {
                    cs.ignore<RideId>();
                    cs.readWrite(staff->StaffOrders);
                    cs.ignore<RideId>();
                }
            }

            cs.readWrite(entity.PeepFlags);
            cs.readWrite(entity.PathfindGoal.x);
            cs.readWrite(entity.PathfindGoal.y);
            cs.readWrite(entity.PathfindGoal.z);
            cs.readWrite(entity.PathfindGoal.direction);
            for (size_t i = 0; i < std::size(entity.PathfindHistory); i++)
            {
                cs.readWrite(entity.PathfindHistory[i].x);
                cs.readWrite(entity.PathfindHistory[i].y);
                cs.readWrite(entity.PathfindHistory[i].z);
                cs.readWrite(entity.PathfindHistory[i].direction);
            }
            cs.readWrite(entity.WalkingAnimationFrameNum);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.readWrite(guest->LitterCount);
                    cs.readWrite(guest->GuestTimeOnRide);
                    cs.readWrite(guest->DisgustingCount);

                    money16 expenditures[4]{};
                    cs.readWrite(expenditures[0]);
                    cs.readWrite(expenditures[1]);
                    cs.readWrite(expenditures[2]);
                    cs.readWrite(expenditures[3]);
                    guest->PaidToEnter = ToMoney64(expenditures[0]);
                    guest->PaidOnRides = ToMoney64(expenditures[1]);
                    guest->PaidOnFood = ToMoney64(expenditures[2]);
                    guest->PaidOnSouvenirs = ToMoney64(expenditures[3]);

                    cs.readWrite(guest->AmountOfFood);
                    cs.readWrite(guest->AmountOfDrinks);
                    cs.readWrite(guest->AmountOfSouvenirs);
                    cs.readWrite(guest->VandalismSeen);
                    cs.readWrite(guest->VoucherType);
                    cs.readWrite(guest->VoucherRideId);
                    cs.readWrite(guest->SurroundingsThoughtTimeout);
                    cs.readWrite(guest->Angriness);
                    cs.readWrite(guest->TimeLost);
                    cs.readWrite(guest->DaysInQueue);
                    cs.readWrite(guest->BalloonColour);
                    cs.readWrite(guest->UmbrellaColour);
                    cs.readWrite(guest->HatColour);
                    cs.readWrite(guest->FavouriteRide);
                    cs.readWrite(guest->FavouriteRideRating);
                }
                else
                {
                    cs.ignore<uint8_t>();
                    cs.readWrite(staff->StaffMowingTimeout);
                    cs.ignore<uint8_t>();
                    cs.readWrite(staff->StaffLawnsMown);
                    cs.readWrite(staff->StaffGardensWatered);
                    cs.readWrite(staff->StaffLitterSwept);
                    cs.readWrite(staff->StaffBinsEmptied);
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<RideId>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<uint8_t>();
                    cs.ignore<RideId>();
                    cs.ignore<uint8_t>();
                }
            }
        }

        template<typename T>
        void WriteEntitiesOfType(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs);
        template<typename... T>
        void WriteEntitiesOfTypes(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename T>
        void ReadEntitiesOfType(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename... T>
        void ReadEntitiesOfTypes(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs);

        void ReadWriteEntitiesChunk(GameState_t& gameState, OrcaStream& os);

        static void ReadWriteStringTable(OrcaStream::ChunkStream& cs, std::string& value, const std::string_view lcode)
        {
            std::vector<std::tuple<std::string, std::string>> table;
            if (cs.getMode() != OrcaStream::Mode::reading)
            {
                table.push_back(std::make_tuple(std::string(lcode), value));
            }
            cs.readWriteVector(table, [&cs](std::tuple<std::string, std::string>& v) {
                cs.readWrite(std::get<0>(v));
                cs.readWrite(std::get<1>(v));
            });
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                auto fr = std::find_if(table.begin(), table.end(), [lcode](const std::tuple<std::string, std::string>& v) {
                    return std::get<0>(v) == lcode;
                });
                if (fr != table.end())
                {
                    value = std::get<1>(*fr);
                }
                else if (table.size() > 0)
                {
                    value = std::get<1>(table[0]);
                }
                else
                {
                    value.clear();
                }
            }
        }
    };

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Vehicle& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.readWrite(entity.SubType);
        cs.readWrite(entity.pitch);
        cs.readWrite(entity.roll);
        cs.readWrite(entity.remaining_distance);
        cs.readWrite(entity.velocity);
        cs.readWrite(entity.acceleration);
        cs.readWrite(entity.ride);
        cs.readWrite(entity.vehicle_type);
        cs.readWrite(entity.colours.Body);
        cs.readWrite(entity.colours.Trim);
        cs.readWrite(entity.track_progress);
        cs.readWrite(entity.BoatLocation);
        cs.readWrite(entity.TrackTypeAndDirection);
        cs.readWrite(entity.TrackLocation.x);
        cs.readWrite(entity.TrackLocation.y);
        cs.readWrite(entity.TrackLocation.z);
        cs.readWrite(entity.next_vehicle_on_train);
        cs.readWrite(entity.prev_vehicle_on_ride);
        cs.readWrite(entity.next_vehicle_on_ride);
        cs.readWrite(entity.var_44);
        cs.readWrite(entity.mass);
        if (cs.getMode() == OrcaStream::Mode::reading && os.getHeader().targetVersion < 18)
        {
            uint16_t updateFlags = 0;
            cs.readWrite(updateFlags);
            entity.Flags = updateFlags;
        }
        else
        {
            cs.readWrite(entity.Flags);
        }
        cs.readWrite(entity.SwingSprite);
        cs.readWrite(entity.current_station);
        cs.readWrite(entity.current_time);
        cs.readWrite(entity.crash_z);
        cs.readWrite(entity.status);
        cs.readWrite(entity.sub_state);
        for (size_t i = 0; i < std::size(entity.peep); i++)
        {
            cs.readWrite(entity.peep[i]);
            cs.readWrite(entity.peep_tshirt_colours[i]);
        }
        cs.readWrite(entity.num_seats);
        cs.readWrite(entity.num_peeps);
        cs.readWrite(entity.next_free_seat);
        cs.readWrite(entity.restraints_position);
        cs.readWrite(entity.crash_x);
        cs.readWrite(entity.sound2_flags);
        cs.readWrite(entity.spin_sprite);
        cs.readWrite(entity.sound1_id);
        cs.readWrite(entity.sound1_volume);
        cs.readWrite(entity.sound2_id);
        cs.readWrite(entity.sound2_volume);
        cs.readWrite(entity.dopplerShift);
        cs.readWrite(entity.time_waiting);
        cs.readWrite(entity.speed);
        cs.readWrite(entity.powered_acceleration);
        cs.readWrite(entity.CollisionDetectionTimer);
        cs.readWrite(entity.animation_frame);
        if (cs.getMode() == OrcaStream::Mode::reading && os.getHeader().targetVersion <= 2)
        {
            uint16_t lower = 0, upper = 0;
            cs.readWrite(lower);
            cs.readWrite(upper);
            entity.animationState = lower | (upper << 16);
        }
        else
        {
            cs.readWrite(entity.animationState);
        }
        cs.readWrite(entity.scream_sound_id);
        cs.readWrite(entity.TrackSubposition);
        cs.readWrite(entity.NumLaps);
        if (cs.getMode() == OrcaStream::Mode::reading && os.getHeader().targetVersion < kBlockBrakeImprovementsVersion)
        {
            uint8_t brakeSpeed;
            cs.readWrite(brakeSpeed);
            if (entity.GetTrackType() == TrackElemType::blockBrakes)
                brakeSpeed = kRCT2DefaultBlockBrakeSpeed;
            entity.brake_speed = brakeSpeed;
        }
        else
        {
            cs.readWrite(entity.brake_speed);
        }
        cs.readWrite(entity.lost_time_out);
        cs.readWrite(entity.vertical_drop_countdown);
        cs.readWrite(entity.var_D3);
        cs.readWrite(entity.mini_golf_current_animation);
        cs.readWrite(entity.mini_golf_flags);
        cs.readWrite(entity.ride_subtype);
        cs.readWrite(entity.colours.Tertiary);
        cs.readWrite(entity.seat_rotation);
        cs.readWrite(entity.target_seat_rotation);
        if (cs.getMode() == OrcaStream::Mode::reading && os.getHeader().targetVersion < 18)
        {
            bool isCrashedVehicle = false;
            cs.readWrite(isCrashedVehicle);
            if (isCrashedVehicle)
            {
                entity.SetFlag(VehicleFlags::Crashed);
            }
        }
        if (cs.getMode() == OrcaStream::Mode::reading && os.getHeader().targetVersion < kBlockBrakeImprovementsVersion)
        {
            entity.BlockBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;
        }
        else
        {
            cs.readWrite(entity.BlockBrakeSpeed);
        }
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Guest& guest)
    {
        ReadWritePeep(os, cs, guest);
        auto version = os.getHeader().targetVersion;

        if (version <= 1)
        {
            return;
        }

        cs.readWrite(guest.GuestNumRides);
        cs.readWrite(guest.GuestNextInQueue);
        cs.readWrite(guest.ParkEntryTime);
        cs.readWrite(guest.GuestHeadingToRideId);
        cs.readWrite(guest.GuestIsLostCountdown);
        cs.readWrite(guest.GuestTimeOnRide);

        if (version <= 18)
        {
            money16 expenditures[5]{};
            cs.readWrite(expenditures[0]);
            cs.readWrite(expenditures[1]);
            cs.readWrite(expenditures[2]);
            cs.readWrite(expenditures[3]);
            cs.readWrite(expenditures[4]);
            guest.PaidToEnter = ToMoney64(expenditures[0]);
            guest.PaidOnRides = ToMoney64(expenditures[1]);
            guest.PaidOnFood = ToMoney64(expenditures[2]);
            guest.PaidOnDrink = ToMoney64(expenditures[3]);
            guest.PaidOnSouvenirs = ToMoney64(expenditures[4]);
        }
        else
        {
            cs.readWrite(guest.PaidToEnter);
            cs.readWrite(guest.PaidOnRides);
            cs.readWrite(guest.PaidOnFood);
            cs.readWrite(guest.PaidOnDrink);
            cs.readWrite(guest.PaidOnSouvenirs);
        }

        cs.readWrite(guest.OutsideOfPark);
        cs.readWrite(guest.Happiness);
        cs.readWrite(guest.HappinessTarget);
        cs.readWrite(guest.Nausea);
        cs.readWrite(guest.NauseaTarget);
        cs.readWrite(guest.Hunger);
        cs.readWrite(guest.Thirst);
        cs.readWrite(guest.Toilet);
        cs.readWrite(guest.TimeToConsume);
        if (cs.getMode() == OrcaStream::Mode::reading)
        {
            guest.Intensity = IntensityRange(cs.read<uint8_t>());
        }
        else
        {
            cs.write(static_cast<uint8_t>(guest.Intensity));
        }
        cs.readWrite(guest.NauseaTolerance);

        if (os.getHeader().targetVersion < 3)
        {
            std::array<uint8_t, 16> rideTypeBeenOn;
            cs.readWriteArray(rideTypeBeenOn, [&cs](uint8_t& rideType) {
                cs.readWrite(rideType);
                return true;
            });
            RideUse::GetTypeHistory().Set(guest.Id, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
        }

        cs.readWrite(guest.TimeInQueue);
        if (os.getHeader().targetVersion < 3)
        {
            std::array<uint8_t, 32> ridesBeenOn;
            cs.readWriteArray(ridesBeenOn, [&cs](uint8_t& rideType) {
                cs.readWrite(rideType);
                return true;
            });
            RideUse::GetHistory().Set(guest.Id, LegacyGetRidesBeenOn(ridesBeenOn));
        }
        else
        {
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                std::vector<RideId> rideUse;
                cs.readWriteVector(rideUse, [&cs](RideId& rideId) { cs.readWrite(rideId); });
                RideUse::GetHistory().Set(guest.Id, std::move(rideUse));
                std::vector<ObjectEntryIndex> rideTypeUse;
                cs.readWriteVector(rideTypeUse, [&cs](ObjectEntryIndex& rideType) { cs.readWrite(rideType); });
                RideUse::GetTypeHistory().Set(guest.Id, std::move(rideTypeUse));
            }
            else
            {
                auto* rideUse = RideUse::GetHistory().GetAll(guest.Id);
                if (rideUse == nullptr)
                {
                    std::vector<RideId> empty;
                    cs.readWriteVector(empty, [&cs](RideId& rideId) { cs.readWrite(rideId); });
                }
                else
                {
                    cs.readWriteVector(*rideUse, [&cs](RideId& rideId) { cs.readWrite(rideId); });
                }
                auto* rideTypeUse = RideUse::GetTypeHistory().GetAll(guest.Id);
                if (rideTypeUse == nullptr)
                {
                    std::vector<ObjectEntryIndex> empty;
                    cs.readWriteVector(empty, [&cs](ObjectEntryIndex& rideId) { cs.readWrite(rideId); });
                }
                else
                {
                    cs.readWriteVector(*rideTypeUse, [&cs](ObjectEntryIndex& rideId) { cs.readWrite(rideId); });
                }
            }
        }
        if (version <= 18)
        {
            money32 tempCashInPocket{};
            money32 tempCashSpent{};
            cs.readWrite(tempCashInPocket);
            cs.readWrite(tempCashSpent);
            guest.CashInPocket = ToMoney64(tempCashInPocket);
            guest.CashSpent = ToMoney64(tempCashSpent);
        }
        else
        {
            cs.readWrite(guest.CashInPocket);
            cs.readWrite(guest.CashSpent);
        }

        cs.readWrite(guest.Photo1RideRef);
        cs.readWrite(guest.Photo2RideRef);
        cs.readWrite(guest.Photo3RideRef);
        cs.readWrite(guest.Photo4RideRef);
        cs.readWrite(guest.RejoinQueueTimeout);
        cs.readWrite(guest.PreviousRide);
        cs.readWrite(guest.PreviousRideTimeOut);
        cs.readWriteArray(guest.Thoughts, [version = os.getHeader().targetVersion, &cs](PeepThought& thought) {
            cs.readWrite(thought.type);
            if (version <= 2)
            {
                int16_t item{};
                cs.readWrite(item);
                thought.item = item;
            }
            else
            {
                cs.readWrite(thought.item);
            }
            cs.readWrite(thought.freshness);
            cs.readWrite(thought.fresh_timeout);
            return true;
        });
        cs.readWrite(guest.LitterCount);
        cs.readWrite(guest.DisgustingCount);
        cs.readWrite(guest.AmountOfFood);
        cs.readWrite(guest.AmountOfDrinks);
        cs.readWrite(guest.AmountOfSouvenirs);
        cs.readWrite(guest.VandalismSeen);
        cs.readWrite(guest.VoucherType);
        cs.readWrite(guest.VoucherRideId);
        cs.readWrite(guest.SurroundingsThoughtTimeout);
        cs.readWrite(guest.Angriness);
        cs.readWrite(guest.TimeLost);
        cs.readWrite(guest.DaysInQueue);
        cs.readWrite(guest.BalloonColour);
        cs.readWrite(guest.UmbrellaColour);
        cs.readWrite(guest.HatColour);
        cs.readWrite(guest.FavouriteRide);
        cs.readWrite(guest.FavouriteRideRating);
        cs.readWrite(guest.ItemFlags);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Staff& entity)
    {
        ReadWritePeep(os, cs, entity);

        std::vector<TileCoordsXY> patrolArea;
        if (cs.getMode() == OrcaStream::Mode::writing && entity.PatrolInfo != nullptr)
        {
            patrolArea = entity.PatrolInfo->ToVector();
        }
        cs.readWriteVector(patrolArea, [&cs](TileCoordsXY& value) { cs.readWrite(value); });
        if (cs.getMode() == OrcaStream::Mode::reading)
        {
            if (patrolArea.empty())
            {
                entity.ClearPatrolArea();
            }
            else
            {
                if (entity.PatrolInfo == nullptr)
                    entity.PatrolInfo = new PatrolArea();
                else
                    entity.PatrolInfo->Clear();
                entity.PatrolInfo->Union(patrolArea);
            }
        }

        if (os.getHeader().targetVersion <= 1)
        {
            return;
        }

        cs.readWrite(entity.AssignedStaffType);
        cs.readWrite(entity.MechanicTimeSinceCall);
        cs.readWrite(entity.HireDate);
        if (os.getHeader().targetVersion <= 4)
        {
            cs.ignore<uint8_t>();
        }
        cs.readWrite(entity.StaffOrders);
        cs.readWrite(entity.StaffMowingTimeout);
        cs.readWrite(entity.StaffLawnsMown);
        cs.readWrite(entity.StaffGardensWatered);
        cs.readWrite(entity.StaffLitterSwept);
        cs.readWrite(entity.StaffBinsEmptied);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, SteamParticle& steamParticle)
    {
        ReadWriteEntityCommon(cs, steamParticle);
        cs.readWrite(steamParticle.time_to_move);
        cs.readWrite(steamParticle.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, MoneyEffect& moneyEffect)
    {
        ReadWriteEntityCommon(cs, moneyEffect);
        cs.readWrite(moneyEffect.MoveDelay);
        cs.readWrite(moneyEffect.NumMovements);
        cs.readWrite(moneyEffect.GuestPurchase);
        cs.readWrite(moneyEffect.Value);
        cs.readWrite(moneyEffect.OffsetX);
        cs.readWrite(moneyEffect.Wiggle);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, VehicleCrashParticle& vehicleCrashParticle)
    {
        ReadWriteEntityCommon(cs, vehicleCrashParticle);
        cs.readWrite(vehicleCrashParticle.frame);
        cs.readWrite(vehicleCrashParticle.time_to_live);
        cs.readWrite(vehicleCrashParticle.frame);
        cs.readWrite(vehicleCrashParticle.colour[0]);
        cs.readWrite(vehicleCrashParticle.colour[1]);
        cs.readWrite(vehicleCrashParticle.crashed_sprite_base);
        cs.readWrite(vehicleCrashParticle.velocity_x);
        cs.readWrite(vehicleCrashParticle.velocity_y);
        cs.readWrite(vehicleCrashParticle.velocity_z);
        cs.readWrite(vehicleCrashParticle.acceleration_x);
        cs.readWrite(vehicleCrashParticle.acceleration_y);
        cs.readWrite(vehicleCrashParticle.acceleration_z);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionCloud& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.readWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, CrashSplashParticle& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.readWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionFlare& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.readWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, JumpingFountain& fountain)
    {
        ReadWriteEntityCommon(cs, fountain);
        cs.readWrite(fountain.NumTicksAlive);
        cs.readWrite(fountain.frame);
        cs.readWrite(fountain.fountainFlags.holder);
        cs.readWrite(fountain.TargetX);
        cs.readWrite(fountain.TargetY);
        cs.readWrite(fountain.TargetY);
        cs.readWrite(fountain.Iteration);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Balloon& balloon)
    {
        ReadWriteEntityCommon(cs, balloon);
        cs.readWrite(balloon.popped);
        cs.readWrite(balloon.time_to_move);
        cs.readWrite(balloon.frame);
        cs.readWrite(balloon.colour);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Duck& duck)
    {
        ReadWriteEntityCommon(cs, duck);
        cs.readWrite(duck.frame);
        cs.readWrite(duck.target_x);
        cs.readWrite(duck.target_y);
        cs.readWrite(duck.state);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Litter& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.readWrite(entity.SubType);
        cs.readWrite(entity.creationTick);
    }

    template<typename T>
    void ParkFile::WriteEntitiesOfType(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        const auto entityList = EntityList<T>();
        // We do not rely on GetEntityListCount as there is a potential issue (#23636) that needs investigating.
        const auto count = [&]() -> uint16_t {
            uint16_t res = 0;
            for ([[maybe_unused]] auto* _ : entityList)
            {
                res++;
            }
            return res;
        }();
        cs.write(T::cEntityType);
        cs.write(count);
        for (auto* ent : entityList)
        {
            cs.write(ent->Id);
            ReadWriteEntity(os, cs, *ent);
        }
    }

    template<typename... T>
    void ParkFile::WriteEntitiesOfTypes(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (WriteEntitiesOfType<T>(gameState, os, cs), ...);
    }

    template<typename T>
    void ParkFile::ReadEntitiesOfType(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        [[maybe_unused]] auto t = cs.read<EntityType>();
        assert(t == T::cEntityType);
        auto count = cs.read<uint16_t>();
        for (auto i = 0; i < count; ++i)
        {
            T placeholder{};

            auto index = cs.read<EntityId>();
            auto* ent = getGameState().entities.CreateEntityAt<T>(index);
            if (ent == nullptr)
            {
                // Unable to allocate entity
                ent = &placeholder;
            }
            ReadWriteEntity(os, cs, *ent);
        }
    }

    template<typename... T>
    void ParkFile::ReadEntitiesOfTypes(GameState_t& gameState, OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (ReadEntitiesOfType<T>(gameState, os, cs), ...);
    }

    void ParkFile::ReadWriteEntitiesChunk(GameState_t& gameState, OrcaStream& os)
    {
        os.readWriteChunk(ParkFileChunkType::ENTITIES, [this, &gameState, &os](OrcaStream::ChunkStream& cs) {
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                getGameState().entities.ResetAllEntities();
            }

            std::vector<uint16_t> entityIndices;
            if (cs.getMode() == OrcaStream::Mode::reading)
            {
                ReadEntitiesOfTypes<
                    Vehicle, Guest, Staff, Litter, SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud,
                    CrashSplashParticle, ExplosionFlare, JumpingFountain, Balloon, Duck>(gameState, os, cs);

                auto version = os.getHeader().targetVersion;
                if (version < kPeepAnimationObjectsVersion)
                {
                    ConvertPeepAnimationTypeToObjects(gameState);
                }
            }
            else
            {
                WriteEntitiesOfTypes<
                    Vehicle, Guest, Staff, Litter, SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud,
                    CrashSplashParticle, ExplosionFlare, JumpingFountain, Balloon, Duck>(gameState, os, cs);
            }
        });
    }

    void ParkFileExporter::Export(GameState_t& gameState, std::string_view path, int16_t compressionLevel)
    {
        auto parkFile = std::make_unique<ParkFile>();
        parkFile->Save(gameState, path, compressionLevel);
    }

    void ParkFileExporter::Export(GameState_t& gameState, IStream& stream, int16_t compressionLevel)
    {
        auto parkFile = std::make_unique<ParkFile>();
        parkFile->ExportObjectsList = ExportObjectsList;
        parkFile->Save(gameState, stream, compressionLevel);
    }
} // namespace OpenRCT2

enum : uint32_t
{
    S6_SAVE_FLAG_EXPORT = 1 << 0,
    S6_SAVE_FLAG_SCENARIO = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

int32_t ScenarioSave(GameState_t& gameState, u8string_view path, int32_t flags)
{
    if (flags & S6_SAVE_FLAG_SCENARIO)
    {
        LOG_VERBOSE("saving scenario");
    }
    else
    {
        LOG_VERBOSE("saving game");
    }

    gIsAutosave = flags & S6_SAVE_FLAG_AUTOMATIC;
    if (!gIsAutosave)
    {
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseConstructionWindows();
    }

    PrepareMapForSave();

    bool result = false;
    auto parkFile = std::make_unique<ParkFile>();
    try
    {
        if (flags & S6_SAVE_FLAG_EXPORT)
        {
            auto& objManager = GetContext()->GetObjectManager();
            parkFile->ExportObjectsList = objManager.GetPackableObjects();
        }
        parkFile->OmitTracklessRides = true;
        if (flags & S6_SAVE_FLAG_SCENARIO)
        {
            // s6exporter->SaveScenario(path);
        }
        else
        {
            // s6exporter->SaveGame(path);
        }
        parkFile->Save(gameState, path, gIsAutosave ? kParkFileAutoCompressionLevel : kParkFileSaveCompressionLevel);
        result = true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());

        Formatter ft;
        ft.Add<const char*>(e.what());
        ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_STRING, ft);
        GfxInvalidateScreen();
    }

    GfxInvalidateScreen();

    if (result && !(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        gScreenAge = 0;
    }
    return result;
}

class ParkFileImporter final : public IParkImporter
{
private:
#ifdef __clang__
    [[maybe_unused]]
#endif
    const IObjectRepository& _objectRepository;
    std::unique_ptr<ParkFile> _parkFile;

public:
    ParkFileImporter(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
    }

    ParkLoadResult Load(const u8string& path, const bool skipObjectCheck) override
    {
        _parkFile = std::make_unique<ParkFile>();
        _parkFile->Load(path, skipObjectCheck);

        auto result = ParkLoadResult(std::move(_parkFile->RequiredObjects));
        result.SemiCompatibleVersion = _parkFile->IsSemiCompatibleVersion(result.MinVersion, result.TargetVersion);
        return result;
    }

    ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) override
    {
        return Load(path, skipObjectCheck);
    }

    ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) override
    {
        return Load(path, skipObjectCheck);
    }

    ParkLoadResult LoadFromStream(
        IStream* stream, bool isScenario, bool skipObjectCheck = false, const u8string& path = {}) override
    {
        _parkFile = std::make_unique<ParkFile>();
        _parkFile->Load(*stream, skipObjectCheck);

        auto result = ParkLoadResult(std::move(_parkFile->RequiredObjects));
        result.SemiCompatibleVersion = _parkFile->IsSemiCompatibleVersion(result.MinVersion, result.TargetVersion);
        return result;
    }

    void Import(GameState_t& gameState) override
    {
        _parkFile->Import(gameState);
        ResearchDetermineFirstOfType();
        GameFixSaveVars();
    }

    bool PopulateIndexEntry(ScenarioIndexEntry* dst) override
    {
        *dst = _parkFile->ReadScenarioChunk();
        return true;
    }

    ParkPreview GetParkPreview() override
    {
        return _parkFile->ReadPreviewChunk();
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateParkFile(IObjectRepository& objectRepository)
{
    return std::make_unique<ParkFileImporter>(objectRepository);
}
