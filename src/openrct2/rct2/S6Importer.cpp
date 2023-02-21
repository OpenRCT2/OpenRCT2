/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Editor.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../ParkImporter.h"
#include "../config/Config.h"
#include "../core/Console.hpp"
#include "../core/FileStream.h"
#include "../core/IStream.hpp"
#include "../core/MemoryStream.h"
#include "../core/Numerics.hpp"
#include "../core/Path.hpp"
#include "../core/Random.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "../entity/EntityTweener.h"
#include "../entity/Fountain.h"
#include "../entity/Guest.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../interface/Viewport.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Marketing.h"
#include "../management/NewsItem.h"
#include "../management/Research.h"
#include "../network/network.h"
#include "../object/LargeSceneryEntry.h"
#include "../object/ObjectLimits.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/WallSceneryEntry.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/EntryList.h"
#include "../rct12/RCT12.h"
#include "../rct12/SawyerChunkReader.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideRatings.h"
#include "../ride/ShopItem.h"
#include "../ride/Station.h"
#include "../ride/Track.h"
#include "../ride/TrainManager.h"
#include "../ride/Vehicle.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/Surface.h"
#include "../world/TilePointerIndex.hpp"

#include <algorithm>

namespace RCT2
{
#define DECRYPT_MONEY(money) (static_cast<money32>(Numerics::rol32((money) ^ 0xF4EC9621, 13)))

    /**
     * Class to import RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
     */
    class S6Importer final : public IParkImporter
    {
    private:
        IObjectRepository& _objectRepository;

        u8string _s6Path;
        S6Data _s6{};
        uint8_t _gameVersion = 0;
        bool _isSV7 = false;
        bool _isScenario = false;
        OpenRCT2::BitSet<Limits::MaxRidesInPark> _isFlatRide{};
        ObjectEntryIndex _pathToSurfaceMap[16];
        ObjectEntryIndex _pathToQueueSurfaceMap[16];
        ObjectEntryIndex _pathToRailingMap[16];
        RCT12::EntryList _terrainSurfaceEntries;
        RCT12::EntryList _terrainEdgeEntries;

    public:
        S6Importer(IObjectRepository& objectRepository)
            : _objectRepository(objectRepository)
        {
        }

        ParkLoadResult Load(const u8string& path) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::Equals(extension, ".sc6", true))
            {
                return LoadScenario(path);
            }
            if (String::Equals(extension, ".sv6", true))
            {
                return LoadSavedGame(path);
            }

