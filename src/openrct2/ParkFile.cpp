/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Context.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "Version.h"
#include "core/Crypt.h"
#include "core/OrcaStream.hpp"
#include "drawing/Drawing.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "localisation/Date.h"
#include "localisation/Localisation.h"
#include "management/Award.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "object/Object.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "ride/ShopItem.h"
#include "scenario/Scenario.h"
#include "world/Climate.h"
#include "world/Entrance.h"
#include "world/Map.h"
#include "world/Park.h"

#include <cstdint>
#include <ctime>
#include <numeric>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    constexpr uint32_t PARK_FILE_MAGIC = 0x4B524150; // PARK

    // Current version that is saved.
    constexpr uint32_t PARK_FILE_CURRENT_VERSION = 0x0;

    // The minimum version that is forwards compatible with the current version.
    constexpr uint32_t PARK_FILE_MIN_VERSION = 0x0;

    namespace ParkFileChunkType
    {
        // clang-format off
        constexpr uint32_t RESERVED_0       = 0x00;

        constexpr uint32_t AUTHORING        = 0x01;
        constexpr uint32_t OBJECTS          = 0x02;
        constexpr uint32_t SCENARIO         = 0x03;
        constexpr uint32_t GENERAL          = 0x04;
        constexpr uint32_t CLIMATE          = 0x05;
        constexpr uint32_t PARK             = 0x06;
        constexpr uint32_t HISTORY          = 0x07;
        constexpr uint32_t RESEARCH         = 0x08;
        constexpr uint32_t NOTIFICATIONS    = 0x09;

        constexpr uint32_t INTERFACE        = 0x20;
        constexpr uint32_t EDITOR           = 0x21;

        constexpr uint32_t TILES            = 0x30;
        constexpr uint32_t THINGS           = 0x31;
        constexpr uint32_t RIDES            = 0x32;
        constexpr uint32_t BANNERS          = 0x33;
        constexpr uint32_t ANIMATIONS       = 0x34;
        constexpr uint32_t STAFF            = 0x35;
        constexpr uint32_t STRINGS          = 0x36;

        constexpr uint32_t DERIVED          = 0x50;
        // clang-format on
    }; // namespace ParkFileChunkType

    class ParkFile
    {
    public:
        std::vector<rct_object_entry> RequiredObjects;

    private:
        std::unique_ptr<OrcaStream> _blob;

    public:
        void Load(const std::string_view& path)
        {
            _blob = std::make_unique<OrcaStream>(path, OrcaStream::Mode::READING);
            RequiredObjects.clear();
            WriteObjectsChunk(*_blob);
        }

        void Import()
        {
            auto& blob = *_blob;
            WriteTilesChunk(blob);
            WriteScenarioChunk(blob);
            WriteGeneralChunk(blob);
            WriteParkChunk(blob);
            WriteClimateChunk(blob);
            WriteResearch(blob);
            WriteNotifications(blob);
            WriteInterfaceChunk(blob);

            // Initial cash will eventually be removed
            gInitialCash = gCash;
            String::Set(gS6Info.name, sizeof(gS6Info.name), gScenarioName.c_str());
            String::Set(gS6Info.details, sizeof(gS6Info.details), gScenarioName.c_str());
        }

        void Save(const std::string_view& path)
        {
            OrcaStream blob(path, OrcaStream::Mode::WRITING);

            auto& header = blob.GetHeader();
            header.Magic = PARK_FILE_MAGIC;
            header.TargetVersion = PARK_FILE_CURRENT_VERSION;
            header.MinVersion = PARK_FILE_MIN_VERSION;

            WriteAuthoringChunk(blob);
            WriteObjectsChunk(blob);
            WriteTilesChunk(blob);
            WriteScenarioChunk(blob);
            WriteGeneralChunk(blob);
            WriteParkChunk(blob);
            WriteClimateChunk(blob);
            WriteResearch(blob);
            WriteNotifications(blob);
            WriteInterfaceChunk(blob);
        }

    private:
        void WriteAuthoringChunk(OrcaStream& blob)
        {
            // Write-only for now
            if (blob.GetMode() == OrcaStream::Mode::WRITING)
            {
                blob.ReadWriteChunk(ParkFileChunkType::AUTHORING, [](OrcaStream::ChunkStream& b) {
                    b.Write(std::string_view(gVersionInfoFull));
                    std::vector<std::string> authors;
                    b.ReadWriteArray(authors, [](std::string& s) {});
                    b.Write(std::string_view());     // custom notes that can be attached to the save
                    b.Write<uint64_t>(std::time(0)); // date started
                    b.Write<uint64_t>(std::time(0)); // date modified
                });
            }
        }

        void WriteObjectsChunk(OrcaStream& blob)
        {
            if (blob.GetMode() == OrcaStream::Mode::READING)
            {
                std::vector<rct_object_entry> entries;
                blob.ReadWriteChunk(ParkFileChunkType::OBJECTS, [&entries](OrcaStream::ChunkStream& b) {
                    b.ReadWriteArray(entries, [&b](rct_object_entry& entry) {
                        auto type = b.Read<uint16_t>();
                        auto id = b.Read<std::string>();
                        auto version = b.Read<std::string>();

                        entry.flags = type & 0x7FFF;
                        strncpy(entry.name, id.c_str(), 8);
                    });
                });
                RequiredObjects = entries;
            }
            else
            {
                std::vector<size_t> objectIds(OBJECT_ENTRY_COUNT);
                std::iota(objectIds.begin(), objectIds.end(), 0);
                blob.ReadWriteChunk(ParkFileChunkType::OBJECTS, [&objectIds](OrcaStream::ChunkStream& b) {
                    auto& objManager = GetContext()->GetObjectManager();
                    b.ReadWriteArray(objectIds, [&b, &objManager](size_t& i) {
                        auto obj = objManager.GetLoadedObject(i);
                        if (obj != nullptr)
                        {
                            auto entry = obj->GetObjectEntry();
                            auto type = (uint16_t)(entry->flags & 0x0F);
                            type |= 0x8000; // Make as legacy object
                            b.Write(type);
                            b.Write(std::string_view(entry->name, 8));
                            b.Write("");
                        }
                        else
                        {
                            b.Write<uint16_t>(0);
                            b.Write("");
                            b.Write("");
                        }
                    });
                });
            }
        }

        void WriteScenarioChunk(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::SCENARIO, [](OrcaStream::ChunkStream& b) {
                b.ReadWriteAs<uint8_t, uint32_t>(gS6Info.category);
                ReadWriteStringTable(b, gScenarioName, "en-GB");

                std::string parkName(128, '\0');
                format_string(parkName.data(), parkName.size(), gParkName, &gParkNameArgs);
                ReadWriteStringTable(b, parkName, "en-GB");

                ReadWriteStringTable(b, gScenarioDetails, "en-GB");

                b.ReadWriteAs<uint8_t, uint32_t>(gScenarioObjectiveType);
                b.ReadWriteAs<uint8_t, uint16_t>(gScenarioObjectiveYear);       // year
                b.ReadWriteAs<uint16_t, uint32_t>(gScenarioObjectiveNumGuests); // guests
                b.Write<uint16_t>(600);                                         // rating
                b.ReadWriteAs<money32, uint16_t>(gScenarioObjectiveCurrency);   // excitement
                b.ReadWriteAs<uint16_t, uint32_t>(gScenarioObjectiveNumGuests); // length
                b.ReadWrite<money32>(gScenarioObjectiveCurrency);               // park value
                b.ReadWrite<money32>(gScenarioObjectiveCurrency);               // ride profit
                b.ReadWrite<money32>(gScenarioObjectiveCurrency);               // shop profit

                b.ReadWrite(gScenarioParkRatingWarningDays);

                b.ReadWrite(gScenarioCompletedCompanyValue);
                if (gScenarioCompletedCompanyValue == MONEY32_UNDEFINED
                    || gScenarioCompletedCompanyValue == (money32)0x80000001)
                {
                    b.Write("");
                }
                else
                {
                    b.ReadWrite(gScenarioCompletedBy);
                }
            });
        }

        void WriteGeneralChunk(OrcaStream& blob)
        {
            auto found = blob.ReadWriteChunk(ParkFileChunkType::GENERAL, [](OrcaStream::ChunkStream& b) {
                b.ReadWriteAs<uint32_t, uint64_t>(gScenarioTicks);
                b.ReadWriteAs<uint16_t, uint32_t>(gDateMonthTicks);
                b.ReadWrite(gDateMonthsElapsed);
                b.ReadWrite(gScenarioSrand0);
                b.ReadWrite(gScenarioSrand1);
                b.ReadWrite(gGuestInitialCash);
                b.ReadWrite(gGuestInitialHunger);
                b.ReadWrite(gGuestInitialThirst);

                b.ReadWrite(gNextGuestNumber);
                b.ReadWriteArray(gPeepSpawns, [&b](PeepSpawn& spawn) {
                    b.ReadWrite(spawn.x);
                    b.ReadWrite(spawn.y);
                    b.ReadWrite(spawn.z);
                    b.ReadWrite(spawn.direction);
                });

                b.ReadWrite(gLandPrice);
                b.ReadWrite(gConstructionRightsPrice);
                b.ReadWrite(gGrassSceneryTileLoopPosition); // TODO (this needs to be xy32)
            });
            if (!found)
            {
                throw std::runtime_error("No general chunk found.");
            }
        }

        void WriteInterfaceChunk(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::INTERFACE, [](OrcaStream::ChunkStream& b) {
                b.ReadWrite(gSavedViewX);
                b.ReadWrite(gSavedViewY);
                b.ReadWrite(gSavedViewZoom);
                b.ReadWrite(gSavedViewRotation);
                b.ReadWriteAs<uint8_t, uint32_t>(gLastEntranceStyle);
            });
        }

        void WriteClimateChunk(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::CLIMATE, [](OrcaStream::ChunkStream& b) {
                b.ReadWrite(gClimate);
                b.ReadWrite(gClimateUpdateTimer);

                for (const auto* cs : { &gClimateCurrent, &gClimateNext })
                {
                    b.ReadWrite(cs->Weather);
                    b.ReadWrite(cs->Temperature);
                    b.ReadWrite(cs->WeatherEffect);
                    b.ReadWrite(cs->WeatherGloom);
                    b.ReadWrite(cs->RainLevel);
                }
            });
        }

        void WriteParkChunk(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::PARK, [](OrcaStream::ChunkStream& b) {
                b.ReadWrite(gParkNameArgs);
                b.ReadWrite(gCash);
                b.ReadWrite(gBankLoan);
                b.ReadWrite(gMaxBankLoan);
                b.ReadWriteAs<uint8_t, uint16_t>(gBankLoanInterestRate);
                b.ReadWriteAs<uint32_t, uint64_t>(gParkFlags);
                b.ReadWriteAs<money16, money32>(gParkEntranceFee);
                b.ReadWrite(gStaffHandymanColour);
                b.ReadWrite(gStaffMechanicColour);
                b.ReadWrite(gStaffSecurityColour);

                // TODO use a uint64 or a list of active items
                b.ReadWrite(gSamePriceThroughoutParkA);
                b.ReadWrite(gSamePriceThroughoutParkB);

                // Marketing
                std::vector<std::tuple<uint32_t, uint32_t>> marketing;
                if (b.GetMode() != OrcaStream::Mode::READING)
                {
                    for (size_t i = 0; i < std::size(gMarketingCampaignDaysLeft); i++)
                    {
                        marketing.push_back(std::make_tuple(gMarketingCampaignDaysLeft[i], gMarketingCampaignRideIndex[i]));
                    }
                }
                b.ReadWriteArray(marketing, [&b](std::tuple<uint32_t, uint32_t>& m) {
                    b.ReadWrite(std::get<0>(m));
                    b.ReadWrite(std::get<1>(m));
                });
                if (b.GetMode() == OrcaStream::Mode::READING)
                {
                    auto count = std::min(std::size(gMarketingCampaignDaysLeft), marketing.size());
                    for (size_t i = 0; i < count; i++)
                    {
                        gMarketingCampaignDaysLeft[i] = std::get<0>(marketing[i]);
                        gMarketingCampaignRideIndex[i] = std::get<1>(marketing[i]);
                    }
                }

                // Awards
                std::vector<Award> awards(std::begin(gCurrentAwards), std::end(gCurrentAwards));
                b.ReadWriteArray(awards, [&b](Award& award) {
                    b.ReadWrite(award.Time);
                    b.ReadWrite(award.Type);
                });
                if (b.GetMode() == OrcaStream::Mode::READING)
                {
                    for (size_t i = 0; i < std::size(gCurrentAwards); i++)
                    {
                        if (awards.size() > i)
                        {
                            gCurrentAwards[i] = awards[i];
                        }
                        else
                        {
                            gCurrentAwards[i].Time = 0;
                            gCurrentAwards[i].Type = 0;
                        }
                    }
                }

                b.ReadWrite(gParkRatingCasualtyPenalty);
                b.ReadWrite(gCurrentExpenditure);
                b.ReadWrite(gCurrentProfit);
                b.ReadWrite(gTotalAdmissions);
                b.ReadWrite(gTotalIncomeFromAdmissions);
            });
        }

        void WriteResearch(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::RESEARCH, [](OrcaStream::ChunkStream& b) {
                // Research status
                b.ReadWrite(gResearchFundingLevel);
                b.ReadWrite(gResearchPriorities);
                b.ReadWrite(gResearchProgressStage);
                b.ReadWrite(gResearchProgress);
                b.ReadWrite(gResearchExpectedMonth);
                b.ReadWrite(gResearchExpectedDay);
                b.ReadWrite(gResearchLastItem);
                b.ReadWrite(gResearchNextItem);

                // Research order
                //   type (uint8_t)
                //   flags (uint8_t)
                //   entry (uint32_t)
            });
        }

        void WriteNotifications(OrcaStream& blob)
        {
            blob.ReadWriteChunk(ParkFileChunkType::NOTIFICATIONS, [](OrcaStream::ChunkStream& b) {
                std::vector<NewsItem> notifications(std::begin(gNewsItems), std::end(gNewsItems));
                b.ReadWriteArray(notifications, [&b](NewsItem& notification) {
                    b.ReadWrite(notification.Type);
                    b.ReadWrite(notification.Flags);
                    b.ReadWrite(notification.Assoc);
                    b.ReadWrite(notification.Ticks);
                    b.ReadWrite(notification.MonthYear);
                    b.ReadWrite(notification.Day);
                    if (b.GetMode() == OrcaStream::Mode::READING)
                    {
                        auto s = b.Read<std::string>();
                        String::Set(notification.Text, sizeof(notification.Text), s.c_str());
                    }
                    else
                    {
                        b.Write(std::string(notification.Text));
                    }
                });
                if (b.GetMode() == OrcaStream::Mode::READING)
                {
                    for (size_t i = 0; i < std::size(gNewsItems); i++)
                    {
                        if (notifications.size() > i)
                        {
                            gNewsItems[i] = notifications[i];
                        }
                        else
                        {
                            gNewsItems[i] = {};
                            gNewsItems[i].Type = NEWS_ITEM_NULL;
                        }
                    }
                }
            });
        }

        void WriteDerivedChunk(OrcaStream& blob)
        {
            if (blob.GetMode() == OrcaStream::Mode::WRITING)
            {
                blob.ReadWriteChunk(ParkFileChunkType::NOTIFICATIONS, [](OrcaStream::ChunkStream& b) {
                    b.Write<uint32_t>(gParkSize);
                    b.Write<uint32_t>(gNumGuestsInPark);
                    b.Write<uint32_t>(gNumGuestsHeadingForPark);
                    b.Write<uint32_t>(gCompanyValue);
                    b.Write<uint32_t>(gParkValue);
                    b.Write<uint32_t>(gParkRating);
                });
            }
        }

        void WriteTilesChunk(OrcaStream& blob)
        {
            auto found = blob.ReadWriteChunk(ParkFileChunkType::TILES, [](OrcaStream::ChunkStream& b) {
                b.ReadWriteAs<int16_t, uint32_t>(gMapSize); // x
                b.Write<uint32_t>(gMapSize);                // y

                if (b.GetMode() == OrcaStream::Mode::READING)
                {
                    OpenRCT2::GetContext()->GetGameState()->InitAll(gMapSize);
                }

                std::vector<TileElement> tiles(std::begin(gTileElements), std::end(gTileElements));
                b.ReadWriteArray(tiles, [&b](TileElement& el) { b.ReadWrite(&el, sizeof(TileElement)); });
                std::copy_n(tiles.data(), std::min(tiles.size(), std::size(gTileElements)), gTileElements);

                map_update_tile_pointers();
                UpdateParkEntranceLocations();
            });
            if (!found)
            {
                throw std::runtime_error("No tiles chunk found.");
            }
        }

        static void ReadWriteStringTable(OrcaStream::ChunkStream& b, std::string& value, const std::string_view& lcode)
        {
            std::vector<std::tuple<std::string, std::string>> table;
            if (b.GetMode() != OrcaStream::Mode::READING)
            {
                table.push_back(std::make_tuple(std::string(lcode), value));
            }
            b.ReadWriteArray(table, [&b](std::tuple<std::string, std::string>& v) {
                b.ReadWrite(std::get<0>(v));
                b.ReadWrite(std::get<1>(v));
            });
            if (b.GetMode() == OrcaStream::Mode::READING)
            {
                auto fr = std::find_if(table.begin(), table.end(), [&lcode](const std::tuple<std::string, std::string>& v) {
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
                    value = "";
                }
            }
        }
    };
} // namespace OpenRCT2

