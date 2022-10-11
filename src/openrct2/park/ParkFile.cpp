/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkFile.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Editor.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../Version.h"
#include "../core/Console.hpp"
#include "../core/Crypt.h"
#include "../core/DataSerialiser.h"
#include "../core/File.h"
#include "../core/OrcaStream.hpp"
#include "../core/Path.hpp"
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
#include "../interface/Window.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/NewsItem.h"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/RideUseSystem.h"
#include "../ride/ShopItem.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scripting/ScriptEngine.h"
#include "../ui/UiContext.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "Legacy.h"

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
        ObjectEntryIndex _pathToSurfaceMap[MAX_PATH_OBJECTS];
        ObjectEntryIndex _pathToQueueSurfaceMap[MAX_PATH_OBJECTS];
        ObjectEntryIndex _pathToRailingsMap[MAX_PATH_OBJECTS];

        void ThrowIfIncompatibleVersion()
        {
            const auto& header = _os->GetHeader();
            if (/*header.TargetVersion < PARK_FILE_MIN_SUPPORTED_VERSION || */ header.MinVersion > PARK_FILE_CURRENT_VERSION)
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
            return targetVersion > PARK_FILE_CURRENT_VERSION;
        }

        void Load(const std::string_view path)
        {
            FileStream fs(path, FILE_MODE_OPEN);
            Load(fs);
        }

        void Load(IStream& stream)
        {
            _os = std::make_unique<OrcaStream>(stream, OrcaStream::Mode::READING);
            ThrowIfIncompatibleVersion();

            RequiredObjects = {};
            ReadWriteObjectsChunk(*_os);
            ReadWritePackedObjectsChunk(*_os);
        }

        void Import()
        {
            auto& os = *_os;
            ReadWriteTilesChunk(os);
            ReadWriteBannersChunk(os);
            ReadWriteRidesChunk(os);
            ReadWriteEntitiesChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);
            ReadWriteCheatsChunk(os);
            ReadWriteRestrictedObjectsChunk(os);
            ReadWritePluginStorageChunk(os);
            if (os.GetHeader().TargetVersion < 0x4)
            {
                UpdateTrackElementsRideType();
            }

            // Initial cash will eventually be removed
            gInitialCash = gCash;
        }

        void Save(IStream& stream)
        {
            OrcaStream os(stream, OrcaStream::Mode::WRITING);

            auto& header = os.GetHeader();
            header.Magic = PARK_FILE_MAGIC;
            header.TargetVersion = PARK_FILE_CURRENT_VERSION;
            header.MinVersion = PARK_FILE_MIN_VERSION;

            ReadWriteAuthoringChunk(os);
            ReadWriteObjectsChunk(os);
            ReadWriteTilesChunk(os);
            ReadWriteBannersChunk(os);
            ReadWriteRidesChunk(os);
            ReadWriteEntitiesChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);
            ReadWriteCheatsChunk(os);
            ReadWriteRestrictedObjectsChunk(os);
            ReadWritePluginStorageChunk(os);
            ReadWritePackedObjectsChunk(os);
        }

        void Save(const std::string_view path)
        {
            FileStream fs(path, FILE_MODE_WRITE);
            Save(fs);
        }

        scenario_index_entry ReadScenarioChunk()
        {
            scenario_index_entry entry{};
            auto& os = *_os;
            os.ReadWriteChunk(ParkFileChunkType::SCENARIO, [&entry](OrcaStream::ChunkStream& cs) {
                entry.category = cs.Read<uint8_t>();

                std::string name;
                ReadWriteStringTable(cs, name, "en-GB");
                String::Set(entry.name, sizeof(entry.name), name.c_str());
                String::Set(entry.internal_name, sizeof(entry.internal_name), name.c_str());

                std::string parkName;
                ReadWriteStringTable(cs, parkName, "en-GB");

                std::string scenarioDetails;
                ReadWriteStringTable(cs, scenarioDetails, "en-GB");
                String::Set(entry.details, sizeof(entry.details), scenarioDetails.c_str());

                entry.objective_type = cs.Read<uint8_t>();
                entry.objective_arg_1 = cs.Read<uint8_t>();
                entry.objective_arg_3 = cs.Read<int16_t>();
                entry.objective_arg_2 = cs.Read<int32_t>();

                entry.source_game = ScenarioSource::Other;
            });
            return entry;
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
                std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
                std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
                std::fill(std::begin(_pathToRailingsMap), std::end(_pathToRailingsMap), OBJECT_ENTRY_INDEX_NULL);
                auto* pathToSurfaceMap = _pathToSurfaceMap;
                auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
                auto* pathToRailingsMap = _pathToRailingsMap;
                const auto version = os.GetHeader().TargetVersion;

                ObjectList requiredObjects;
                os.ReadWriteChunk(
                    ParkFileChunkType::OBJECTS,
                    [&requiredObjects, pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap,
                     version](OrcaStream::ChunkStream& cs) {
                        ObjectEntryIndex surfaceCount = 0;
                        ObjectEntryIndex railingsCount = 0;
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
                                        rct_object_entry datEntry;
                                        cs.Read(&datEntry, sizeof(datEntry));
                                        ObjectEntryDescriptor desc(datEntry);
                                        if (version <= 2 && datEntry.GetType() == ObjectType::Paths)
                                        {
                                            auto footpathMapping = GetFootpathMapping(desc);
                                            if (footpathMapping != nullptr)
                                            {
                                                UpdateFootpathsFromMapping(
                                                    pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap, requiredObjects,
                                                    surfaceCount, railingsCount, j, footpathMapping);

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
                                        desc.Identifier = identifier;
                                        desc.Version = cs.Read<std::string>();

                                        if (version <= 2)
                                        {
                                            auto footpathMapping = GetFootpathMapping(desc);
                                            if (footpathMapping != nullptr)
                                            {
                                                // We have surface objects for this footpath
                                                UpdateFootpathsFromMapping(
                                                    pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap, requiredObjects,
                                                    surfaceCount, railingsCount, j, footpathMapping);

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
                RequiredObjects = std::move(requiredObjects);
            }
            else
            {
                os.ReadWriteChunk(ParkFileChunkType::OBJECTS, [](OrcaStream::ChunkStream& cs) {
                    auto& objManager = GetContext()->GetObjectManager();
                    auto objectList = objManager.GetLoadedObjects();

                    // Write number of object sub lists
                    cs.Write(static_cast<uint16_t>(TransientObjectTypes.size()));
                    for (auto objectType : TransientObjectTypes)
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
                                    cs.Write(""); // reserved for version
                                }
                                else
                                {
                                    cs.Write(DESCRIPTOR_DAT);
                                    cs.Write(&entry.Entry, sizeof(rct_object_entry));
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

        void ReadWriteScenarioChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::SCENARIO, [&os](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gScenarioCategory);
                ReadWriteStringTable(cs, gScenarioName, "en-GB");

                auto& park = GetContext()->GetGameState()->GetPark();
                ReadWriteStringTable(cs, park.Name, "en-GB");

                ReadWriteStringTable(cs, gScenarioDetails, "en-GB");

                cs.ReadWrite(gScenarioObjective.Type);
                cs.ReadWrite(gScenarioObjective.Year);
                cs.ReadWrite(gScenarioObjective.NumGuests);
                cs.ReadWrite(gScenarioObjective.Currency);

                cs.ReadWrite(gScenarioParkRatingWarningDays);

                cs.ReadWrite(gScenarioCompletedCompanyValue);
                if (gScenarioCompletedCompanyValue == MONEY64_UNDEFINED
                    || gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
                {
                    cs.Write("");
                }
                else
                {
                    cs.ReadWrite(gScenarioCompletedBy);
                }

                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto earlyCompletion = cs.Read<bool>();
                    if (network_get_mode() == NETWORK_MODE_CLIENT)
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
                    cs.ReadWrite(gScenarioFileName);
                }
            });
        }

        void ReadWriteGeneralChunk(OrcaStream& os)
        {
            auto found = os.ReadWriteChunk(ParkFileChunkType::GENERAL, [this, &os](OrcaStream::ChunkStream& cs) {
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
                cs.ReadWrite(gCurrentTicks);
                cs.ReadWrite(gDateMonthTicks);
                cs.ReadWrite(gDateMonthsElapsed);

                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    uint32_t s0{}, s1{};
                    cs.ReadWrite(s0);
                    cs.ReadWrite(s1);
                    Random::Rct2::Seed s{ s0, s1 };
                    gScenarioRand.seed(s);
                }
                else
                {
                    auto randState = gScenarioRand.state();
                    cs.Write(randState.s0);
                    cs.Write(randState.s1);
                }

                cs.ReadWrite(gGuestInitialHappiness);
                cs.ReadWrite(gGuestInitialCash);
                cs.ReadWrite(gGuestInitialHunger);
                cs.ReadWrite(gGuestInitialThirst);

                cs.ReadWrite(gNextGuestNumber);
                cs.ReadWriteVector(gPeepSpawns, [&cs](PeepSpawn& spawn) {
                    cs.ReadWrite(spawn.x);
                    cs.ReadWrite(spawn.y);
                    cs.ReadWrite(spawn.z);
                    cs.ReadWrite(spawn.direction);
                });

                cs.ReadWrite(gLandPrice);
                cs.ReadWrite(gConstructionRightsPrice);
                cs.ReadWrite(gGrassSceneryTileLoopPosition);
                cs.ReadWrite(gWidePathTileLoopPosition);

                ReadWriteRideRatingCalculationData(cs, gRideRatingUpdateState);

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

        void ReadWriteInterfaceChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::INTERFACE, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gSavedView.x);
                cs.ReadWrite(gSavedView.y);
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto savedZoomlevel = static_cast<ZoomLevel>(cs.Read<int8_t>());
                    gSavedViewZoom = std::clamp(savedZoomlevel, ZoomLevel::min(), ZoomLevel::max());
                }
                else
                {
                    cs.Write(static_cast<int8_t>(gSavedViewZoom));
                }
                cs.ReadWrite(gSavedViewRotation);
                cs.ReadWrite(gLastEntranceStyle);
                cs.ReadWrite(gEditorStep);
            });
        }

        void ReadWriteCheatsChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CHEATS, [](OrcaStream::ChunkStream& cs) {
                DataSerialiser ds(cs.GetMode() == OrcaStream::Mode::WRITING, cs.GetStream());
                CheatsSerialise(ds);
            });
        }

        void ReadWriteRestrictedObjectsChunk(OrcaStream& os)
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

        void ReadWritePluginStorageChunk(OrcaStream& os)
        {
            auto& park = GetContext()->GetGameState()->GetPark();
            if (os.GetMode() == OrcaStream::Mode::WRITING)
            {
#ifdef ENABLE_SCRIPTING
                // Dump the plugin storage to JSON (stored in park)
                auto& scriptEngine = GetContext()->GetScriptEngine();
                park.PluginStorage = scriptEngine.GetParkStorageAsJSON();
#endif
                if (park.PluginStorage.empty() || park.PluginStorage == "{}")
                {
                    // Don't write the chunk if there is no plugin storage
                    return;
                }
            }

            os.ReadWriteChunk(
                ParkFileChunkType::PLUGIN_STORAGE, [&park](OrcaStream::ChunkStream& cs) { cs.ReadWrite(park.PluginStorage); });

            if (os.GetMode() == OrcaStream::Mode::READING)
            {
#ifdef ENABLE_SCRIPTING
                auto& scriptEngine = GetContext()->GetScriptEngine();
                scriptEngine.SetParkStorageFromJSON(park.PluginStorage);
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
                            rct_object_entry entry;
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
                        if (String::Equals(extension, ".dat", true))
                        {
                            cs.Write(DESCRIPTOR_DAT);
                            cs.Write(&ori->ObjectEntry, sizeof(rct_object_entry));
                        }
                        else if (String::Equals(extension, ".parkobj", true))
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

        void ReadWriteClimateChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CLIMATE, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gClimate);
                cs.ReadWrite(gClimateUpdateTimer);

                for (auto* cl : { &gClimateCurrent, &gClimateNext })
                {
                    cs.ReadWrite(cl->Weather);
                    cs.ReadWrite(cl->Temperature);
                    cs.ReadWrite(cl->WeatherEffect);
                    cs.ReadWrite(cl->WeatherGloom);
                    cs.ReadWrite(cl->Level);
                }
            });
        }

        void ReadWriteParkChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::PARK, [version = os.GetHeader().TargetVersion](OrcaStream::ChunkStream& cs) {
                auto& park = GetContext()->GetGameState()->GetPark();
                cs.ReadWrite(park.Name);
                cs.ReadWrite(gCash);
                cs.ReadWrite(gBankLoan);
                cs.ReadWrite(gMaxBankLoan);
                cs.ReadWrite(gBankLoanInterestRate);
                cs.ReadWrite(gParkFlags);
                cs.ReadWrite(gParkEntranceFee);
                cs.ReadWrite(gStaffHandymanColour);
                cs.ReadWrite(gStaffMechanicColour);
                cs.ReadWrite(gStaffSecurityColour);
                cs.ReadWrite(gSamePriceThroughoutPark);

                // Finances
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    auto numMonths = std::min<uint32_t>(EXPENDITURE_TABLE_MONTH_COUNT, cs.Read<uint32_t>());
                    auto numTypes = std::min<uint32_t>(static_cast<uint32_t>(ExpenditureType::Count), cs.Read<uint32_t>());
                    for (uint32_t i = 0; i < numMonths; i++)
                    {
                        for (uint32_t j = 0; j < numTypes; j++)
                        {
                            gExpenditureTable[i][j] = cs.Read<money64>();
                        }
                    }
                }
                else
                {
                    auto numMonths = static_cast<uint32_t>(EXPENDITURE_TABLE_MONTH_COUNT);
                    auto numTypes = static_cast<uint32_t>(ExpenditureType::Count);

                    cs.Write(numMonths);
                    cs.Write(numTypes);
                    for (uint32_t i = 0; i < numMonths; i++)
                    {
                        for (uint32_t j = 0; j < numTypes; j++)
                        {
                            cs.Write(gExpenditureTable[i][j]);
                        }
                    }
                }
                cs.ReadWrite(gHistoricalProfit);

                // Marketing
                cs.ReadWriteVector(gMarketingCampaigns, [&cs](MarketingCampaign& campaign) {
                    cs.ReadWrite(campaign.Type);
                    cs.ReadWrite(campaign.WeeksLeft);
                    cs.ReadWrite(campaign.Flags);
                    cs.ReadWrite(campaign.RideId);
                });

                // Awards
                if (version <= 6)
                {
                    Award awards[RCT2::Limits::MaxAwards];
                    cs.ReadWriteArray(awards, [&cs](Award& award) {
                        if (award.Time != 0)
                        {
                            cs.ReadWrite(award.Time);
                            cs.ReadWrite(award.Type);
                            GetAwards().push_back(award);
                            return true;
                        }

                        return false;
                    });
                }
                else
                {
                    cs.ReadWriteVector(GetAwards(), [&cs](Award& award) {
                        cs.ReadWrite(award.Time);
                        cs.ReadWrite(award.Type);
                    });
                }
                cs.ReadWrite(gParkValue);
                cs.ReadWrite(gCompanyValue);
                cs.ReadWrite(gParkSize);
                cs.ReadWrite(gNumGuestsInPark);
                cs.ReadWrite(gNumGuestsHeadingForPark);
                cs.ReadWrite(gParkRating);
                cs.ReadWrite(gParkRatingCasualtyPenalty);
                cs.ReadWrite(gCurrentExpenditure);
                cs.ReadWrite(gCurrentProfit);
                cs.ReadWrite(gWeeklyProfitAverageDividend);
                cs.ReadWrite(gWeeklyProfitAverageDivisor);
                cs.ReadWrite(gTotalAdmissions);
                cs.ReadWrite(gTotalIncomeFromAdmissions);
                cs.ReadWrite(gTotalRideValueForMoney);
                cs.ReadWrite(gNumGuestsInParkLastWeek);
                cs.ReadWrite(gGuestChangeModifier);
                cs.ReadWrite(_guestGenerationProbability);
                cs.ReadWrite(_suggestedGuestMaximum);

                cs.ReadWriteArray(gPeepWarningThrottle, [&cs](uint8_t& value) {
                    cs.ReadWrite(value);
                    return true;
                });

                cs.ReadWriteArray(gParkRatingHistory, [&cs](uint8_t& value) {
                    cs.ReadWrite(value);
                    return true;
                });

                cs.ReadWriteArray(gGuestsInParkHistory, [&cs](uint32_t& value) {
                    cs.ReadWrite(value);
                    return true;
                });

                cs.ReadWriteArray(gCashHistory, [&cs](money64& value) {
                    cs.ReadWrite(value);
                    return true;
                });
                cs.ReadWriteArray(gWeeklyProfitHistory, [&cs](money64& value) {
                    cs.ReadWrite(value);
                    return true;
                });
                cs.ReadWriteArray(gParkValueHistory, [&cs](money64& value) {
                    cs.ReadWrite(value);
                    return true;
                });
            });
        }

        void ReadWriteResearchChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::RESEARCH, [](OrcaStream::ChunkStream& cs) {
                // Research status
                cs.ReadWrite(gResearchFundingLevel);
                cs.ReadWrite(gResearchPriorities);
                cs.ReadWrite(gResearchProgressStage);
                cs.ReadWrite(gResearchProgress);
                cs.ReadWrite(gResearchExpectedMonth);
                cs.ReadWrite(gResearchExpectedDay);
                ReadWriteResearchItem(cs, gResearchLastItem);
                ReadWriteResearchItem(cs, gResearchNextItem);

                // Invention list
                cs.ReadWriteVector(gResearchItemsUninvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
                cs.ReadWriteVector(gResearchItemsInvented, [&cs](ResearchItem& item) { ReadWriteResearchItem(cs, item); });
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

        void ReadWriteNotificationsChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::NOTIFICATIONS, [](OrcaStream::ChunkStream& cs) {
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    gNewsItems.Clear();

                    std::vector<News::Item> recent;
                    cs.ReadWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });
                    for (size_t i = 0; i < std::min<size_t>(recent.size(), News::ItemHistoryStart); i++)
                    {
                        gNewsItems[i] = recent[i];
                    }

                    std::vector<News::Item> archived;
                    cs.ReadWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });
                    size_t offset = News::ItemHistoryStart;
                    for (size_t i = 0; i < std::min<size_t>(archived.size(), News::MaxItemsArchive); i++)
                    {
                        gNewsItems[offset + i] = archived[i];
                    }

                    // Still need to set the correct type to properly terminate the queue
                    if (archived.size() < News::MaxItemsArchive)
                        gNewsItems[offset + archived.size()].Type = News::ItemType::Null;
                }
                else
                {
                    std::vector<News::Item> recent(std::begin(gNewsItems.GetRecent()), std::end(gNewsItems.GetRecent()));
                    cs.ReadWriteVector(recent, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });

                    std::vector<News::Item> archived(std::begin(gNewsItems.GetArchived()), std::end(gNewsItems.GetArchived()));
                    cs.ReadWriteVector(archived, [&cs](News::Item& item) { ReadWriteNewsItem(cs, item); });
                }
            });
        }

        static void ReadWriteNewsItem(OrcaStream::ChunkStream& cs, News::Item& item)
        {
            cs.ReadWrite(item.Type);
            cs.ReadWrite(item.Flags);
            cs.ReadWrite(item.Assoc);
            cs.ReadWrite(item.Ticks);
            cs.ReadWrite(item.MonthYear);
            cs.ReadWrite(item.Day);
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                auto s = cs.Read<std::string>();
                item.Text = s;
            }
            else
            {
                cs.Write(std::string_view(item.Text));
            }
        }

        void ReadWriteTilesChunk(OrcaStream& os)
        {
            auto* pathToSurfaceMap = _pathToSurfaceMap;
            auto* pathToQueueSurfaceMap = _pathToQueueSurfaceMap;
            auto* pathToRailingsMap = _pathToRailingsMap;

            auto found = os.ReadWriteChunk(
                ParkFileChunkType::TILES,
                [pathToSurfaceMap, pathToQueueSurfaceMap, pathToRailingsMap](OrcaStream::ChunkStream& cs) {
                    cs.ReadWrite(gMapSize.x);
                    cs.ReadWrite(gMapSize.y);

                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        OpenRCT2::GetContext()->GetGameState()->InitAll(gMapSize);

                        auto numElements = cs.Read<uint32_t>();

                        std::vector<TileElement> tileElements;
                        tileElements.resize(numElements);
                        cs.Read(tileElements.data(), tileElements.size() * sizeof(TileElement));
                        SetTileElements(std::move(tileElements));
                        {
                            tile_element_iterator it;
                            tile_element_iterator_begin(&it);
                            while (tile_element_iterator_next(&it))
                            {
                                if (it.element->GetType() == TileElementType::Path)
                                {
                                    auto* pathElement = it.element->AsPath();
                                    if (pathElement->HasLegacyPathEntry())
                                    {
                                        auto pathEntryIndex = pathElement->GetLegacyPathEntryIndex();
                                        if (pathToRailingsMap[pathEntryIndex] != OBJECT_ENTRY_INDEX_NULL)
                                        {
                                            if (pathElement->IsQueue())
                                                pathElement->SetSurfaceEntryIndex(pathToQueueSurfaceMap[pathEntryIndex]);
                                            else
                                                pathElement->SetSurfaceEntryIndex(pathToSurfaceMap[pathEntryIndex]);

                                            pathElement->SetRailingsEntryIndex(pathToRailingsMap[pathEntryIndex]);
                                        }
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
            for (int32_t y = 0; y < gMapSize.y; y++)
            {
                for (int32_t x = 0; x < gMapSize.x; x++)
                {
                    TileElement* tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });
                    if (tileElement == nullptr)
                        continue;
                    do
                    {
                        if (tileElement->GetType() != TileElementType::Track)
                            continue;

                        auto* trackElement = tileElement->AsTrack();
                        const auto* ride = get_ride(trackElement->GetRideIndex());
                        if (ride != nullptr)
                        {
                            trackElement->SetRideType(ride->type);
                        }

                    } while (!(tileElement++)->IsLastForTile());
                }
            }
        }

        void ReadWriteBannersChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::BANNERS, [&os](OrcaStream::ChunkStream& cs) {
                auto version = os.GetHeader().TargetVersion;
                if (cs.GetMode() == OrcaStream::Mode::WRITING)
                {
                    auto numBanners = GetNumBanners();
                    cs.Write(static_cast<uint32_t>(numBanners));

                    [[maybe_unused]] size_t numWritten = 0;
                    for (BannerIndex::UnderlyingType i = 0; i < MAX_BANNERS; i++)
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
            cs.ReadWrite(banner.flags);
            cs.ReadWrite(banner.text);
            cs.ReadWrite(banner.colour);
            cs.ReadWrite(banner.ride_index);
            cs.ReadWrite(banner.text_colour);
            cs.ReadWrite(banner.position.x);
            cs.ReadWrite(banner.position.y);
        }

        void ReadWriteRidesChunk(OrcaStream& os)
        {
            const auto version = os.GetHeader().TargetVersion;
            os.ReadWriteChunk(ParkFileChunkType::RIDES, [this, &version](OrcaStream::ChunkStream& cs) {
                std::vector<RideId> rideIds;
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    ride_init_all();
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
                cs.ReadWriteVector(rideIds, [&cs, &version](RideId& rideId) {
                    // Ride ID
                    cs.ReadWrite(rideId);

                    auto& ride = *GetOrAllocateRide(rideId);

                    // Status
                    cs.ReadWrite(ride.type);
                    cs.ReadWrite(ride.subtype);
                    cs.ReadWrite(ride.mode);
                    cs.ReadWrite(ride.status);
                    cs.ReadWrite(ride.depart_flags);
                    cs.ReadWrite(ride.lifecycle_flags);

                    // Meta
                    cs.ReadWrite(ride.custom_name);
                    cs.ReadWrite(ride.default_name_number);

                    cs.ReadWriteArray(ride.price, [&cs](money16& price) {
                        cs.ReadWrite(price);
                        return true;
                    });

                    // Colours
                    cs.ReadWrite(ride.entrance_style);
                    cs.ReadWrite(ride.colour_scheme_type);
                    cs.ReadWriteArray(ride.track_colour, [&cs](TrackColour& tc) {
                        cs.ReadWrite(tc.main);
                        cs.ReadWrite(tc.additional);
                        cs.ReadWrite(tc.supports);
                        return true;
                    });

                    cs.ReadWriteArray(ride.vehicle_colours, [&cs](VehicleColour& vc) {
                        cs.ReadWrite(vc.Body);
                        cs.ReadWrite(vc.Trim);
                        cs.ReadWrite(vc.Tertiary);
                        return true;
                    });

                    // Stations
                    cs.ReadWrite(ride.num_stations);
                    cs.ReadWriteArray(ride.GetStations(), [&cs](RideStation& station) {
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

                    cs.ReadWrite(ride.overall_view.x);
                    cs.ReadWrite(ride.overall_view.y);

                    // Vehicles
                    cs.ReadWrite(ride.NumTrains);
                    cs.ReadWrite(ride.num_cars_per_train);
                    cs.ReadWrite(ride.ProposedNumTrains);
                    cs.ReadWrite(ride.proposed_num_cars_per_train);
                    cs.ReadWrite(ride.max_trains);
                    if (version < 0x5)
                    {
                        uint8_t value;
                        cs.ReadWrite(value);
                        ride.MinCarsPerTrain = GetMinCarsPerTrain(value);
                        ride.MaxCarsPerTrain = GetMaxCarsPerTrain(value);
                    }
                    else
                    {
                        cs.ReadWrite(ride.MinCarsPerTrain);
                        cs.ReadWrite(ride.MaxCarsPerTrain);
                    }

                    cs.ReadWrite(ride.min_waiting_time);
                    cs.ReadWrite(ride.max_waiting_time);
                    cs.ReadWriteArray(ride.vehicles, [&cs](EntityId& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    // Operation
                    cs.ReadWrite(ride.operation_option);
                    cs.ReadWrite(ride.lift_hill_speed);
                    cs.ReadWrite(ride.num_circuits);

                    // Special
                    cs.ReadWrite(ride.boat_hire_return_direction);
                    cs.ReadWrite(ride.boat_hire_return_position);
                    cs.ReadWrite(ride.ChairliftBullwheelLocation[0]);
                    cs.ReadWrite(ride.ChairliftBullwheelLocation[1]);
                    cs.ReadWrite(ride.chairlift_bullwheel_rotation);
                    cs.ReadWrite(ride.slide_in_use);
                    cs.ReadWrite(ride.slide_peep);
                    cs.ReadWrite(ride.slide_peep_t_shirt_colour);
                    cs.ReadWrite(ride.spiral_slide_progress);
                    cs.ReadWrite(ride.race_winner);
                    cs.ReadWrite(ride.cable_lift);
                    cs.ReadWrite(ride.CableLiftLoc);

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

                    cs.ReadWrite(ride.special_track_elements);
                    cs.ReadWrite(ride.max_speed);
                    cs.ReadWrite(ride.average_speed);
                    cs.ReadWrite(ride.current_test_segment);
                    cs.ReadWrite(ride.average_speed_test_timeout);

                    cs.ReadWrite(ride.max_positive_vertical_g);
                    cs.ReadWrite(ride.max_negative_vertical_g);
                    cs.ReadWrite(ride.max_lateral_g);
                    cs.ReadWrite(ride.previous_vertical_g);
                    cs.ReadWrite(ride.previous_lateral_g);

                    cs.ReadWrite(ride.testing_flags);
                    cs.ReadWrite(ride.CurTestTrackLocation);

                    cs.ReadWrite(ride.turn_count_default);
                    cs.ReadWrite(ride.turn_count_banked);
                    cs.ReadWrite(ride.turn_count_sloped);

                    cs.ReadWrite(ride.inversions);
                    cs.ReadWrite(ride.drops);
                    cs.ReadWrite(ride.start_drop_height);
                    cs.ReadWrite(ride.highest_drop_height);
                    cs.ReadWrite(ride.sheltered_length);
                    cs.ReadWrite(ride.var_11C);
                    cs.ReadWrite(ride.num_sheltered_sections);
                    if (version > 5)
                    {
                        cs.ReadWrite(ride.sheltered_eighths);
                        cs.ReadWrite(ride.holes);
                    }
                    cs.ReadWrite(ride.current_test_station);
                    cs.ReadWrite(ride.num_block_brakes);
                    cs.ReadWrite(ride.total_air_time);

                    cs.ReadWrite(ride.excitement);
                    cs.ReadWrite(ride.intensity);
                    cs.ReadWrite(ride.nausea);

                    cs.ReadWrite(ride.value);

                    cs.ReadWrite(ride.num_riders);
                    cs.ReadWrite(ride.build_date);
                    cs.ReadWrite(ride.upkeep_cost);

                    cs.ReadWrite(ride.cur_num_customers);
                    cs.ReadWrite(ride.num_customers_timeout);

                    cs.ReadWriteArray(ride.num_customers, [&cs](uint16_t& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    cs.ReadWrite(ride.total_customers);
                    cs.ReadWrite(ride.total_profit);
                    cs.ReadWrite(ride.popularity);
                    cs.ReadWrite(ride.popularity_time_out);
                    cs.ReadWrite(ride.popularity_next);
                    cs.ReadWrite(ride.guests_favourite);
                    cs.ReadWrite(ride.no_primary_items_sold);
                    cs.ReadWrite(ride.no_secondary_items_sold);
                    cs.ReadWrite(ride.income_per_hour);
                    cs.ReadWrite(ride.profit);
                    cs.ReadWrite(ride.satisfaction);
                    cs.ReadWrite(ride.satisfaction_time_out);
                    cs.ReadWrite(ride.satisfaction_next);

                    // Breakdown
                    cs.ReadWrite(ride.breakdown_reason_pending);
                    cs.ReadWrite(ride.mechanic_status);
                    cs.ReadWrite(ride.mechanic);
                    cs.ReadWrite(ride.inspection_station);
                    cs.ReadWrite(ride.broken_vehicle);
                    cs.ReadWrite(ride.broken_car);
                    cs.ReadWrite(ride.breakdown_reason);
                    cs.ReadWrite(ride.reliability_subvalue);
                    cs.ReadWrite(ride.reliability_percentage);
                    cs.ReadWrite(ride.unreliability_factor);
                    cs.ReadWrite(ride.downtime);
                    cs.ReadWrite(ride.inspection_interval);
                    cs.ReadWrite(ride.last_inspection);

                    cs.ReadWriteArray(ride.downtime_history, [&cs](uint8_t& v) {
                        cs.ReadWrite(v);
                        return true;
                    });

                    cs.ReadWrite(ride.breakdown_sound_modifier);
                    cs.ReadWrite(ride.not_fixed_timeout);
                    cs.ReadWrite(ride.last_crash_type);
                    cs.ReadWrite(ride.connected_message_throttle);

                    cs.ReadWrite(ride.vehicle_change_timeout);

                    cs.ReadWrite(ride.current_issues);
                    cs.ReadWrite(ride.last_issue_time);

                    // Music
                    cs.ReadWrite(ride.music);
                    cs.ReadWrite(ride.music_tune_id);
                    cs.ReadWrite(ride.music_position);
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

        template<typename T> static void ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, T& entity);

        static void ReadWriteEntityCommon(OrcaStream::ChunkStream& cs, EntityBase& entity)
        {
            cs.ReadWrite(entity.sprite_index);
            cs.ReadWrite(entity.sprite_height_negative);
            cs.ReadWrite(entity.x);
            cs.ReadWrite(entity.y);
            cs.ReadWrite(entity.z);
            cs.ReadWrite(entity.sprite_width);
            cs.ReadWrite(entity.sprite_height_positive);
            cs.ReadWrite(entity.sprite_direction);
        }

        static std::vector<ObjectEntryIndex> LegacyGetRideTypesBeenOn(const std::array<uint8_t, 16>& srcArray)
        {
            std::vector<ObjectEntryIndex> ridesTypesBeenOn;
            for (ObjectEntryIndex i = 0; i < RCT2::Limits::MaxRideObjects; i++)
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
            for (uint16_t i = 0; i < RCT2::Limits::MaxRidesInPark; i++)
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
            cs.ReadWrite(entity.SpriteType);

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
                    cs.ReadWrite(guest->PaidOnDrink);
                    std::array<uint8_t, 16> rideTypeBeenOn;
                    cs.ReadWriteArray(rideTypeBeenOn, [&cs](uint8_t& rideType) {
                        cs.ReadWrite(rideType);
                        return true;
                    });
                    OpenRCT2::RideUse::GetTypeHistory().Set(guest->sprite_index, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
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
            cs.ReadWrite(entity.ActionSpriteType);
            cs.ReadWrite(entity.NextActionSpriteType);
            cs.ReadWrite(entity.ActionSpriteImageOffset);
            cs.ReadWrite(entity.Action);
            cs.ReadWrite(entity.ActionFrame);
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
                    OpenRCT2::RideUse::GetHistory().Set(guest->sprite_index, LegacyGetRidesBeenOn(ridesBeenOn));
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

            cs.ReadWrite(entity.Id);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->CashInPocket);
                    cs.ReadWrite(guest->CashSpent);
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
                            thought.item = PeepThoughtItemNone;
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
            cs.ReadWrite(entity.WalkingFrameNum);

            if (version <= 1)
            {
                if (guest != nullptr)
                {
                    cs.ReadWrite(guest->LitterCount);
                    cs.ReadWrite(guest->GuestTimeOnRide);
                    cs.ReadWrite(guest->DisgustingCount);
                    cs.ReadWrite(guest->PaidToEnter);
                    cs.ReadWrite(guest->PaidOnRides);
                    cs.ReadWrite(guest->PaidOnFood);
                    cs.ReadWrite(guest->PaidOnSouvenirs);
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

        template<typename T> void WriteEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs);
        template<typename... T> void WriteEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename T> void ReadEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs);

        template<typename... T> void ReadEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs);

        void ReadWriteEntitiesChunk(OrcaStream& os);

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

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Vehicle& entity)
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
        cs.ReadWrite(entity.update_flags);
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
        cs.ReadWrite(entity.brake_speed);
        cs.ReadWrite(entity.lost_time_out);
        cs.ReadWrite(entity.vertical_drop_countdown);
        cs.ReadWrite(entity.var_D3);
        cs.ReadWrite(entity.mini_golf_current_animation);
        cs.ReadWrite(entity.mini_golf_flags);
        cs.ReadWrite(entity.ride_subtype);
        cs.ReadWrite(entity.colours.Tertiary);
        cs.ReadWrite(entity.seat_rotation);
        cs.ReadWrite(entity.target_seat_rotation);
        cs.ReadWrite(entity.IsCrashedVehicle);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Guest& guest)
    {
        ReadWritePeep(os, cs, guest);

        if (os.GetHeader().TargetVersion <= 1)
        {
            return;
        }

        cs.ReadWrite(guest.GuestNumRides);
        cs.ReadWrite(guest.GuestNextInQueue);
        cs.ReadWrite(guest.ParkEntryTime);
        cs.ReadWrite(guest.GuestHeadingToRideId);
        cs.ReadWrite(guest.GuestIsLostCountdown);
        cs.ReadWrite(guest.GuestTimeOnRide);
        cs.ReadWrite(guest.PaidToEnter);
        cs.ReadWrite(guest.PaidOnRides);
        cs.ReadWrite(guest.PaidOnFood);
        cs.ReadWrite(guest.PaidOnDrink);
        cs.ReadWrite(guest.PaidOnSouvenirs);
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
            OpenRCT2::RideUse::GetTypeHistory().Set(guest.sprite_index, LegacyGetRideTypesBeenOn(rideTypeBeenOn));
        }

        cs.ReadWrite(guest.TimeInQueue);
        if (os.GetHeader().TargetVersion < 3)
        {
            std::array<uint8_t, 32> ridesBeenOn;
            cs.ReadWriteArray(ridesBeenOn, [&cs](uint8_t& rideType) {
                cs.ReadWrite(rideType);
                return true;
            });
            OpenRCT2::RideUse::GetHistory().Set(guest.sprite_index, LegacyGetRidesBeenOn(ridesBeenOn));
        }
        else
        {
            if (cs.GetMode() == OrcaStream::Mode::READING)
            {
                std::vector<RideId> rideUse;
                cs.ReadWriteVector(rideUse, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                OpenRCT2::RideUse::GetHistory().Set(guest.sprite_index, std::move(rideUse));
                std::vector<ObjectEntryIndex> rideTypeUse;
                cs.ReadWriteVector(rideTypeUse, [&cs](ObjectEntryIndex& rideType) { cs.ReadWrite(rideType); });
                OpenRCT2::RideUse::GetTypeHistory().Set(guest.sprite_index, std::move(rideTypeUse));
            }
            else
            {
                auto* rideUse = OpenRCT2::RideUse::GetHistory().GetAll(guest.sprite_index);
                if (rideUse == nullptr)
                {
                    std::vector<RideId> empty;
                    cs.ReadWriteVector(empty, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                }
                else
                {
                    cs.ReadWriteVector(*rideUse, [&cs](RideId& rideId) { cs.ReadWrite(rideId); });
                }
                auto* rideTypeUse = OpenRCT2::RideUse::GetTypeHistory().GetAll(guest.sprite_index);
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
        cs.ReadWrite(guest.CashInPocket);
        cs.ReadWrite(guest.CashSpent);
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

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Staff& entity)
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

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, SteamParticle& steamParticle)
    {
        ReadWriteEntityCommon(cs, steamParticle);
        cs.ReadWrite(steamParticle.time_to_move);
        cs.ReadWrite(steamParticle.frame);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, MoneyEffect& moneyEffect)
    {
        ReadWriteEntityCommon(cs, moneyEffect);
        cs.ReadWrite(moneyEffect.MoveDelay);
        cs.ReadWrite(moneyEffect.NumMovements);
        cs.ReadWrite(moneyEffect.Vertical);
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

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionCloud& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, CrashSplashParticle& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, ExplosionFlare& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.frame);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, JumpingFountain& fountain)
    {
        ReadWriteEntityCommon(cs, fountain);
        cs.ReadWrite(fountain.NumTicksAlive);
        cs.ReadWrite(fountain.frame);
        cs.ReadWrite(fountain.FountainFlags);
        cs.ReadWrite(fountain.TargetX);
        cs.ReadWrite(fountain.TargetY);
        cs.ReadWrite(fountain.TargetY);
        cs.ReadWrite(fountain.Iteration);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Balloon& balloon)
    {
        ReadWriteEntityCommon(cs, balloon);
        cs.ReadWrite(balloon.popped);
        cs.ReadWrite(balloon.time_to_move);
        cs.ReadWrite(balloon.frame);
        cs.ReadWrite(balloon.colour);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Duck& duck)
    {
        ReadWriteEntityCommon(cs, duck);
        cs.ReadWrite(duck.frame);
        cs.ReadWrite(duck.target_x);
        cs.ReadWrite(duck.target_y);
        cs.ReadWrite(duck.state);
    }

    template<> void ParkFile::ReadWriteEntity(OrcaStream& os, OrcaStream::ChunkStream& cs, Litter& entity)
    {
        ReadWriteEntityCommon(cs, entity);
        cs.ReadWrite(entity.SubType);
        cs.ReadWrite(entity.creationTick);
    }

    template<typename T> void ParkFile::WriteEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        uint16_t count = GetEntityListCount(T::cEntityType);
        cs.Write(T::cEntityType);
        cs.Write(count);
        for (auto* ent : EntityList<T>())
        {
            cs.Write(ent->sprite_index);
            ReadWriteEntity(os, cs, *ent);
        }
    }

    template<typename... T> void ParkFile::WriteEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (WriteEntitiesOfType<T>(os, cs), ...);
    }

    template<typename T> void ParkFile::ReadEntitiesOfType(OrcaStream& os, OrcaStream::ChunkStream& cs)
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

    template<typename... T> void ParkFile::ReadEntitiesOfTypes(OrcaStream& os, OrcaStream::ChunkStream& cs)
    {
        (ReadEntitiesOfType<T>(os, cs), ...);
    }

    void ParkFile::ReadWriteEntitiesChunk(OrcaStream& os)
    {
        os.ReadWriteChunk(ParkFileChunkType::ENTITIES, [this, &os](OrcaStream::ChunkStream& cs) {
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
            }
            else
            {
                WriteEntitiesOfTypes<
                    Vehicle, Guest, Staff, Litter, SteamParticle, MoneyEffect, VehicleCrashParticle, ExplosionCloud,
                    CrashSplashParticle, ExplosionFlare, JumpingFountain, Balloon, Duck>(os, cs);
            }
        });
    }
} // namespace OpenRCT2

void ParkFileExporter::Export(std::string_view path)
{
    auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
    parkFile->Save(path);
}

void ParkFileExporter::Export(IStream& stream)
{
    auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
    parkFile->ExportObjectsList = ExportObjectsList;
    parkFile->Save(stream);
}

enum : uint32_t
{
    S6_SAVE_FLAG_EXPORT = 1 << 0,
    S6_SAVE_FLAG_SCENARIO = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

int32_t scenario_save(u8string_view path, int32_t flags)
{
    if (flags & S6_SAVE_FLAG_SCENARIO)
    {
        log_verbose("saving scenario");
    }
    else
    {
        log_verbose("saving game");
    }

    gIsAutosave = flags & S6_SAVE_FLAG_AUTOMATIC;
    if (!gIsAutosave)
    {
        window_close_construction_windows();
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
        parkFile->Save(path);
        result = true;
    }
    catch (const std::exception& e)
    {
        log_error(e.what());

        Formatter ft;
        ft.Add<const char*>(e.what());
        context_show_error(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_STRING, ft);
        gfx_invalidate_screen();

        auto ctx = OpenRCT2::GetContext();
        auto uictx = ctx->GetUiContext();

        std::string title = "Error while saving";
        std::string message
            = "There was an error while saving scenario.\nhttps://github.com/OpenRCT2/OpenRCT2/issues/17664\nWe would like to "
              "collect more information about this issue, if this did not happen due to missing permissions, lack of space, "
              "etc. please consider submitting a bug report. To collect information we would like to trigger an assert.";

        std::string report_bug_button = "Report bug, trigger an assert, potentially terminating the game";
        std::string skip_button = "Skip reporting, let me continue";

        std::vector<std::string> buttons{ std::move(report_bug_button), std::move(skip_button) };
        int choice = uictx->ShowMessageBox(title, message, buttons);

        if (choice == 0)
        {
            Guard::Assert(false, "Error while saving: %s", e.what());
        }
    }

    gfx_invalidate_screen();

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

    ParkLoadResult Load(const utf8* path) override
    {
        _parkFile = std::make_unique<OpenRCT2::ParkFile>();
        _parkFile->Load(path);

        auto result = ParkLoadResult(std::move(_parkFile->RequiredObjects));
        result.SemiCompatibleVersion = _parkFile->IsSemiCompatibleVersion(result.MinVersion, result.TargetVersion);
        return result;
    }

    ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) override
    {
        return Load(path);
    }

    ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) override
    {
        return Load(path);
    }

    ParkLoadResult LoadFromStream(
        OpenRCT2::IStream* stream, bool isScenario, bool skipObjectCheck = false, const utf8* path = String::Empty) override
    {
        _parkFile = std::make_unique<OpenRCT2::ParkFile>();
        _parkFile->Load(*stream);

        auto result = ParkLoadResult(std::move(_parkFile->RequiredObjects));
        result.SemiCompatibleVersion = _parkFile->IsSemiCompatibleVersion(result.MinVersion, result.TargetVersion);
        return result;
    }

    void Import() override
    {
        _parkFile->Import();
        research_determine_first_of_type();
        game_fix_save_vars();
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        *dst = _parkFile->ReadScenarioChunk();
        return true;
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateParkFile(IObjectRepository& objectRepository)
{
    return std::make_unique<ParkFileImporter>(objectRepository);
}