            throw std::runtime_error("Invalid RCT2 park extension.");
        }

        ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, false, skipObjectCheck);
            _s6Path = path;
            return result;
        }

        ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
            auto result = LoadFromStream(&fs, true, skipObjectCheck);
            _s6Path = path;
            return result;
        }

        ParkLoadResult LoadFromStream(
            OpenRCT2::IStream* stream, bool isScenario, [[maybe_unused]] bool skipObjectCheck = false,
            const u8string& path = {}) override
        {
            auto chunkReader = SawyerChunkReader(stream);
            chunkReader.ReadChunk(&_s6.Header, sizeof(_s6.Header));

            LOG_VERBOSE("saved game classic_flag = 0x%02x", _s6.Header.ClassicFlag);
            if (isScenario)
            {
                if (_s6.Header.Type != S6_TYPE_SCENARIO)
                {
                    throw std::runtime_error("Park is not a scenario.");
                }
                chunkReader.ReadChunk(&_s6.Info, sizeof(_s6.Info));
            }
            else
            {
                if (_s6.Header.Type != S6_TYPE_SAVEDGAME)
                {
                    throw std::runtime_error("Park is not a saved game.");
                }
            }

            // Read packed objects
            // TODO try to contain this more and not store objects until later
            for (uint16_t i = 0; i < _s6.Header.NumPackedObjects; i++)
            {
                _objectRepository.ExportPackedObject(stream);
            }

            if (!path.empty())
            {
                auto extension = Path::GetExtension(path);
                _isSV7 = _stricmp(extension.c_str(), ".sv7") == 0;
            }

            chunkReader.ReadChunk(&_s6.Objects, sizeof(_s6.Objects));

            if (isScenario)
            {
                chunkReader.ReadChunk(&_s6.ElapsedMonths, 16);
                chunkReader.ReadChunk(&_s6.TileElements, sizeof(_s6.TileElements));
                ReadChunk6(chunkReader, 76);
                chunkReader.ReadChunk(&_s6.GuestsInPark, 4);
                chunkReader.ReadChunk(&_s6.LastGuestsInPark, 8);
                chunkReader.ReadChunk(&_s6.ParkRating, 2);
                chunkReader.ReadChunk(&_s6.ActiveResearchTypes, 1082);
                chunkReader.ReadChunk(&_s6.CurrentExpenditure, 16);
                chunkReader.ReadChunk(&_s6.ParkValue, 4);
                chunkReader.ReadChunk(&_s6.CompletedCompanyValue, 483816);
            }
            else
            {
                chunkReader.ReadChunk(&_s6.ElapsedMonths, 16);
                chunkReader.ReadChunk(&_s6.TileElements, sizeof(_s6.TileElements));
                ReadChunk6(chunkReader, 488816);
            }

            _isScenario = isScenario;
            _s6Path = path;

            return ParkLoadResult(GetRequiredObjects());
        }

        void ReadChunk6(SawyerChunkReader& chunkReader, uint32_t sizeWithoutEntities)
        {
            uint32_t entitiesSize = GetMaxEntities() * sizeof(Entity);
            size_t bufferSize = sizeWithoutEntities + entitiesSize;
            std::vector<uint8_t> buffer(bufferSize);
            chunkReader.ReadChunk(buffer.data(), buffer.size());
            auto stream = OpenRCT2::MemoryStream(buffer.data(), buffer.size());

            uint32_t preEntitiesSize = sizeof(_s6.NextFreeTileElementPointerIndex);
            uint32_t postEntitiesSize = sizeWithoutEntities - preEntitiesSize;

            stream.Read(&_s6.NextFreeTileElementPointerIndex, preEntitiesSize);
            stream.Read(&_s6.Entities, entitiesSize);
            stream.Read(&_s6.EntityListsHead, postEntitiesSize);
        }

        bool GetDetails(ScenarioIndexEntry* dst) override
        {
            *dst = {};
            return false;
        }

        void Import() override
        {
            Initialise();

            gEditorStep = _s6.Info.EditorStep;
            gScenarioCategory = static_cast<SCENARIO_CATEGORY>(_s6.Info.Category);

            // Some scenarios have their scenario details in UTF-8, due to earlier bugs in OpenRCT2.
            auto loadMaybeUTF8 = [](std::string_view str) -> std::string {
                return !IsLikelyUTF8(str) ? RCT2StringToUTF8(str, RCT2LanguageId::EnglishUK) : std::string(str);
            };

            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                gScenarioName = loadMaybeUTF8(_s6.Info.Name);
                gScenarioDetails = loadMaybeUTF8(_s6.Info.Details);
            }
            else
            {
                // Saved games do not have an info chunk
                gScenarioName = loadMaybeUTF8(_s6.ScenarioName);
                gScenarioDetails = loadMaybeUTF8(_s6.ScenarioDescription);
            }

            OpenRCT2::GetContext()->GetGameState()->SetDate(OpenRCT2::Date(_s6.ElapsedMonths, _s6.CurrentDay));
            gCurrentTicks = _s6.GameTicks1;

            ScenarioRandSeed(_s6.ScenarioSrand0, _s6.ScenarioSrand1);

            DetermineFlatRideStatus();
            ImportTileElements();
            ImportEntities();

            gInitialCash = ToMoney64(_s6.InitialCash);
            gBankLoan = ToMoney64(_s6.CurrentLoan);

            gParkFlags = _s6.ParkFlags & ~PARK_FLAGS_NO_MONEY_SCENARIO;

            // RCT2 used a different flag for `no money` when the park is a scenario
            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                if (_s6.ParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
                    gParkFlags |= PARK_FLAGS_NO_MONEY;
                else
                    gParkFlags &= ~PARK_FLAGS_NO_MONEY;
            }

            gParkEntranceFee = _s6.ParkEntranceFee;
            // rct1_park_entranceX
            // rct1_park_entrance_y
            // Pad013573EE
            // rct1_park_entrance_z

            ImportPeepSpawns();

            gGuestChangeModifier = _s6.GuestCountChangeModifier;
            gResearchFundingLevel = _s6.CurrentResearchLevel;
            // Pad01357400
            // _s6.ResearchedTrackTypesA
            // _s6.ResearchedTrackTypesB

            gNumGuestsInPark = _s6.GuestsInPark;
            gNumGuestsHeadingForPark = _s6.GuestsHeadingForPark;

            for (size_t i = 0; i < Limits::ExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::ExpenditureTypeCount; j++)
                {
                    gExpenditureTable[i][j] = ToMoney64(_s6.ExpenditureTable[i][j]);
                }
            }

            gNumGuestsInParkLastWeek = _s6.LastGuestsInPark;
            // Pad01357BCA
            gStaffHandymanColour = _s6.HandymanColour;
            gStaffMechanicColour = _s6.MechanicColour;
            gStaffSecurityColour = _s6.SecurityColour;

            gParkRating = _s6.ParkRating;

            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            park.ResetHistories();
            std::copy(std::begin(_s6.ParkRatingHistory), std::end(_s6.ParkRatingHistory), gParkRatingHistory);
            for (size_t i = 0; i < std::size(_s6.GuestsInParkHistory); i++)
            {
                if (_s6.GuestsInParkHistory[i] != RCT12ParkHistoryUndefined)
                {
                    gGuestsInParkHistory[i] = _s6.GuestsInParkHistory[i] * RCT12GuestsInParkHistoryFactor;
                }
            }

            gResearchPriorities = _s6.ActiveResearchTypes;
            gResearchProgressStage = _s6.ResearchProgressStage;
            if (_s6.LastResearchedItemSubject != RCT12_RESEARCHED_ITEMS_SEPARATOR)
                gResearchLastItem = RCT12ResearchItem{ _s6.LastResearchedItemSubject, EnumValue(ResearchCategory::Transport) }
                                        .ToResearchItem();
            else
                gResearchLastItem = std::nullopt;
            // Pad01357CF8
            if (_s6.NextResearchItem != RCT12_RESEARCHED_ITEMS_SEPARATOR)
                gResearchNextItem = RCT12ResearchItem{ _s6.NextResearchItem, _s6.NextResearchCategory }.ToResearchItem();
            else
                gResearchNextItem = std::nullopt;

            gResearchProgress = _s6.ResearchProgress;
            gResearchExpectedDay = _s6.NextResearchExpectedDay;
            gResearchExpectedMonth = _s6.NextResearchExpectedMonth;
            gGuestInitialHappiness = _s6.GuestInitialHappiness;
            gParkSize = _s6.ParkSize;
            _guestGenerationProbability = _s6.GuestGenerationProbability;
            gTotalRideValueForMoney = _s6.TotalRideValueForMoney;
            gMaxBankLoan = ToMoney64(_s6.MaximumLoan);
            gGuestInitialCash = ToMoney64(_s6.GuestInitialCash);
            gGuestInitialHunger = _s6.GuestInitialHunger;
            gGuestInitialThirst = _s6.GuestInitialThirst;
            gScenarioObjective.Type = _s6.ObjectiveType;
            gScenarioObjective.Year = _s6.ObjectiveYear;
            // Pad013580FA
            gScenarioObjective.Currency = _s6.ObjectiveCurrency;
            // In RCT2, the ride string IDs start at index STR_0002 and are directly mappable.
            // This is not always the case in OpenRCT2, so we use the actual ride ID.
            if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
                gScenarioObjective.RideId = _s6.ObjectiveGuests - RCT2_RIDE_STRING_START;
            else
                gScenarioObjective.NumGuests = _s6.ObjectiveGuests;
            ImportMarketingCampaigns();

            gCurrentExpenditure = ToMoney64(_s6.CurrentExpenditure);
            gCurrentProfit = ToMoney64(_s6.CurrentProfit);
            gWeeklyProfitAverageDividend = ToMoney64(_s6.WeeklyProfitAverageDividend);
            gWeeklyProfitAverageDivisor = _s6.WeeklyProfitAverageDivisor;
            // Pad0135833A

            gParkValue = ToMoney64(_s6.ParkValue);

            for (size_t i = 0; i < Limits::FinanceGraphSize; i++)
            {
                gCashHistory[i] = ToMoney64(_s6.BalanceHistory[i]);
                gWeeklyProfitHistory[i] = ToMoney64(_s6.WeeklyProfitHistory[i]);
                gParkValueHistory[i] = ToMoney64(_s6.ParkValueHistory[i]);
            }

            gScenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s6.CompletedCompanyValue);
            gTotalAdmissions = _s6.TotalAdmissions;
            gTotalIncomeFromAdmissions = ToMoney64(_s6.IncomeFromAdmissions);
            gCompanyValue = ToMoney64(_s6.CompanyValue);
            std::memcpy(gPeepWarningThrottle, _s6.PeepWarningThrottle, sizeof(_s6.PeepWarningThrottle));

            // Awards
            auto& awards = GetAwards();
            for (auto& src : _s6.Awards)
            {
                if (src.Time != 0)
                {
                    awards.push_back(Award{ src.Time, static_cast<AwardType>(src.Type) });
                }
            }

            gLandPrice = ToMoney64(_s6.LandPrice);
            gConstructionRightsPrice = ToMoney64(_s6.ConstructionRightsPrice);
            // unk_01358774
            // Pad01358776
            // _s6.CdKey
            _gameVersion = _s6.GameVersionNumber;
            gScenarioCompanyValueRecord = _s6.CompletedCompanyValueRecord;
            // _s6.LoanHash;
            // Pad013587CA
            gHistoricalProfit = ToMoney64(_s6.HistoricalProfit);
            // Pad013587D4
            gScenarioCompletedBy = std::string_view(_s6.ScenarioCompletedName, sizeof(_s6.ScenarioCompletedName));
            gCash = ToMoney64(DECRYPT_MONEY(_s6.Cash));
            // Pad013587FC
            gParkRatingCasualtyPenalty = _s6.ParkRatingCasualtyPenalty;
            gMapSize = { _s6.MapSize, _s6.MapSize };
            gSamePriceThroughoutPark = _s6.SamePriceThroughout | (static_cast<uint64_t>(_s6.SamePriceThroughoutExtended) << 32);
            _suggestedGuestMaximum = _s6.SuggestedMaxGuests;
            gScenarioParkRatingWarningDays = _s6.ParkRatingWarningDays;
            gLastEntranceStyle = _s6.LastEntranceStyle;
            // rct1_water_colour
            // Pad01358842
            ImportResearchList();
            gMapBaseZ = _s6.MapBaseZ;
            gBankLoanInterestRate = _s6.CurrentInterestRate;
            // Pad0135934B
            // Preserve compatibility with vanilla RCT2's save format.
            gParkEntrances.clear();
            for (uint8_t i = 0; i < Limits::MaxParkEntrances; i++)
            {
                if (_s6.ParkEntranceX[i] != LOCATION_NULL)
                {
                    CoordsXYZD entrance;
                    entrance.x = _s6.ParkEntranceX[i];
                    entrance.y = _s6.ParkEntranceY[i];
                    entrance.z = _s6.ParkEntranceZ[i];
                    entrance.direction = _s6.ParkEntranceDirection[i];
                    gParkEntrances.push_back(entrance);
                }
            }
            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                // _s6.ScenarioFilename is wrong for some RCT2 expansion scenarios, so we use the real filename
                gScenarioFileName = Path::GetFileName(_s6Path);
            }
            else
            {
                // For savegames the filename can be arbitrary, so we have no choice but to rely on the name provided
                gScenarioFileName = std::string(String::ToStringView(_s6.ScenarioFilename, std::size(_s6.ScenarioFilename)));
            }
            gCurrentRealTimeTicks = 0;

            ImportRides();

            gSavedAge = _s6.SavedAge;
            gSavedView = ScreenCoordsXY{ _s6.SavedViewX, _s6.SavedViewY };
            gSavedViewZoom = ZoomLevel{ static_cast<int8_t>(_s6.SavedViewZoom) };
            gSavedViewRotation = _s6.SavedViewRotation;

            ImportRideRatingsCalcData();
            ImportRideMeasurements();
            gNextGuestNumber = _s6.NextGuestIndex;
            gGrassSceneryTileLoopPosition = _s6.GrassAndSceneryTilepos;
            // unk_13CA73E
            // Pad13CA73F
            // unk_13CA740
            gClimate = ClimateType{ _s6.Climate };
            // Pad13CA741;
            // Byte13CA742
            // Pad013CA747
            gClimateUpdateTimer = _s6.ClimateUpdateTimer;
            gClimateCurrent.Weather = WeatherType{ _s6.CurrentWeather };
            gClimateNext.Weather = WeatherType{ _s6.NextWeather };
            gClimateCurrent.Temperature = _s6.Temperature;
            gClimateNext.Temperature = _s6.NextTemperature;
            gClimateCurrent.WeatherEffect = WeatherEffectType{ _s6.CurrentWeatherEffect };
            gClimateNext.WeatherEffect = WeatherEffectType{ _s6.NextWeatherEffect };
            gClimateCurrent.WeatherGloom = _s6.CurrentWeatherGloom;
            gClimateNext.WeatherGloom = _s6.NextWeatherGloom;
            gClimateCurrent.Level = static_cast<WeatherLevel>(_s6.CurrentWeatherLevel);
            gClimateNext.Level = static_cast<WeatherLevel>(_s6.NextWeatherLevel);

            // News items
            News::InitQueue();
            for (size_t i = 0; i < Limits::MaxNewsItems; i++)
            {
                const RCT12NewsItem* src = &_s6.NewsItems[i];
                News::Item* dst = &gNewsItems[i];
                if (src->Type < News::ItemTypeCount)
                {
                    dst->Type = static_cast<News::ItemType>(src->Type);
                    dst->Flags = src->Flags;
                    dst->Assoc = src->Assoc;
                    dst->Ticks = src->Ticks;
                    dst->MonthYear = src->MonthYear;
                    dst->Day = src->Day;
                    dst->Text = ConvertFormattedStringToOpenRCT2(std::string_view(src->Text, sizeof(src->Text)));
                }
                else
                {
                    // In case where news item type is broken, consider all remaining news items invalid.
                    LOG_ERROR("Invalid news type 0x%x for news item %d, ignoring remaining news items", src->Type, i);
                    // Still need to set the correct type to properly terminate the queue
                    dst->Type = News::ItemType::Null;
                    break;
                }
            }

            // Pad13CE730
            // rct1_scenario_flags
            gWidePathTileLoopPosition.x = _s6.WidePathTileLoopX;
            gWidePathTileLoopPosition.y = _s6.WidePathTileLoopY;
            // Pad13CE778

            // Fix and set dynamic variables
            MapStripGhostFlagFromElements();
            ConvertScenarioStringsToUTF8();
            DetermineRideEntranceAndExitLocations();

            park.Name = GetUserString(_s6.ParkName);

            FixLandOwnership();
            FixWater();
            FixAyersRockScenario();

            ResearchDetermineFirstOfType();
            UpdateConsolidatedPatrolAreas();

            CheatsReset();
            ClearRestrictedScenery();
        }

        void AddDefaultEntries()
        {
            // Add default surfaces
            _terrainSurfaceEntries.AddRange(DefaultTerrainSurfaces);

            // Add default edges
            _terrainEdgeEntries.AddRange(DefaultTerrainEdges);
        }

        void ConvertScenarioStringsToUTF8()
        {
            // Scenario details
            gScenarioCompletedBy = RCT2StringToUTF8(gScenarioCompletedBy, RCT2LanguageId::EnglishUK);
            gScenarioName = RCT2StringToUTF8(gScenarioName, RCT2LanguageId::EnglishUK);
            gScenarioDetails = RCT2StringToUTF8(gScenarioDetails, RCT2LanguageId::EnglishUK);
        }

        void FixLandOwnership() const
        {
            // Checking _s6.ScenarioFilename is generally more reliable as it survives renaming.
            // However, some WW/TT scenarios have this incorrectly set to "Six Flags Magic Mountain.SC6",
            // so for those cases (as well as for SFMM proper, we’ll have to check the filename.
            if (String::Equals(_s6.ScenarioFilename, "Europe - European Cultural Festival.SC6"))
            {
                // This scenario breaks pathfinding. Create passages between the worlds. (List is grouped by neighbouring
                // tiles.)
                // clang-format off
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 67, 94 }, { 68, 94 }, { 69, 94 },
                        { 58, 24 }, { 58, 25 }, { 58, 26 }, { 58, 27 }, { 58, 28 }, { 58, 29 }, { 58, 30 }, { 58, 31 }, { 58, 32 },
                        { 26, 44 }, { 26, 45 },
                        { 32, 79 }, { 32, 80 }, { 32, 81 },
                    },
                    OWNERSHIP_OWNED);
                // clang-format on
            }
            else if (String::Equals(_s6.ScenarioFilename, "Six Flags Holland.SC6"))
            {
                // clang-format off
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 112, 33 }, { 112, 34 },
                        { 113, 117 }, { 114, 117 }, { 115, 117 }, { 116, 117 }, { 117, 117 }, { 114, 118 }, { 115, 118 }, { 116, 118 }, { 117, 118 },
                    },
                    OWNERSHIP_AVAILABLE, true);
                // clang-format on
            }
            else if (String::Equals(_s6.ScenarioFilename, "North America - Grand Canyon.SC6"))
            {
                // clang-format off
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 128, 90 },
                        { 135, 91 }, { 136, 91 },
                        { 129, 90 }, { 130, 90 }, { 131, 90 }, { 132, 90 }, 
                        { 137, 92 }, { 138, 92 }, { 139, 92 }, { 140, 92 },
                        { 125, 88 }, { 126, 89 }, { 127, 91 }, { 127, 92 }, { 127, 93 },
                        {  47, 85 }, {  48, 85 },
                        {  32, 97 },
                    },
                    OWNERSHIP_CONSTRUCTION_RIGHTS_AVAILABLE, true);
                FixLandOwnershipTilesWithOwnership(
                    {
                        {  98, 64 }, {  98, 65 }, {  98, 66 },
                        {  96, 84 },
                    },
                    OWNERSHIP_CONSTRUCTION_RIGHTS_OWNED, true);
                // clang-format on
            }
            else if (
                String::Equals(gScenarioFileName, "Six Flags Magic Mountain.SC6", true)
                || String::Equals(gScenarioFileName, "six flags magic mountain.sea", true))
            {
                // clang-format off
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 104, 190 }, { 105, 190 }, { 108, 197 }, 
                        { 75, 167 }, 
                        { 61, 92 }, { 61, 93 }, { 61, 94 }, { 61, 95 }, { 62, 90 }, { 62, 91 }, { 62, 92 }, { 62, 93 }, { 62, 94 },
                        { 92, 57 }, { 93, 57 },
                        { 89, 40 }, { 89, 41 }, { 89, 42 }, { 90, 42 }, 
                        { 168, 20 }, { 169, 20 },
                    },
                    OWNERSHIP_AVAILABLE, true);
                // clang-format on
            }
            else if (String::Equals(_s6.ScenarioFilename, "Great Wall of China Tourism Enhancement.SC6"))
            {
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 127, 31 },
                    },
                    OWNERSHIP_OWNED);
            }
            else if (
                String::Equals(gScenarioFileName, "N America - Extreme Hawaiian Island.SC6", true)
                || String::Equals(gScenarioFileName, "n america - extreme hawaiian island.sea", true))
            {
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 132, 124 },
                        { 133, 124 },
                        { 133, 125 },
                        { 133, 126 },
                        { 119, 35 },
                        { 132, 62 },
                        { 133, 67 },
                        { 136, 71 },
                        { 87, 33 },
                        { 87, 34 },
                        { 90, 36 },
                        { 91, 36 },
                    },
                    OWNERSHIP_OWNED);
                // We set the doNotDowngrade flag for cases where the player has used a cheat to own all land.
                FixLandOwnershipTilesWithOwnership(
                    {
                        { 49, 99 },
                        { 50, 99 },
                        { 88, 110 },
                    },
                    OWNERSHIP_AVAILABLE, true);
            }
        }

        void FixWater() const
        {
            if (!_isScenario)
            {
                return;
            }
            if (String::Equals(_s6.ScenarioFilename, "Infernal Views.SC6", true)
                || String::Equals(_s6.ScenarioFilename, "infernal views.sea", true))
            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ 45, 62 }.ToCoordsXY());

                surfaceElement->SetWaterHeight(96);
            }
            else if (
                String::Equals(_s6.ScenarioFilename, "Six Flags Holland.SC6")
                || String::Equals(_s6.ScenarioFilename, "six flags holland.sea", true))

            {
                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ 126, 73 }.ToCoordsXY());

                surfaceElement->SetWaterHeight(96);
            }
        }

        void FixAyersRockScenario() const
        {
            if (!_isScenario || !String::Equals(_s6.ScenarioFilename, "Australasia - Ayers Rock.SC6"))
                return;

            TileCoordsXY tilesToUncovered[] = {
                { 123, 59 }, { 123, 60 }, { 123, 61 }, { 118, 69 }, { 118, 70 }, { 118, 71 },
                { 118, 72 }, { 118, 73 }, { 112, 79 }, { 112, 80 }, { 112, 81 }, { 112, 82 },
            };
            for (const auto& tile : tilesToUncovered)
            {
                auto* tileElement = MapGetFirstElementAt(tile);
                if (tileElement == nullptr)
                    continue;

                do
                {
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;

                    auto* trackElement = tileElement->AsTrack();
                    if (trackElement->GetTrackType() != TrackElemType::FlatCovered)
                        continue;

                    trackElement->SetTrackType(TrackElemType::Flat);
                } while (!(tileElement++)->IsLastForTile());
            }

            TileCoordsXY tilesToCovered[] = {
                { 123, 83 },
                { 123, 84 },
                { 123, 85 },
                { 123, 86 },
            };
            for (const auto& tile : tilesToCovered)
            {
                auto* tileElement = MapGetFirstElementAt(tile);
                if (tileElement == nullptr)
                    continue;

                do
                {
                    if (tileElement->GetType() != TileElementType::Track)
                        continue;

                    auto* trackElement = tileElement->AsTrack();
                    if (trackElement->GetTrackType() != TrackElemType::Flat)
                        continue;

                    trackElement->SetTrackType(TrackElemType::FlatCovered);
                } while (!(tileElement++)->IsLastForTile());
            }
        }

        void ImportRides()
        {
            for (uint8_t index = 0; index < Limits::MaxRidesInPark; index++)
            {
                auto src = &_s6.Rides[index];
                if (src->Type != RIDE_TYPE_NULL)
                {
                    const auto rideId = RideId::FromUnderlying(index);
                    auto dst = RideAllocateAtIndex(rideId);
                    ImportRide(dst, src, rideId);
                }
            }
        }

        /**
         * This code is needed to detect hacks where a tracked ride has been made invisible
         * by setting its ride type to a flat ride.
         *
         * The function should classify rides as follows:
         * 1. If the ride type is tracked and its vehicles also belong on tracks, it should be classified as tracked.
         * 2. If the ride type is a flat ride, but its vehicles belong on tracks,
         *     it should be classified as tracked (Crooked House mod).
         * 3. If the ride type is tracked and its vehicles belong to a flat ride, it should be classified as tracked.
         * 4. If the ride type is a flat ride and its vehicles also belong to a flat ride, it should be classified as a flat
         * ride.
         */
        void DetermineFlatRideStatus()
        {
            for (uint8_t index = 0; index < Limits::MaxRidesInPark; index++)
            {
                auto src = &_s6.Rides[index];
                if (src->Type == RIDE_TYPE_NULL)
                    continue;

                auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->Subtype);
                auto* rideEntry = GetRideEntryByIndex(subtype);
                // If the ride is tracked, we don’t need to check the vehicle any more.
                if (!GetRideTypeDescriptor(src->Type).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                {
                    _isFlatRide[index] = false;
                    continue;
                }

                // We have established the ride type is a flat ride, which means the vehicle now determines whether it is a
                // true flat ride (scenario 4) or a tracked ride with an invisibility hack (scenario 2).
                ObjectEntryIndex originalRideType = src->Type;
                if (rideEntry != nullptr)
                {
                    originalRideType = rideEntry->GetFirstNonNullRideType();
                }
                const auto isFlatRide = GetRideTypeDescriptor(originalRideType).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE);
                _isFlatRide.set(static_cast<size_t>(index), isFlatRide);
            }
        }

        bool IsFlatRide(const uint8_t rct12RideIndex)
        {
            if (rct12RideIndex == RCT12_RIDE_ID_NULL)
                return false;
            return _isFlatRide[rct12RideIndex];
        }

        void ImportRide(::Ride* dst, const RCT2::Ride* src, const RideId rideIndex)
        {
            *dst = {};
            dst->id = rideIndex;

            ObjectEntryIndex rideType = src->Type;
            auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->Subtype);
            if (RCT2RideTypeNeedsConversion(src->Type))
            {
                auto* rideEntry = GetRideEntryByIndex(subtype);
                if (rideEntry != nullptr)
                {
                    rideType = RCT2RideTypeToOpenRCT2RideType(src->Type, *rideEntry);
                }
            }

            if (rideType >= RIDE_TYPE_COUNT)
            {
                LOG_ERROR("Invalid ride type for a ride in this save.");
                throw UnsupportedRideTypeException(rideType);
            }
            dst->type = rideType;
            dst->subtype = subtype;
            // Pad002;
            dst->mode = static_cast<RideMode>(src->Mode);
            dst->colour_scheme_type = src->ColourSchemeType;

            for (uint8_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicle_colours[i].Body = src->VehicleColours[i].BodyColour;
                dst->vehicle_colours[i].Trim = src->VehicleColours[i].TrimColour;
            }

            // Pad046;
            dst->status = static_cast<RideStatus>(src->Status);

            dst->default_name_number = src->NameArgumentsNumber;
            if (IsUserStringID(src->Name))
            {
                dst->custom_name = GetUserString(src->Name);
            }
            else
            {
                dst->default_name_number = src->NameArgumentsNumber;
            }

            if (src->OverallView.IsNull())
            {
                dst->overall_view.SetNull();
            }
            else
            {
                auto tileLoc = TileCoordsXY(src->OverallView.x, src->OverallView.y);
                dst->overall_view = tileLoc.ToCoordsXY();
            }

            for (StationIndex::UnderlyingType i = 0; i < Limits::MaxStationsPerRide; i++)
            {
                StationIndex stationIndex = StationIndex::FromUnderlying(i);
                auto& destStation = dst->GetStation(stationIndex);

                if (src->StationStarts[i].IsNull())
                {
                    destStation.Start.SetNull();
                }
                else
                {
                    auto tileStartLoc = TileCoordsXY(src->StationStarts[i].x, src->StationStarts[i].y);
                    destStation.Start = tileStartLoc.ToCoordsXY();
                }
                destStation.Height = src->StationHeights[i];
                destStation.Length = src->StationLength[i];
                destStation.Depart = src->StationDepart[i];
                destStation.TrainAtStation = src->TrainAtStation[i];
                // Direction is fixed later.

                if (src->Entrances[i].IsNull())
                    destStation.Entrance.SetNull();
                else
                    destStation.Entrance = { src->Entrances[i].x, src->Entrances[i].y, src->StationHeights[i], 0 };

                if (src->Exits[i].IsNull())
                    destStation.Exit.SetNull();
                else
                    destStation.Exit = { src->Exits[i].x, src->Exits[i].y, src->StationHeights[i], 0 };

                destStation.LastPeepInQueue = EntityId::FromUnderlying(src->LastPeepInQueue[i]);

                destStation.SegmentLength = src->Length[i];
                destStation.SegmentTime = src->Time[i];

                destStation.QueueTime = src->QueueTime[i];

                destStation.QueueLength = src->QueueLength[i];
            }
            // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
            for (int32_t i = Limits::MaxStationsPerRide; i < OpenRCT2::Limits::MaxStationsPerRide; i++)
            {
                StationIndex stationIndex = StationIndex::FromUnderlying(i);
                auto& destStation = dst->GetStation(stationIndex);

                destStation.Start.SetNull();
                destStation.TrainAtStation = RideStation::NO_TRAIN;
                destStation.Entrance.SetNull();
                destStation.Exit.SetNull();
                destStation.LastPeepInQueue = EntityId::GetNull();
            }

            for (int32_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->Vehicles[i]);
            }
            for (int32_t i = Limits::MaxTrainsPerRide - 1; i <= OpenRCT2::Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->depart_flags = src->DepartFlags;

            dst->num_stations = src->NumStations;
            dst->NumTrains = src->NumTrains;
            dst->num_cars_per_train = src->NumCarsPerTrain;
            dst->ProposedNumTrains = src->ProposedNumTrains;
            dst->proposed_num_cars_per_train = src->ProposedNumCarsPerTrain;
            dst->max_trains = src->MaxTrains;
            dst->MinCarsPerTrain = src->GetMinCarsPerTrain();
            dst->MaxCarsPerTrain = src->GetMaxCarsPerTrain();
            dst->min_waiting_time = src->MinWaitingTime;
            dst->max_waiting_time = src->MaxWaitingTime;

            // Includes time_limit, NumLaps, launch_speed, speed, rotations
            dst->operation_option = src->OperationOption;

            dst->boat_hire_return_direction = src->BoatHireReturnDirection;
            dst->boat_hire_return_position = { src->BoatHireReturnPosition.x, src->BoatHireReturnPosition.y };

            dst->special_track_elements = src->SpecialTrackElements;
            // Pad0D6[2];

            dst->max_speed = src->MaxSpeed;
            dst->average_speed = src->AverageSpeed;
            dst->current_test_segment = src->CurrentTestSegment;
            dst->average_speed_test_timeout = src->AverageSpeedTestTimeout;
            // Pad0E2[0x2];

            dst->max_positive_vertical_g = src->MaxPositiveVerticalG;
            dst->max_negative_vertical_g = src->MaxNegativeVerticalG;
            dst->max_lateral_g = src->MaxLateralG;
            dst->previous_vertical_g = src->PreviousVerticalG;
            dst->previous_lateral_g = src->PreviousLateralG;
            // Pad106[0x2];
            dst->testing_flags = src->TestingFlags;

            if (src->CurTestTrackLocation.IsNull())
            {
                dst->CurTestTrackLocation.SetNull();
            }
            else
            {
                dst->CurTestTrackLocation = { src->CurTestTrackLocation.x, src->CurTestTrackLocation.y, src->CurTestTrackZ };
            }

            dst->turn_count_default = src->TurnCountDefault;
            dst->turn_count_banked = src->TurnCountBanked;
            dst->turn_count_sloped = src->TurnCountSloped;
            if (dst->type == RIDE_TYPE_MINI_GOLF)
                dst->holes = src->Inversions & 0x1F;
            else
                dst->inversions = src->Inversions & 0x1F;
            dst->sheltered_eighths = src->Inversions >> 5;
            dst->drops = src->Drops;
            dst->start_drop_height = src->StartDropHeight;
            dst->highest_drop_height = src->HighestDropHeight;
            dst->sheltered_length = src->ShelteredLength;
            dst->var_11C = src->Var11C;
            dst->num_sheltered_sections = src->NumShelteredSections;

            dst->cur_num_customers = src->CurNumCustomers;
            dst->num_customers_timeout = src->NumCustomersTimeout;

            for (uint8_t i = 0; i < Limits::CustomerHistorySize; i++)
            {
                dst->num_customers[i] = src->NumCustomers[i];
            }

            dst->price[0] = src->Price;

            for (uint8_t i = 0; i < 2; i++)
            {
                dst->ChairliftBullwheelLocation[i] = { src->ChairliftBullwheelLocation[i].x,
                                                       src->ChairliftBullwheelLocation[i].y, src->ChairliftBullwheelZ[i] };
            }

            dst->ratings = src->Ratings;
            dst->value = ToMoney64(src->Value);

            dst->chairlift_bullwheel_rotation = src->ChairliftBullwheelRotation;

            dst->satisfaction = src->Satisfaction;
            dst->satisfaction_time_out = src->SatisfactionTimeOut;
            dst->satisfaction_next = src->SatisfactionNext;

            dst->window_invalidate_flags = src->WindowInvalidateFlags;
            // Pad14E[0x02];

            dst->total_customers = src->TotalCustomers;
            dst->total_profit = ToMoney64(src->TotalProfit);
            dst->popularity = src->Popularity;
            dst->popularity_time_out = src->PopularityTimeOut;
            dst->popularity_next = src->PopularityNext;

            ImportNumRiders(dst, rideIndex);

            dst->music_tune_id = src->MusicTuneId;
            dst->slide_in_use = src->SlideInUse;
            // Includes maze_tiles
            dst->slide_peep = EntityId::FromUnderlying(src->SlidePeep);
            // Pad160[0xE];
            dst->slide_peep_t_shirt_colour = src->SlidePeepTShirtColour;
            // Pad16F[0x7];
            dst->spiral_slide_progress = src->SpiralSlideProgress;
            // Pad177[0x9];
            dst->build_date = static_cast<int32_t>(src->BuildDate);
            dst->upkeep_cost = ToMoney64(src->UpkeepCost);
            dst->race_winner = EntityId::FromUnderlying(src->RaceWinner);
            // Pad186[0x02];
            dst->music_position = src->MusicPosition;

            dst->breakdown_reason_pending = src->BreakdownReasonPending;
            dst->mechanic_status = src->MechanicStatus;
            dst->mechanic = EntityId::FromUnderlying(src->Mechanic);
            dst->inspection_station = StationIndex::FromUnderlying(src->InspectionStation);
            dst->broken_vehicle = src->BrokenVehicle;
            dst->broken_car = src->BrokenCar;
            dst->breakdown_reason = src->BreakdownReason;

            dst->price[1] = src->PriceSecondary;

            dst->reliability = src->Reliability;
            dst->unreliability_factor = src->UnreliabilityFactor;
            dst->downtime = src->Downtime;
            dst->inspection_interval = src->InspectionInterval;
            dst->last_inspection = src->LastInspection;

            for (uint8_t i = 0; i < Limits::DowntimeHistorySize; i++)
            {
                dst->downtime_history[i] = src->DowntimeHistory[i];
            }

            dst->no_primary_items_sold = src->NoPrimaryItemsSold;
            dst->no_secondary_items_sold = src->NoSecondaryItemsSold;

            dst->breakdown_sound_modifier = src->BreakdownSoundModifier;
            dst->not_fixed_timeout = src->NotFixedTimeout;
            dst->last_crash_type = src->LastCrashType;
            dst->connected_message_throttle = src->ConnectedMessageThrottle;

            dst->income_per_hour = ToMoney64(src->IncomePerHour);
            dst->profit = ToMoney64(src->Profit);

            for (uint8_t i = 0; i < Limits::NumColourSchemes; i++)
            {
                dst->track_colour[i].main = src->TrackColourMain[i];
                dst->track_colour[i].additional = src->TrackColourAdditional[i];
                dst->track_colour[i].supports = src->TrackColourSupports[i];
            }
            // This stall was not colourable in RCT2.
            if (dst->type == RIDE_TYPE_FOOD_STALL)
            {
                auto object = ObjectEntryGetObject(ObjectType::Ride, dst->subtype);
                if (object != nullptr && object->GetIdentifier() == "rct2.ride.icecr1")
                {
                    dst->track_colour[0].main = COLOUR_LIGHT_BLUE;
                }
            }

            auto musicStyle = OBJECT_ENTRY_INDEX_NULL;
            if (GetRideTypeDescriptor(dst->type).HasFlag(RIDE_TYPE_FLAG_ALLOW_MUSIC))
            {
                musicStyle = src->Music;
            }
            dst->music = musicStyle;

            // In SV7, "plain" entrances are invisible.
            auto entranceStyle = OBJECT_ENTRY_INDEX_NULL;
            if (!_isSV7 && GetRideTypeDescriptor(dst->type).HasFlag(RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT))
            {
                entranceStyle = src->EntranceStyle;
            }
            dst->entrance_style = entranceStyle;

            dst->vehicle_change_timeout = src->VehicleChangeTimeout;
            dst->num_block_brakes = src->NumBlockBrakes;
            dst->lift_hill_speed = src->LiftHillSpeed;
            dst->guests_favourite = src->GuestsFavourite;
            dst->lifecycle_flags = src->LifecycleFlags;

            for (uint8_t i = 0; i < Limits::MaxTrainsPerRide; i++)
            {
                dst->vehicle_colours[i].Tertiary = src->VehicleColoursExtended[i];
            }

            dst->total_air_time = src->TotalAirTime;
            dst->current_test_station = StationIndex::FromUnderlying(src->CurrentTestStation);
            dst->num_circuits = src->NumCircuits;
            dst->CableLiftLoc = { src->CableLiftX, src->CableLiftY, src->CableLiftZ * COORDS_Z_STEP };
            // Pad1FD;
            dst->cable_lift = EntityId::FromUnderlying(src->CableLift);

            // Pad208[0x58];
        }

        void ImportRideRatingsCalcData()
        {
            const auto& src = _s6.RideRatingsCalcData;
            // S6 has only one state, ensure we reset all states before reading the first one.
            RideRatingResetUpdateStates();
            auto& rideRatingStates = RideRatingGetUpdateStates();
            auto& dst = rideRatingStates[0];
            dst = {};
            dst.Proximity = { src.ProximityX, src.ProximityY, src.ProximityZ };
            dst.ProximityStart = { src.ProximityStartX, src.ProximityStartY, src.ProximityStartZ };
            dst.CurrentRide = RCT12RideIdToOpenRCT2RideId(src.CurrentRide);
            dst.State = src.State;
            if (src.CurrentRide < Limits::MaxRidesInPark && _s6.Rides[src.CurrentRide].Type < std::size(RideTypeDescriptors))
                dst.ProximityTrackType = RCT2TrackTypeToOpenRCT2(
                    src.ProximityTrackType, _s6.Rides[src.CurrentRide].Type, IsFlatRide(src.CurrentRide));
            else
                dst.ProximityTrackType = 0xFF;
            dst.ProximityBaseHeight = src.ProximityBaseHeight;
            dst.ProximityTotal = src.ProximityTotal;
            for (size_t i = 0; i < std::size(src.ProximityScores); i++)
            {
                dst.ProximityScores[i] = src.ProximityScores[i];
            }
            dst.AmountOfBrakes = src.NumBrakes;
            dst.AmountOfReversers = src.NumReversers;
            dst.StationFlags = src.StationFlags;
        }

        void ImportRideMeasurements()
        {
            for (const auto& src : _s6.RideMeasurements)
            {
                if (src.RideIndex != RCT12_RIDE_ID_NULL)
                {
                    const auto rideId = RideId::FromUnderlying(src.RideIndex);
                    auto ride = GetRide(rideId);
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
                dst.velocity[i] = src.Velocity[i];
                dst.altitude[i] = src.Altitude[i];
                dst.vertical[i] = src.Vertical[i];
                dst.lateral[i] = src.Lateral[i];
            }
        }

        void ImportResearchList()
        {
            bool invented = true;
            for (const auto& researchItem : _s6.ResearchItems)
            {
                if (researchItem.IsInventedEndMarker())
                {
                    invented = false;
                    continue;
                }
                if (researchItem.IsUninventedEndMarker() || researchItem.IsRandomEndMarker())
                {
                    break;
                }

                if (invented)
                    gResearchItemsInvented.emplace_back(researchItem.ToResearchItem());
                else
                    gResearchItemsUninvented.emplace_back(researchItem.ToResearchItem());
            }
        }

        void ImportBanner(Banner* dst, const RCT12Banner* src)
        {
            auto id = dst->id;

            *dst = {};
            dst->id = id;
            dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->Type);
            dst->flags = src->Flags;

            if (!(src->Flags & BANNER_FLAG_LINKED_TO_RIDE) && IsUserStringID(src->StringID))
            {
                dst->text = GetUserString(src->StringID);
            }

            if (src->Flags & BANNER_FLAG_LINKED_TO_RIDE)
            {
                dst->ride_index = RCT12RideIdToOpenRCT2RideId(src->RideIndex);
            }
            else
            {
                dst->colour = src->Colour;
            }

            dst->text_colour = src->TextColour;
            dst->position.x = src->x;
            dst->position.y = src->y;
        }

        void Initialise()
        {
            OpenRCT2::GetContext()->GetGameState()->InitAll({ _s6.MapSize, _s6.MapSize });
        }

        /**
         * Imports guest entry points.
         * Includes fixes for incorrectly set guest entry points in some scenarios.
         */
        void ImportPeepSpawns()
        {
            // Many WW and TT have scenario_filename fields containing an incorrect filename. Check for both this filename
            // and the corrected filename.

            // In this park, peep_spawns[0] is incorrect, and peep_spawns[1] is correct.
            if (String::Equals(_s6.ScenarioFilename, "WW South America - Rio Carnival.SC6")
                || String::Equals(_s6.ScenarioFilename, "South America - Rio Carnival.SC6"))
            {
                _s6.PeepSpawns[0] = { 2160, 3167, 6, 1 };
                _s6.PeepSpawns[1].x = RCT12_PEEP_SPAWN_UNDEFINED;
            }
            // In this park, peep_spawns[0] is correct. Just clear the other.
            else if (
                String::Equals(_s6.ScenarioFilename, "Great Wall of China Tourism Enhancement.SC6")
                || String::Equals(_s6.ScenarioFilename, "Asia - Great Wall of China Tourism Enhancement.SC6"))
            {
                _s6.PeepSpawns[1].x = RCT12_PEEP_SPAWN_UNDEFINED;
            }
            // Amity Airfield has peeps entering from the corner of the tile, instead of the middle.
            else if (String::Equals(_s6.ScenarioFilename, "Amity Airfield.SC6"))
            {
                _s6.PeepSpawns[0].y = 1296;
            }
            // #9926: Africa - Oasis has peeps spawning on the edge underground near the entrance
            else if (String::Equals(_s6.ScenarioFilename, "Africa - Oasis.SC6"))
            {
                _s6.PeepSpawns[0].y = 2128;
                _s6.PeepSpawns[0].z = 7;
            }

            gPeepSpawns.clear();
            for (size_t i = 0; i < Limits::MaxPeepSpawns; i++)
            {
                if (_s6.PeepSpawns[i].x != RCT12_PEEP_SPAWN_UNDEFINED)
                {
                    PeepSpawn spawn = { _s6.PeepSpawns[i].x, _s6.PeepSpawns[i].y, _s6.PeepSpawns[i].z * 16,
                                        _s6.PeepSpawns[i].direction };
                    gPeepSpawns.push_back(spawn);
                }
            }
        }

        void ImportNumRiders(::Ride* dst, const RideId rideIndex)
        {
            // The number of riders might have overflown or underflown. Re-calculate the value.
            uint16_t numRiders = 0;
            for (int32_t i = 0; i < GetMaxEntities(); i++)
            {
                const auto& entity = _s6.Entities[i];
                if (entity.Unknown.EntityIdentifier == RCT12EntityIdentifier::Peep)
                {
                    if (entity.Peep.CurrentRide == static_cast<RCT12RideId>(rideIndex.ToUnderlying())
                        && (static_cast<PeepState>(entity.Peep.State) == PeepState::OnRide
                            || static_cast<PeepState>(entity.Peep.State) == PeepState::EnteringRide))
                    {
                        numRiders++;
                    }
                }
            }
            dst->num_riders = numRiders;
        }

        void ImportTileElements()
        {
            // Build tile pointer cache (needed to get the first element at a certain location)
            auto tilePointerIndex = TilePointerIndex<RCT12TileElement>(
                Limits::MaxMapSize, _s6.TileElements, std::size(_s6.TileElements));

            std::vector<TileElement> tileElements;
            bool nextElementInvisible = false;
            bool restOfTileInvisible = false;
            const auto maxSize = std::min(Limits::MaxMapSize, _s6.MapSize);
            for (TileCoordsXY coords = { 0, 0 }; coords.y < MAXIMUM_MAP_SIZE_TECHNICAL; coords.y++)
            {
                for (coords.x = 0; coords.x < MAXIMUM_MAP_SIZE_TECHNICAL; coords.x++)
                {
                    nextElementInvisible = false;
                    restOfTileInvisible = false;

                    auto tileAdded = false;
                    if (coords.x < maxSize && coords.y < maxSize)
                    {
                        const auto* srcElement = tilePointerIndex.GetFirstElementAt(coords);
                        if (srcElement != nullptr)
                        {
                            do
                            {
                                if (srcElement->BaseHeight == RCT12::Limits::MaxElementHeight)
                                {
                                    continue;
                                }

                                auto tileElementType = srcElement->GetType();
                                if (tileElementType == RCT12TileElementType::Corrupt)
                                {
                                    // One property of corrupt elements was to hide tops of tower tracks, and to avoid the next
                                    // element from being hidden, multiple consecutive corrupt elements were sometimes used.
                                    // This would essentially toggle the flag, so we inverse nextElementInvisible here instead
                                    // of always setting it to true.
                                    nextElementInvisible = !nextElementInvisible;
                                    continue;
                                }
                                if (tileElementType == RCT12TileElementType::EightCarsCorrupt14
                                    || tileElementType == RCT12TileElementType::EightCarsCorrupt15)
                                {
                                    restOfTileInvisible = true;
                                    continue;
                                }

                                auto& dstElement = tileElements.emplace_back();
                                ImportTileElement(&dstElement, srcElement, nextElementInvisible || restOfTileInvisible);
                                nextElementInvisible = false;
                                tileAdded = true;
                            } while (!(srcElement++)->IsLastForTile());
                        }
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
        }

        void ImportTileElement(TileElement* dst, const RCT12TileElement* src, bool invisible)
        {
            const auto rct12Type = src->GetType();
            dst->ClearAs(ToOpenRCT2TileElementType(rct12Type));
            dst->SetDirection(src->GetDirection());
            dst->SetBaseZ(src->BaseHeight * COORDS_Z_STEP);
            dst->SetClearanceZ(src->ClearanceHeight * COORDS_Z_STEP);

            // All saved in "flags"
            dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
            dst->SetGhost(src->IsGhost());
            dst->SetLastForTile(src->IsLastForTile());
            dst->SetInvisible(invisible);

            switch (rct12Type)
            {
                case RCT12TileElementType::Surface:
                {
                    auto dst2 = dst->AsSurface();
                    auto src2 = src->AsSurface();

                    dst2->SetSlope(src2->GetSlope());

                    dst2->SetSurfaceStyle(src2->GetSurfaceStyle());
                    dst2->SetEdgeStyle(src2->GetEdgeStyle());

                    dst2->SetGrassLength(src2->GetGrassLength());
                    dst2->SetOwnership(src2->GetOwnership());
                    dst2->SetParkFences(src2->GetParkFences());
                    dst2->SetWaterHeight(src2->GetWaterHeight());
                    dst2->SetHasTrackThatNeedsWater(src2->HasTrackThatNeedsWater());

                    break;
                }
                case RCT12TileElementType::Path:
                {
                    auto dst2 = dst->AsPath();
                    auto src2 = src->AsPath();

                    auto pathEntryIndex = src2->GetEntryIndex();
                    auto surfaceEntry = src2->IsQueue() ? _pathToQueueSurfaceMap[pathEntryIndex]
                                                        : _pathToSurfaceMap[pathEntryIndex];
                    if (surfaceEntry == OBJECT_ENTRY_INDEX_NULL)
                    {
                        // Legacy footpath object
                        dst2->SetLegacyPathEntryIndex(pathEntryIndex);
                    }
                    else
                    {
                        // Surface / railing
                        dst2->SetSurfaceEntryIndex(surfaceEntry);
                        dst2->SetRailingsEntryIndex(_pathToRailingMap[pathEntryIndex]);
                    }

                    dst2->SetQueueBannerDirection(src2->GetQueueBannerDirection());
                    dst2->SetSloped(src2->IsSloped());
                    dst2->SetSlopeDirection(src2->GetSlopeDirection());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetWide(src2->IsWide());
                    dst2->SetIsQueue(src2->IsQueue());
                    dst2->SetHasQueueBanner(src2->HasQueueBanner());
                    dst2->SetEdges(src2->GetEdges());
                    dst2->SetCorners(src2->GetCorners());
                    dst2->SetAddition(src2->GetAddition());
                    dst2->SetAdditionIsGhost(src2->AdditionIsGhost());
                    dst2->SetAdditionStatus(src2->GetAdditionStatus());
                    dst2->SetIsBroken(src2->IsBroken());
                    dst2->SetIsBlockedByVehicle(src2->IsBlockedByVehicle());

                    break;
                }
                case RCT12TileElementType::Track:
                {
                    auto dst2 = dst->AsTrack();
                    auto src2 = src->AsTrack();

                    auto rideType = _s6.Rides[src2->GetRideIndex()].Type;
                    track_type_t trackType = static_cast<track_type_t>(src2->GetTrackType());

                    dst2->SetTrackType(RCT2TrackTypeToOpenRCT2(trackType, rideType, IsFlatRide(src2->GetRideIndex())));
                    dst2->SetRideType(rideType);
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetColourScheme(src2->GetColourScheme());
                    dst2->SetHasChain(src2->HasChain());
                    dst2->SetHasCableLift(src2->HasCableLift());
                    dst2->SetInverted(src2->IsInverted());
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetHasGreenLight(src2->HasGreenLight());
                    dst2->SetBrakeClosed(src2->BlockBrakeClosed());
                    dst2->SetIsIndestructible(src2->IsIndestructible());
                    // Skipping IsHighlighted()

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
                    else if (rideType == RIDE_TYPE_GHOST_TRAIN)
                    {
                        dst2->SetDoorAState(src2->GetDoorAState());
                        dst2->SetDoorBState(src2->GetDoorBState());
                    }
                    else
                    {
                        dst2->SetSeatRotation(src2->GetSeatRotation());
                    }

                    if (TrackTypeMustBeMadeInvisible(rideType, dst2->GetTrackType()))
                    {
                        dst->SetInvisible(true);
                    }

                    break;
                }
                case RCT12TileElementType::SmallScenery:
                {
                    auto dst2 = dst->AsSmallScenery();
                    auto src2 = src->AsSmallScenery();

                    dst2->SetEntryIndex(src2->GetEntryIndex());
                    dst2->SetAge(src2->GetAge());
                    dst2->SetSceneryQuadrant(src2->GetSceneryQuadrant());
                    dst2->SetSecondaryColour(src2->GetSecondaryColour());
                    if (src2->NeedsSupports())
                        dst2->SetNeedsSupports();
                    dst2->SetPrimaryColour(src2->GetPrimaryColour() & RCT12_TILE_ELEMENT_COLOUR_MASK);

                    break;
                }
                case RCT12TileElementType::Entrance:
                {
                    auto dst2 = dst->AsEntrance();
                    auto src2 = src->AsEntrance();

                    dst2->SetEntranceType(src2->GetEntranceType());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());

                    if (src2->GetSequenceIndex() == 0)
                    {
                        auto pathEntryIndex = src2->GetPathType();
                        auto surfaceEntry = _pathToSurfaceMap[pathEntryIndex];
                        if (surfaceEntry == OBJECT_ENTRY_INDEX_NULL)
                        {
                            // Legacy footpath object
                            dst2->SetLegacyPathEntryIndex(pathEntryIndex);
                        }
                        else
                        {
                            // Surface
                            dst2->SetSurfaceEntryIndex(surfaceEntry);
                        }
                    }
                    else
                    {
                        dst2->SetSurfaceEntryIndex(OBJECT_ENTRY_INDEX_NULL);
                    }
                    break;
                }
                case RCT12TileElementType::Wall:
                {
                    auto dst2 = dst->AsWall();
                    auto src2 = src->AsWall();

                    dst2->SetEntryIndex(src2->GetEntryIndex());
                    dst2->SetSlope(src2->GetSlope());
                    dst2->SetPrimaryColour(src2->GetPrimaryColour());
                    dst2->SetSecondaryColour(src2->GetSecondaryColour());
                    dst2->SetTertiaryColour(src2->GetTertiaryColour());
                    dst2->SetAnimationFrame(src2->GetAnimationFrame());
                    dst2->SetAcrossTrack(src2->IsAcrossTrack());
                    dst2->SetAnimationIsBackwards(src2->AnimationIsBackwards());

                    // Import banner information
                    dst2->SetBannerIndex(BannerIndex::GetNull());
                    auto entry = dst2->GetEntry();
                    if (entry != nullptr && entry->scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        auto bannerIndex = src2->GetBannerIndex();
                        if (bannerIndex < std::size(_s6.Banners))
                        {
                            auto srcBanner = &_s6.Banners[bannerIndex];
                            auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                            if (dstBanner == nullptr)
                            {
                                dst2->SetBannerIndex(BannerIndex::GetNull());
                            }
                            else
                            {
                                ImportBanner(dstBanner, srcBanner);
                                dst2->SetBannerIndex(BannerIndex::FromUnderlying(src2->GetBannerIndex()));
                            }
                        }
                    }
                    break;
                }
                case RCT12TileElementType::LargeScenery:
                {
                    auto dst2 = dst->AsLargeScenery();
                    auto src2 = src->AsLargeScenery();

                    dst2->SetEntryIndex(src2->GetEntryIndex());
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetPrimaryColour(src2->GetPrimaryColour());
                    dst2->SetSecondaryColour(src2->GetSecondaryColour());

                    // Import banner information
                    dst2->SetBannerIndex(BannerIndex::GetNull());
                    auto entry = dst2->GetEntry();
                    if (entry != nullptr && entry->scrolling_mode != SCROLLING_MODE_NONE)
                    {
                        auto bannerIndex = src2->GetBannerIndex();
                        if (bannerIndex < std::size(_s6.Banners))
                        {
                            auto srcBanner = &_s6.Banners[bannerIndex];
                            auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                            if (dstBanner == nullptr)
                            {
                                dst2->SetBannerIndex(BannerIndex::GetNull());
                            }
                            else
                            {
                                ImportBanner(dstBanner, srcBanner);
                                dst2->SetBannerIndex(BannerIndex::FromUnderlying(src2->GetBannerIndex()));
                            }
                        }
                    }
                    break;
                }
                case RCT12TileElementType::Banner:
                {
                    auto dst2 = dst->AsBanner();
                    auto src2 = src->AsBanner();

                    dst2->SetPosition(src2->GetPosition());
                    dst2->SetAllowedEdges(src2->GetAllowedEdges());

                    auto bannerIndex = src2->GetIndex();
                    if (bannerIndex < std::size(_s6.Banners))
                    {
                        auto srcBanner = &_s6.Banners[bannerIndex];
                        auto dstBanner = GetOrCreateBanner(BannerIndex::FromUnderlying(bannerIndex));
                        if (dstBanner == nullptr)
                        {
                            dst2->SetIndex(BannerIndex::GetNull());
                        }
                        else
                        {
                            ImportBanner(dstBanner, srcBanner);
                            dst2->SetIndex(BannerIndex::FromUnderlying(bannerIndex));
                        }
                    }
                    else
                    {
                        dst2->SetIndex(BannerIndex::GetNull());
                    }
                    break;
                }
                default:
                    assert(false);
            }
        }

        void ImportMarketingCampaigns()
        {
            for (size_t i = 0; i < ADVERTISING_CAMPAIGN_COUNT; i++)
            {
                if (_s6.CampaignWeeksLeft[i] & CAMPAIGN_ACTIVE_FLAG)
                {
                    MarketingCampaign campaign{};
                    campaign.Type = static_cast<uint8_t>(i);
                    campaign.WeeksLeft = _s6.CampaignWeeksLeft[i] & ~(CAMPAIGN_ACTIVE_FLAG | CAMPAIGN_FIRST_WEEK_FLAG);
                    if ((_s6.CampaignWeeksLeft[i] & CAMPAIGN_FIRST_WEEK_FLAG) != 0)
                    {
                        campaign.Flags |= MarketingCampaignFlags::FIRST_WEEK;
                    }
                    if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                    {
                        campaign.RideId = RCT12RideIdToOpenRCT2RideId(_s6.CampaignRideIndex[i]);
                    }
                    else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        campaign.ShopItemType = ShopItem(_s6.CampaignRideIndex[i]);
                    }
                    gMarketingCampaigns.push_back(campaign);
                }
            }
        }

        void ImportStaffPatrolArea(Staff* staffmember, uint8_t staffId)
        {
            // First check staff mode as vanilla did not clean up patrol areas when switching from patrol to walk
            // without doing this we could accidentally add a patrol when it didn't exist.
            if (_s6.StaffModes[staffId] != StaffMode::Patrol)
            {
                return;
            }
            int32_t peepOffset = staffId * Limits::PatrolAreaSize;
            for (int32_t i = 0; i < Limits::PatrolAreaSize; i++)
            {
                if (_s6.PatrolAreas[peepOffset + i] == 0)
                {
                    // No patrol for this area
                    continue;
                }

                // Loop over the bits of the uint32_t
                for (int32_t j = 0; j < 32; j++)
                {
                    int8_t bit = (_s6.PatrolAreas[peepOffset + i] >> j) & 1;
                    if (bit == 0)
                    {
                        // No patrol for this area
                        continue;
                    }
                    // val contains the 6 highest bits of both the x and y coordinates
                    int32_t val = j | (i << 5);
                    int32_t x = val & 0x03F;
                    x <<= 7;
                    int32_t y = val & 0xFC0;
                    y <<= 1;
                    staffmember->SetPatrolArea(
                        MapRange(x, y, x + (4 * COORDS_XY_STEP) - 1, y + (4 * COORDS_XY_STEP) - 1), true);
                }
            }
        }

        void ImportEntities()
        {
            for (int32_t i = 0; i < GetMaxEntities(); i++)
            {
                ImportEntity(_s6.Entities[i].Unknown);
            }
        }

        uint16_t GetMaxEntities()
        {
            return (_s6.Header.ClassicFlag == 0xf) ? Limits::MaxEntitiesRCTCExtended : Limits::MaxEntities;
        }

        template<typename OpenRCT2_T> void ImportEntity(const RCT12EntityBase& src);

        void ImportEntityPeep(::Peep* dst, const Peep* src)
        {
            const auto isNullLocation = [](const RCT12xyzd8& pos) {
                return pos.x == 0xFF && pos.y == 0xFF && pos.z == 0xFF && pos.direction == INVALID_DIRECTION;
            };

            ImportEntityCommonProperties(static_cast<EntityBase*>(dst), src);
            if (IsUserStringID(src->NameStringIdx))
            {
                dst->SetName(GetUserString(src->NameStringIdx));
            }
            dst->NextLoc = { src->NextX, src->NextY, src->NextZ * COORDS_Z_STEP };
            dst->NextFlags = src->NextFlags;
            dst->State = static_cast<PeepState>(src->State);
            dst->SubState = src->SubState;
            dst->SpriteType = static_cast<PeepSpriteType>(src->SpriteType);
            dst->TshirtColour = src->TshirtColour;
            dst->TrousersColour = src->TrousersColour;
            dst->DestinationX = src->DestinationX;
            dst->DestinationY = src->DestinationY;
            dst->DestinationTolerance = src->DestinationTolerance;
            dst->Var37 = src->Var37;
            dst->Energy = src->Energy;
            dst->EnergyTarget = src->EnergyTarget;
            dst->Mass = src->Mass;
            dst->WindowInvalidateFlags = src->WindowInvalidateFlags;
            dst->CurrentRide = RCT12RideIdToOpenRCT2RideId(src->CurrentRide);
            dst->CurrentRideStation = StationIndex::FromUnderlying(src->CurrentRideStation);
            dst->CurrentTrain = src->CurrentTrain;
            dst->TimeToSitdown = src->TimeToSitdown;
            dst->SpecialSprite = src->SpecialSprite;
            dst->ActionSpriteType = static_cast<PeepActionSpriteType>(src->ActionSpriteType);
            dst->NextActionSpriteType = static_cast<PeepActionSpriteType>(src->NextActionSpriteType);
            dst->ActionSpriteImageOffset = src->ActionSpriteImageOffset;
            dst->Action = static_cast<PeepActionType>(src->Action);
            dst->ActionFrame = src->ActionFrame;
            dst->StepProgress = src->StepProgress;
            dst->PeepDirection = src->Direction;
            dst->InteractionRideIndex = RCT12RideIdToOpenRCT2RideId(src->InteractionRideIndex);
            dst->PeepId = src->Id;
            dst->PathCheckOptimisation = src->PathCheckOptimisation;
            dst->PeepFlags = src->PeepFlags;
            if (isNullLocation(src->PathfindGoal))
            {
                dst->PathfindGoal.SetNull();
                dst->PathfindGoal.direction = INVALID_DIRECTION;
            }
            else
            {
                dst->PathfindGoal = { src->PathfindGoal.x, src->PathfindGoal.y, src->PathfindGoal.z,
                                      src->PathfindGoal.direction };
            }
            for (size_t i = 0; i < std::size(src->PathfindHistory); i++)
            {
                if (isNullLocation(src->PathfindHistory[i]))
                {
                    dst->PathfindHistory[i].SetNull();
                    dst->PathfindHistory[i].direction = INVALID_DIRECTION;
                }
                else
                {
                    dst->PathfindHistory[i] = { src->PathfindHistory[i].x, src->PathfindHistory[i].y, src->PathfindHistory[i].z,
                                                src->PathfindHistory[i].direction };
                }
            }
            dst->WalkingFrameNum = src->NoActionFrameNum;
        }

        constexpr EntityType GetEntityTypeFromRCT2Sprite(const RCT12EntityBase* src)
        {
            EntityType output = EntityType::Null;
            switch (src->EntityIdentifier)
            {
                case RCT12EntityIdentifier::Vehicle:
                    output = EntityType::Vehicle;
                    break;
                case RCT12EntityIdentifier::Peep:
                {
                    const auto& peep = static_cast<const Peep&>(*src);
                    if (RCT12PeepType(peep.PeepType) == RCT12PeepType::Guest)
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

                    switch (RCT12MiscEntityType(src->Type))
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

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12EntityBase* src)
        {
            dst->Type = GetEntityTypeFromRCT2Sprite(src);
            dst->sprite_height_negative = src->SpriteHeightNegative;
            dst->Id = EntityId::FromUnderlying(src->EntityIndex);
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
            dst->sprite_width = src->SpriteWidth;
            dst->sprite_height_positive = src->SpriteHeightPositive;
            dst->SpriteRect = ScreenRect(src->SpriteLeft, src->SpriteTop, src->SpriteRight, src->SpriteBottom);
            dst->sprite_direction = src->EntityDirection;
        }

        void ImportEntity(const RCT12EntityBase& src);

        std::string GetUserString(StringId stringId)
        {
            const auto originalString = _s6.CustomStrings[stringId % 1024];
            auto originalStringView = std::string_view(
                originalString, GetRCT2StringBufferLen(originalString, USER_STRING_MAX_LENGTH));
            auto asUtf8 = RCT2StringToUTF8(originalStringView, RCT2LanguageId::EnglishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        ObjectList GetRequiredObjects()
        {
            std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), OBJECT_ENTRY_INDEX_NULL);
            std::fill(std::begin(_pathToRailingMap), std::end(_pathToRailingMap), OBJECT_ENTRY_INDEX_NULL);

            ObjectList objectList;
            int objectIt = 0;
            ObjectEntryIndex surfaceCount = 0;
            ObjectEntryIndex railingCount = 0;
            for (int16_t objectType = EnumValue(ObjectType::Ride); objectType <= EnumValue(ObjectType::Water); objectType++)
            {
                for (int16_t i = 0; i < RCT2ObjectEntryGroupCounts[objectType]; i++, objectIt++)
                {
                    auto entry = ObjectEntryDescriptor(_s6.Objects[objectIt]);
                    if (entry.HasValue())
                    {
                        if (objectType == EnumValue(ObjectType::Paths))
                        {
                            auto footpathMapping = GetFootpathSurfaceId(entry);
                            if (footpathMapping == nullptr)
                            {
                                // Unsupported footpath
                                objectList.SetObject(i, entry);
                            }
                            else
                            {
                                // We have surface objects for this footpath
                                auto surfaceIndex = objectList.Find(
                                    ObjectType::FootpathSurface, footpathMapping->NormalSurface);
                                if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(
                                        ObjectType::FootpathSurface, surfaceCount, footpathMapping->NormalSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToSurfaceMap[i] = surfaceIndex;

                                surfaceIndex = objectList.Find(ObjectType::FootpathSurface, footpathMapping->QueueSurface);
                                if (surfaceIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(
                                        ObjectType::FootpathSurface, surfaceCount, footpathMapping->QueueSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToQueueSurfaceMap[i] = surfaceIndex;

                                auto railingIndex = objectList.Find(ObjectType::FootpathRailings, footpathMapping->Railing);
                                if (railingIndex == OBJECT_ENTRY_INDEX_NULL)
                                {
                                    objectList.SetObject(ObjectType::FootpathRailings, railingCount, footpathMapping->Railing);
                                    railingIndex = railingCount++;
                                }
                                _pathToRailingMap[i] = railingIndex;
                            }
                        }
                        else
                        {
                            objectList.SetObject(i, entry);
                        }
                    }
                }
            }

            // Add default rct2 terrain surfaces and edges
            AddDefaultEntries();

            // Find if any rct1 terrain surfaces or edges have been used
            const bool hasRCT1Terrain = std::any_of(
                std::begin(_s6.TileElements), std::end(_s6.TileElements), [](RCT12TileElement& tile) {
                    auto* surface = tile.AsSurface();
                    if (surface == nullptr)
                    {
                        return false;
                    }
                    if (surface->GetSurfaceStyle() >= std::size(RCT2::DefaultTerrainSurfaces))
                    {
                        return true;
                    }
                    if (surface->GetEdgeStyle() >= std::size(RCT2::DefaultTerrainEdges))
                    {
                        return true;
                    }
                    return false;
                });

            // If an rct1 surface or edge then load all the Hybrid surfaces and edges
            if (hasRCT1Terrain)
            {
                _terrainSurfaceEntries.AddRange(OpenRCT2HybridTerrainSurfaces);
                _terrainEdgeEntries.AddRange(OpenRCT2HybridTerrainEdges);
            }

            AppendRequiredObjects(objectList, ObjectType::TerrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(objectList, ObjectType::TerrainEdge, _terrainEdgeEntries);
            RCT12AddDefaultObjects(objectList);
            return objectList;
        }
    };

    template<> void S6Importer::ImportEntity<::Vehicle>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT2::Vehicle*>(&baseSrc);
        const auto& ride = _s6.Rides[src->Ride];

        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Vehicle::Type(src->Type);
        dst->Pitch = src->Pitch;
        dst->bank_rotation = src->BankRotation;
        dst->remaining_distance = src->RemainingDistance;
        dst->velocity = src->Velocity;
        dst->acceleration = src->Acceleration;
        dst->ride = RideId::FromUnderlying(src->Ride);
        dst->vehicle_type = src->VehicleType;
        dst->colours.Body = src->Colours.BodyColour;
        dst->colours.Trim = src->Colours.TrimColour;
        dst->colours.Tertiary = src->ColoursExtended;
        dst->track_progress = src->TrackProgress;
        dst->TrackLocation = { src->TrackX, src->TrackY, src->TrackZ };
        if (src->BoatLocation.IsNull() || static_cast<RideMode>(ride.Mode) != RideMode::BoatHire
            || src->Status != static_cast<uint8_t>(::Vehicle::Status::TravellingBoat))
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            dst->SetTrackType(src->GetTrackType());
            // RotationControlToggle and Booster are saved as the same track piece ID
            // Which one the vehicle is using must be determined
            if (IsFlatRide(src->Ride))
            {
                dst->SetTrackType(RCT12FlatTrackTypeToOpenRCT2(src->GetTrackType()));
            }
            else if (src->GetTrackType() == TrackElemType::RotationControlToggleAlias)
            {
                // Merging hacks mean the track type that's appropriate for the ride type is not necessarily the track type the
                // ride is on. It's possible to create unwanted behavior if a user layers spinning control track on top of
                // booster track but this is unlikely since only two rides have spinning control track - by default they load as
                // booster
                TileElement* tileElement2 = MapGetTrackElementAtOfTypeSeq(
                    dst->TrackLocation, TrackElemType::RotationControlToggle, 0);

                if (tileElement2 != nullptr)
                    dst->SetTrackType(TrackElemType::RotationControlToggle);
            }
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->BoatLocation.x, src->BoatLocation.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(0);
        }

        dst->next_vehicle_on_train = EntityId::FromUnderlying(src->NextVehicleOnTrain);
        dst->prev_vehicle_on_ride = EntityId::FromUnderlying(src->PrevVehicleOnRide);
        dst->next_vehicle_on_ride = EntityId::FromUnderlying(src->NextVehicleOnRide);
        dst->var_44 = src->Var44;
        dst->mass = src->Mass;
        dst->Flags = src->UpdateFlags;
        dst->SwingSprite = src->SwingSprite;
        dst->current_station = StationIndex::FromUnderlying(src->CurrentStation);
        dst->current_time = src->CurrentTime;
        dst->crash_z = src->CrashZ;

        ::Vehicle::Status statusSrc = ::Vehicle::Status::MovingToEndOfStation;
        if (src->Status <= static_cast<uint8_t>(::Vehicle::Status::StoppedByBlockBrakes))
        {
            statusSrc = static_cast<::Vehicle::Status>(src->Status);
        }

        dst->status = statusSrc;
        dst->sub_state = src->SubState;
        for (size_t i = 0; i < std::size(src->Peep); i++)
        {
            dst->peep[i] = EntityId::FromUnderlying(src->Peep[i]);
            dst->peep_tshirt_colours[i] = src->PeepTshirtColours[i];
        }
        dst->num_seats = src->NumSeats;
        dst->num_peeps = src->NumPeeps;
        dst->next_free_seat = src->NextFreeSeat;
        dst->restraints_position = src->RestraintsPosition;
        dst->crash_x = src->CrashX;
        dst->sound2_flags = src->Sound2Flags;
        dst->spin_sprite = src->SpinSprite;
        dst->sound1_id = static_cast<OpenRCT2::Audio::SoundId>(src->Sound1Id);
        dst->sound1_volume = src->Sound1Volume;
        dst->sound2_id = static_cast<OpenRCT2::Audio::SoundId>(src->Sound2Id);
        dst->sound2_volume = src->Sound2Volume;
        dst->sound_vector_factor = src->SoundVectorFactor;
        dst->time_waiting = src->TimeWaiting;
        dst->speed = src->Speed;
        dst->powered_acceleration = src->PoweredAcceleration;
        dst->CollisionDetectionTimer = src->CollisionDetectionTimer;
        dst->animation_frame = src->AnimationFrame;
        dst->animationState = src->AnimationState;
        dst->scream_sound_id = static_cast<OpenRCT2::Audio::SoundId>(src->ScreamSoundId);
        dst->TrackSubposition = VehicleTrackSubposition{ src->TrackSubposition };
        dst->NumLaps = src->NumLaps;
        dst->brake_speed = src->BrakeSpeed;
        dst->lost_time_out = src->LostTimeOut;
        dst->vertical_drop_countdown = src->VerticalDropCountdown;
        dst->var_D3 = src->VarD3;
        dst->mini_golf_current_animation = MiniGolfAnimation(src->MiniGolfCurrentAnimation);
        dst->mini_golf_flags = src->MiniGolfFlags;
        dst->ride_subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->RideSubtype);
        dst->seat_rotation = src->SeatRotation;
        dst->target_seat_rotation = src->TargetSeatRotation;
        if (src->Flags & RCT12_ENTITY_FLAGS_IS_CRASHED_VEHICLE_ENTITY)
        {
            dst->SetFlag(VehicleFlags::Crashed);
        }
    }

    static uint32_t AdjustScenarioToCurrentTicks(const S6Data& s6, uint32_t tick)
    {
        // Previously gScenarioTicks was used as a time point, now it's gCurrentTicks.
        // gCurrentTicks and gScenarioTicks are now exported as the same, older saves that have a different
        // scenario tick must account for the difference between the two.
        uint32_t ticksElapsed = s6.ScenarioTicks - tick;
        return s6.GameTicks1 - ticksElapsed;
    }

    template<> void S6Importer::ImportEntity<::Guest>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Guest>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const Peep*>(&baseSrc);
        ImportEntityPeep(dst, src);

        dst->OutsideOfPark = static_cast<bool>(src->OutsideOfPark);
        dst->GuestNumRides = src->NoOfRides;
        dst->Happiness = src->Happiness;
        dst->HappinessTarget = src->HappinessTarget;
        dst->Nausea = src->Nausea;
        dst->NauseaTarget = src->NauseaTarget;
        dst->Hunger = src->Hunger;
        dst->Thirst = src->Thirst;
        dst->Toilet = src->Toilet;
        dst->TimeToConsume = src->TimeToConsume;
        dst->Intensity = static_cast<IntensityRange>(src->Intensity);
        dst->NauseaTolerance = static_cast<PeepNauseaTolerance>(src->NauseaTolerance);
        dst->PaidOnDrink = src->PaidOnDrink;

        OpenRCT2::RideUse::GetHistory().Set(dst->Id, RCT12GetRidesBeenOn(src));
        OpenRCT2::RideUse::GetTypeHistory().Set(dst->Id, RCT12GetRideTypesBeenOn(src));

        dst->SetItemFlags(src->GetItemFlags());
        dst->Photo1RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo1RideRef);
        dst->Photo2RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo2RideRef);
        dst->Photo3RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo3RideRef);
        dst->Photo4RideRef = RCT12RideIdToOpenRCT2RideId(src->Photo4RideRef);
        dst->GuestNextInQueue = EntityId::FromUnderlying(src->NextInQueue);
        dst->TimeInQueue = src->TimeInQueue;
        dst->CashInPocket = src->CashInPocket;
        dst->CashSpent = src->CashSpent;
        dst->ParkEntryTime = AdjustScenarioToCurrentTicks(_s6, src->ParkEntryTime);
        dst->RejoinQueueTimeout = src->RejoinQueueTimeout;
        dst->PreviousRide = RCT12RideIdToOpenRCT2RideId(src->PreviousRide);
        dst->PreviousRideTimeOut = src->PreviousRideTimeOut;
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
        dst->GuestHeadingToRideId = RCT12RideIdToOpenRCT2RideId(src->GuestHeadingToRideId);
        dst->GuestIsLostCountdown = src->PeepIsLostCountdown;
        dst->LitterCount = src->LitterCount;
        dst->GuestTimeOnRide = src->TimeOnRide;
        dst->DisgustingCount = src->DisgustingCount;
        dst->PaidToEnter = src->PaidToEnter;
        dst->PaidOnRides = src->PaidOnRides;
        dst->PaidOnFood = src->PaidOnFood;
        dst->PaidOnSouvenirs = src->PaidOnSouvenirs;
        dst->AmountOfFood = src->NoOfFood;
        dst->AmountOfDrinks = src->NoOfDrinks;
        dst->AmountOfSouvenirs = src->NoOfSouvenirs;
        dst->VandalismSeen = src->VandalismSeen;
        dst->VoucherType = src->VoucherType;
        dst->VoucherRideId = RCT12RideIdToOpenRCT2RideId(src->VoucherArguments);
        dst->SurroundingsThoughtTimeout = src->SurroundingsThoughtTimeout;
        dst->Angriness = src->Angriness;
        dst->TimeLost = src->TimeLost;
        dst->DaysInQueue = src->DaysInQueue;
        dst->BalloonColour = src->BalloonColour;
        dst->UmbrellaColour = src->UmbrellaColour;
        dst->HatColour = src->HatColour;
        dst->FavouriteRide = RCT12RideIdToOpenRCT2RideId(src->FavouriteRide);
        dst->FavouriteRideRating = src->FavouriteRideRating;
    }

    template<> void S6Importer::ImportEntity<::Staff>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Staff>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const Peep*>(&baseSrc);
        ImportEntityPeep(dst, src);

        dst->AssignedStaffType = StaffType(src->StaffType);
        dst->MechanicTimeSinceCall = src->MechanicTimeSinceCall;

        dst->HireDate = src->ParkEntryTime;
        dst->StaffOrders = src->StaffOrders;
        dst->StaffMowingTimeout = src->StaffMowingTimeout;
        dst->StaffLawnsMown = src->PaidToEnter;
        dst->StaffGardensWatered = src->PaidOnRides;
        dst->StaffLitterSwept = src->PaidOnFood;
        dst->StaffBinsEmptied = src->PaidOnSouvenirs;

        ImportStaffPatrolArea(dst, src->StaffId);
    }

    template<> void S6Importer::ImportEntity<::SteamParticle>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::SteamParticle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntitySteamParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->time_to_move = src->TimeToMove;
        dst->frame = src->Frame;
    }

    template<> void S6Importer::ImportEntity<::MoneyEffect>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::MoneyEffect>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityMoneyEffect*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->MoveDelay;
        dst->NumMovements = src->NumMovements;
        dst->GuestPurchase = src->Vertical;
        dst->Value = src->Value;
        dst->OffsetX = src->OffsetX;
        dst->Wiggle = src->Wiggle;
    }

    template<> void S6Importer::ImportEntity<::VehicleCrashParticle>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::VehicleCrashParticle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityCrashedVehicleParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_live = src->TimeToLive;
        dst->frame = src->Frame;
        dst->colour[0] = src->Colour[0];
        dst->colour[1] = src->Colour[1];
        dst->crashed_sprite_base = src->CrashedEntityBase;
        dst->velocity_x = src->VelocityX;
        dst->velocity_y = src->VelocityY;
        dst->velocity_z = src->VelocityZ;
        dst->acceleration_x = src->AccelerationX;
        dst->acceleration_y = src->AccelerationY;
        dst->acceleration_z = src->AccelerationZ;
    }

    template<> void S6Importer::ImportEntity<::ExplosionCloud>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::ExplosionCloud>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S6Importer::ImportEntity<::ExplosionFlare>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::ExplosionFlare>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S6Importer::ImportEntity<::CrashSplashParticle>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::CrashSplashParticle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<> void S6Importer::ImportEntity<::JumpingFountain>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::JumpingFountain>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityJumpingFountain*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->NumTicksAlive = src->NumTicksAlive;
        dst->frame = src->Frame;
        dst->FountainFlags = src->FountainFlags;
        dst->TargetX = src->TargetX;
        dst->TargetY = src->TargetY;
        dst->Iteration = src->Iteration;
        dst->FountainType = RCT12MiscEntityType(src->Type) == RCT12MiscEntityType::JumpingFountainSnow
            ? ::JumpingFountainType::Snow
            : ::JumpingFountainType::Water;
    }

    template<> void S6Importer::ImportEntity<::Balloon>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Balloon>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityBalloon*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->popped = src->Popped;
        dst->time_to_move = src->TimeToMove;
        dst->frame = src->Frame;
        dst->colour = src->Colour;
    }

    template<> void S6Importer::ImportEntity<::Duck>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Duck>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityDuck*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->target_x = src->TargetX;
        dst->target_y = src->TargetY;
        dst->state = static_cast<::Duck::DuckState>(src->State);
    }

    template<> void S6Importer::ImportEntity<::Litter>(const RCT12EntityBase& baseSrc)
    {
        auto dst = CreateEntityAt<::Litter>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityLitter*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Litter::Type(src->Type);
        dst->creationTick = AdjustScenarioToCurrentTicks(_s6, src->CreationTick);
    }

    void S6Importer::ImportEntity(const RCT12EntityBase& src)
    {
        switch (GetEntityTypeFromRCT2Sprite(&src))
        {
            case EntityType::Vehicle:
                ImportEntity<::Vehicle>(src);
                break;
            case EntityType::Guest:
                ImportEntity<::Guest>(src);
                break;
            case EntityType::Staff:
                ImportEntity<::Staff>(src);
                break;
            case EntityType::SteamParticle:
                ImportEntity<::SteamParticle>(src);
                break;
            case EntityType::MoneyEffect:
                ImportEntity<::MoneyEffect>(src);
                break;
            case EntityType::CrashedVehicleParticle:
                ImportEntity<::VehicleCrashParticle>(src);
                break;
            case EntityType::ExplosionCloud:
                ImportEntity<::ExplosionCloud>(src);
                break;
            case EntityType::ExplosionFlare:
                ImportEntity<::ExplosionFlare>(src);
                break;
            case EntityType::CrashSplash:
                ImportEntity<::CrashSplashParticle>(src);
                break;
            case EntityType::JumpingFountain:
                ImportEntity<::JumpingFountain>(src);
                break;
            case EntityType::Balloon:
                ImportEntity<::Balloon>(src);
                break;
            case EntityType::Duck:
                ImportEntity<::Duck>(src);
                break;
            case EntityType::Litter:
                ImportEntity<::Litter>(src);
                break;
            default:
                // Null elements do not need imported
                break;
        }
    }
} // namespace RCT2

std::unique_ptr<IParkImporter> ParkImporter::CreateS6(IObjectRepository& objectRepository)
{
    return std::make_unique<RCT2::S6Importer>(objectRepository);
}
