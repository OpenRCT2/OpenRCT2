/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cheats.h"
#include "Context.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "Version.h"
#include "core/Crypt.h"
#include "core/DataSerialiser.h"
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
#include "peep/Staff.h"
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
        constexpr uint32_t SPRITES          = 0x31;
        constexpr uint32_t RIDES            = 0x32;
        constexpr uint32_t BANNERS          = 0x33;
        constexpr uint32_t ANIMATIONS       = 0x34;
        constexpr uint32_t STAFF            = 0x35;
        constexpr uint32_t CHEATS           = 0x36;

        constexpr uint32_t DERIVED          = 0x50;
        // clang-format on
    }; // namespace ParkFileChunkType

    class ParkFile
    {
    public:
        std::vector<rct_object_entry> RequiredObjects;

    private:
        std::unique_ptr<OrcaStream> _os;

    public:
        void Load(const std::string_view& path)
        {
            _os = std::make_unique<OrcaStream>(path, OrcaStream::Mode::READING);
            RequiredObjects.clear();
            ReadWriteObjectsChunk(*_os);
        }

        void Import()
        {
            auto& os = *_os;
            ReadWriteTilesChunk(os);
            ReadWriteBannersChunk(os);
            ReadWriteRidesChunk(os);
            ReadWriteSpritesChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);
            ReadWriteCheatsChunk(os);

            // Initial cash will eventually be removed
            gInitialCash = gCash;
            String::Set(gS6Info.name, sizeof(gS6Info.name), gScenarioName.c_str());
            String::Set(gS6Info.details, sizeof(gS6Info.details), gScenarioName.c_str());

            AutoDeriveVariables();
        }

        void Save(const std::string_view& path)
        {
            OrcaStream os(path, OrcaStream::Mode::WRITING);

            auto& header = os.GetHeader();
            header.Magic = PARK_FILE_MAGIC;
            header.TargetVersion = PARK_FILE_CURRENT_VERSION;
            header.MinVersion = PARK_FILE_MIN_VERSION;

            ReadWriteAuthoringChunk(os);
            ReadWriteObjectsChunk(os);
            ReadWriteTilesChunk(os);
            ReadWriteBannersChunk(os);
            ReadWriteRidesChunk(os);
            ReadWriteSpritesChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);
            ReadWriteCheatsChunk(os);
        }

    private:
        void ReadWriteAuthoringChunk(OrcaStream& os)
        {
            // Write-only for now
            if (os.GetMode() == OrcaStream::Mode::WRITING)
            {
                os.ReadWriteChunk(ParkFileChunkType::AUTHORING, [](OrcaStream::ChunkStream& cs) {
                    cs.Write(std::string_view(gVersionInfoFull));
                    std::vector<std::string> authors;
                    cs.ReadWriteVector(authors, [](std::string& s) {});
                    cs.Write(std::string_view());     // custom notes that can be attached to the save
                    cs.Write<uint64_t>(std::time(0)); // date started
                    cs.Write<uint64_t>(std::time(0)); // date modified
                });
            }
        }

        void ReadWriteObjectsChunk(OrcaStream& os)
        {
            if (os.GetMode() == OrcaStream::Mode::READING)
            {
                std::vector<rct_object_entry> entries;
                os.ReadWriteChunk(ParkFileChunkType::OBJECTS, [&entries](OrcaStream::ChunkStream& cs) {
                    cs.ReadWriteVector(entries, [&cs](rct_object_entry& entry) {
                        auto type = cs.Read<uint16_t>();
                        auto id = cs.Read<std::string>();
                        auto version = cs.Read<std::string>();

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
                os.ReadWriteChunk(ParkFileChunkType::OBJECTS, [&objectIds](OrcaStream::ChunkStream& cs) {
                    auto& objManager = GetContext()->GetObjectManager();
                    cs.ReadWriteVector(objectIds, [&cs, &objManager](size_t& i) {
                        auto obj = objManager.GetLoadedObject(i);
                        if (obj != nullptr)
                        {
                            auto entry = obj->GetObjectEntry();
                            auto type = static_cast<uint16_t>(entry->flags & 0x0F);
                            type |= 0x8000; // Make as legacy object
                            cs.Write(type);
                            cs.Write(std::string_view(entry->name, 8));
                            cs.Write("");
                        }
                        else
                        {
                            cs.Write<uint16_t>(0);
                            cs.Write("");
                            cs.Write("");
                        }
                    });
                });
            }
        }

        void ReadWriteScenarioChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::SCENARIO, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWriteAs<uint8_t, uint32_t>(gS6Info.category);
                ReadWriteStringTable(cs, gScenarioName, "en-GB");

                auto& park = GetContext()->GetGameState()->GetPark();
                ReadWriteStringTable(cs, park.Name, "en-GB");

                ReadWriteStringTable(cs, gScenarioDetails, "en-GB");

                cs.ReadWriteAs<uint8_t, uint32_t>(gScenarioObjectiveType);
                cs.ReadWriteAs<uint8_t, uint16_t>(gScenarioObjectiveYear);       // year
                cs.ReadWriteAs<uint16_t, uint32_t>(gScenarioObjectiveNumGuests); // guests
                cs.Write<uint16_t>(600);                                         // rating
                cs.ReadWriteAs<money32, uint16_t>(gScenarioObjectiveCurrency);   // excitement
                cs.ReadWriteAs<uint16_t, uint32_t>(gScenarioObjectiveNumGuests); // length
                cs.ReadWrite<money32>(gScenarioObjectiveCurrency);               // park value
                cs.ReadWrite<money32>(gScenarioObjectiveCurrency);               // ride profit
                cs.ReadWrite<money32>(gScenarioObjectiveCurrency);               // shop profit

                cs.ReadWrite(gScenarioParkRatingWarningDays);

                cs.ReadWrite(gScenarioCompletedCompanyValue);
                if (gScenarioCompletedCompanyValue == MONEY32_UNDEFINED
                    || gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
                {
                    cs.Write("");
                }
                else
                {
                    cs.ReadWrite(gScenarioCompletedBy);
                }
            });
        }

        void ReadWriteGeneralChunk(OrcaStream& os)
        {
            auto found = os.ReadWriteChunk(ParkFileChunkType::GENERAL, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWriteAs<uint32_t, uint64_t>(gScenarioTicks);
                cs.ReadWriteAs<uint16_t, uint32_t>(gDateMonthTicks);
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
                cs.ReadWrite(gGrassSceneryTileLoopPosition); // TODO (this needs to be xy32)
            });
            if (!found)
            {
                throw std::runtime_error("No general chunk found.");
            }
        }

        void ReadWriteInterfaceChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::INTERFACE, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gSavedView);
                cs.ReadWrite(gSavedViewZoom);
                cs.ReadWrite(gSavedViewRotation);
                cs.ReadWriteAs<uint8_t, uint32_t>(gLastEntranceStyle);
            });
        }

        void ReadWriteCheatsChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CHEATS, [](OrcaStream::ChunkStream& cs) {
                DataSerialiser ds(cs.GetMode() == OrcaStream::Mode::WRITING, cs.GetStream());
                CheatsSerialise(ds);
            });
        }

        void ReadWriteClimateChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::CLIMATE, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWrite(gClimate);
                cs.ReadWrite(gClimateUpdateTimer);

                for (const auto* cl : { &gClimateCurrent, &gClimateNext })
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
            os.ReadWriteChunk(ParkFileChunkType::PARK, [](OrcaStream::ChunkStream& cs) {
                auto& park = GetContext()->GetGameState()->GetPark();
                cs.ReadWrite(park.Name);
                cs.ReadWrite(gCash);
                cs.ReadWrite(gBankLoan);
                cs.ReadWrite(gMaxBankLoan);
                cs.ReadWriteAs<uint8_t, uint16_t>(gBankLoanInterestRate);
                cs.ReadWriteAs<uint32_t, uint64_t>(gParkFlags);
                cs.ReadWriteAs<money16, money32>(gParkEntranceFee);
                cs.ReadWrite(gStaffHandymanColour);
                cs.ReadWrite(gStaffMechanicColour);
                cs.ReadWrite(gStaffSecurityColour);
                cs.ReadWrite(gSamePriceThroughoutPark);

                // Marketing
                cs.ReadWriteVector(gMarketingCampaigns, [&cs](MarketingCampaign& campaign) {
                    cs.ReadWrite(campaign.Type);
                    cs.ReadWrite(campaign.WeeksLeft);
                    cs.ReadWrite(campaign.Flags);
                    cs.ReadWrite(campaign.RideId);
                });

                // Awards
                cs.ReadWriteArray(gCurrentAwards, [&cs](Award& award) {
                    if (award.Time != 0)
                    {
                        cs.ReadWrite(award.Time);
                        cs.ReadWrite(award.Type);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                });

                cs.ReadWrite(gParkRatingCasualtyPenalty);
                cs.ReadWrite(gCurrentExpenditure);
                cs.ReadWrite(gCurrentProfit);
                cs.ReadWrite(gTotalAdmissions);
                cs.ReadWrite(gTotalIncomeFromAdmissions);
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
                cs.ReadWrite(gResearchLastItem);
                cs.ReadWrite(gResearchNextItem);

                // Research order
                //   type (uint8_t)
                //   flags (uint8_t)
                //   entry (uint32_t)
            });
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
                String::Set(item.Text, sizeof(item.Text), s.c_str());
            }
            else
            {
                cs.Write(std::string_view(item.Text, std::size(item.Text)));
            }
        }

        void ReadWriteDerivedChunk(OrcaStream& os)
        {
            if (os.GetMode() == OrcaStream::Mode::WRITING)
            {
                os.ReadWriteChunk(ParkFileChunkType::NOTIFICATIONS, [](OrcaStream::ChunkStream& cs) {
                    cs.Write<uint32_t>(gParkSize);
                    cs.Write<uint32_t>(gNumGuestsInPark);
                    cs.Write<uint32_t>(gNumGuestsHeadingForPark);
                    cs.Write<uint32_t>(gCompanyValue);
                    cs.Write<uint32_t>(gParkValue);
                    cs.Write<uint32_t>(gParkRating);
                });
            }
        }

        void ReadWriteTilesChunk(OrcaStream& os)
        {
            auto found = os.ReadWriteChunk(ParkFileChunkType::TILES, [](OrcaStream::ChunkStream& cs) {
                cs.ReadWriteAs<int16_t, uint32_t>(gMapSize); // x
                cs.Write<uint32_t>(gMapSize);                // y

                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    OpenRCT2::GetContext()->GetGameState()->InitAll(gMapSize);
                }

                std::vector<TileElement> tiles(std::begin(gTileElements), std::end(gTileElements));
                cs.ReadWriteVector(tiles, [&cs](TileElement& el) { cs.ReadWrite(&el, sizeof(TileElement)); });
                std::copy_n(tiles.data(), std::min(tiles.size(), std::size(gTileElements)), gTileElements);

                map_update_tile_pointers();
                UpdateParkEntranceLocations();
            });
            if (!found)
            {
                throw std::runtime_error("No tiles chunk found.");
            }
        }

        void ReadWriteBannersChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::BANNERS, [](OrcaStream::ChunkStream& cs) {
                std::vector<Banner> banners;
                if (cs.GetMode() == OrcaStream::Mode::WRITING)
                {
                    for (BannerIndex i = 0; i < MAX_BANNERS; i++)
                    {
                        banners.push_back(*GetBanner(i));
                    }
                }
                cs.ReadWriteVector(banners, [&cs](Banner& banner) {
                    cs.ReadWrite(banner.type);
                    cs.ReadWrite(banner.flags);
                    cs.ReadWrite(banner.text);
                    cs.ReadWrite(banner.colour);
                    cs.ReadWrite(banner.ride_index);
                    cs.ReadWrite(banner.text_colour);
                    cs.ReadWrite(banner.position);
                });
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    for (BannerIndex i = 0; i < MAX_BANNERS; i++)
                    {
                        auto banner = GetBanner(i);
                        *banner = banners[i];
                    }
                }
            });
        }

        void ReadWriteRidesChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::RIDES, [](OrcaStream::ChunkStream& cs) {
                std::vector<ride_id_t> rideIds;
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    ride_init_all();
                }
                else
                {
                    for (const auto& ride : GetRideManager())
                    {
                        rideIds.push_back(ride.id);
                    }
                }
                cs.ReadWriteVector(rideIds, [&cs](ride_id_t& rideId) {
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

                    cs.ReadWrite(ride.price[0]);
                    cs.ReadWrite(ride.price[1]);

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
                        cs.ReadWrite(vc);
                        return true;
                    });

                    // Stations
                    cs.ReadWrite(ride.num_stations);
                    cs.ReadWriteArray(ride.stations, [&cs](RideStation& station) {
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

                    cs.ReadWrite(ride.overall_view);

                    // Vehicles
                    cs.ReadWrite(ride.num_vehicles);
                    cs.ReadWrite(ride.num_cars_per_train);
                    cs.ReadWrite(ride.proposed_num_vehicles);
                    cs.ReadWrite(ride.proposed_num_cars_per_train);
                    cs.ReadWrite(ride.max_trains);
                    cs.ReadWrite(ride.min_max_cars_per_train);
                    cs.ReadWrite(ride.min_waiting_time);
                    cs.ReadWrite(ride.max_waiting_time);
                    cs.ReadWriteArray(ride.vehicles, [&cs](uint16_t& v) {
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
                    cs.ReadWrite(ride.num_customers);
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
                    cs.ReadWrite(ride.downtime_history);
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

        void ReadWriteSpritesChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::SPRITES, [](OrcaStream::ChunkStream& cs) {
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    reset_sprite_list();
                }

                for (size_t i = 0; i < static_cast<uint8_t>(EntityListId::Count); i++)
                {
                    cs.ReadWrite(gSpriteListHead[i]);
                    cs.ReadWrite(gSpriteListCount[i]);
                }

                std::vector<uint16_t> entityIndices;
                if (cs.GetMode() == OrcaStream::Mode::READING)
                {
                    cs.ReadWriteVector(entityIndices, [&cs](uint16_t& index) {
                        cs.ReadWrite(index);
                        auto& entity = *(get_sprite(index));
                        ReadWriteEntity(cs, entity);
                    });
                }
                else
                {
                    for (uint16_t i = 0; i < MAX_SPRITES; i++)
                    {
                        auto entity = get_sprite(i);
                        if (entity->sprite_identifier != SPRITE_IDENTIFIER_NULL)
                        {
                            entityIndices.push_back(i);
                        }
                    }
                    cs.ReadWriteVector(entityIndices, [&cs](uint16_t index) {
                        auto& entity = *(get_sprite(index));
                        cs.ReadWrite(index);
                        ReadWriteEntity(cs, entity);
                    });
                }
            });
        }

        static void ReadWriteEntity(OrcaStream::ChunkStream& cs, SpriteBase& entity)
        {
            ReadWriteEntityCommon(cs, entity);
            switch (entity.sprite_identifier)
            {
                case SPRITE_IDENTIFIER_VEHICLE:
                    ReadWriteEntityVehicle(cs, *entity.As<Vehicle>());
                    break;
                case SPRITE_IDENTIFIER_PEEP:
                    ReadWriteEntityPeep(cs, *entity.As<Peep>());
                    break;
                case SPRITE_IDENTIFIER_MISC:
                    ReadWriteEntityMisc(cs, entity);
                    break;
                case SPRITE_IDENTIFIER_LITTER:
                    ReadWriteEntityLitter(cs, *entity.As<Litter>());
                    break;
            }
        }

        static void ReadWriteEntityCommon(OrcaStream::ChunkStream& cs, SpriteBase& entity)
        {
            cs.ReadWrite(entity.sprite_identifier);
            cs.ReadWrite(entity.type);
            cs.ReadWrite(entity.next_in_quadrant);
            cs.ReadWrite(entity.next);
            cs.ReadWrite(entity.previous);
            cs.ReadWrite(entity.sprite_height_negative);
            cs.ReadWrite(entity.sprite_index);
            cs.ReadWrite(entity.flags);
            cs.ReadWrite(entity.x);
            cs.ReadWrite(entity.y);
            cs.ReadWrite(entity.z);
            cs.ReadWrite(entity.sprite_width);
            cs.ReadWrite(entity.sprite_height_positive);
            cs.ReadWrite(entity.sprite_left);
            cs.ReadWrite(entity.sprite_top);
            cs.ReadWrite(entity.sprite_right);
            cs.ReadWrite(entity.sprite_bottom);
            cs.ReadWrite(entity.sprite_direction);
        }

        static void ReadWriteEntityVehicle(OrcaStream::ChunkStream& cs, Vehicle& entity)
        {
            auto ride = entity.GetRide();

            cs.ReadWrite(entity.vehicle_sprite_type);
            cs.ReadWrite(entity.bank_rotation);
            cs.ReadWrite(entity.remaining_distance);
            cs.ReadWrite(entity.velocity);
            cs.ReadWrite(entity.acceleration);
            cs.ReadWrite(entity.ride);
            cs.ReadWrite(entity.vehicle_type);
            cs.ReadWrite(entity.colours);
            cs.ReadWrite(entity.track_progress);
            if (ride != nullptr && ride->mode == RIDE_MODE_BOAT_HIRE && entity.status == Vehicle::Status::TravellingBoat)
            {
                cs.ReadWrite(entity.BoatLocation);
            }
            else
            {
                // Track direction and type are in the same field
                cs.ReadWrite(entity.track_direction);
            }
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
            cs.ReadWrite(entity.dodgems_collision_direction);
            cs.ReadWrite(entity.animation_frame);
            cs.ReadWrite(entity.var_C8);
            cs.ReadWrite(entity.var_CA);
            cs.ReadWrite(entity.scream_sound_id);
            cs.ReadWrite(entity.TrackSubposition);
            cs.ReadWrite(entity.var_CE);
            cs.ReadWrite(entity.var_CF);
            cs.ReadWrite(entity.lost_time_out);
            cs.ReadWrite(entity.vertical_drop_countdown);
            cs.ReadWrite(entity.var_D3);
            cs.ReadWrite(entity.mini_golf_current_animation);
            cs.ReadWrite(entity.mini_golf_flags);
            cs.ReadWrite(entity.ride_subtype);
            cs.ReadWrite(entity.colours_extended);
            cs.ReadWrite(entity.seat_rotation);
            cs.ReadWrite(entity.target_seat_rotation);
        }

        static void ReadWriteEntityPeep(OrcaStream::ChunkStream& cs, Peep& entity)
        {
            cs.ReadWrite(entity.Name);
            cs.ReadWrite(entity.NextLoc);
            cs.ReadWrite(entity.NextFlags);
            cs.ReadWrite(entity.OutsideOfPark);
            cs.ReadWrite(entity.State);
            cs.ReadWrite(entity.SubState);
            cs.ReadWrite(entity.SpriteType);
            cs.ReadWrite(entity.AssignedPeepType);
            cs.ReadWrite(entity.GuestNumRides);
            cs.ReadWrite(entity.TshirtColour);
            cs.ReadWrite(entity.TrousersColour);
            cs.ReadWrite(entity.DestinationX);
            cs.ReadWrite(entity.DestinationY);
            cs.ReadWrite(entity.DestinationTolerance);
            cs.ReadWrite(entity.Var37);
            cs.ReadWrite(entity.Energy);
            cs.ReadWrite(entity.EnergyTarget);
            cs.ReadWrite(entity.Happiness);
            cs.ReadWrite(entity.HappinessTarget);
            cs.ReadWrite(entity.Nausea);
            cs.ReadWrite(entity.NauseaTarget);
            cs.ReadWrite(entity.Hunger);
            cs.ReadWrite(entity.Thirst);
            cs.ReadWrite(entity.Toilet);
            cs.ReadWrite(entity.Mass);
            cs.ReadWrite(entity.TimeToConsume);
            cs.ReadWrite(entity.Intensity);
            cs.ReadWrite(entity.NauseaTolerance);
            cs.ReadWrite(entity.WindowInvalidateFlags);
            cs.ReadWrite(entity.PaidOnDrink);
            cs.ReadWriteArray(entity.RideTypesBeenOn, [&cs](uint8_t& rideType) {
                cs.ReadWrite(rideType);
                return true;
            });
            cs.ReadWrite(entity.ItemExtraFlags);
            cs.ReadWrite(entity.Photo2RideRef);
            cs.ReadWrite(entity.Photo3RideRef);
            cs.ReadWrite(entity.Photo4RideRef);
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
            cs.ReadWrite(entity.GuestNextInQueue);
            cs.ReadWrite(entity.PeepDirection);
            cs.ReadWrite(entity.InteractionRideIndex);
            cs.ReadWrite(entity.TimeInQueue);
            cs.ReadWriteArray(entity.RidesBeenOn, [&cs](ride_id_t rideId) {
                cs.ReadWrite(rideId);
                return true;
            });
            cs.ReadWrite(entity.Id);
            cs.ReadWrite(entity.CashInPocket);
            cs.ReadWrite(entity.CashSpent);
            cs.ReadWrite(entity.TimeInPark);
            cs.ReadWrite(entity.RejoinQueueTimeout);
            cs.ReadWrite(entity.PreviousRide);
            cs.ReadWrite(entity.PreviousRideTimeOut);
            cs.ReadWriteArray(entity.Thoughts, [&cs](rct_peep_thought thought) {
                if (thought.type != PEEP_THOUGHT_TYPE_NONE)
                {
                    cs.ReadWrite(thought.type);
                    cs.ReadWrite(thought.item);
                    cs.ReadWrite(thought.freshness);
                    cs.ReadWrite(thought.fresh_timeout);
                    return true;
                }
                else
                {
                    return false;
                }
            });
            cs.ReadWrite(entity.PathCheckOptimisation);
            cs.ReadWrite(entity.GuestHeadingToRideId);
            cs.ReadWrite(entity.GuestIsLostCountdown);
            cs.ReadWrite(entity.Photo1RideRef);
            cs.ReadWrite(entity.PeepFlags);
            cs.ReadWrite(entity.PathfindGoal);
            for (size_t i = 0; i < std::size(entity.PathfindHistory); i++)
            {
                cs.ReadWrite(entity.PathfindHistory[i]);
            }
            cs.ReadWrite(entity.WalkingFrameNum);
            cs.ReadWrite(entity.LitterCount);
            cs.ReadWrite(entity.GuestTimeOnRide);
            cs.ReadWrite(entity.DisgustingCount);
            cs.ReadWrite(entity.PaidToEnter);
            cs.ReadWrite(entity.PaidOnRides);
            cs.ReadWrite(entity.PaidOnFood);
            cs.ReadWrite(entity.PaidOnSouvenirs);
            cs.ReadWrite(entity.AmountOfFood);
            cs.ReadWrite(entity.AmountOfDrinks);
            cs.ReadWrite(entity.AmountOfSouvenirs);
            cs.ReadWrite(entity.VandalismSeen);
            cs.ReadWrite(entity.VoucherType);
            cs.ReadWrite(entity.VoucherRideId);
            cs.ReadWrite(entity.SurroundingsThoughtTimeout);
            cs.ReadWrite(entity.Angriness);
            cs.ReadWrite(entity.TimeLost);
            cs.ReadWrite(entity.DaysInQueue);
            cs.ReadWrite(entity.BalloonColour);
            cs.ReadWrite(entity.UmbrellaColour);
            cs.ReadWrite(entity.HatColour);
            cs.ReadWrite(entity.FavouriteRide);
            cs.ReadWrite(entity.FavouriteRideRating);
            cs.ReadWrite(entity.ItemStandardFlags);
        }

        static void ReadWriteEntityMisc(OrcaStream::ChunkStream& cs, SpriteBase& entity)
        {
            switch (entity.type)
            {
                case SPRITE_MISC_STEAM_PARTICLE:
                {
                    auto steamParticle = entity.As<SteamParticle>();
                    cs.ReadWrite(steamParticle->time_to_move);
                    cs.ReadWrite(steamParticle->frame);
                    break;
                }
                case SPRITE_MISC_MONEY_EFFECT:
                {
                    auto moneyEffect = entity.As<MoneyEffect>();
                    cs.ReadWrite(moneyEffect->MoveDelay);
                    cs.ReadWrite(moneyEffect->NumMovements);
                    cs.ReadWrite(moneyEffect->Vertical);
                    cs.ReadWrite(moneyEffect->Value);
                    cs.ReadWrite(moneyEffect->OffsetX);
                    cs.ReadWrite(moneyEffect->Wiggle);
                    break;
                }
                case SPRITE_MISC_CRASHED_VEHICLE_PARTICLE:
                {
                    auto vehicleCrashParticle = entity.As<VehicleCrashParticle>();
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->frame);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->time_to_live);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->frame);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->colour[0]);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->colour[1]);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->crashed_sprite_base);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->velocity_x);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->velocity_y);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->velocity_z);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->acceleration_x);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->acceleration_y);
                    cs.ReadWrite(vehicleCrashParticle, vehicleCrashParticle->acceleration_z);
                    break;
                }
                case SPRITE_MISC_EXPLOSION_CLOUD:
                case SPRITE_MISC_CRASH_SPLASH:
                case SPRITE_MISC_EXPLOSION_FLARE:
                {
                    auto generic = static_cast<SpriteGeneric&>(entity);
                    cs.ReadWrite(generic.frame);
                    break;
                }
                case SPRITE_MISC_JUMPING_FOUNTAIN_WATER:
                case SPRITE_MISC_JUMPING_FOUNTAIN_SNOW:
                {
                    auto fountain = entity.As<JumpingFountain>();
                    cs.ReadWrite(fountain->NumTicksAlive);
                    cs.ReadWrite(fountain->frame);
                    cs.ReadWrite(fountain->FountainFlags);
                    cs.ReadWrite(fountain->TargetX);
                    cs.ReadWrite(fountain->TargetY);
                    cs.ReadWrite(fountain->TargetY);
                    cs.ReadWrite(fountain->Iteration);
                    break;
                }
                case SPRITE_MISC_BALLOON:
                {
                    auto balloon = entity.As<Balloon>();
                    cs.ReadWrite(balloon->popped);
                    cs.ReadWrite(balloon->time_to_move);
                    cs.ReadWrite(balloon->frame);
                    cs.ReadWrite(balloon->colour);
                    break;
                }
                case SPRITE_MISC_DUCK:
                {
                    auto duck = entity.As<Duck>();
                    cs.ReadWrite(duck->frame);
                    cs.ReadWrite(duck->target_x);
                    cs.ReadWrite(duck->target_y);
                    cs.ReadWrite(duck->state);
                    break;
                }
            }
        }

        static void ReadWriteEntityLitter(OrcaStream::ChunkStream& cs, Litter& entity)
        {
            cs.ReadWrite(entity.creationTick);
        }

        void AutoDeriveVariables()
        {
            uint16_t numGuestsInPark = 0;
            uint16_t numGuestsHeadingsForPark = 0;
            for (auto peep : EntityList<Peep>(EntityListId::Peep))
            {
                if (peep->State == PEEP_STATE_ENTERING_PARK)
                {
                    numGuestsHeadingsForPark++;
                }
                if (!peep->OutsideOfPark)
                {
                    numGuestsInPark++;
                }
            }

            gNumGuestsInPark = numGuestsInPark;
            gNumGuestsHeadingForPark = numGuestsHeadingsForPark;

            auto& park = GetContext()->GetGameState()->GetPark();
            gParkSize = park.CalculateParkSize();
            gParkValue = park.CalculateParkValue();
            gCompanyValue = park.CalculateCompanyValue();
            gParkRating = park.CalculateParkRating();
        }

        static void ReadWriteStringTable(OrcaStream::ChunkStream& cs, std::string& value, const std::string_view& lcode)
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
        OpenRCT2::IStream* stream, bool isScenario, bool skipObjectCheck = false, const utf8* path = String::Empty) override
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