enum : uint32_t
{
    S6_SAVE_FLAG_EXPORT = 1 << 0,
    S6_SAVE_FLAG_SCENARIO = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

int32_t scenario_save(const utf8* path, int32_t flags)
{
    if (flags & S6_SAVE_FLAG_SCENARIO)
    {
        log_verbose("saving scenario");
    }
    else
    {
        log_verbose("saving game");
    }

    if (!(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        window_close_construction_windows();
    }

    map_reorganise_elements();
    viewport_set_saved_view();

    bool result = false;
    auto parkFile = std::make_unique<OpenRCT2::ParkFile>();
    try
    {
        // if (flags & S6_SAVE_FLAG_EXPORT)
        // {
        //     auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        //     s6exporter->ExportObjectsList = objManager.GetPackableObjects();
        // }
        // s6exporter->RemoveTracklessRides = true;
        // s6exporter->Export();
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
    catch (const std::exception&)
    {
    }

    gfx_invalidate_screen();

    if (result && !(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        gScreenAge = 0;
    }
    return result;
}

class ParkFileImporter : public IParkImporter
{
private:
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
        return ParkLoadResult(std::move(_parkFile->RequiredObjects));
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
        IStream* stream, bool isScenario, bool skipObjectCheck = false, const utf8* path = String::Empty) override
    {
        return Load(path);
    }

    void Import() override
    {
        _parkFile->Import();
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        return false;
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateParkFile(IObjectRepository& objectRepository)
{
    return std::make_unique<ParkFileImporter>(objectRepository);
}
