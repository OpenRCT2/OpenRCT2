/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../object/Object.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/RideUseSystem.h"
#include "../rct2/RCT2.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ride/Vehicle.h"
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
    }; // namespace ParkFileChunkType

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
            const auto& header = _os->GetHeader();
            if (/*header.TargetVersion < kParkFileMinSupportedVersion || */ header.MinVersion > kParkFileCurrentVersion)
            {
                throw UnsupportedVersionException(header.MinVersion, header.TargetVersion);
            }
        }

    public:
        bool IsSemiCompatibleVersion(uint32_t& minVersion, uint32_t& targetVersion)
        {
            const auto& header = _os->GetHeader();
            minVersion = header.MinVersion;
            targetVersion = header.TargetVersion;
            return targetVersion > kParkFileCurrentVersion;
        }

        void Load(const std::string_view path, const bool skipObjectCheck)
        {
            FileStream fs(path, FileMode::open);
            Load(fs, skipObjectCheck);
        }

        void Load(IStream& stream, const bool skipObjectCheck)
        {
            _os = std::make_unique<OrcaStream>(stream, OrcaStream::Mode::READING);
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
            if (os.GetHeader().TargetVersion < 0x4)
            {
                UpdateTrackElementsRideType();
            }

            // Initial cash will eventually be removed
            gameState.initialCash = gameState.park.cash;
        }

        void Save(GameState_t& gameState, IStream& stream, int16_t compressionLevel)
        {
            OrcaStream os(stream, OrcaStream::Mode::WRITING, compressionLevel);

            auto& header = os.GetHeader();
            header.Magic = kParkFileMagic;
            header.TargetVersion = kParkFileCurrentVersion;
            header.MinVersion = kParkFileMinVersion;

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
            os.ReadWriteChunk(ParkFileChunkType::SCENARIO, [&entry](OrcaStream::ChunkStream& cs) {
                entry.Category = cs.Read<ScenarioCategory>();

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
                entry.ObjectiveType = cs.Read<uint8_t>();
                entry.ObjectiveArg1 = cs.Read<uint8_t>();
                entry.ObjectiveArg3 = cs.Read<uint16_t>();
                entry.ObjectiveArg2 = cs.Read<int32_t>();

                entry.SourceGame = ScenarioSource::Other;
            });
            return entry;
        }

        ParkPreview ReadPreviewChunk()
        {
            ParkPreview preview{};
            auto& os = *_os;
            os.ReadWriteChunk(ParkFileChunkType::PREVIEW, [&preview](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(preview.parkName);
                cs.ReadWrite(preview.parkRating);
                cs.ReadWrite(preview.year);
                cs.ReadWrite(preview.month);
                cs.ReadWrite(preview.day);
                cs.ReadWrite(preview.parkUsesMoney);
                cs.ReadWrite(preview.cash);
                cs.ReadWrite(preview.numRides);
                cs.ReadWrite(preview.numGuests);

                cs.ReadWriteVector(preview.images, [&cs](PreviewImage& image) {
                    cs.ReadWrite(image.type);
                    cs.ReadWrite(image.width);
                    cs.ReadWrite(image.height);
                    cs.ReadWriteArray(image.pixels, [&cs](uint8_t& pixel) {
                        cs.ReadWrite(pixel);
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
            if (os.GetMode() == OrcaStream::Mode::WRITING)
            {
                os.ReadWriteChunk(ParkFileChunkType::AUTHORING, [](OrcaStream::ChunkStream& cs) {
                    cs.Write(std::string_view(gVersionInfoFull));
                    std::vector<std::string> authors;
                    cs.ReadWriteVector(authors, [](std::string& s) {});
                    cs.Write(std::string_view());                  // custom notes that can be attached to the save
                    cs.Write(static_cast<uint64_t>(std::time(0))); // date started
                    cs.Write(static_cast<uint64_t>(std::time(0))); // date modified
                });
            }
        }

        void ReadWriteObjectsChunk(OrcaStream& os)
        {
            static constexpr uint8_t DESCRIPTOR_NONE = 0;
            static constexpr uint8_t DESCRIPTOR_DAT = 1;
            static constexpr uint8_t DESCRIPTOR_JSON = 2;

            if (os.GetMode() == OrcaStream::Mode::READING)
            {
                std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), kObjectEntryIndexNull);
                std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), kObjectEntryIndexNull);
                std::fill(std::begin(_pathToRailingsMap), std::end(_pathToRailingsMap), kObjectEntryIndexNull);
                auto* pathToSurfaceMap = _pathToSurfaceMap;
                auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
                auto* pathToRailingsMap = _pathToRailingsMap;
                const auto version = os.GetHeader().TargetVersion;

                ObjectList requiredObjects;
                struct LegacyFootpathMapping
                {
                    ObjectEntryIndex originalEntryIndex;
                    const RCT2::FootpathMapping& mapping;
                };
                std::vector<LegacyFootpathMapping> legacyPathMappings;
                os.ReadWriteChunk(
                    ParkFileChunkType::OBJECTS, [&requiredObjects, version, &legacyPathMappings](OrcaStream::ChunkStream& cs) {
                        auto numSubLists = cs.Read<uint16_t>();
                        for (size_t i = 0; i < numSubLists; i++)
                        {
                            auto objectType = static_cast<ObjectType>(cs.Read<uint16_t>());
                            auto subListSize = static_cast<ObjectEntryIndex>(cs.Read<uint32_t>());
                            for (ObjectEntryIndex j = 0; j < subListSize; j++)
                            {
                                auto kind = cs.Read<uint8_t>();

                                switch (kind)
                                {
                                    case DESCRIPTOR_NONE:
                                        break;
                                    case DESCRIPTOR_DAT:
                                    {
                                        RCTObjectEntry datEntry;
                                        cs.Read(&datEntry, sizeof(datEntry));
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
                                        auto identifier = cs.Read<std::string>();
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
                                        desc.Version = VersionTuple(cs.Read<std::string>());

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
                    os.ReadWriteChunk(ParkFileChunkType::CLIMATE, [&legacyClimate](OrcaStream::ChunkStream& cs) {
                        cs.ReadWrite(legacyClimate);
                    });

                    auto climateObjId = GetClimateObjectIdFromLegacyClimateType(legacyClimate);
                    AppendRequiredObjects(requiredObjects, ObjectType::climate, std::vector({ climateObjId }));
                }

                RequiredObjects = std::move(requiredObjects);
            }
            else
            {
                os.ReadWriteChunk(ParkFileChunkType::OBJECTS, [](OrcaStream::ChunkStream& cs) {
                    auto& objManager = GetContext()->GetObjectManager();
                    auto objectList = objManager.GetLoadedObjects();

                    // Write number of object sub lists
                    cs.Write(static_cast<uint16_t>(getTransientObjectTypes().size()));
                    for (auto objectType : getTransientObjectTypes())
                    {
                        // Write sub list
                        const auto& list = objectList.GetList(objectType);
                        cs.Write(static_cast<uint16_t>(objectType));
                        cs.Write(static_cast<uint32_t>(list.size()));
                        for (const auto& entry : list)
                        {
                            if (entry.HasValue())
                            {
                                if (entry.Generation == ObjectGeneration::JSON)
                                {
                                    cs.Write(DESCRIPTOR_JSON);
                                    cs.Write(entry.Identifier);
                                    cs.Write(VersionString(entry.Version));
                                }
                                else
                                {
                                    cs.Write(DESCRIPTOR_DAT);
                                    cs.Write(&entry.Entry, sizeof(RCTObjectEntry));
                                }
                            }
                            else
                            {
                                cs.Write(DESCRIPTOR_NONE);
                            }
                        }
                    }
                });
            }
        }

        void ReadWriteScenarioChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::SCENARIO, [&gameState, &os](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gameState.scenarioCategory);
                ReadWriteStringTable(cs, gameState.scenarioName, "en-GB");
                ReadWriteStringTable(cs, gameState.park.Name, "en-GB");
                ReadWriteStringTable(cs, gameState.scenarioDetails, "en-GB");

                cs.ReadWrite(gameState.scenarioObjective.Type);
                cs.ReadWrite(gameState.scenarioObjective.Year);
                cs.ReadWrite(gameState.scenarioObjective.NumGuests);
                cs.ReadWrite(gameState.scenarioObjective.Currency);

                cs.ReadWrite(gameState.scenarioParkRatingWarningDays);

                cs.ReadWrite(gameState.scenarioCompletedCompanyValue);
                if (gameState.scenarioCompletedCompanyValue == kMoney64Undefined
                    || gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                {
                    cs.Write("");
                }
                else
                {
                    cs.ReadWrite(gameState.scenarioCompletedBy);
                }

                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto earlyCompletion = cs.Read<bool>();
                    if (NetworkGetMode() == NETWORK_MODE_CLIENT)
                    {
                        gAllowEarlyCompletionInNetworkPlay = earlyCompletion;
                    }
                }
                else
                {
                    cs.Write(AllowEarlyCompletion());
                }

                if (os.GetHeader().TargetVersion >= 1)
                {
                    cs.ReadWrite(gameState.scenarioFileName);
                }
            });
        }

        void ReadWritePreviewChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::PREVIEW, [&gameState](OrcaStream::ChunkStream& cs) {
                auto preview = OpenRCT2::generatePreviewFromGameState(gameState);

                cs.ReadWrite(preview.parkName);
                cs.ReadWrite(preview.parkRating);
                cs.ReadWrite(preview.year);
                cs.ReadWrite(preview.month);
                cs.ReadWrite(preview.day);
                cs.ReadWrite(preview.parkUsesMoney);
                cs.ReadWrite(preview.cash);
                cs.ReadWrite(preview.numRides);
                cs.ReadWrite(preview.numGuests);

                cs.ReadWriteVector(preview.images, [&cs](PreviewImage& image) {
                    cs.ReadWrite(image.type);
                    cs.ReadWrite(image.width);
                    cs.ReadWrite(image.height);
                    cs.ReadWriteArray(image.pixels, [&cs](uint8_t& pixel) {
                        cs.ReadWrite(pixel);
                        return true;
                    });
                });
            });
        }

        void ReadWriteGeneralChunk(GameState_t& gameState, OrcaStream& os)
        {
            const auto version = os.GetHeader().TargetVersion;
            auto found = os.ReadWriteChunk(ParkFileChunkType::GENERAL, [&](OrcaStream::ChunkStream& cs) {
                // Only GAME_PAUSED_NORMAL from gGamePaused is relevant.
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    const uint8_t isPaused = cs.Read<uint8_t>();
                    gGamePaused &= ~GAME_PAUSED_NORMAL;
                    gGamePaused |= (isPaused & GAME_PAUSED_NORMAL);
                }
                else
                {
                    const uint8_t isPaused = (gGamePaused & GAME_PAUSED_NORMAL);
                    cs.Write(isPaused);
                }
                cs.ReadWrite(gameState.currentTicks);
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    uint16_t monthTicks;
                    uint32_t monthsElapsed;
                    cs.ReadWrite(monthTicks);
                    cs.ReadWrite(monthsElapsed);
                    gameState.date = Date{ monthsElapsed, monthTicks };
                }
                else
                {
                    auto& date = GetDate();
                    cs.Write(date.GetMonthTicks());
                    cs.Write(date.GetMonthsElapsed());
                }

                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    uint32_t s0{}, s1{};
                    cs.ReadWrite(s0);
                    cs.ReadWrite(s1);
                    Random::RCT2::Seed s{ s0, s1 };
                    gameState.scenarioRand.seed(s);
                }
                else
                {
                    auto randState = gameState.scenarioRand.state();
                    cs.Write(randState.s0);
                    cs.Write(randState.s1);
                }

                cs.ReadWrite(gameState.guestInitialHappiness);
                if (version <= 18)
                {
                    money16 tempGuestInitialCash{};
                    cs.ReadWrite(tempGuestInitialCash);
                    gameState.guestInitialCash = ToMoney64(tempGuestInitialCash);
                }
                else
                {
                    cs.ReadWrite(gameState.guestInitialCash);
                }
                cs.ReadWrite(gameState.guestInitialHunger);
                cs.ReadWrite(gameState.guestInitialThirst);

                cs.ReadWrite(gameState.nextGuestNumber);
                cs.ReadWriteVector(gameState.peepSpawns, [&cs](PeepSpawn& spawn) {
                    cs.ReadWrite(spawn.x);
                    cs.ReadWrite(spawn.y);
                    cs.ReadWrite(spawn.z);
                    cs.ReadWrite(spawn.direction);
                });

                if (version <= 18)
                {
                    money16 tempLandPrice{};
                    money16 tempConstructionRightPrice{};
                    cs.ReadWrite(tempLandPrice);
                    cs.ReadWrite(tempConstructionRightPrice);
                    gameState.landPrice = ToMoney64(tempLandPrice);
                    gameState.constructionRightsPrice = ToMoney64(tempConstructionRightPrice);
                }
                else
                {
                    cs.ReadWrite(gameState.landPrice);
                    cs.ReadWrite(gameState.constructionRightsPrice);
                }
                cs.ReadWrite(gameState.grassSceneryTileLoopPosition);
                cs.ReadWrite(gameState.widePathTileLoopPosition);

                auto& rideRatings = gameState.rideRatingUpdateStates;
                if (os.GetHeader().TargetVersion >= 21)
                {
                    cs.ReadWriteArray(rideRatings, [this, &cs](RideRatingUpdateState& calcData) {
                        ReadWriteRideRatingCalculationData(cs, calcData);
                        return true;
                    });
                }
                else
                {
                    // Only single state was stored prior to version 20.
                    if (os.GetMode() == OrcaStream::Mode::READING)
                    {
                        // Since we read only one state ensure the rest is reset.
                        RideRatingResetUpdateStates();
                    }
                    auto& rideRatingUpdateState = rideRatings[0];
                    ReadWriteRideRatingCalculationData(cs, rideRatingUpdateState);
                }

                if (os.GetHeader().TargetVersion >= 14)
                {
                    cs.ReadWrite(gIsAutosave);
                }
            });
            if (!found)
            {
                throw std::runtime_error("No general chunk found.");
            }
        }

        void ReadWriteRideRatingCalculationData(OrcaStream::ChunkStream& cs, RideRatingUpdateState& calcData)
        {
            cs.ReadWrite(calcData.AmountOfBrakes);
            cs.ReadWrite(calcData.Proximity);
            cs.ReadWrite(calcData.ProximityStart);
            cs.ReadWrite(calcData.CurrentRide);
            cs.ReadWrite(calcData.State);
            cs.ReadWrite(calcData.ProximityTrackType);
            cs.ReadWrite(calcData.ProximityBaseHeight);
            cs.ReadWrite(calcData.ProximityTotal);
            cs.ReadWriteArray(calcData.ProximityScores, [&cs](uint16_t& value) {
                cs.ReadWrite(value);
                return true;
            });
            cs.ReadWrite(calcData.AmountOfBrakes);
            cs.ReadWrite(calcData.AmountOfReversers);
            cs.ReadWrite(calcData.StationFlags);
        }

        void ReadWriteInterfaceChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::INTERFACE, [&gameState](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gameState.savedView.x);
                cs.ReadWrite(gameState.savedView.y);
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto savedZoomlevel = static_cast<ZoomLevel>(cs.Read<int8_t>());
                    gameState.savedViewZoom = std::clamp(savedZoomlevel, ZoomLevel::min(), ZoomLevel::max());
                }
                else
                {
                    cs.Write(static_cast<int8_t>(gameState.savedViewZoom));
                }
                cs.ReadWrite(gameState.savedViewRotation);
                cs.ReadWrite(gameState.lastEntranceStyle);
                cs.ReadWrite(gameState.editorStep);
            });
        }

        void ReadWriteCheatsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CHEATS, [](OrcaStream::ChunkStream& cs) {
                DataSerialiser ds(cs.GetMode() == OrcaStream::Mode::WRITING, cs.GetStream());
                CheatsSerialise(ds);
            });
        }

        void ReadWriteRestrictedObjectsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::RESTRICTED_OBJECTS, [](OrcaStream::ChunkStream& cs) {
                auto& restrictedScenery = GetRestrictedScenery();

                // We are want to support all object types in the future, so convert scenery type
                // to object type when we write the list
                cs.ReadWriteVector(restrictedScenery, [&cs](ScenerySelection& item) {
                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        item.SceneryType = GetSceneryTypeFromObjectType(static_cast<ObjectType>(cs.Read<uint16_t>()));
                        item.EntryIndex = cs.Read<ObjectEntryIndex>();
                    }
                    else
                    {
                        cs.Write(static_cast<uint16_t>(GetObjectTypeFromSceneryType(item.SceneryType)));
                        cs.Write(item.EntryIndex);
                    }
                });
            });
        }

        void ReadWritePluginStorageChunk(GameState_t& gameState, OrcaStream& os)
        {
            if (os.GetMode() == OrcaStream::Mode::WRITING)
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

            os.ReadWriteChunk(ParkFileChunkType::PLUGIN_STORAGE, [&gameState](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gameState.pluginStorage);
            });

            if (os.GetMode() == OrcaStream::Mode::READING)
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

            if (os.GetMode() == OrcaStream::Mode::WRITING && ExportObjectsList.size() == 0)
            {
                // Do not emit chunk if there are no packed objects
                return;
            }

            os.ReadWriteChunk(ParkFileChunkType::PACKED_OBJECTS, [this](OrcaStream::ChunkStream& cs) {
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto& objRepository = GetContext()->GetObjectRepository();
                    auto numObjects = cs.Read<uint32_t>();
                    for (uint32_t i = 0; i < numObjects; i++)
                    {
                        auto type = cs.Read<uint8_t>();
                        if (type == DESCRIPTOR_DAT)
                        {
                            RCTObjectEntry entry;
                            cs.Read(&entry, sizeof(entry));
                            auto size = cs.Read<uint32_t>();
                            std::vector<uint8_t> data;
                            data.resize(size);
                            cs.Read(data.data(), data.size());

                            auto legacyIdentifier = entry.GetName();
                            if (objRepository.FindObjectLegacy(legacyIdentifier) == nullptr)
                            {
                                objRepository.AddObjectFromFile(
                                    ObjectGeneration::DAT, legacyIdentifier, data.data(), data.size());
                            }
                        }
                        else if (type == DESCRIPTOR_PARKOBJ)
                        {
                            auto identifier = cs.Read<std::string>();
                            auto size = cs.Read<uint32_t>();
                            std::vector<uint8_t> data;
                            data.resize(size);
                            cs.Read(data.data(), data.size());
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
                    auto& stream = cs.GetStream();
                    auto countPosition = stream.GetPosition();

                    // Write placeholder count, update later
                    uint32_t count = 0;
                    cs.Write(count);

                    // Write objects
                    for (const auto* ori : ExportObjectsList)
                    {
                        auto extension = Path::GetExtension(ori->Path);
                        if (String::iequals(extension, ".dat"))
                        {
                            cs.Write(DESCRIPTOR_DAT);
                            cs.Write(&ori->ObjectEntry, sizeof(RCTObjectEntry));
                        }
                        else if (String::iequals(extension, ".parkobj"))
                        {
                            cs.Write(DESCRIPTOR_PARKOBJ);
                            cs.Write(ori->Identifier);
                        }
                        else
                        {
                            Console::WriteLine("%s not packed: unsupported extension.", ori->Identifier.c_str());
                            continue;
                        }

                        auto data = File::ReadAllBytes(ori->Path);
                        cs.Write<uint32_t>(static_cast<uint32_t>(data.size()));
                        cs.Write(data.data(), data.size());
                        count++;
                    }

                    auto backupPosition = stream.GetPosition();
                    stream.SetPosition(countPosition);
                    cs.Write(count);
                    stream.SetPosition(backupPosition);
                }
            });
        }

        void ReadWriteClimateChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CLIMATE, [&os, &gameState](OrcaStream::ChunkStream& cs) {
                auto version = os.GetHeader().TargetVersion;
                if (version < kClimateObjectsVersion)
                {
                    // Legacy climate is converted elsewhere, so we can skip it here.
                    cs.Ignore<RCT12::ClimateType>();
                }

                cs.ReadWrite(gameState.weatherUpdateTimer);

                for (auto* cl : { &gameState.weatherCurrent, &gameState.weatherNext })
                {
                    cs.ReadWrite(cl->weatherType);
                    cs.ReadWrite(cl->temperature);
                    cs.ReadWrite(cl->weatherEffect);
                    cs.ReadWrite(cl->weatherGloom);
                    cs.ReadWrite(cl->level);
                }
            });
        }

        void ReadWriteParkChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(
                ParkFileChunkType::PARK, [version = os.GetHeader().TargetVersion, &gameState](OrcaStream::ChunkStream& cs) {
                    cs.ReadWrite(gameState.park.Name);
                    cs.ReadWrite(gameState.park.cash);
                    cs.ReadWrite(gameState.park.bankLoan);
                    cs.ReadWrite(gameState.park.maxBankLoan);
                    cs.ReadWrite(gameState.bankLoanInterestRate);
                    cs.ReadWrite(gameState.park.Flags);
                    if (version <= 18)
                    {
                        money16 tempParkEntranceFee{};
                        cs.ReadWrite(tempParkEntranceFee);
                        gameState.park.EntranceFee = ToMoney64(tempParkEntranceFee);
                    }
                    else
                    {
                        cs.ReadWrite(gameState.park.EntranceFee);
                    }

                    cs.ReadWrite(gameState.park.staffHandymanColour);
                    cs.ReadWrite(gameState.park.staffMechanicColour);
                    cs.ReadWrite(gameState.park.staffSecurityColour);
                    cs.ReadWrite(gameState.park.samePriceThroughoutPark);

                    // Finances
                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        auto numMonths = std::min<uint32_t>(kExpenditureTableMonthCount, cs.Read<uint32_t>());
                        auto numTypes = std::min<uint32_t>(static_cast<uint32_t>(ExpenditureType::Count), cs.Read<uint32_t>());
                        for (uint32_t i = 0; i < numMonths; i++)
                        {
                            for (uint32_t j = 0; j < numTypes; j++)
                            {
                                gameState.park.expenditureTable[i][j] = cs.Read<money64>();
                            }
                        }
                    }
                    else
                    {
                        auto numMonths = static_cast<uint32_t>(kExpenditureTableMonthCount);
                        auto numTypes = static_cast<uint32_t>(ExpenditureType::Count);

                        cs.Write(numMonths);
                        cs.Write(numTypes);
                        for (uint32_t i = 0; i < numMonths; i++)
                        {
                            for (uint32_t j = 0; j < numTypes; j++)
                            {
                                cs.Write(gameState.park.expenditureTable[i][j]);
                            }
                        }
                    }
                    cs.ReadWrite(gameState.park.historicalProfit);

                    // Marketing
                    cs.ReadWriteVector(gameState.park.marketingCampaigns, [&cs](MarketingCampaign& campaign) {
                        cs.ReadWrite(campaign.Type);
                        cs.ReadWrite(campaign.WeeksLeft);
                        cs.ReadWrite(campaign.flags.holder);
                        cs.ReadWrite(campaign.RideId);
                    });

                    // Awards
                    auto& currentAwards = gameState.park.currentAwards;
                    if (version <= 6)
                    {
                        Award awards[RCT2::Limits::kMaxAwards]{};
                        cs.ReadWriteArray(awards, [&cs, &currentAwards](Award& award) {
                            if (award.Time != 0)
                            {
                                cs.ReadWrite(award.Time);
                                cs.ReadWrite(award.Type);
                                currentAwards.push_back(award);
                                return true;
                            }

                            return false;
                        });
                    }
                    else
                    {
                        cs.ReadWriteVector(currentAwards, [&cs](Award& award) {
                            cs.ReadWrite(award.Time);
                            cs.ReadWrite(award.Type);
                        });
                    }
                    cs.ReadWrite(gameState.park.Value);
                    cs.ReadWrite(gameState.park.companyValue);
                    cs.ReadWrite(gameState.park.Size);
                    cs.ReadWrite(gameState.park.numGuestsInPark);
                    cs.ReadWrite(gameState.park.numGuestsHeadingForPark);
                    cs.ReadWrite(gameState.park.Rating);
                    cs.ReadWrite(gameState.park.RatingCasualtyPenalty);
                    cs.ReadWrite(gameState.park.currentExpenditure);
                    cs.ReadWrite(gameState.park.currentProfit);
                    cs.ReadWrite(gameState.park.weeklyProfitAverageDividend);
                    cs.ReadWrite(gameState.park.weeklyProfitAverageDivisor);
                    cs.ReadWrite(gameState.park.totalAdmissions);
                    cs.ReadWrite(gameState.park.totalIncomeFromAdmissions);
                    if (version <= 16)
                    {
                        money16 legacyTotalRideValueForMoney = 0;
                        cs.ReadWrite(legacyTotalRideValueForMoney);
                        gameState.totalRideValueForMoney = legacyTotalRideValueForMoney;
                    }
                    else
                    {
                        cs.ReadWrite(gameState.totalRideValueForMoney);
                    }
                    cs.ReadWrite(gameState.park.numGuestsInParkLastWeek);
                    cs.ReadWrite(gameState.guestChangeModifier);
                    cs.ReadWrite(gameState.guestGenerationProbability);
                    cs.ReadWrite(gameState.suggestedGuestMaximum);

                    cs.ReadWriteArray(gameState.peepWarningThrottle, [&cs](uint8_t& value) {
                        cs.ReadWrite(value);
                        return true;
                    });

                    if (version < k16BitParkHistoryVersion)
                    {
                        if (cs.GetMode() == OrcaStream::Mode::READING)
                        {
                            uint8_t smallHistory[kParkRatingHistorySize];
                            cs.ReadWriteArray(smallHistory, [&cs](uint8_t& value) {
                                cs.ReadWrite(value);
                                return true;
                            });
                            for (int i = 0; i < kParkRatingHistorySize; i++)
                            {
                                if (smallHistory[i] == kRCT12ParkHistoryUndefined)
                                    gameState.park.RatingHistory[i] = kParkRatingHistoryUndefined;
                                else
                                {
                                    gameState.park.RatingHistory[i] = static_cast<uint16_t>(
                                        smallHistory[i] * kRCT12ParkRatingHistoryFactor);
                                }
                            }
                        }
                        else
                        {
                            uint8_t smallHistory[kParkRatingHistorySize];
                            for (int i = 0; i < kParkRatingHistorySize; i++)
                            {
                                if (gameState.park.RatingHistory[i] == kParkRatingHistoryUndefined)
                                    smallHistory[i] = kRCT12ParkHistoryUndefined;
                                else
                                {
                                    smallHistory[i] = static_cast<uint8_t>(
                                        gameState.park.RatingHistory[i] / kRCT12ParkRatingHistoryFactor);
                                }
                            }
                            cs.ReadWriteArray(smallHistory, [&cs](uint8_t& value) {
                                cs.ReadWrite(value);
                                return true;
                            });
                        }
                    }
                    else
                    {
                        cs.ReadWriteArray(gameState.park.RatingHistory, [&cs](uint16_t& value) {
                            cs.ReadWrite(value);
                            return true;
                        });
                    }

                    cs.ReadWriteArray(gameState.park.guestsInParkHistory, [&cs](uint32_t& value) {
                        cs.ReadWrite(value);
                        return true;
                    });

                    cs.ReadWriteArray(gameState.park.cashHistory, [&cs](money64& value) {
                        cs.ReadWrite(value);
                        return true;
                    });
                    cs.ReadWriteArray(gameState.park.weeklyProfitHistory, [&cs](money64& value) {
                        cs.ReadWrite(value);
                        return true;
                    });
                    cs.ReadWriteArray(gameState.park.ValueHistory, [&cs](money64& value) {
                        cs.ReadWrite(value);
                        return true;
                    });
                });
        }

        void ReadWriteResearchChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::RESEARCH, [&gameState](OrcaStream::ChunkStream& cs) {
                // Research status
                cs.ReadWrite(gameState.researchFundingLevel);
                cs.ReadWrite(gameState.researchPriorities);
                cs.ReadWrite(gameState.researchProgressStage);
                cs.ReadWrite(gameState.researchProgress);
                cs.ReadWrite(gameState.researchExpectedMonth);
                cs.ReadWrite(gameState.researchExpectedDay);
                ReadWriteResearchItem(cs, gameState.researchLastItem);
                ReadWriteResearchItem(cs, gameState.researchNextItem);

                // Invention list
                cs.ReadWriteVector(
                    gameState.researchItemsUninvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
                cs.ReadWriteVector(
                    gameState.researchItemsInvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
            });
        }

        static void ReadWriteResearchItem(OrcaStream::ChunkStream& cs, std::optional<ResearchItem>& item)
        {
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                auto hasValue = cs.Read<bool>();
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
                    cs.Write<bool>(true);
                    ReadWriteResearchItem(cs, *item);
                }
                else
                {
                    cs.Write<bool>(false);
                }
            }
        }

        static void ReadWriteResearchItem(OrcaStream::ChunkStream& cs, ResearchItem& item)
        {
            cs.ReadWrite(item.type);
            cs.ReadWrite(item.baseRideType);
            cs.ReadWrite(item.entryIndex);
            cs.ReadWrite(item.flags);
            cs.ReadWrite(item.category);
        }

        void ReadWriteNotificationsChunk(GameState_t& gameState, OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::NOTIFICATIONS, [&gameState](OrcaStream::ChunkStream& cs) {
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    std::vector<News::Item> recent;
                    cs.ReadWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    std::vector<News::Item> archived;
                    cs.ReadWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    News::importNewsItems(gameState, recent, archived);
                }
                else
                {
                    std::vector<News::Item> recent(
                        std::begin(gameState.newsItems.GetRecent()), std::end(gameState.newsItems.GetRecent()));
                    cs.ReadWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    std::vector<News::Item> archived(
                        std::begin(gameState.newsItems.GetArchived()), std::end(gameState.newsItems.GetArchived()));
                    cs.ReadWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });
                }
            });
        }

        static void ReadWriteNewsItem(OrcaStream::ChunkStream& cs, News::Item& item)
        {
            cs.ReadWrite(item.type);
            cs.ReadWrite(item.flags);
            cs.ReadWrite(item.assoc);
            cs.ReadWrite(item.ticks);
            cs.ReadWrite(item.monthYear);
            cs.ReadWrite(item.day);
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                auto s = cs.Read<std::string>();
                item.text = s;
            }
            else
            {
                cs.Write(std::string_view(item.text));
            }
        }

        void ReadWriteTilesChunk(GameState_t& gameState, OrcaStream& os)
        {
            auto* pathToSurfaceMap = _pathToSurfaceMap;
            auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
            auto* pathToRailingsMap = _pathToRailingsMap;

            auto found = os.ReadWriteChunk(
                ParkFileChunkType::TILES,
                [pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap, &os, &gameState](OrcaStream::ChunkStream& cs) {
                    cs.ReadWrite(gameState.mapSize.x);
                    cs.ReadWrite(gameState.mapSize.y);

                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        gameStateInitAll(gameState, gameState.mapSize);

                        auto numElements = cs.Read<uint32_t>();

                        std::vector<TileElement> tileElements;
                        tileElements.resize(numElements);
                        cs.Read(tileElements.data(), tileElements.size() * sizeof(TileElement));
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
                                            trackElement->GetRideType(), trackType, os.GetHeader().TargetVersion))
                                    {
                                        it.element->SetInvisible(true);
                                    }
                                    if (os.GetHeader().TargetVersion < kBlockBrakeImprovementsVersion)
                                    {
                                        if (trackType == TrackElemType::Brakes)
                                            trackElement->SetBrakeClosed(true);
                                        if (trackType == TrackElemType::BlockBrakes)
                                            trackElement->SetBrakeBoosterSpeed(kRCT2DefaultBlockBrakeSpeed);
                                    }
                                }
                                else if (
                                    it.element->GetType() == TileElementType::SmallScenery && os.GetHeader().TargetVersion < 23)
                                {
                                    auto* sceneryElement = it.element->AsSmallScenery();
                                    // Previous formats stored the needs supports flag in the primary colour
                                    // We have moved it into a flags field to support extended colour sets
                                    bool needsSupports = sceneryElement->GetPrimaryColour()
                                        & kRCT12SmallSceneryElementNeedsSupportsFlag;
                                    if (needsSupports)
                                    {
                                        sceneryElement->SetPrimaryColour(
                                            sceneryElement->GetPrimaryColour() & ~kRCT12SmallSceneryElementNeedsSupportsFlag);
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
                        cs.Write(static_cast<uint32_t>(tileElements.size()));
                        cs.Write(tileElements.data(), tileElements.size() * sizeof(TileElement));
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
            os.ReadWriteChunk(ParkFileChunkType::BANNERS, [&os](OrcaStream::ChunkStream& cs) {
                auto version = os.GetHeader().TargetVersion;
                if (cs.GetMode() == OrcaStream::Mode::WRITING)
                {
                    auto numBanners = GetNumBanners();
                    cs.Write(static_cast<uint32_t>(numBanners));

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
                else if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    if (version == 0)
                    {
                        std::vector<Banner> banners;
                        cs.ReadWriteVector(banners, [version, &cs](Banner& banner) { ReadWriteBanner(version, cs, banner); });
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
                        auto numBanners = cs.Read<uint32_t>();
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
                cs.ReadWrite(banner.id);
            }
            cs.ReadWrite(banner.type);
            cs.ReadWrite(banner.flags.holder);
            cs.ReadWrite(banner.text);
            cs.ReadWrite(banner.colour);
            cs.ReadWrite(banner.rideIndex);
            cs.ReadWrite(banner.textColour);
            cs.ReadWrite(banner.position.x);
            cs.ReadWrite(banner.position.y);
        }

        void ReadWriteRidesChunk(GameState_t& gameState, OrcaStream& os)
        {
            const auto version = os.GetHeader().TargetVersion;
            os.ReadWriteChunk(ParkFileChunkType::RIDES, [this, &version, &os](OrcaStream::ChunkStream& cs) {
                std::vector<RideId> rideIds;
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    RideInitAll();
                }
                else
                {
                    if (OmitTracklessRides)
                    {
                        auto tracklessRides = GetTracklessRides();
                        for (const auto& ride : GetRideManager())
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
                        for (const auto& ride : GetRideManager())
                        {
                            rideIds.push_back(ride.id);
                        }
                    }
                }
                cs.ReadWriteVector(rideIds, [&cs, &version, &os](RideId& rideId) {
                    // Ride ID
                    cs.ReadWrite(rideId);

                    auto& ride = [&]() -> Ride& {
                        if (cs.GetMode() == OrcaStream::Mode::WRITING)
                            return *GetRide(rideId);
                        else
                            return *RideAllocateAtIndex(rideId);
                    }();

                    // Status
                    cs.ReadWrite(ride.type);
                    cs.ReadWrite(ride.subtype);
                    cs.ReadWrite(ride.mode);
                    cs.ReadWrite(ride.status);
                    cs.ReadWrite(ride.departFlags);
                    cs.ReadWrite(ride.lifecycleFlags);

                    // Meta
                    cs.ReadWrite(ride.customName);
                    cs.ReadWrite(ride.defaultNameNumber);

                    if (version <= 18)
                    {
                        money16 prices[2] = {};
                        cs.ReadWriteArray(prices, [&cs](money16& price) {
                            cs.ReadWrite(price);
                            return true;
                        });
                        ride.price[0] = prices[0];
                        ride.price[1] = prices[1];
                    }
                    else
                    {
                        cs.ReadWriteArray(ride.price, [&cs](money64& price) {
                            cs.ReadWrite(price);
                            return true;
                        });
                    }

                    // Colours
                    cs.ReadWrite(ride.entranceStyle);
                    cs.ReadWrite(ride.vehicleColourSettings);
                    cs.ReadWriteArray(ride.trackColours, [&cs](TrackColour& tc) {
                        cs.ReadWrite(tc.main);
                        cs.ReadWrite(tc.additional);
                        cs.ReadWrite(tc.supports);
                        return true;
                    });

                    cs.ReadWriteArray(ride.vehicleColours, [&cs](VehicleColour& vc) {
                        cs.ReadWrite(vc.Body);
                        cs.ReadWrite(vc.Trim);
                        cs.ReadWrite(vc.Tertiary);
                        return true;
                    });

                    // Stations
                    cs.ReadWrite(ride.numStations);
                    cs.ReadWriteArray(ride.getStations(), [&cs](RideStation& station) {
                        cs.ReadWrite(station.Start);
                        cs.ReadWrite(station.Height);
                        cs.ReadWrite(station.Length);
                        cs.ReadWrite(station.Depart);
                        cs.ReadWrite(station.TrainAtStation);
                        cs.ReadWrite(station.Entrance);
                        cs.ReadWrite(station.Exit);
                        cs.ReadWrite(station.SegmentLength);
                        cs.ReadWrite(station.SegmentTime);
                        cs.ReadWrite(station.QueueTime);
                        cs.ReadWrite(station.QueueLength);
                        cs.ReadWrite(station.LastPeepInQueue);
                        return true;
                    });

                    cs.ReadWrite(ride.overallView.x);
                    cs.ReadWrite(ride.overallView.y);

                    // Vehicles
                    cs.ReadWrite(ride.numTrains);
                    cs.ReadWrite(ride.numCarsPerTrain);
                    cs.ReadWrite(ride.proposedNumTrains);
                    cs.ReadWrite(ride.proposedNumCarsPerTrain);
                    cs.ReadWrite(ride.maxTrains);
                    if (version < 0x5)
                    {
                        uint8_t value;
                        cs.ReadWrite(value);
                        ride.minCarsPerTrain = GetMinCarsPerTrain(value);
                        ride.maxCarsPerTrain = GetMaxCarsPerTrain(value);
                    }
                    else
                    {
                        cs.ReadWrite(ride.minCarsPerTrain);
                        cs.ReadWrite(ride.maxCarsPerTrain);
                    }

                    cs.ReadWrite(ride.minWaitingTime);
                    cs.ReadWrite(ride.maxWaitingTime);
                    cs.ReadWriteArray(ride.vehicles, [&cs](EntityId& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    // Operation
                    cs.ReadWrite(ride.operationOption);
                    cs.ReadWrite(ride.liftHillSpeed);
                    cs.ReadWrite(ride.numCircuits);

                    // Special
                    cs.ReadWrite(ride.boatHireReturnDirection);
                    cs.ReadWrite(ride.boatHireReturnPosition);
                    cs.ReadWrite(ride.chairliftBullwheelLocation[0]);
                    cs.ReadWrite(ride.chairliftBullwheelLocation[1]);
                    cs.ReadWrite(ride.chairliftBullwheelRotation);
                    cs.ReadWrite(ride.slideInUse);
                    cs.ReadWrite(ride.slidePeep);
                    cs.ReadWrite(ride.slidePeepTShirtColour);
                    cs.ReadWrite(ride.spiralSlideProgress);
                    cs.ReadWrite(ride.raceWinner);
                    cs.ReadWrite(ride.cableLift);
                    cs.ReadWrite(ride.cableLiftLoc);

                    // Stats
                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        auto hasMeasurement = cs.Read<uint8_t>();
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
                            cs.Write<uint8_t>(0);
                        }
                        else
                        {
                            cs.Write<uint8_t>(1);
                            ReadWriteRideMeasurement(cs, *ride.measurement);
                        }
                    }

                    if (os.GetHeader().TargetVersion < kHigherInversionsHolesHelicesStatsVersion)
                    {
                        uint8_t combinedValue = cs.Read<uint8_t>();
                        auto split = splitCombinedHelicesAndSpecialElements(combinedValue);
                        ride.numHelices = split.first;
                        ride.specialTrackElements = split.second;
                    }
                    else
                    {
                        cs.ReadWrite(ride.numHelices);
                        cs.ReadWrite(ride.specialTrackElements.holder);
                    }

                    cs.ReadWrite(ride.maxSpeed);
                    cs.ReadWrite(ride.averageSpeed);
                    cs.ReadWrite(ride.currentTestSegment);
                    cs.ReadWrite(ride.averageSpeedTestTimeout);

                    cs.ReadWrite(ride.maxPositiveVerticalG);
                    cs.ReadWrite(ride.maxNegativeVerticalG);
                    cs.ReadWrite(ride.maxLateralG);
                    cs.ReadWrite(ride.previousVerticalG);
                    cs.ReadWrite(ride.previousLateralG);

                    cs.ReadWrite(ride.testingFlags.holder);
                    cs.ReadWrite(ride.curTestTrackLocation);

                    cs.ReadWrite(ride.turnCountDefault);
                    cs.ReadWrite(ride.turnCountBanked);
                    cs.ReadWrite(ride.turnCountSloped);

                    if (version < kHigherInversionsHolesHelicesStatsVersion)
                    {
                        uint16_t numInversions = cs.Read<uint16_t>();
                        ride.numInversions = numInversions;
                        auto combinedValue = cs.Read<uint8_t>();
                        auto split = splitCombinedNumDropsPoweredLifts(combinedValue);
                        ride.numDrops = split.first;
                        ride.numPoweredLifts = split.second;
                    }
                    else
                    {
                        cs.ReadWrite(ride.numInversions);
                        cs.ReadWrite(ride.numDrops);
                        cs.ReadWrite(ride.numPoweredLifts);
                    }
                    cs.ReadWrite(ride.startDropHeight);
                    cs.ReadWrite(ride.highestDropHeight);
                    cs.ReadWrite(ride.shelteredLength);
                    cs.ReadWrite(ride.var11C);
                    cs.ReadWrite(ride.numShelteredSections);
                    if (version >= kInversionsHolesShelteredEightsSplit)
                    {
                        cs.ReadWrite(ride.shelteredEighths);
                        if (version < kHigherInversionsHolesHelicesStatsVersion)
                        {
                            auto numHoles = cs.Read<uint16_t>();
                            ride.numHoles = numHoles;
                        }
                        else
                        {
                            cs.ReadWrite(ride.numHoles);
                        }
                    }
                    cs.ReadWrite(ride.currentTestStation);
                    cs.ReadWrite(ride.numBlockBrakes);
                    cs.ReadWrite(ride.totalAirTime);

                    cs.ReadWrite(ride.ratings.excitement);
                    cs.ReadWrite(ride.ratings.intensity);
                    cs.ReadWrite(ride.ratings.nausea);

                    if (version <= 18)
                    {
                        uint16_t tempRideValue{};
                        cs.ReadWrite(tempRideValue);
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
                        cs.ReadWrite(tempRideValue);
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
                        cs.ReadWrite(ride.value);
                    }

                    cs.ReadWrite(ride.numRiders);
                    cs.ReadWrite(ride.buildDate);

                    if (version <= 18)
                    {
                        money16 tempUpkeepCost{};
                        cs.ReadWrite(tempUpkeepCost);
                        ride.upkeepCost = ToMoney64(tempUpkeepCost);
                    }
                    else
                    {
                        cs.ReadWrite(ride.upkeepCost);
                    }

                    cs.ReadWrite(ride.curNumCustomers);
                    cs.ReadWrite(ride.numCustomersTimeout);

                    cs.ReadWriteArray(ride.numCustomers, [&cs](uint16_t& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    cs.ReadWrite(ride.totalCustomers);
                    cs.ReadWrite(ride.totalProfit);
                    cs.ReadWrite(ride.popularity);
                    cs.ReadWrite(ride.popularityTimeout);
                    cs.ReadWrite(ride.popularityNext);
                    cs.ReadWrite(ride.guestsFavourite);
                    cs.ReadWrite(ride.numPrimaryItemsSold);
                    cs.ReadWrite(ride.numSecondaryItemsSold);
                    cs.ReadWrite(ride.incomePerHour);
                    cs.ReadWrite(ride.profit);
                    cs.ReadWrite(ride.satisfaction);
                    cs.ReadWrite(ride.satisfactionTimeout);
                    cs.ReadWrite(ride.satisfactionNext);

                    // Breakdown
                    cs.ReadWrite(ride.breakdownReasonPending);
                    cs.ReadWrite(ride.mechanicStatus);
                    cs.ReadWrite(ride.mechanic);
                    cs.ReadWrite(ride.inspectionStation);
                    cs.ReadWrite(ride.brokenTrain);
                    cs.ReadWrite(ride.brokenCar);
                    cs.ReadWrite(ride.breakdownReason);
                    cs.ReadWrite(ride.reliabilitySubvalue);
                    cs.ReadWrite(ride.reliabilityPercentage);
                    cs.ReadWrite(ride.unreliabilityFactor);
                    cs.ReadWrite(ride.downtime);
                    cs.ReadWrite(ride.inspectionInterval);
                    cs.ReadWrite(ride.lastInspection);

                    cs.ReadWriteArray(ride.downtimeHistory, [&cs](uint8_t& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    cs.ReadWrite(ride.breakdownSoundModifier);
                    cs.ReadWrite(ride.notFixedTimeout);
                    cs.ReadWrite(ride.lastCrashType);
                    cs.ReadWrite(ride.connectedMessageThrottle);

                    cs.ReadWrite(ride.vehicleChangeTimeout);

                    cs.ReadWrite(ride.currentIssues);
                    cs.ReadWrite(ride.lastIssueTime);

                    // Music
                    cs.ReadWrite(ride.music);
                    cs.ReadWrite(ride.musicTuneId);
                    cs.ReadWrite(ride.musicPosition);
                    return true;
                });
            });
        }

        static void ReadWriteRideMeasurement(OrcaStream::ChunkStream& cs, RideMeasurement& measurement)
        {
            cs.ReadWrite(measurement.flags);
            cs.ReadWrite(measurement.last_use_tick);
            cs.ReadWrite(measurement.num_items);
            cs.ReadWrite(measurement.current_item);
            cs.ReadWrite(measurement.vehicle_index);
            cs.ReadWrite(measurement.current_station);
            for (size_t i = 0; i < measurement.num_items; i++)
            {
                cs.ReadWrite(measurement.vertical[i]);
                cs.ReadWrite(measurement.lateral[i]);
                cs.ReadWrite(measurement.velocity[i]);
                cs.ReadWrite(measurement.altitude[i]);
            }
        }

        template<typename T>
        static void ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, T& entity);

        static void ReadWriteEntityCommon(OrcaStream::ChunkStream& cs, EntityBase& entity)
        {
            cs.ReadWrite(entity.Id);
            cs.ReadWrite(entity.SpriteData.HeightMin);
            cs.ReadWrite(entity.x);
            cs.ReadWrite(entity.y);
            cs.ReadWrite(entity.z);
            cs.ReadWrite(entity.SpriteData.Width);
            cs.ReadWrite(entity.SpriteData.HeightMax);
            cs.ReadWrite(entity.Orientation);
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
            auto version = os.GetHeader().TargetVersion;

            ReadWriteEntityCommon(cs, entity);

            auto guest = entity.As<Guest>();
            auto staff = entity.As<Staff>();

            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                auto name = cs.Read<std::string>();
                entity.SetName(name);
            }
            else
            {
                cs.Write(static_cast<const char*>(entity.Name));
            }

            cs.ReadWrite(entity.NextLoc);
            cs.ReadWrite(entity.NextFlags);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->OutsideOfPark);
                }
                else
                {
                    cs.Ignore<bool>();
                }
            }

            cs.ReadWrite(entity.State);
            cs.ReadWrite(entity.SubState);

            if (version >= kPeepAnimationObjectsVersion)
                cs.ReadWrite(entity.AnimationObjectIndex);
            else
                entity.AnimationObjectIndex = kObjectEntryIndexNull;

            cs.ReadWrite(entity.AnimationGroup);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->GuestNumRides);
                }
                else
                {
                    cs.ReadWrite(staff->AssignedStaffType);
                }
            }

            cs.ReadWrite(entity.TshirtColour);
            cs.ReadWrite(entity.TrousersColour);
            cs.ReadWrite(entity.DestinationX);
            cs.ReadWrite(entity.DestinationY);
            cs.ReadWrite(entity.DestinationTolerance);
            cs.ReadWrite(entity.Var37);
            cs.ReadWrite(entity.Energy);
            cs.ReadWrite(entity.EnergyTarget);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->Happiness);
                    cs.ReadWrite(guest->HappinessTarget);
                    cs.ReadWrite(guest->Nausea);
                    cs.ReadWrite(guest->NauseaTarget);
                    cs.ReadWrite(guest->Hunger);
                    cs.ReadWrite(guest->Thirst);
                    cs.ReadWrite(guest->Toilet);
                }
                else
                {
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                }
            }

            cs.ReadWrite(entity.Mass);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->TimeToConsume);
                }
                else
                {
                    uint8_t temp{};
                    cs.ReadWrite(temp);
                }
            }

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        guest->Intensity = IntensityRange(cs.Read<uint8_t>());
                    }
                    else
                    {
                        cs.Write(static_cast<uint8_t>(guest->Intensity));
                    }
                    cs.ReadWrite(guest->NauseaTolerance);
                }
                else
                {
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                }
            }

            cs.ReadWrite(entity.WindowInvalidateFlags);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    money16 tempPaidOnDrink{};
                    cs.ReadWrite(tempPaidOnDrink);
                    guest->PaidOnDrink = ToMoney64(tempPaidOnDrink);
                    std::array<uint8_t, 16> rideTypeBeenOn;
                    cs.ReadWriteArray(rideTypeBeenOn, [&cs](uint8_t& rideType) {
                        cs.ReadWrite(rideType);
                        return true;
                    });
                    OpenRCT2::RideUse::GetTypeHistory().Set(guest->Id, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
                    cs.ReadWrite(guest->ItemFlags);
                    cs.ReadWrite(guest->Photo2RideRef);
                    cs.ReadWrite(guest->Photo3RideRef);
                    cs.ReadWrite(guest->Photo4RideRef);
                }
                else
                {
                    cs.Ignore<money16>();

                    std::vector<uint8_t> temp;
                    cs.ReadWriteVector(temp, [&cs](uint8_t& rideType) {
                        cs.ReadWrite(rideType);
                        return true;
                    });
                    cs.Ignore<uint64_t>();
                    cs.Ignore<RideId>();
                    cs.Ignore<RideId>();
                    cs.Ignore<RideId>();
                }
            }

            cs.ReadWrite(entity.CurrentRide);
            cs.ReadWrite(entity.CurrentRideStation);
            cs.ReadWrite(entity.CurrentTrain);
            cs.ReadWrite(entity.TimeToSitdown);
            cs.ReadWrite(entity.SpecialSprite);
            cs.ReadWrite(entity.AnimationType);
            cs.ReadWrite(entity.NextAnimationType);
            cs.ReadWrite(entity.AnimationImageIdOffset);
            cs.ReadWrite(entity.Action);
            cs.ReadWrite(entity.AnimationFrameNum);
            cs.ReadWrite(entity.StepProgress);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->GuestNextInQueue);
                }
                else
                {
                    cs.ReadWrite(staff->MechanicTimeSinceCall);
                }
            }

            cs.ReadWrite(entity.PeepDirection);
            cs.ReadWrite(entity.InteractionRideIndex);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->TimeInQueue);
                    std::array<uint8_t, 32> ridesBeenOn;
                    cs.ReadWriteArray(ridesBeenOn, [&cs](uint8_t& rideType) {
                        cs.ReadWrite(rideType);
                        return true;
                    });
                    OpenRCT2::RideUse::GetHistory().Set(guest->Id, LegacyGetRidesBeenOn(ridesBeenOn));
                }
                else
                {
                    cs.Ignore<uint16_t>();

                    std::vector<uint8_t> ridesBeenOn;
                    cs.ReadWriteVector(ridesBeenOn, [&cs](uint8_t& rideId) {
                        cs.ReadWrite(rideId);
                        return true;
                    });
                }
            }

            cs.ReadWrite(entity.PeepId);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    money32 tempCashInPocket{};
                    money32 tempCashSpent{};
                    cs.ReadWrite(tempCashInPocket);
                    cs.ReadWrite(tempCashSpent);
                    guest->CashInPocket = ToMoney64(tempCashInPocket);
                    guest->CashSpent = ToMoney64(tempCashSpent);

                    cs.ReadWrite(guest->ParkEntryTime);
                    cs.ReadWrite(guest->RejoinQueueTimeout);
                    cs.ReadWrite(guest->PreviousRide);
                    cs.ReadWrite(guest->PreviousRideTimeOut);
                    cs.ReadWriteArray(guest->Thoughts, [&cs](PeepThought& thought) {
                        cs.ReadWrite(thought.type);

                        uint8_t item{};
                        cs.ReadWrite(item);
                        if (item == 255)
                        {
                            thought.item = kPeepThoughtItemNone;
                        }
                        else
                        {
                            thought.item = item;
                        }

                        cs.ReadWrite(thought.freshness);
                        cs.ReadWrite(thought.fresh_timeout);
                        return true;
                    });
                }
                else
                {
                    cs.Ignore<money32>();
                    cs.Ignore<money32>();
                    cs.ReadWrite(staff->HireDate);
                    cs.Ignore<int8_t>();
                    cs.Ignore<RideId>();
                    cs.Ignore<uint16_t>();

                    std::vector<PeepThought> temp;
                    cs.ReadWriteVector(temp, [&cs](PeepThought& thought) {
                        cs.ReadWrite(thought.type);
                        cs.ReadWrite(thought.item);
                        cs.ReadWrite(thought.freshness);
                        cs.ReadWrite(thought.fresh_timeout);
                        return true;
                    });
                }
            }

            cs.ReadWrite(entity.PathCheckOptimisation);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->GuestHeadingToRideId);
                    cs.ReadWrite(guest->GuestIsLostCountdown);
                    cs.ReadWrite(guest->Photo1RideRef);
                }
                else
                {
                    cs.Ignore<RideId>();
                    cs.ReadWrite(staff->StaffOrders);
                    cs.Ignore<RideId>();
                }
            }

            cs.ReadWrite(entity.PeepFlags);
            cs.ReadWrite(entity.PathfindGoal.x);
            cs.ReadWrite(entity.PathfindGoal.y);
            cs.ReadWrite(entity.PathfindGoal.z);
            cs.ReadWrite(entity.PathfindGoal.direction);
            for (size_t i = 0; i < std::size(entity.PathfindHistory); i++)
            {
                cs.ReadWrite(entity.PathfindHistory[i].x);
                cs.ReadWrite(entity.PathfindHistory[i].y);
                cs.ReadWrite(entity.PathfindHistory[i].z);
                cs.ReadWrite(entity.PathfindHistory[i].direction);
            }
            cs.ReadWrite(entity.WalkingAnimationFrameNum);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->LitterCount);
                    cs.ReadWrite(guest->GuestTimeOnRide);
                    cs.ReadWrite(guest->DisgustingCount);

                    money16 expenditures[4]{};
                    cs.ReadWrite(expenditures[0]);
                    cs.ReadWrite(expenditures[1]);
                    cs.ReadWrite(expenditures[2]);
                    cs.ReadWrite(expenditures[3]);
                    guest->PaidToEnter = ToMoney64(expenditures[0]);
                    guest->PaidOnRides = ToMoney64(expenditures[1]);
                    guest->PaidOnFood = ToMoney64(expenditures[2]);
                    guest->PaidOnSouvenirs = ToMoney64(expenditures[3]);

                    cs.ReadWrite(guest->AmountOfFood);
                    cs.ReadWrite(guest->AmountOfDrinks);
                    cs.ReadWrite(guest->AmountOfSouvenirs);
                    cs.ReadWrite(guest->VandalismSeen);
                    cs.ReadWrite(guest->VoucherType);
                    cs.ReadWrite(guest->VoucherRideId);
                    cs.ReadWrite(guest->SurroundingsThoughtTimeout);
                    cs.ReadWrite(guest->Angriness);
                    cs.ReadWrite(guest->TimeLost);
                    cs.ReadWrite(guest->DaysInQueue);
                    cs.ReadWrite(guest->BalloonColour);
                    cs.ReadWrite(guest->UmbrellaColour);
                    cs.ReadWrite(guest->HatColour);
                    cs.ReadWrite(guest->FavouriteRide);
                    cs.ReadWrite(guest->FavouriteRideRating);
                }
                else
                {
                    cs.Ignore<uint8_t>();
                    cs.ReadWrite(staff->StaffMowingTimeout);
                    cs.Ignore<uint8_t>();
                    cs.ReadWrite(staff->StaffLawnsMown);
                    cs.ReadWrite(staff->StaffGardensWatered);
                    cs.ReadWrite(staff->StaffLitterSwept);
                    cs.ReadWrite(staff->StaffBinsEmptied);
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<RideId>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<uint8_t>();
                    cs.Ignore<RideId>();
                    cs.Ignore<uint8_t>();
                }
            }
        }

        template<typename T>
        void WriteEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs);
        template<typename... T>
        void WriteEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename T>
        void ReadEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename... T>
        void ReadEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs);

        void ReadWriteEntitiesChunk(GameState_t& gameState, OrcaStream& os);

        static void ReadWriteStringTable(OrcaStream::ChunkStream& cs, std::string& value, const std::string_view lcode)
        {
            std::vector<std::tuple<std::string, std::string>> table;
            if (cs.GetMode() != OrcaStream::Mode::READING)
            {
                table.push_back(std::make_tuple(std::string(lcode), value));
            }
            cs.ReadWriteVector(table, [&cs](std::tuple<std::string, std::string>& v) {
                cs.ReadWrite(std::get<0>(v));
                cs.ReadWrite(std::get<1>(v));
            });
            if (cs.GetMode() == OrcaStream::Mode::READING)
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
        cs.ReadWrite(entity.SubType);
        cs.ReadWrite(entity.Pitch);
        cs.ReadWrite(entity.bank_rotation);
        cs.ReadWrite(entity.remaining_distance);
        cs.ReadWrite(entity.velocity);
        cs.ReadWrite(entity.acceleration);
        cs.ReadWrite(entity.ride);
        cs.ReadWrite(entity.vehicle_type);
        cs.ReadWrite(entity.colours.Body);
        cs.ReadWrite(entity.colours.Trim);
        cs.ReadWrite(entity.track_progress);
        cs.ReadWrite(entity.BoatLocation);
        cs.ReadWrite(entity.TrackTypeAndDirection);
        cs.ReadWrite(entity.TrackLocation.x);
        cs.ReadWrite(entity.TrackLocation.y);
        cs.ReadWrite(entity.TrackLocation.z);
        cs.ReadWrite(entity.next_vehicle_on_train);
        cs.ReadWrite(entity.prev_vehicle_on_ride);
        cs.ReadWrite(entity.next_vehicle_on_ride);
        cs.ReadWrite(entity.var_44);
        cs.ReadWrite(entity.mass);
        if (cs.GetMode() == OrcaStream::Mode::READING && os.GetHeader().TargetVersion < 18)
        {
            uint16_t updateFlags = 0;
            cs.ReadWrite(updateFlags);
            entity.Flags = updateFlags;
        }
        else
        {
            cs.ReadWrite(entity.Flags);
        }
        cs.ReadWrite(entity.SwingSprite);
        cs.ReadWrite(entity.current_station);
        cs.ReadWrite(entity.current_time);
        cs.ReadWrite(entity.crash_z);
        cs.ReadWrite(entity.status);
        cs.ReadWrite(entity.sub_state);
        for (size_t i = 0; i < std::size(entity.peep); i++)
        {
            cs.ReadWrite(entity.peep[i]);
            cs.ReadWrite(entity.peep_tshirt_colours[i]);
        }
        cs.ReadWrite(entity.num_seats);
        cs.ReadWrite(entity.num_peeps);
        cs.ReadWrite(entity.next_free_seat);
        cs.ReadWrite(entity.restraints_position);
        cs.ReadWrite(entity.crash_x);
        cs.ReadWrite(entity.sound2_flags);
        cs.ReadWrite(entity.spin_sprite);
        cs.ReadWrite(entity.sound1_id);
        cs.ReadWrite(entity.sound1_volume);
        cs.ReadWrite(entity.sound2_id);
        cs.ReadWrite(entity.sound2_volume);
        cs.ReadWrite(entity.sound_vector_factor);
        cs.ReadWrite(entity.time_waiting);
        cs.ReadWrite(entity.speed);
        cs.ReadWrite(entity.powered_acceleration);
        cs.ReadWrite(entity.CollisionDetectionTimer);
        cs.ReadWrite(entity.animation_frame);
        if (cs.GetMode() == OrcaStream::Mode::READING && os.GetHeader().TargetVersion <= 2)
        {
            uint16_t lower = 0, upper = 0;
            cs.ReadWrite(lower);
            cs.ReadWrite(upper);
            entity.animationState = lower | (upper << 16);
        }
        else
        {
            cs.ReadWrite(entity.animationState);
        }
        cs.ReadWrite(entity.scream_sound_id);
        cs.ReadWrite(entity.TrackSubposition);
        cs.ReadWrite(entity.NumLaps);
        if (cs.GetMode() == OrcaStream::Mode::READING && os.GetHeader().TargetVersion < kBlockBrakeImprovementsVersion)
        {
            uint8_t brakeSpeed;
            cs.ReadWrite(brakeSpeed);
            if (entity.GetTrackType() == TrackElemType::BlockBrakes)
                brakeSpeed = kRCT2DefaultBlockBrakeSpeed;
            entity.brake_speed = brakeSpeed;
        }
        else
        {
            cs.ReadWrite(entity.brake_speed);
        }
        cs.ReadWrite(entity.lost_time_out);
        cs.ReadWrite(entity.vertical_drop_countdown);
        cs.ReadWrite(entity.var_D3);
        cs.ReadWrite(entity.mini_golf_current_animation);
        cs.ReadWrite(entity.mini_golf_flags);
        cs.ReadWrite(entity.ride_subtype);
        cs.ReadWrite(entity.colours.Tertiary);
        cs.ReadWrite(entity.seat_rotation);
        cs.ReadWrite(entity.target_seat_rotation);
        if (cs.GetMode() == OrcaStream::Mode::READING && os.GetHeader().TargetVersion < 18)
        {
            bool isCrashedVehicle = false;
            cs.ReadWrite(isCrashedVehicle);
            if (isCrashedVehicle)
            {
                entity.SetFlag(VehicleFlags::Crashed);
            }
        }
        if (cs.GetMode() == OrcaStream::Mode::READING && os.GetHeader().TargetVersion < kBlockBrakeImprovementsVersion)
        {
            entity.BlockBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;
        }
        else
        {
            cs.ReadWrite(entity.BlockBrakeSpeed);
        }
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Guest& guest)
    {
        ReadWritePeep(os, cs, guest);
        auto version = os.GetHeader().TargetVersion;

        if (version <= 1)
        {
            return;
        }

        cs.ReadWrite(guest.GuestNumRides);
        cs.ReadWrite(guest.GuestNextInQueue);
        cs.ReadWrite(guest.ParkEntryTime);
        cs.ReadWrite(guest.GuestHeadingToRideId);
        cs.ReadWrite(guest.GuestIsLostCountdown);
        cs.ReadWrite(guest.GuestTimeOnRide);

        if (version <= 18)
        {
            money16 expenditures[5]{};
            cs.ReadWrite(expenditures[0]);
            cs.ReadWrite(expenditures[1]);
            cs.ReadWrite(expenditures[2]);
            cs.ReadWrite(expenditures[3]);
            cs.ReadWrite(expenditures[4]);
            guest.PaidToEnter = ToMoney64(expenditures[0]);
            guest.PaidOnRides = ToMoney64(expenditures[1]);
            guest.PaidOnFood = ToMoney64(expenditures[2]);
            guest.PaidOnDrink = ToMoney64(expenditures[3]);
            guest.PaidOnSouvenirs = ToMoney64(expenditures[4]);
        }
        else
        {
            cs.ReadWrite(guest.PaidToEnter);
            cs.ReadWrite(guest.PaidOnRides);
            cs.ReadWrite(guest.PaidOnFood);
            cs.ReadWrite(guest.PaidOnDrink);
            cs.ReadWrite(guest.PaidOnSouvenirs);
        }

        cs.ReadWrite(guest.OutsideOfPark);
        cs.ReadWrite(guest.Happiness);
        cs.ReadWrite(guest.HappinessTarget);
        cs.ReadWrite(guest.Nausea);
        cs.ReadWrite(guest.NauseaTarget);
        cs.ReadWrite(guest.Hunger);
        cs.ReadWrite(guest.Thirst);
        cs.ReadWrite(guest.Toilet);
        cs.ReadWrite(guest.TimeToConsume);
        if (cs.GetMode() == OrcaStream::Mode::READING)
        {
            guest.Intensity = IntensityRange(cs.Read<uint8_t>());
        }
        else
        {
            cs.Write(static_cast<uint8_t>(guest.Intensity));
        }
        cs.ReadWrite(guest.NauseaTolerance);

        if (os.GetHeader().TargetVersion < 3)
        {
            std::array<uint8_t, 16> rideTypeBeenOn;
            cs.ReadWriteArray(rideTypeBeenOn, [&cs](uint8_t& rideType) {
                cs.ReadWrite(rideType);
                return true;
            });
            OpenRCT2::RideUse::GetTypeHistory().Set(guest.Id, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
        }

        cs.ReadWrite(guest.TimeInQueue);
        if (os.GetHeader().TargetVersion < 3)
        {
            std::array<uint8_t, 32> ridesBeenOn;
            cs.ReadWriteArray(ridesBeenOn, [&cs](uint8_t& rideType) {
                cs.ReadWrite(rideType);
                return true;
            });
            OpenRCT2::RideUse::GetHistory().Set(guest.Id, LegacyGetRidesBeenOn(ridesBeenOn));
        }
        else
        {
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                std::vector<RideId> rideUse;
                cs.ReadWriteVector(rideUse, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                OpenRCT2::RideUse::GetHistory().Set(guest.Id, std::move(rideUse));
                std::vector<ObjectEntryIndex> rideTypeUse;
                cs.ReadWriteVector(rideTypeUse, [&cs](ObjectEntryIndex& rideType) { cs.ReadWrite(rideType); });
                OpenRCT2::RideUse::GetTypeHistory().Set(guest.Id, std::move(rideTypeUse));
            }
            else
            {
                auto* rideUse = OpenRCT2::RideUse::GetHistory().GetAll(guest.Id);
                if (rideUse == nullptr)
                {
                    std::vector<RideId> empty;
                    cs.ReadWriteVector(empty, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                }
                else
                {
                    cs.ReadWriteVector(*rideUse, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                }
                auto* rideTypeUse = OpenRCT2::RideUse::GetTypeHistory().GetAll(guest.Id);
                if (rideTypeUse == nullptr)
                {
                    std::vector<ObjectEntryIndex> empty;
                    cs.ReadWriteVector(empty, [&cs](ObjectEntryIndex& rideId) { cs.ReadWrite(rideId); });
                }
                else
                {
                    cs.ReadWriteVector(*rideTypeUse, [&cs](ObjectEntryIndex& rideId) { cs.ReadWrite(rideId); });
                }
            }
        }
        if (version <= 18)
        {
            money32 tempCashInPocket{};
            money32 tempCashSpent{};
            cs.ReadWrite(tempCashInPocket);
            cs.ReadWrite(tempCashSpent);
            guest.CashInPocket = ToMoney64(tempCashInPocket);
            guest.CashSpent = ToMoney64(tempCashSpent);
        }
        else
        {
            cs.ReadWrite(guest.CashInPocket);
            cs.ReadWrite(guest.CashSpent);
        }

        cs.ReadWrite(guest.Photo1RideRef);
        cs.ReadWrite(guest.Photo2RideRef);
        cs.ReadWrite(guest.Photo3RideRef);
        cs.ReadWrite(guest.Photo4RideRef);
        cs.ReadWrite(guest.RejoinQueueTimeout);
        cs.ReadWrite(guest.PreviousRide);
        cs.ReadWrite(guest.PreviousRideTimeOut);
        cs.ReadWriteArray(guest.Thoughts, [version = os.GetHeader().TargetVersion, &cs](PeepThought& thought) {
            cs.ReadWrite(thought.type);
            if (version <= 2)
            {
                int16_t item{};
                cs.ReadWrite(item);
                thought.item = item;
            }
            else
            {
                cs.ReadWrite(thought.item);
            }
            cs.ReadWrite(thought.freshness);
            cs.ReadWrite(thought.fresh_timeout);
            return true;
        });
        cs.ReadWrite(guest.LitterCount);
        cs.ReadWrite(guest.DisgustingCount);
        cs.ReadWrite(guest.AmountOfFood);
        cs.ReadWrite(guest.AmountOfDrinks);
        cs.ReadWrite(guest.AmountOfSouvenirs);
        cs.ReadWrite(guest.VandalismSeen);
        cs.ReadWrite(guest.VoucherType);
        cs.ReadWrite(guest.VoucherRideId);
        cs.ReadWrite(guest.SurroundingsThoughtTimeout);
        cs.ReadWrite(guest.Angriness);
        cs.ReadWrite(guest.TimeLost);
        cs.ReadWrite(guest.DaysInQueue);
        cs.ReadWrite(guest.BalloonColour);
        cs.ReadWrite(guest.UmbrellaColour);
        cs.ReadWrite(guest.HatColour);
        cs.ReadWrite(guest.FavouriteRide);
        cs.ReadWrite(guest.FavouriteRideRating);
        cs.ReadWrite(guest.ItemFlags);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Staff& entity)
    {
        ReadWritePeep(os, cs, entity);

        std::vector<TileCoordsXY> patrolArea;
        if (cs.GetMode() == OrcaStream::Mode::WRITING && entity.PatrolInfo != nullptr)
        {
            patrolArea = entity.PatrolInfo->ToVector();
        }
        cs.ReadWriteVector(patrolArea, [&cs](TileCoordsXY& value) { cs.ReadWrite(value); });
        if (cs.GetMode() == OrcaStream::Mode::READING)
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

        if (os.GetHeader().TargetVersion <= 1)
        {
            return;
        }

        cs.ReadWrite(entity.AssignedStaffType);
        cs.ReadWrite(entity.MechanicTimeSinceCall);
        cs.ReadWrite(entity.HireDate);
        if (os.GetHeader().TargetVersion <= 4)
        {
            cs.Ignore<uint8_t>();
        }
        cs.ReadWrite(entity.StaffOrders);
        cs.ReadWrite(entity.StaffMowingTimeout);
        cs.ReadWrite(entity.StaffLawnsMown);
        cs.ReadWrite(entity.StaffGardensWatered);
        cs.ReadWrite(entity.StaffLitterSwept);
        cs.ReadWrite(entity.StaffBinsEmptied);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, SteamParticle& steamParticle)
    {
        ReadWriteEntityCommon(cs, steamParticle);
        cs.ReadWrite(steamParticle.time_to_move);
        cs.ReadWrite(steamParticle.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, MoneyEffect& moneyEffect)
    {
        ReadWriteEntityCommon(cs, moneyEffect);
        cs.ReadWrite(moneyEffect.MoveDelay);
        cs.ReadWrite(moneyEffect.NumMovements);
        cs.ReadWrite(moneyEffect.GuestPurchase);
        cs.ReadWrite(moneyEffect.Value);
        cs.ReadWrite(moneyEffect.OffsetX);
        cs.ReadWrite(moneyEffect.Wiggle);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, VehicleCrashParticle& vehicleCrashParticle)
    {
        ReadWriteEntityCommon(cs, vehicleCrashParticle);
        cs.ReadWrite(vehicleCrashParticle.frame);
        cs.ReadWrite(vehicleCrashParticle.time_to_live);
        cs.ReadWrite(vehicleCrashParticle.frame);
        cs.ReadWrite(vehicleCrashParticle.colour[0]);
        cs.ReadWrite(vehicleCrashParticle.colour[1]);
        cs.ReadWrite(vehicleCrashParticle.crashed_sprite_base);
        cs.ReadWrite(vehicleCrashParticle.velocity_x);
        cs.ReadWrite(vehicleCrashParticle.velocity_y);
        cs.ReadWrite(vehicleCrashParticle.velocity_z);
        cs.ReadWrite(vehicleCrashParticle.acceleration_x);
        cs.ReadWrite(vehicleCrashParticle.acceleration_y);
        cs.ReadWrite(vehicleCrashParticle.acceleration_z);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionCloud& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, CrashSplashParticle& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionFlare& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, JumpingFountain& fountain)
    {
        ReadWriteEntityCommon(cs, fountain);
        cs.ReadWrite(fountain.NumTicksAlive);
        cs.ReadWrite(fountain.frame);
        cs.ReadWrite(fountain.fountainFlags.holder);
        cs.ReadWrite(fountain.TargetX);
        cs.ReadWrite(fountain.TargetY);
        cs.ReadWrite(fountain.TargetY);
        cs.ReadWrite(fountain.Iteration);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Balloon& balloon)
    {
        ReadWriteEntityCommon(cs, balloon);
        cs.ReadWrite(balloon.popped);
        cs.ReadWrite(balloon.time_to_move);
        cs.ReadWrite(balloon.frame);
        cs.ReadWrite(balloon.colour);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Duck& duck)
    {
        ReadWriteEntityCommon(cs, duck);
        cs.ReadWrite(duck.frame);
        cs.ReadWrite(duck.target_x);
        cs.ReadWrite(duck.target_y);
        cs.ReadWrite(duck.state);
    }

    template<>
    void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Litter& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.SubType);
        cs.ReadWrite(entity.creationTick);
    }

    template<typename T>
    void ParkFile::WriteEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        uint16_t count = GetEntityListCount(T::cEntityType);
        cs.Write(T::cEntityType);
        cs.Write(count);
        for (auto* ent : EntityList<T>())
        {
            cs.Write(ent->Id);
            ReadWriteEntity(os, cs, *ent);
        }
    }

    template<typename... T>
    void ParkFile::WriteEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (WriteEntitiesOfType<T>(os, cs), ...);
    }

    template<typename T>
    void ParkFile::ReadEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        [[maybe_unused]] auto t = cs.Read<EntityType>();
        assert(t == T::cEntityType);
        auto count = cs.Read<uint16_t>();
        for (auto i = 0; i < count; ++i)
        {
            T placeholder{};

            auto index = cs.Read<EntityId>();
            auto* ent = CreateEntityAt<T>(index);
            if (ent == nullptr)
            {
                // Unable to allocate entity
                ent = &placeholder;
            }
            ReadWriteEntity(os, cs, *ent);
        }
    }

    template<typename... T>
    void ParkFile::ReadEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (ReadEntitiesOfType<T>(os, cs), ...);
    }

    void ParkFile::ReadWriteEntitiesChunk(GameState_t& gameState, OrcaStream& os)
    {
        os.ReadWriteChunk(ParkFileChunkType::ENTITIES, [this, &gameState, &os](OrcaStream::ChunkStream& cs) {
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                ResetAllEntities();
            }

            std::vector<uint16_t> entityIndices;
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                ReadEntitiesOfTypes<
                    Vehicle, Guest, Staff, Litter, SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud,
                    CrashSplashParticle, ExplosionFlare, JumpingFountain, Balloon, Duck>(os, cs);

                auto version = os.GetHeader().TargetVersion;
                if (version < kPeepAnimationObjectsVersion)
                {
                    ConvertPeepAnimationTypeToObjects(gameState);
                }
            }
            else
            {
                WriteEntitiesOfTypes<
                    Vehicle, Guest, Staff, Litter, SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud,
                    CrashSplashParticle, ExplosionFlare, JumpingFountain, Balloon, Duck>(os, cs);
            }
        });
    }

    void ParkFileExporter::Export(GameState_t& gameState, std::string_view path, int16_t compressionLevel)
    {
        auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
        parkFile->Save(gameState, path, compressionLevel);
    }

    void ParkFileExporter::Export(GameState_t& gameState, IStream& stream, int16_t compressionLevel)
    {
        auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
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
    auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
    try
    {
        if (flags & S6_SAVE_FLAG_EXPORT)
        {
            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
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
    std::unique_ptr<OpenRCT2::ParkFile> _parkFile;

public:
    ParkFileImporter(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
    }

    ParkLoadResult Load(const u8string& path, const bool skipObjectCheck) override
    {
        _parkFile = std::make_unique<OpenRCT2::ParkFile>();
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
        OpenRCT2::IStream* stream, bool isScenario, bool skipObjectCheck = false, const u8string& path = {}) override
    {
        _parkFile = std::make_unique<OpenRCT2::ParkFile>();
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
