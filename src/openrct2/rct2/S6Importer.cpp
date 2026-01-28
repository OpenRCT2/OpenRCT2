/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../ParkImporter.h"
#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../entity/Balloon.h"
#include "../entity/Duck.h"
#include "../entity/Fountain.h"
#include "../entity/Guest.h"
#include "../entity/Litter.h"
#include "../entity/MoneyEffect.h"
#include "../entity/Particle.h"
#include "../entity/PatrolArea.h"
#include "../entity/Staff.h"
#include "../localisation/Formatting.h"
#include "../object/ObjectList.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../object/ScenarioMetaObject.h"
#include "../park/Legacy.h"
#include "../park/ParkPreview.h"
#include "../peep/RideUseSystem.h"
#include "../rct12/CSStringConverter.h"
#include "../rct12/EntryList.h"
#include "../rct12/RCT12.h"
#include "../rct12/ScenarioPatcher.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/Vehicle.h"
#include "../sawyer_coding/SawyerChunkReader.h"
#include "../scenario/Scenario.h"
#include "../scenario/ScenarioRepository.h"
#include "../scenario/ScenarioSources.h"
#include "../world/Climate.h"
#include "../world/Entrance.h"
#include "../world/Map.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/Scenery.h"
#include "../world/TilePointerIndex.hpp"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/EntranceElement.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "../world/tile_element/WallElement.h"

#include <cassert>

using namespace OpenRCT2;
using namespace OpenRCT2::SawyerCoding;

