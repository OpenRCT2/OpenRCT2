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
            ReadWriteThingsChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);

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
            ReadWriteThingsChunk(os);
            ReadWriteScenarioChunk(os);
            ReadWriteGeneralChunk(os);
            ReadWriteParkChunk(os);
            ReadWriteClimateChunk(os);
            ReadWriteResearchChunk(os);
            ReadWriteNotificationsChunk(os);
            ReadWriteInterfaceChunk(os);
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
                            auto type = (uint16_t)(entry->flags & 0x0F);
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
                    || gScenarioCompletedCompanyValue == (money32)0x80000001)
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
                cs.ReadWrite(gSavedViewX);
                cs.ReadWrite(gSavedViewY);
                cs.ReadWrite(gSavedViewZoom);
                cs.ReadWrite(gSavedViewRotation);
                cs.ReadWriteAs<uint8_t, uint32_t>(gLastEntranceStyle);
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
                    cs.ReadWrite(cl->RainLevel);
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
                std::vector<NewsItem> notifications(std::begin(gNewsItems), std::end(gNewsItems));
                cs.ReadWriteVector(notifications, [&cs](NewsItem& notification) {
                    cs.ReadWrite(notification.Type);
                    cs.ReadWrite(notification.Flags);
                    cs.ReadWrite(notification.Assoc);
                    cs.ReadWrite(notification.Ticks);
                    cs.ReadWrite(notification.MonthYear);
                    cs.ReadWrite(notification.Day);
                    if (cs.GetMode() == OrcaStream::Mode::READING)
                    {
                        auto s = cs.Read<std::string>();
                        String::Set(notification.Text, sizeof(notification.Text), s.c_str());
                    }
                    else
                    {
                        cs.Write(std::string(notification.Text));
                    }
                });
                if (cs.GetMode() == OrcaStream::Mode::READING)
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

                    cs.ReadWrite(ride.price);
                    cs.ReadWrite(ride.price_secondary);

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
                    cs.ReadWrite(ride.chairlift_bullwheel_location[0]);
                    cs.ReadWrite(ride.chairlift_bullwheel_z[0]);
                    cs.ReadWrite(ride.chairlift_bullwheel_location[1]);
                    cs.ReadWrite(ride.chairlift_bullwheel_z[1]);
                    cs.ReadWrite(ride.chairlift_bullwheel_rotation);
                    cs.ReadWrite(ride.slide_in_use);
                    cs.ReadWrite(ride.slide_peep);
                    cs.ReadWrite(ride.slide_peep_t_shirt_colour);
                    cs.ReadWrite(ride.spiral_slide_progress);
                    cs.ReadWrite(ride.race_winner);
                    cs.ReadWrite(ride.cable_lift);
                    cs.ReadWrite(ride.cable_lift_x);
                    cs.ReadWrite(ride.cable_lift_y);
                    cs.ReadWrite(ride.cable_lift_z);

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
                    cs.ReadWrite(ride.cur_test_track_location);

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
                    cs.ReadWrite(ride.cur_test_track_z);
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

        void ReadWriteThingsChunk(OrcaStream& os)
        {
            os.ReadWriteChunk(ParkFileChunkType::THINGS, [](OrcaStream::ChunkStream& cs) {
                for (size_t i = 0; i < SPRITE_LIST_COUNT; i++)
                {
                    cs.ReadWrite(gSpriteListHead[i]);
                    cs.ReadWrite(gSpriteListCount[i]);
                }
                for (size_t i = 0; i < MAX_SPRITES; i++)
                {
                    auto& sprite = *(get_sprite(i));
                    cs.ReadWrite(sprite);
                }
            });
        }

        void AutoDeriveVariables()
        {
            Peep* peep{};
            uint16_t spriteIndex{};
            uint16_t numGuestsInPark = 0;
            uint16_t numGuestsHeadingsForPark = 0;
            FOR_ALL_GUESTS (spriteIndex, peep)
            {
                if (peep->state == PEEP_STATE_ENTERING_PARK)
                {
                    numGuestsHeadingsForPark++;
                }
                if (!peep->outside_of_park)
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