namespace OpenRCT2::RCT2
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
        OpenRCT2::BitSet<Limits::kMaxRidesInPark> _isFlatRide{};
        ObjectEntryIndex _pathToSurfaceMap[16];
        ObjectEntryIndex _pathToQueueSurfaceMap[16];
        ObjectEntryIndex _pathToRailingMap[16];
        RCT12::EntryList _terrainSurfaceEntries;
        RCT12::EntryList _terrainEdgeEntries;
        RCT12::EntryList _stationEntries;

    public:
        S6Importer(IObjectRepository& objectRepository)
            : _objectRepository(objectRepository)
        {
        }

        ParkLoadResult Load(const u8string& path, const bool skipObjectCheck) override
        {
            const auto extension = Path::GetExtension(path);
            if (String::iequals(extension, ".sc6"))
            {
                return LoadScenario(path, skipObjectCheck);
            }
            if (String::iequals(extension, ".sv6") || String::iequals(extension, ".sv7"))
            {
                return LoadSavedGame(path, skipObjectCheck);
            }

            throw std::runtime_error("Invalid RCT2 park extension.");
        }

        ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FileMode::open);
            auto result = LoadFromStream(&fs, false, skipObjectCheck);
            _s6Path = path;
            return result;
        }

        ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) override
        {
            auto fs = FileStream(path, FileMode::open);
            auto result = LoadFromStream(&fs, true, skipObjectCheck);
            _s6Path = path;
            return result;
        }

        ParkLoadResult LoadFromStream(
            IStream* stream, bool isScenario, bool skipObjectCheck = false, const u8string& path = {}) override
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

                // If the name or the details contain a colour code, they might be in UTF-8 already.
                // This is caused by a bug that was in OpenRCT2 for 3 years.
                if (!IsLikelyUTF8(_s6.Info.Name) && !IsLikelyUTF8(_s6.Info.Details))
                {
                    RCT2StringToUTF8Self(_s6.Info.Name, sizeof(_s6.Info.Name));
                    RCT2StringToUTF8Self(_s6.Info.Details, sizeof(_s6.Info.Details));
                }
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
            if (!skipObjectCheck)
            {
                for (uint16_t i = 0; i < _s6.Header.NumPackedObjects; i++)
                {
                    _objectRepository.ExportPackedObject(stream);
                }
            }

            if (!path.empty())
            {
                auto extension = Path::GetExtension(path);
                _isSV7 = String::iequals(extension, ".sv7");
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
            auto stream = MemoryStream(buffer.data(), buffer.size());

            uint32_t preEntitiesSize = sizeof(_s6.NextFreeTileElementPointerIndex);
            uint32_t postEntitiesSize = sizeWithoutEntities - preEntitiesSize;

            stream.Read(&_s6.NextFreeTileElementPointerIndex, preEntitiesSize);
            stream.Read(&_s6.Entities, entitiesSize);
            stream.Read(&_s6.EntityListsHead, postEntitiesSize);
        }

        bool PopulateIndexEntry(ScenarioIndexEntry* dst) override
        {
            *dst = {};

            dst->Category = _s6.Info.Category;
            dst->ObjectiveType = _s6.Info.ObjectiveType;
            dst->ObjectiveArg1 = _s6.Info.ObjectiveArg1;
            dst->ObjectiveArg2 = _s6.Info.ObjectiveArg2;
            dst->ObjectiveArg3 = _s6.Info.ObjectiveArg3;
            dst->Highscore = nullptr;

            if (String::isNullOrEmpty(_s6.Info.Name))
            {
                // If the scenario doesn't have a name, set it to the filename
                dst->Name = Path::GetFileNameWithoutExtension(dst->Path);
            }
            else
            {
                // Normalise the name to make the scenario as recognisable as possible.
                dst->Name = ScenarioSources::NormaliseName(_s6.Info.Name);
            }

            // Look up and store information regarding the origins of this scenario.
            SourceDescriptor desc;
            if (ScenarioSources::TryGetByName(dst->Name.c_str(), &desc))
            {
                dst->ScenarioId = desc.id;
                dst->SourceIndex = desc.index;
                dst->SourceGame = ScenarioSource{ desc.source };
                dst->Category = desc.category;
            }
            else
            {
                dst->ScenarioId = SC_UNIDENTIFIED;
                dst->SourceIndex = -1;
                if (dst->Category == Scenario::Category::real)
                {
                    dst->SourceGame = ScenarioSource::Real;
                }
                else
                {
                    dst->SourceGame = ScenarioSource::Other;
                }
            }

            // dst->name will be translated later so keep the untranslated name here
            dst->InternalName = dst->Name;
            dst->Details = _s6.Info.Details;

            if (!desc.textObjectId.empty())
            {
                auto& objManager = GetContext()->GetObjectManager();

                if (auto obj = objManager.LoadTempObject(desc.textObjectId, false); obj != nullptr)
                {
                    auto& textObject = reinterpret_cast<ScenarioMetaObject&>(*obj);
                    dst->Name = textObject.GetScenarioName();
                    dst->Details = textObject.GetScenarioDetails();
                }
            }

            return true;
        }

        ParkPreview GetParkPreview() override
        {
            return {};
        }

        std::vector<News::Item> convertNewsQueue(std::span<const RCT12NewsItem> queue)
        {
            std::vector<News::Item> output{};

            auto index = 0;
            for (const auto& src : queue)
            {
                if (src.Type == 0)
                    break;

                if (src.Type >= News::ItemTypeCount)
                {
                    LOG_ERROR("Invalid news type 0x%x for news item %d, ignoring remaining news items", src.Type, index);
                    break;
                }

                News::Item dst{};
                dst.type = static_cast<News::ItemType>(src.Type);
                dst.flags = src.Flags;
                dst.assoc = src.Assoc;
                dst.ticks = src.Ticks;
                dst.monthYear = src.MonthYear;
                dst.day = src.Day;
                dst.text = ConvertFormattedStringToOpenRCT2(std::string_view(src.Text, sizeof(src.Text)));

                output.emplace_back(dst);
                index++;
            }

            return output;
        }

        void Import(GameState_t& gameState) override
        {
            Initialise(gameState);

            gameState.editorStep = _s6.Info.EditorStep;
            gameState.scenarioOptions.category = _s6.Info.Category;

            // Some scenarios have their scenario details in UTF-8, due to earlier bugs in OpenRCT2.
            auto loadMaybeUTF8 = [](std::string_view str) -> std::string {
                return !IsLikelyUTF8(str) ? RCT2StringToUTF8(str, RCT2LanguageId::englishUK) : std::string(str);
            };

            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                gameState.scenarioOptions.name = loadMaybeUTF8(_s6.Info.Name);
                gameState.scenarioOptions.details = loadMaybeUTF8(_s6.Info.Details);
            }
            else
            {
                // Saved games do not have an info chunk
                gameState.scenarioOptions.name = loadMaybeUTF8(_s6.ScenarioName);
                gameState.scenarioOptions.details = loadMaybeUTF8(_s6.ScenarioDescription);
            }

            gameState.date = Date{ _s6.ElapsedMonths, _s6.CurrentDay };
            gameState.currentTicks = _s6.GameTicks1;

            ScenarioRandSeed(_s6.ScenarioSrand0, _s6.ScenarioSrand1);

            DetermineFlatRideStatus();
            ImportTileElements(gameState);
            ImportEntities(gameState);
            ConvertPeepAnimationTypeToObjects(gameState);

            auto& park = gameState.park;

            gameState.scenarioOptions.initialCash = ToMoney64(_s6.InitialCash);
            park.bankLoan = ToMoney64(_s6.CurrentLoan);

            park.flags = _s6.ParkFlags & ~PARK_FLAGS_NO_MONEY_SCENARIO;

            // RCT2 used a different flag for `no money` when the park is a scenario
            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                if (_s6.ParkFlags & PARK_FLAGS_NO_MONEY_SCENARIO)
                    park.flags |= PARK_FLAGS_NO_MONEY;
                else
                    park.flags &= ~PARK_FLAGS_NO_MONEY;
            }

            park.entranceFee = _s6.ParkEntranceFee;
            // rct1_park_entranceX
            // rct1_park_entrance_y
            // Pad013573EE
            // rct1_park_entrance_z

            ImportPeepSpawns(gameState);

            park.guestChangeModifier = _s6.GuestCountChangeModifier;
            gameState.researchFundingLevel = _s6.CurrentResearchLevel;
            // Pad01357400
            // _s6.ResearchedTrackTypesA
            // _s6.ResearchedTrackTypesB

            park.numGuestsInPark = _s6.GuestsInPark;
            park.numGuestsHeadingForPark = _s6.GuestsHeadingForPark;

            for (size_t i = 0; i < Limits::kExpenditureTableMonthCount; i++)
            {
                for (size_t j = 0; j < Limits::kExpenditureTypeCount; j++)
                {
                    park.expenditureTable[i][j] = ToMoney64(_s6.ExpenditureTable[i][j]);
                }
            }

            park.numGuestsInParkLastWeek = _s6.LastGuestsInPark;
            // Pad01357BCA
            park.staffHandymanColour = _s6.HandymanColour;
            park.staffMechanicColour = _s6.MechanicColour;
            park.staffSecurityColour = _s6.SecurityColour;

            park.rating = _s6.ParkRating;

            Park::ResetHistories(park);
            for (size_t i = 0; i < std::size(_s6.ParkRatingHistory); i++)
            {
                if (_s6.ParkRatingHistory[i] != kRCT12ParkHistoryUndefined)
                {
                    park.ratingHistory[i] = _s6.ParkRatingHistory[i] * kRCT12ParkRatingHistoryFactor;
                }
            }
            for (size_t i = 0; i < std::size(_s6.GuestsInParkHistory); i++)
            {
                if (_s6.GuestsInParkHistory[i] != kRCT12ParkHistoryUndefined)
                {
                    park.guestsInParkHistory[i] = _s6.GuestsInParkHistory[i] * kRCT12GuestsInParkHistoryFactor;
                }
            }

            gameState.researchPriorities = _s6.ActiveResearchTypes;
            gameState.researchProgressStage = _s6.ResearchProgressStage;
            if (_s6.LastResearchedItemSubject != kRCT12ResearchedItemsSeparator)
                gameState.researchLastItem = RCT12ResearchItem{ _s6.LastResearchedItemSubject,
                                                                EnumValue(ResearchCategory::transport) }
                                                 .ToResearchItem();
            else
                gameState.researchLastItem = std::nullopt;
            // Pad01357CF8
            if (_s6.NextResearchItem != kRCT12ResearchedItemsSeparator)
                gameState.researchNextItem = RCT12ResearchItem{ _s6.NextResearchItem, _s6.NextResearchCategory }
                                                 .ToResearchItem();
            else
                gameState.researchNextItem = std::nullopt;

            gameState.researchProgress = _s6.ResearchProgress;
            gameState.researchExpectedDay = _s6.NextResearchExpectedDay;
            gameState.researchExpectedMonth = _s6.NextResearchExpectedMonth;
            gameState.scenarioOptions.guestInitialHappiness = _s6.GuestInitialHappiness;
            park.size = _s6.ParkSize;
            park.guestGenerationProbability = _s6.GuestGenerationProbability;
            park.totalRideValueForMoney = _s6.TotalRideValueForMoney;
            park.maxBankLoan = ToMoney64(_s6.MaximumLoan);
            gameState.scenarioOptions.guestInitialCash = ToMoney64(_s6.GuestInitialCash);
            gameState.scenarioOptions.guestInitialHunger = _s6.GuestInitialHunger;
            gameState.scenarioOptions.guestInitialThirst = _s6.GuestInitialThirst;
            gameState.scenarioOptions.objective.Type = _s6.ObjectiveType;
            gameState.scenarioOptions.objective.Year = _s6.ObjectiveYear;
            // Pad013580FA
            gameState.scenarioOptions.objective.Currency = _s6.ObjectiveCurrency;
            // In RCT2, the ride string IDs start at index STR_0002 and are directly mappable.
            // This is not always the case in OpenRCT2, so we use the actual ride ID.
            if (gameState.scenarioOptions.objective.Type == Scenario::ObjectiveType::buildTheBest)
                gameState.scenarioOptions.objective.RideId = _s6.ObjectiveGuests - kRCT2RideStringStart;
            else
                gameState.scenarioOptions.objective.NumGuests = _s6.ObjectiveGuests;
            ImportMarketingCampaigns(park);

            park.currentExpenditure = ToMoney64(_s6.CurrentExpenditure);
            park.currentProfit = ToMoney64(_s6.CurrentProfit);
            park.weeklyProfitAverageDividend = ToMoney64(_s6.WeeklyProfitAverageDividend);
            park.weeklyProfitAverageDivisor = _s6.WeeklyProfitAverageDivisor;
            // Pad0135833A

            park.value = ToMoney64(_s6.ParkValue);

            for (size_t i = 0; i < Limits::kFinanceGraphSize; i++)
            {
                park.cashHistory[i] = ToMoney64(_s6.BalanceHistory[i]);
                park.weeklyProfitHistory[i] = ToMoney64(_s6.WeeklyProfitHistory[i]);
                park.valueHistory[i] = ToMoney64(_s6.ParkValueHistory[i]);
            }

            gameState.scenarioCompletedCompanyValue = RCT12CompletedCompanyValueToOpenRCT2(_s6.CompletedCompanyValue);
            park.totalAdmissions = _s6.TotalAdmissions;
            park.totalIncomeFromAdmissions = ToMoney64(_s6.IncomeFromAdmissions);
            park.companyValue = ToMoney64(_s6.CompanyValue);
            std::memcpy(park.peepWarningThrottle, _s6.PeepWarningThrottle, sizeof(_s6.PeepWarningThrottle));

            // Awards
            auto& currentAwards = park.currentAwards;
            for (auto& src : _s6.Awards)
            {
                if (src.Time != 0)
                {
                    currentAwards.push_back(Award{ src.Time, static_cast<AwardType>(src.Type) });
                }
            }

            gameState.scenarioOptions.landPrice = ToMoney64(_s6.LandPrice);
            gameState.scenarioOptions.constructionRightsPrice = ToMoney64(_s6.ConstructionRightsPrice);
            // unk_01358774
            // Pad01358776
            // _s6.CdKey
            _gameVersion = _s6.GameVersionNumber;
            gameState.scenarioCompanyValueRecord = _s6.CompletedCompanyValueRecord;
            // _s6.LoanHash;
            // Pad013587CA
            park.historicalProfit = ToMoney64(_s6.HistoricalProfit);
            // Pad013587D4
            gameState.scenarioCompletedBy = std::string_view(_s6.ScenarioCompletedName, sizeof(_s6.ScenarioCompletedName));
            park.cash = ToMoney64(DECRYPT_MONEY(_s6.Cash));
            // Pad013587FC
            park.ratingCasualtyPenalty = _s6.ParkRatingCasualtyPenalty;
            gameState.mapSize = { _s6.MapSize, _s6.MapSize };
            park.samePriceThroughoutPark = _s6.SamePriceThroughout
                | (static_cast<uint64_t>(_s6.SamePriceThroughoutExtended) << 32);
            park.suggestedGuestMaximum = _s6.SuggestedMaxGuests;
            gameState.scenarioParkRatingWarningDays = _s6.ParkRatingWarningDays;
            gameState.lastEntranceStyle = _s6.LastEntranceStyle;
            // rct1_water_colour
            // Pad01358842
            ImportResearchList(gameState);
            park.bankLoanInterestRate = _s6.CurrentInterestRate;
            // Pad0135934B
            // Preserve compatibility with vanilla RCT2's save format.
            park.entrances.clear();
            for (uint8_t i = 0; i < Limits::kMaxParkEntrances; i++)
            {
                if (_s6.ParkEntranceX[i] != kLocationNull)
                {
                    CoordsXYZD entrance;
                    entrance.x = _s6.ParkEntranceX[i];
                    entrance.y = _s6.ParkEntranceY[i];
                    entrance.z = _s6.ParkEntranceZ[i];
                    entrance.direction = _s6.ParkEntranceDirection[i];
                    park.entrances.push_back(entrance);
                }
            }
            if (_s6.Header.Type == S6_TYPE_SCENARIO)
            {
                // _s6.ScenarioFilename is wrong for some RCT2 expansion scenarios, so we use the real filename
                gameState.scenarioFileName = Path::GetFileName(_s6Path);
            }
            else
            {
                // For savegames the filename can be arbitrary, so we have no choice but to rely on the name provided
                gameState.scenarioFileName = std::string(
                    String::toStringView(_s6.ScenarioFilename, std::size(_s6.ScenarioFilename)));
            }
            gCurrentRealTimeTicks = 0;

            ImportRides();

            gameState.savedView = ScreenCoordsXY{ _s6.SavedViewX, _s6.SavedViewY };
            gameState.savedViewZoom = ZoomLevel{ static_cast<int8_t>(_s6.SavedViewZoom) };
            gameState.savedViewRotation = _s6.SavedViewRotation;

            ImportMapAnimations();

            ImportRideRatingsCalcData();
            ImportRideMeasurements();
            gameState.nextGuestNumber = _s6.NextGuestIndex;
            gameState.grassSceneryTileLoopPosition = _s6.GrassAndSceneryTilepos;
            // unk_13CA73E
            // Pad13CA73F
            // unk_13CA740
            // gameState.climate = ClimateType{ _s6.Climate };
            // Pad13CA741;
            // Byte13CA742
            // Pad013CA747
            gameState.weatherUpdateTimer = _s6.WeatherUpdateTimer;
            gameState.weatherCurrent = {
                .weatherType = WeatherType{ _s6.CurrentWeather },
                .temperature = static_cast<int8_t>(_s6.Temperature),
                .weatherEffect = WeatherEffectType{ _s6.CurrentWeatherEffect },
                .weatherGloom = _s6.CurrentWeatherGloom,
                .level = static_cast<WeatherLevel>(_s6.CurrentWeatherLevel),
            };
            gameState.weatherNext = {
                .weatherType = WeatherType{ _s6.NextWeather },
                .temperature = static_cast<int8_t>(_s6.NextTemperature),
                .weatherEffect = WeatherEffectType{ _s6.NextWeatherEffect },
                .weatherGloom = _s6.NextWeatherGloom,
                .level = static_cast<WeatherLevel>(_s6.NextWeatherLevel),
            };

            // News items
            auto recentMessages = convertNewsQueue(_s6.recentMessages);
            auto archivedMessages = convertNewsQueue(_s6.archivedMessages);
            News::importNewsItems(gameState, recentMessages, archivedMessages);

            // Pad13CE730
            // rct1_scenario_flags
            gameState.widePathTileLoopPosition = { _s6.WidePathTileLoopX, _s6.WidePathTileLoopY };
            // Pad13CE778

            // Fix and set dynamic variables
            MapStripGhostFlagFromElements();
            ConvertScenarioStringsToUTF8(gameState);
            DetermineRideEntranceAndExitLocations();

            park.name = GetUserString(_s6.ParkName);

            if (_isScenario)
            {
                RCT12::FetchAndApplyScenarioPatch(_s6Path);
            }

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

            _stationEntries.AddRange(kDefaultStationStyles);
        }

        void ConvertScenarioStringsToUTF8(GameState_t& gameState)
        {
            // Scenario details
            gameState.scenarioCompletedBy = RCT2StringToUTF8(gameState.scenarioCompletedBy, RCT2LanguageId::englishUK);
            gameState.scenarioOptions.name = RCT2StringToUTF8(gameState.scenarioOptions.name, RCT2LanguageId::englishUK);
            gameState.scenarioOptions.details = RCT2StringToUTF8(gameState.scenarioOptions.details, RCT2LanguageId::englishUK);
        }

        void ImportRides()
        {
            for (uint8_t index = 0; index < Limits::kMaxRidesInPark; index++)
            {
                auto src = &_s6.Rides[index];
                if (src->type != kRideTypeNull)
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
            for (uint8_t index = 0; index < Limits::kMaxRidesInPark; index++)
            {
                auto src = &_s6.Rides[index];
                if (src->type == kRideTypeNull)
                    continue;

                auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->subtype);
                auto* rideEntry = GetRideEntryByIndex(subtype);
                // If the ride is tracked, we don’t need to check the vehicle any more.
                if (!GetRideTypeDescriptor(src->type).HasFlag(RtdFlag::isFlatRide))
                {
                    _isFlatRide[index] = false;
                    continue;
                }

                // We have established the ride type is a flat ride, which means the vehicle now determines whether it is a
                // true flat ride (scenario 4) or a tracked ride with an invisibility hack (scenario 2).
                ObjectEntryIndex originalRideType = src->type;
                if (rideEntry != nullptr)
                {
                    originalRideType = rideEntry->GetFirstNonNullRideType();
                }
                const auto isFlatRide = GetRideTypeDescriptor(originalRideType).HasFlag(RtdFlag::isFlatRide);
                _isFlatRide.set(static_cast<size_t>(index), isFlatRide);
            }
        }

        bool IsFlatRide(const uint8_t rct12RideIndex)
        {
            if (rct12RideIndex == kRCT12RideIdNull)
                return false;
            return _isFlatRide[rct12RideIndex];
        }

        void ImportRide(::Ride* dst, const Ride* src, const RideId rideIndex)
        {
            *dst = {};
            dst->id = rideIndex;

            auto rideType = src->type;
            auto subtype = RCTEntryIndexToOpenRCT2EntryIndex(src->subtype);
            if (RCT2RideTypeNeedsConversion(src->type))
            {
                auto* rideEntry = GetRideEntryByIndex(subtype);
                if (rideEntry != nullptr)
                {
                    rideType = RCT2RideTypeToOpenRCT2RideType(src->type, *rideEntry);
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
            dst->mode = static_cast<RideMode>(src->mode);
            dst->vehicleColourSettings = src->vehicleColourSettings;

            for (uint8_t i = 0; i < Limits::kMaxVehicleColours; i++)
            {
                dst->vehicleColours[i].Body = static_cast<Drawing::Colour>(src->vehicleColours[i].BodyColour);
                dst->vehicleColours[i].Trim = static_cast<Drawing::Colour>(src->vehicleColours[i].TrimColour);
            }

            // Pad046;
            dst->status = static_cast<RideStatus>(src->status);

            dst->defaultNameNumber = src->nameArgumentsNumber;
            if (IsUserStringID(src->name))
            {
                dst->customName = GetUserString(src->name);
            }
            else
            {
                dst->defaultNameNumber = src->nameArgumentsNumber;
            }

            if (src->overallView.IsNull())
            {
                dst->overallView.SetNull();
            }
            else
            {
                auto tileLoc = TileCoordsXY(src->overallView.x, src->overallView.y);
                dst->overallView = tileLoc.ToCoordsXY();
            }

            for (StationIndex::UnderlyingType i = 0; i < Limits::kMaxStationsPerRide; i++)
            {
                StationIndex stationIndex = StationIndex::FromUnderlying(i);
                auto& destStation = dst->getStation(stationIndex);

                if (src->stationStarts[i].IsNull())
                {
                    destStation.Start.SetNull();
                }
                else
                {
                    auto tileStartLoc = TileCoordsXY(src->stationStarts[i].x, src->stationStarts[i].y);
                    destStation.Start = tileStartLoc.ToCoordsXY();
                }
                destStation.Height = src->stationHeights[i];
                destStation.Length = src->stationLength[i];
                destStation.Depart = src->stationDepart[i];
                destStation.TrainAtStation = src->trainAtStation[i];
                // Direction is fixed later.

                if (src->entrances[i].IsNull())
                    destStation.Entrance.SetNull();
                else
                    destStation.Entrance = { src->entrances[i].x, src->entrances[i].y, src->stationHeights[i], 0 };

                if (src->exits[i].IsNull())
                    destStation.Exit.SetNull();
                else
                    destStation.Exit = { src->exits[i].x, src->exits[i].y, src->stationHeights[i], 0 };

                destStation.LastPeepInQueue = EntityId::FromUnderlying(src->lastPeepInQueue[i]);

                destStation.SegmentLength = src->length[i];
                destStation.SegmentTime = src->time[i];

                destStation.QueueTime = src->queueTime[i];

                destStation.QueueLength = src->queueLength[i];
            }
            // All other values take 0 as their default. Since they're already memset to that, no need to do it again.
            for (int32_t i = Limits::kMaxStationsPerRide; i < OpenRCT2::Limits::kMaxStationsPerRide; i++)
            {
                StationIndex stationIndex = StationIndex::FromUnderlying(i);
                auto& destStation = dst->getStation(stationIndex);

                destStation.Start.SetNull();
                destStation.TrainAtStation = RideStation::kNoTrain;
                destStation.Entrance.SetNull();
                destStation.Exit.SetNull();
                destStation.LastPeepInQueue = EntityId::GetNull();
            }

            for (int32_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::FromUnderlying(src->vehicles[i]);
            }
            for (int32_t i = Limits::kMaxTrainsPerRide - 1; i <= OpenRCT2::Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicles[i] = EntityId::GetNull();
            }

            dst->departFlags = src->departFlags;

            dst->numStations = src->numStations;
            dst->numTrains = src->numTrains;
            dst->numCarsPerTrain = src->numCarsPerTrain;
            dst->proposedNumTrains = src->proposedNumTrains;
            dst->proposedNumCarsPerTrain = src->proposedNumCarsPerTrain;
            dst->maxTrains = src->maxTrains;
            dst->minCarsPerTrain = src->getMinCarsPerTrain();
            dst->maxCarsPerTrain = src->getMaxCarsPerTrain();
            dst->minWaitingTime = src->minWaitingTime;
            dst->maxWaitingTime = src->maxWaitingTime;

            // Includes timeLimit, NumLaps, launchSpeed, speed, rotations
            dst->operationOption = src->operationOption;

            dst->boatHireReturnDirection = src->boatHireReturnDirection;
            dst->boatHireReturnPosition = { src->boatHireReturnPosition.x, src->boatHireReturnPosition.y };

            auto split = splitCombinedHelicesAndSpecialElements(src->specialTrackElements);
            dst->numHelices = split.first;
            dst->specialTrackElements = split.second;
            // Pad0D6[2];

            dst->maxSpeed = src->maxSpeed;
            dst->averageSpeed = src->averageSpeed;
            dst->currentTestSegment = src->currentTestSegment;
            dst->averageSpeedTestTimeout = src->averageSpeedTestTimeout;
            // Pad0E2[0x2];

            dst->maxPositiveVerticalG = src->maxPositiveVerticalG;
            dst->maxNegativeVerticalG = src->maxNegativeVerticalG;
            dst->maxLateralG = src->maxLateralG;
            dst->previousVerticalG = src->previousVerticalG;
            dst->previousLateralG = src->previousLateralG;
            // Pad106[0x2];
            dst->testingFlags.holder = src->testingFlags;

            if (src->curTestTrackLocation.IsNull())
            {
                dst->curTestTrackLocation.SetNull();
            }
            else
            {
                dst->curTestTrackLocation = { src->curTestTrackLocation.x, src->curTestTrackLocation.y, src->curTestTrackZ };
            }

            dst->turnCountDefault = src->turnCountDefault;
            dst->turnCountBanked = src->turnCountBanked;
            dst->turnCountSloped = src->turnCountSloped;
            if (src->type == RIDE_TYPE_MINI_GOLF)
                dst->numHoles = src->inversions & kRCT12InversionAndHoleMask;
            else
                dst->numInversions = src->inversions & kRCT12InversionAndHoleMask;
            dst->shelteredEighths = src->inversions >> 5;
            auto splitDropsLifts = splitCombinedNumDropsPoweredLifts(src->drops);
            dst->numDrops = splitDropsLifts.first;
            dst->numPoweredLifts = splitDropsLifts.second;
            dst->startDropHeight = src->startDropHeight;
            dst->highestDropHeight = src->highestDropHeight;
            dst->shelteredLength = src->shelteredLength;
            dst->var11C = src->var11C;
            dst->numShelteredSections = src->numShelteredSections;

            dst->curNumCustomers = src->curNumCustomers;
            dst->numCustomersTimeout = src->numCustomersTimeout;

            for (uint8_t i = 0; i < Limits::kCustomerHistorySize; i++)
            {
                dst->numCustomers[i] = src->numCustomers[i];
            }

            dst->price[0] = src->price;

            for (uint8_t i = 0; i < 2; i++)
            {
                dst->chairliftBullwheelLocation[i] = { src->chairliftBullwheelLocation[i].x,
                                                       src->chairliftBullwheelLocation[i].y, src->chairliftBullwheelZ[i] };
            }

            dst->ratings = src->ratings;
            dst->value = ToMoney64(src->value);

            dst->chairliftBullwheelRotation = src->chairliftBullwheelRotation;

            dst->satisfaction = src->satisfaction;
            dst->satisfactionTimeout = src->satisfactionTimeOut;
            dst->satisfactionNext = src->satisfactionNext;

            dst->windowInvalidateFlags = src->windowInvalidateFlags;
            // Pad14E[0x02];

            dst->totalCustomers = src->totalCustomers;
            dst->totalProfit = ToMoney64(src->totalProfit);
            dst->popularity = src->popularity;
            dst->popularityTimeout = src->popularityTimeOut;
            dst->popularityNext = src->popularityNext;

            ImportNumRiders(dst, rideIndex);

            dst->musicTuneId = src->musicTuneId;
            dst->slideInUse = src->slideInUse;
            // Includes mazeTiles
            dst->slidePeep = EntityId::FromUnderlying(src->slidePeep);
            // Pad160[0xE];
            dst->slidePeepTShirtColour = src->slidePeepTShirtColour;
            // Pad16F[0x7];
            dst->spiralSlideProgress = src->spiralSlideProgress;
            // Pad177[0x9];
            dst->buildDate = static_cast<int32_t>(src->buildDate);
            dst->upkeepCost = ToMoney64(src->upkeepCost);
            dst->raceWinner = EntityId::FromUnderlying(src->raceWinner);
            // Pad186[0x02];
            dst->musicPosition = src->musicPosition;

            dst->breakdownReasonPending = src->breakdownReasonPending;
            dst->mechanicStatus = src->mechanicStatus;
            dst->mechanic = EntityId::FromUnderlying(src->mechanic);
            dst->inspectionStation = StationIndex::FromUnderlying(src->inspectionStation);
            dst->brokenTrain = src->brokenTrain;
            dst->brokenCar = src->brokenCar;
            dst->breakdownReason = src->breakdownReason;

            dst->price[1] = src->priceSecondary;

            dst->reliability = src->reliability;
            dst->unreliabilityFactor = src->unreliabilityFactor;
            dst->downtime = src->downtime;
            auto clampedInspectionInterval = std::clamp<uint8_t>(src->inspectionInterval, 0, EnumValue(RideInspection::never));
            dst->inspectionInterval = static_cast<RideInspection>(clampedInspectionInterval);
            dst->lastInspection = src->lastInspection;

            for (uint8_t i = 0; i < Limits::kDowntimeHistorySize; i++)
            {
                dst->downtimeHistory[i] = src->downtimeHistory[i];
            }

            dst->numPrimaryItemsSold = src->numPrimaryItemsSold;
            dst->numSecondaryItemsSold = src->numSecondaryItemsSold;

            dst->breakdownSoundModifier = src->breakdownSoundModifier;
            dst->notFixedTimeout = src->notFixedTimeout;
            dst->lastCrashType = src->lastCrashType;
            dst->connectedMessageThrottle = src->connectedMessageThrottle;

            dst->incomePerHour = ToMoney64(src->incomePerHour);
            dst->profit = ToMoney64(src->profit);

            for (uint8_t i = 0; i < Limits::kNumColourSchemes; i++)
            {
                dst->trackColours[i].main = src->trackColourMain[i];
                dst->trackColours[i].additional = src->trackColourAdditional[i];
                dst->trackColours[i].supports = src->trackColourSupports[i];
            }
            // This stall was not colourable in RCT2.
            if (dst->type == RIDE_TYPE_FOOD_STALL)
            {
                auto object = ObjectEntryGetObject(ObjectType::ride, dst->subtype);
                if (object != nullptr && object->GetIdentifier() == "rct2.ride.icecr1")
                {
                    dst->trackColours[0].main = Drawing::Colour::lightBlue;
                }
            }

            auto musicStyle = kObjectEntryIndexNull;
            if (GetRideTypeDescriptor(dst->type).HasFlag(RtdFlag::allowMusic))
            {
                musicStyle = src->music;
            }
            dst->music = musicStyle;

            auto entranceStyle = src->entranceStyle;
            // In SV7, "plain" entrances are invisible.
            if (_isSV7)
            {
                entranceStyle = _stationEntries.GetOrAddEntry(kNoEntranceNoPlatformIdentifier);
            }
            dst->entranceStyle = entranceStyle;

            dst->vehicleChangeTimeout = src->vehicleChangeTimeout;
            dst->numBlockBrakes = src->numBlockBrakes;
            dst->liftHillSpeed = src->liftHillSpeed;
            dst->guestsFavourite = src->guestsFavourite;
            dst->lifecycleFlags = src->lifecycleFlags;

            for (uint8_t i = 0; i < Limits::kMaxTrainsPerRide; i++)
            {
                dst->vehicleColours[i].Tertiary = src->vehicleColoursExtended[i];
            }

            dst->totalAirTime = src->totalAirTime;
            dst->currentTestStation = StationIndex::FromUnderlying(src->currentTestStation);
            dst->numCircuits = src->numCircuits;
            dst->cableLiftLoc = { src->cableLiftX, src->cableLiftY, src->cableLiftZ * kCoordsZStep };
            // Pad1FD;
            dst->cableLift = EntityId::FromUnderlying(src->cableLift);

            // Pad208[0x58];
        }

        void ImportMapAnimations()
        {
            for (const auto& mapAnimation : std::span(_s6.MapAnimations, _s6.NumMapAnimations))
            {
                switch (mapAnimation.Type)
                {
                    case kRCT12MapAnimationTypeOnRidePhoto:
                        MapAnimations::CreateTemporary(
                            { mapAnimation.x, mapAnimation.y, mapAnimation.BaseZ * kCoordsZStep },
                            MapAnimations::TemporaryType::onRidePhoto);
                        break;
                    case kRCT12MapAnimationTypeWallDoor:
                    {
                        const CoordsXYZ coords{ mapAnimation.x, mapAnimation.y, mapAnimation.BaseZ * kCoordsZStep };
                        const TileCoordsXYZ tileCoords{ coords };
                        TileElement* tileElement = MapGetFirstElementAt(tileCoords);
                        if (tileElement == nullptr)
                        {
                            continue;
                        }

                        do
                        {
                            if (tileElement->GetType() != TileElementType::Wall || tileElement->BaseHeight != tileCoords.z)
                            {
                                continue;
                            }

                            tileElement->AsWall()->SetIsAnimating(true);
                            MapAnimations::MarkTileForUpdate(tileCoords);
                        } while (!(tileElement++)->IsLastForTile());
                        break;
                    }
                }
            }
        }

        void ImportRideRatingsCalcData()
        {
            const auto& src = _s6.RideRatingsCalcData;
            // S6 has only one state, ensure we reset all states before reading the first one.
            RideRating::ResetUpdateStates();
            auto& rideRatingStates = getGameState().rideRatingUpdateStates;
            auto& dst = rideRatingStates[0];
            dst = {};
            dst.Proximity = { src.ProximityX, src.ProximityY, src.ProximityZ };
            dst.ProximityStart = { src.ProximityStartX, src.ProximityStartY, src.ProximityStartZ };
            dst.CurrentRide = RCT12RideIdToOpenRCT2RideId(src.CurrentRide);
            dst.State = src.State;
            if (src.CurrentRide < Limits::kMaxRidesInPark && _s6.Rides[src.CurrentRide].type < std::size(kRideTypeDescriptors))
            {
                dst.ProximityTrackType = RCT2TrackTypeToOpenRCT2(
                    src.ProximityTrackType, _s6.Rides[src.CurrentRide].type, IsFlatRide(src.CurrentRide));
            }
            else
            {
                dst.ProximityTrackType = TrackElemType::none;
            }
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
                if (src.RideIndex != kRCT12RideIdNull)
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
            dst.flags.holder = src.Flags;
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

        void ImportResearchList(GameState_t& gameState)
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
                    gameState.researchItemsInvented.emplace_back(researchItem.ToResearchItem());
                else
                    gameState.researchItemsUninvented.emplace_back(researchItem.ToResearchItem());
            }
        }

        void ImportBanner(Banner* dst, const RCT12Banner* src)
        {
            auto id = dst->id;

            *dst = {};
            dst->id = id;
            dst->type = RCTEntryIndexToOpenRCT2EntryIndex(src->Type);
            dst->flags = src->flags;

            if (!src->flags.has(BannerFlag::linkedToRide) && IsUserStringID(src->StringID))
            {
                dst->text = GetUserString(src->StringID);
            }

            if (src->flags.has(BannerFlag::linkedToRide))
            {
                dst->rideIndex = RCT12RideIdToOpenRCT2RideId(src->RideIndex);
            }
            else
            {
                dst->colour = static_cast<Drawing::Colour>(src->Colour);
            }

            dst->textColour = src->textColour;
            dst->position.x = src->x;
            dst->position.y = src->y;
        }

        void Initialise(GameState_t& gameState)
        {
            gameStateInitAll(gameState, { _s6.MapSize, _s6.MapSize });
        }

        /**
         * Imports guest entry points.
         * Includes fixes for incorrectly set guest entry points in some scenarios.
         */
        void ImportPeepSpawns(GameState_t& gameState)
        {
            // Many WW and TT have scenario_filename fields containing an incorrect filename. Check for both this filename
            // and the corrected filename.

            // In this park, peep_spawns[0] is incorrect, and peep_spawns[1] is correct.
            if (String::equals(_s6.ScenarioFilename, "WW South America - Rio Carnival.SC6")
                || String::equals(_s6.ScenarioFilename, "South America - Rio Carnival.SC6"))
            {
                _s6.PeepSpawns[0] = { 2160, 3167, 6, 1 };
                _s6.PeepSpawns[1].x = kRCT12PeepSpawnUndefined;
            }
            // In this park, peep_spawns[0] is correct. Just clear the other.
            else if (
                String::equals(_s6.ScenarioFilename, "Great Wall of China Tourism Enhancement.SC6")
                || String::equals(_s6.ScenarioFilename, "Asia - Great Wall of China Tourism Enhancement.SC6"))
            {
                _s6.PeepSpawns[1].x = kRCT12PeepSpawnUndefined;
            }
            // Amity Airfield has peeps entering from the corner of the tile, instead of the middle.
            else if (String::equals(_s6.ScenarioFilename, "Amity Airfield.SC6"))
            {
                _s6.PeepSpawns[0].y = 1296;
            }
            // #9926: Africa - Oasis has peeps spawning on the edge underground near the entrance
            else if (String::equals(_s6.ScenarioFilename, "Africa - Oasis.SC6"))
            {
                _s6.PeepSpawns[0].y = 2128;
                _s6.PeepSpawns[0].z = 7;
            }

            gameState.peepSpawns.clear();
            for (size_t i = 0; i < Limits::kMaxPeepSpawns; i++)
            {
                if (_s6.PeepSpawns[i].x != kRCT12PeepSpawnUndefined)
                {
                    PeepSpawn spawn = { _s6.PeepSpawns[i].x, _s6.PeepSpawns[i].y, _s6.PeepSpawns[i].z * 16,
                                        _s6.PeepSpawns[i].direction };
                    gameState.peepSpawns.push_back(spawn);
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
                if (entity.Unknown.EntityIdentifier == RCT12EntityIdentifier::peep)
                {
                    if (entity.Peep.CurrentRide == static_cast<RCT12RideId>(rideIndex.ToUnderlying())
                        && (static_cast<PeepState>(entity.Peep.State) == PeepState::onRide
                            || static_cast<PeepState>(entity.Peep.State) == PeepState::enteringRide))
                    {
                        numRiders++;
                    }
                }
            }
            dst->numRiders = numRiders;
        }

        void ImportTileElements(GameState_t& gameState)
        {
            // Build tile pointer cache (needed to get the first element at a certain location)
            auto tilePointerIndex = TilePointerIndex<RCT12TileElement>(
                Limits::kMaxMapSize, _s6.TileElements, std::size(_s6.TileElements));

            std::vector<TileElement> tileElements;
            bool nextElementInvisible = false;
            bool restOfTileInvisible = false;
            const auto maxSize = std::min(Limits::kMaxMapSize, _s6.MapSize);
            for (TileCoordsXY coords = { 0, 0 }; coords.y < kMaximumMapSizeTechnical; coords.y++)
            {
                for (coords.x = 0; coords.x < kMaximumMapSizeTechnical; coords.x++)
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
                                if (srcElement->BaseHeight == Limits::kMaxElementHeight)
                                {
                                    continue;
                                }

                                auto tileElementType = srcElement->GetType();
                                if (tileElementType == RCT12TileElementType::corrupt)
                                {
                                    // One property of corrupt elements was to hide tops of tower tracks, and to avoid the next
                                    // element from being hidden, multiple consecutive corrupt elements were sometimes used.
                                    // This would essentially toggle the flag, so we inverse nextElementInvisible here instead
                                    // of always setting it to true.
                                    nextElementInvisible = !nextElementInvisible;
                                    continue;
                                }
                                if (tileElementType == RCT12TileElementType::eightCarsCorrupt14
                                    || tileElementType == RCT12TileElementType::eightCarsCorrupt15)
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
            SetTileElements(gameState, std::move(tileElements));
        }

        void ImportTileElement(TileElement* dst, const RCT12TileElement* src, bool invisible)
        {
            const auto rct12Type = src->GetType();
            dst->ClearAs(ToOpenRCT2TileElementType(rct12Type));
            dst->SetDirection(src->GetDirection());
            dst->SetBaseZ(src->BaseHeight * kCoordsZStep);
            dst->SetClearanceZ(src->ClearanceHeight * kCoordsZStep);

            // All saved in "flags"
            dst->SetOccupiedQuadrants(src->GetOccupiedQuadrants());
            dst->SetGhost(src->IsGhost());
            dst->SetLastForTile(src->IsLastForTile());
            dst->SetInvisible(invisible);

            switch (rct12Type)
            {
                case RCT12TileElementType::surface:
                {
                    auto dst2 = dst->AsSurface();
                    auto src2 = src->AsSurface();

                    dst2->SetSlope(src2->GetSlope());

                    dst2->SetSurfaceObjectIndex(src2->GetSurfaceStyle());
                    dst2->SetEdgeObjectIndex(src2->GetEdgeStyle());

                    dst2->SetGrassLength(src2->GetGrassLength());
                    dst2->SetOwnership(src2->GetOwnership());
                    dst2->SetParkFences(src2->GetParkFences());
                    dst2->SetWaterHeight(src2->GetWaterHeight());
                    dst2->SetHasTrackThatNeedsWater(src2->HasTrackThatNeedsWater());

                    break;
                }
                case RCT12TileElementType::path:
                {
                    auto dst2 = dst->AsPath();
                    auto src2 = src->AsPath();

                    auto pathEntryIndex = src2->GetEntryIndex();
                    auto surfaceEntry = src2->IsQueue() ? _pathToQueueSurfaceMap[pathEntryIndex]
                                                        : _pathToSurfaceMap[pathEntryIndex];
                    if (surfaceEntry == kObjectEntryIndexNull)
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
                case RCT12TileElementType::track:
                {
                    auto dst2 = dst->AsTrack();
                    auto src2 = src->AsTrack();

                    auto rideType = _s6.Rides[src2->GetRideIndex()].type;
                    auto oldTrackType = src2->GetTrackType();
                    TrackElemType trackType = RCT2TrackTypeToOpenRCT2(oldTrackType, rideType, IsFlatRide(src2->GetRideIndex()));

                    dst2->SetTrackType(trackType);
                    dst2->SetRideType(rideType);
                    dst2->SetSequenceIndex(src2->GetSequenceIndex());
                    dst2->SetRideIndex(RCT12RideIdToOpenRCT2RideId(src2->GetRideIndex()));
                    dst2->SetColourScheme(src2->GetColourScheme());
                    dst2->SetHasChain(src2->HasChain());
                    dst2->SetHasCableLift(src2->HasCableLift());
                    dst2->SetInverted(src2->IsInverted());
                    dst2->SetStationIndex(StationIndex::FromUnderlying(src2->GetStationIndex()));
                    dst2->SetHasGreenLight(src2->HasGreenLight());
                    // Brakes import as closed to preserve legacy behaviour
                    dst2->SetBrakeClosed(src2->BlockBrakeClosed() || (trackType == TrackElemType::brakes));
                    dst2->SetIsIndestructible(src2->IsIndestructible());
                    // Skipping IsHighlighted()

                    // Import block brakes to keep legacy behaviour
                    if (trackType == TrackElemType::blockBrakes)
                    {
                        dst2->SetBrakeBoosterSpeed(kRCT2DefaultBlockBrakeSpeed);
                    }
                    else if (TrackTypeHasSpeedSetting(trackType))
                    {
                        dst2->SetBrakeBoosterSpeed(src2->GetBrakeBoosterSpeed());
                    }
                    else if (trackType == TrackElemType::onRidePhoto)
                    {
                        dst2->SetPhotoTimeout(src2->GetPhotoTimeout());
                    }

                    // This has to be done last, since the maze entry shares fields with the colour and sequence fields.
                    const auto& rtd = GetRideTypeDescriptor(rideType);
                    if (rtd.specialType == RtdSpecialType::maze)
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
                case RCT12TileElementType::smallScenery:
                {
                    auto dst2 = dst->AsSmallScenery();
                    auto src2 = src->AsSmallScenery();

                    dst2->SetEntryIndex(src2->GetEntryIndex());
                    dst2->SetAge(src2->GetAge());
                    dst2->SetSceneryQuadrant(src2->GetSceneryQuadrant());
                    dst2->SetPrimaryColour(src2->GetPrimaryColour());
                    dst2->SetSecondaryColour(src2->GetSecondaryColour());
                    if (src2->NeedsSupports())
                        dst2->SetNeedsSupports();

                    break;
                }
                case RCT12TileElementType::entrance:
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
                        if (surfaceEntry == kObjectEntryIndexNull)
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
                        dst2->SetSurfaceEntryIndex(kObjectEntryIndexNull);
                    }
                    break;
                }
                case RCT12TileElementType::wall:
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
                    if (entry != nullptr && entry->scrolling_mode != kScrollingModeNone)
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
                case RCT12TileElementType::largeScenery:
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
                    if (entry != nullptr && entry->scrolling_mode != kScrollingModeNone)
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
                case RCT12TileElementType::banner:
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

        void ImportMarketingCampaigns(Park::ParkData& park)
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
                        campaign.flags.set(MarketingCampaignFlag::firstWeek);
                    }
                    if (campaign.Type == ADVERTISING_CAMPAIGN_RIDE_FREE || campaign.Type == ADVERTISING_CAMPAIGN_RIDE)
                    {
                        campaign.RideId = RCT12RideIdToOpenRCT2RideId(_s6.CampaignRideIndex[i]);
                    }
                    else if (campaign.Type == ADVERTISING_CAMPAIGN_FOOD_OR_DRINK_FREE)
                    {
                        campaign.ShopItemType = ShopItem(_s6.CampaignRideIndex[i]);
                    }
                    park.marketingCampaigns.push_back(campaign);
                }
            }
        }

        void ImportStaffPatrolArea(Staff* staffmember, uint8_t staffId)
        {
            // First check staff mode as vanilla did not clean up patrol areas when switching from patrol to walk
            // without doing this we could accidentally add a patrol when it didn't exist.
            if (_s6.StaffModes[staffId] != StaffMode::patrol)
            {
                return;
            }
            int32_t peepOffset = staffId * Limits::kPatrolAreaSize;
            for (int32_t i = 0; i < Limits::kPatrolAreaSize; i++)
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
                    staffmember->SetPatrolArea(MapRange(x, y, x + (4 * kCoordsXYStep) - 1, y + (4 * kCoordsXYStep) - 1), true);
                }
            }
        }

        void ImportEntities(GameState_t& gameState)
        {
            for (int32_t i = 0; i < GetMaxEntities(); i++)
            {
                ImportEntity(gameState, _s6.Entities[i].Unknown);
            }
        }

        uint16_t GetMaxEntities()
        {
            return (_s6.Header.ClassicFlag == 0xf) ? Limits::kMaxEntitiesRCTCExtended : Limits::kMaxEntities;
        }

        template<typename OpenRCT2_T>
        void ImportEntity(GameState_t& gameState, const RCT12EntityBase& src);

        void ImportEntityPeep(::Peep* dst, const Peep* src)
        {
            const auto isNullLocation = [](const RCT12xyzd8& pos) {
                return pos.x == 0xFF && pos.y == 0xFF && pos.z == 0xFF && pos.direction == kInvalidDirection;
            };

            ImportEntityCommonProperties(static_cast<EntityBase*>(dst), src);
            if (IsUserStringID(src->NameStringIdx))
            {
                dst->SetName(GetUserString(src->NameStringIdx));
            }
            dst->NextLoc = { src->NextX, src->NextY, src->NextZ * kCoordsZStep };
            dst->NextFlags = src->NextFlags;
            dst->State = static_cast<PeepState>(src->State);
            dst->SubState = src->SubState;

            // TODO
            dst->AnimationObjectIndex = kObjectEntryIndexNull;
            dst->AnimationGroup = static_cast<PeepAnimationGroup>(src->AnimationGroup);

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
            dst->AnimationType = static_cast<PeepAnimationType>(src->AnimationType);
            dst->NextAnimationType = static_cast<PeepAnimationType>(src->NextAnimationType);
            dst->AnimationImageIdOffset = src->AnimationImageIdOffset;
            dst->Action = static_cast<PeepActionType>(src->Action);
            dst->AnimationFrameNum = src->AnimationFrameNum;
            dst->StepProgress = src->StepProgress;
            dst->PeepDirection = src->Direction;
            dst->InteractionRideIndex = RCT12RideIdToOpenRCT2RideId(src->InteractionRideIndex);
            dst->PeepId = src->Id;
            dst->PathCheckOptimisation = src->PathCheckOptimisation;
            dst->PeepFlags = src->PeepFlags;
            if (isNullLocation(src->PathfindGoal))
            {
                dst->PathfindGoal.SetNull();
                dst->PathfindGoal.direction = kInvalidDirection;
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
                    dst->PathfindHistory[i].direction = kInvalidDirection;
                }
                else
                {
                    dst->PathfindHistory[i] = { src->PathfindHistory[i].x, src->PathfindHistory[i].y, src->PathfindHistory[i].z,
                                                src->PathfindHistory[i].direction };
                }
            }
            dst->WalkingAnimationFrameNum = src->NoActionFrameNum;
        }

        constexpr EntityType GetEntityTypeFromRCT2Sprite(const RCT12EntityBase* src)
        {
            EntityType output = EntityType::null;
            switch (src->EntityIdentifier)
            {
                case RCT12EntityIdentifier::vehicle:
                    output = EntityType::vehicle;
                    break;
                case RCT12EntityIdentifier::peep:
                {
                    const auto& peep = static_cast<const Peep&>(*src);
                    if (RCT12PeepType(peep.PeepType) == RCT12PeepType::guest)
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

                    switch (RCT12MiscEntityType(src->Type))
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

        void ImportEntityCommonProperties(EntityBase* dst, const RCT12EntityBase* src)
        {
            dst->Type = GetEntityTypeFromRCT2Sprite(src);
            dst->Id = EntityId::FromUnderlying(src->EntityIndex);
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
            dst->SpriteData.Width = src->SpriteWidth;
            dst->SpriteData.HeightMin = src->SpriteHeightNegative;
            dst->SpriteData.HeightMax = src->SpriteHeightPositive;
            dst->SpriteData.SpriteRect = ScreenRect(src->SpriteLeft, src->SpriteTop, src->SpriteRight, src->SpriteBottom);
            dst->Orientation = src->EntityDirection;
        }

        void ImportEntity(GameState_t& gameState, const RCT12EntityBase& src);

        std::string GetUserString(StringId stringId)
        {
            const auto originalString = _s6.CustomStrings[stringId % 1024];
            auto originalStringView = std::string_view(
                originalString, RCT12::GetRCTStringBufferLen(originalString, kUserStringMaxLength));
            auto asUtf8 = RCT2StringToUTF8(originalStringView, RCT2LanguageId::englishUK);
            auto justText = RCT12RemoveFormattingUTF8(asUtf8);
            return justText.data();
        }

        ObjectList GetRequiredObjects()
        {
            std::fill(std::begin(_pathToSurfaceMap), std::end(_pathToSurfaceMap), kObjectEntryIndexNull);
            std::fill(std::begin(_pathToQueueSurfaceMap), std::end(_pathToQueueSurfaceMap), kObjectEntryIndexNull);
            std::fill(std::begin(_pathToRailingMap), std::end(_pathToRailingMap), kObjectEntryIndexNull);

            ObjectList objectList;
            int objectIt = 0;
            ObjectEntryIndex surfaceCount = 0;
            ObjectEntryIndex railingCount = 0;
            for (int16_t objectType = EnumValue(ObjectType::ride); objectType <= EnumValue(ObjectType::water); objectType++)
            {
                for (int16_t i = 0; i < kRCT2ObjectEntryGroupCounts[objectType]; i++, objectIt++)
                {
                    auto entry = ObjectEntryDescriptor(_s6.Objects[objectIt]);
                    if (entry.HasValue())
                    {
                        if (objectType == EnumValue(ObjectType::paths))
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
                                    ObjectType::footpathSurface, footpathMapping->NormalSurface);
                                if (surfaceIndex == kObjectEntryIndexNull)
                                {
                                    objectList.SetObject(
                                        ObjectType::footpathSurface, surfaceCount, footpathMapping->NormalSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToSurfaceMap[i] = surfaceIndex;

                                surfaceIndex = objectList.Find(ObjectType::footpathSurface, footpathMapping->QueueSurface);
                                if (surfaceIndex == kObjectEntryIndexNull)
                                {
                                    objectList.SetObject(
                                        ObjectType::footpathSurface, surfaceCount, footpathMapping->QueueSurface);
                                    surfaceIndex = surfaceCount++;
                                }
                                _pathToQueueSurfaceMap[i] = surfaceIndex;

                                auto railingIndex = objectList.Find(ObjectType::footpathRailings, footpathMapping->Railing);
                                if (railingIndex == kObjectEntryIndexNull)
                                {
                                    objectList.SetObject(ObjectType::footpathRailings, railingCount, footpathMapping->Railing);
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
                    if (surface->GetSurfaceStyle() >= std::size(DefaultTerrainSurfaces))
                    {
                        return true;
                    }
                    if (surface->GetEdgeStyle() >= std::size(DefaultTerrainEdges))
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

            AppendRequiredObjects(objectList, ObjectType::terrainSurface, _terrainSurfaceEntries);
            AppendRequiredObjects(objectList, ObjectType::terrainEdge, _terrainEdgeEntries);

            const bool hasInvisibleEntrance = std::any_of(std::begin(_s6.Rides), std::end(_s6.Rides), [](Ride& ride) {
                if (ride.type == kRideTypeNull)
                    return false;

                return ride.entranceStyle == RCT12_STATION_STYLE_INVISIBLE;
            });
            if (hasInvisibleEntrance)
            {
                _stationEntries.GetOrAddEntry(GetStationIdentifierFromStyle(RCT12_STATION_STYLE_INVISIBLE));
            }
            if (_isSV7)
            {
                _stationEntries.GetOrAddEntry(kNoEntranceNoPlatformIdentifier);
            }
            AppendRequiredObjects(objectList, ObjectType::station, _stationEntries);

            AppendRequiredObjects(
                objectList, ObjectType::peepNames, std::vector<std::string_view>({ "rct2.peep_names.original" }));
            RCT12AddDefaultMusic(objectList);

            // Normalise the name to make the scenario as recognisable as possible
            auto normalisedName = ScenarioSources::NormaliseName(_s6.Info.Name);

            // Infer what scenario meta object to use, if any
            SourceDescriptor desc;
            if (ScenarioSources::TryGetByName(normalisedName.c_str(), &desc) && !desc.textObjectId.empty())
                AppendRequiredObjects(
                    objectList, ObjectType::scenarioMeta, std::vector<std::string_view>({ desc.textObjectId }));

            auto animObjects = GetLegacyPeepAnimationObjects();
            AppendRequiredObjects(objectList, ObjectType::peepAnimations, animObjects);

            auto climateObjId = GetClimateObjectIdFromLegacyClimateType(_s6.Climate);
            AppendRequiredObjects(objectList, ObjectType::climate, std::vector({ climateObjId }));

            return objectList;
        }
    };

    template<>
    void S6Importer::ImportEntity<::Vehicle>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Vehicle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const Vehicle*>(&baseSrc);
        const auto& ride = _s6.Rides[src->Ride];

        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Vehicle::Type(src->Type);
        dst->pitch = src->pitch;
        dst->roll = src->roll;
        dst->remaining_distance = src->RemainingDistance;
        dst->velocity = src->Velocity;
        dst->acceleration = src->Acceleration;
        dst->ride = RideId::FromUnderlying(src->Ride);
        dst->vehicle_type = src->VehicleType;
        dst->colours.Body = static_cast<Drawing::Colour>(src->Colours.BodyColour);
        dst->colours.Trim = static_cast<Drawing::Colour>(src->Colours.TrimColour);
        dst->colours.Tertiary = static_cast<Drawing::Colour>(src->ColoursExtended);
        dst->track_progress = src->TrackProgress;
        dst->TrackLocation = { src->TrackX, src->TrackY, src->TrackZ };
        if (src->BoatLocation.IsNull() || static_cast<RideMode>(ride.mode) != RideMode::boatHire
            || src->Status != static_cast<uint8_t>(::Vehicle::Status::travellingBoat))
        {
            dst->BoatLocation.SetNull();
            dst->SetTrackDirection(src->GetTrackDirection());
            // Skipping OriginalRideClass::wildMouse - this is handled specifically.
            auto originalClass = IsFlatRide(src->Ride) ? OriginalRideClass::flatRide : OriginalRideClass::regular;
            auto convertedType = RCT2TrackTypeToOpenRCT2(src->GetTrackType(), originalClass);
            dst->SetTrackType(convertedType);
            // RotationControlToggle and Booster are saved as the same track piece ID
            // Which one the vehicle is using must be determined
            if (src->GetTrackType() == RCT12::TrackElemType::rotationControlToggleAlias)
            {
                // Merging hacks mean the track type that's appropriate for the ride type is not necessarily the track type the
                // ride is on. It's possible to create unwanted behavior if a user layers spinning control track on top of
                // booster track but this is unlikely since only two rides have spinning control track - by default they load as
                // booster.
                TileElement* tileElement2 = MapGetTrackElementAtOfTypeSeq(
                    dst->TrackLocation, TrackElemType::rotationControlToggle, 0);

                if (tileElement2 != nullptr)
                    dst->SetTrackType(TrackElemType::rotationControlToggle);
            }
            else if (src->GetTrackType() == RCT12::TrackElemType::blockBrakes)
            {
                dst->brake_speed = kRCT2DefaultBlockBrakeSpeed;
            }
        }
        else
        {
            dst->BoatLocation = TileCoordsXY{ src->BoatLocation.x, src->BoatLocation.y }.ToCoordsXY();
            dst->SetTrackDirection(0);
            dst->SetTrackType(TrackElemType::flat);
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

        ::Vehicle::Status statusSrc = ::Vehicle::Status::movingToEndOfStation;
        if (src->Status <= static_cast<uint8_t>(::Vehicle::Status::stoppedByBlockBrakes))
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
        dst->sound1_id = static_cast<Audio::SoundId>(src->Sound1Id);
        dst->sound1_volume = src->Sound1Volume;
        dst->sound2_id = static_cast<Audio::SoundId>(src->Sound2Id);
        dst->sound2_volume = src->Sound2Volume;
        dst->dopplerShift = src->SoundVectorFactor;
        dst->time_waiting = src->TimeWaiting;
        dst->speed = src->Speed;
        dst->powered_acceleration = src->PoweredAcceleration;
        dst->CollisionDetectionTimer = src->CollisionDetectionTimer;
        dst->animation_frame = src->AnimationFrame;
        dst->animationState = src->AnimationState;
        dst->scream_sound_id = static_cast<Audio::SoundId>(src->ScreamSoundId);
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
        dst->BlockBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;
    }

    static uint32_t AdjustScenarioToCurrentTicks(const S6Data& s6, uint32_t tick)
    {
        // Previously gScenarioTicks was used as a time point, now it's gCurrentTicks.
        // gCurrentTicks and gScenarioTicks are now exported as the same, older saves that have a different
        // scenario tick must account for the difference between the two.
        uint32_t ticksElapsed = s6.ScenarioTicks - tick;
        return s6.GameTicks1 - ticksElapsed;
    }

    template<>
    void S6Importer::ImportEntity<::Guest>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Guest>(EntityId::FromUnderlying(baseSrc.EntityIndex));
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

        RideUse::GetHistory().Set(dst->Id, RCT12GetRidesBeenOn(src));
        RideUse::GetTypeHistory().Set(dst->Id, RCT12GetRideTypesBeenOn(src));

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
            if (srcThought->Item == kRCT12PeepThoughtItemNone)
                dstThought->item = kPeepThoughtItemNone;
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

    template<>
    void S6Importer::ImportEntity<::Staff>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Staff>(EntityId::FromUnderlying(baseSrc.EntityIndex));
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

    template<>
    void S6Importer::ImportEntity<::SteamParticle>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::SteamParticle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntitySteamParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->time_to_move = src->TimeToMove;
        dst->frame = src->Frame;
    }

    template<>
    void S6Importer::ImportEntity<::MoneyEffect>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::MoneyEffect>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityMoneyEffect*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->MoveDelay = src->MoveDelay;
        dst->NumMovements = src->NumMovements;
        dst->GuestPurchase = src->Vertical;
        dst->Value = src->Value;
        dst->OffsetX = src->OffsetX;
        dst->Wiggle = src->Wiggle;
    }

    template<>
    void S6Importer::ImportEntity<::VehicleCrashParticle>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::VehicleCrashParticle>(
            EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityCrashedVehicleParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->time_to_live = src->TimeToLive;
        dst->frame = src->Frame;
        dst->colour[0] = static_cast<Drawing::Colour>(src->Colour[0]);
        dst->colour[1] = static_cast<Drawing::Colour>(src->Colour[1]);
        dst->crashed_sprite_base = src->CrashedEntityBase;
        dst->velocity_x = src->VelocityX;
        dst->velocity_y = src->VelocityY;
        dst->velocity_z = src->VelocityZ;
        dst->acceleration_x = src->AccelerationX;
        dst->acceleration_y = src->AccelerationY;
        dst->acceleration_z = src->AccelerationZ;
    }

    template<>
    void S6Importer::ImportEntity<::ExplosionCloud>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::ExplosionCloud>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S6Importer::ImportEntity<::ExplosionFlare>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::ExplosionFlare>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S6Importer::ImportEntity<::CrashSplashParticle>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::CrashSplashParticle>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityParticle*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
    }

    template<>
    void S6Importer::ImportEntity<::JumpingFountain>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::JumpingFountain>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityJumpingFountain*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->NumTicksAlive = src->NumTicksAlive;
        dst->frame = src->Frame;
        dst->fountainFlags = src->fountainFlags;
        dst->TargetX = src->TargetX;
        dst->TargetY = src->TargetY;
        dst->Iteration = src->Iteration;
        dst->FountainType = RCT12MiscEntityType(src->Type) == RCT12MiscEntityType::jumpingFountainSnow
            ? ::JumpingFountainType::Snow
            : ::JumpingFountainType::Water;
    }

    template<>
    void S6Importer::ImportEntity<::Balloon>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Balloon>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityBalloon*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->popped = src->Popped;
        dst->time_to_move = src->TimeToMove;
        dst->frame = src->Frame;
        dst->colour = static_cast<Drawing::Colour>(src->Colour);
    }

    template<>
    void S6Importer::ImportEntity<::Duck>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Duck>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityDuck*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->frame = src->Frame;
        dst->target_x = src->TargetX;
        dst->target_y = src->TargetY;
        dst->state = static_cast<::Duck::DuckState>(src->State);
    }

    template<>
    void S6Importer::ImportEntity<::Litter>(GameState_t& gameState, const RCT12EntityBase& baseSrc)
    {
        auto dst = getGameState().entities.CreateEntityAt<::Litter>(EntityId::FromUnderlying(baseSrc.EntityIndex));
        auto src = static_cast<const RCT12EntityLitter*>(&baseSrc);
        ImportEntityCommonProperties(dst, src);
        dst->SubType = ::Litter::Type(src->Type);
        dst->creationTick = AdjustScenarioToCurrentTicks(_s6, src->CreationTick);
    }

    void S6Importer::ImportEntity(GameState_t& gameState, const RCT12EntityBase& src)
    {
        switch (GetEntityTypeFromRCT2Sprite(&src))
        {
            case EntityType::vehicle:
                ImportEntity<::Vehicle>(gameState, src);
                break;
            case EntityType::guest:
                ImportEntity<::Guest>(gameState, src);
                break;
            case EntityType::staff:
                ImportEntity<::Staff>(gameState, src);
                break;
            case EntityType::steamParticle:
                ImportEntity<::SteamParticle>(gameState, src);
                break;
            case EntityType::moneyEffect:
                ImportEntity<::MoneyEffect>(gameState, src);
                break;
            case EntityType::crashedVehicleParticle:
                ImportEntity<::VehicleCrashParticle>(gameState, src);
                break;
            case EntityType::explosionCloud:
                ImportEntity<::ExplosionCloud>(gameState, src);
                break;
            case EntityType::explosionFlare:
                ImportEntity<::ExplosionFlare>(gameState, src);
                break;
            case EntityType::crashSplash:
                ImportEntity<::CrashSplashParticle>(gameState, src);
                break;
            case EntityType::jumpingFountain:
                ImportEntity<::JumpingFountain>(gameState, src);
                break;
            case EntityType::balloon:
                ImportEntity<::Balloon>(gameState, src);
                break;
            case EntityType::duck:
                ImportEntity<::Duck>(gameState, src);
                break;
            case EntityType::litter:
                ImportEntity<::Litter>(gameState, src);
                break;
            default:
                // Null elements do not need imported
                break;
        }
    }
} // namespace OpenRCT2::RCT2

std::unique_ptr<IParkImporter> ParkImporter::CreateS6(IObjectRepository& objectRepository)
{
    return std::make_unique<RCT2::S6Importer>(objectRepository);
}
