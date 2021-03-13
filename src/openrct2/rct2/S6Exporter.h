/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../object/ObjectList.h"
#include "../scenario/Scenario.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    struct IStream;
}

struct Litter;
struct ObjectRepositoryItem;
struct RCT12SpriteBase;
union rct_sprite;
struct SpriteBase;

/**
 * Class to export RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Exporter final
{
public:
    bool RemoveTracklessRides;
    std::vector<const ObjectRepositoryItem*> ExportObjectsList;

    S6Exporter();

    void SaveGame(const utf8* path);
    void SaveGame(OpenRCT2::IStream* stream);
    void SaveScenario(const utf8* path);
    void SaveScenario(OpenRCT2::IStream* stream);
    void Export();
    void ExportParkName();
    void ExportRides();
    void ExportRide(rct2_ride* dst, const Ride* src);
    void ExportEntities();
    template<typename RCT12_T, typename OpenRCT2_T> void ExportEntity(RCT12_T* dst, const OpenRCT2_T* src);
    void ExportEntityCommonProperties(RCT12SpriteBase* dst, const SpriteBase* src);
    void ExportEntityPeep(RCT2SpritePeep* dst, const Peep* src);

private:
    rct_s6_data _s6{};
    std::vector<std::string> _userStrings;

    void Save(OpenRCT2::IStream* stream, bool isScenario);
    static uint32_t GetLoanHash(money32 initialCash, money32 bankLoan, uint32_t maxBankLoan);
    void ExportResearchedRideTypes();
    void ExportResearchedRideEntries();
    void ExportResearchedSceneryItems();
    void ExportResearchList();
    void ExportMarketingCampaigns();
    void ExportPeepSpawns();
    void ExportRideRatingsCalcData();
    void ExportRideMeasurements();
    void ExportRideMeasurement(RCT12RideMeasurement& dst, const RideMeasurement& src);
    void ExportBanners();
    void ExportBanner(RCT12Banner& dst, const Banner& src);
    void ExportMapAnimations();

    void ExportTileElements();
    void ExportTileElement(RCT12TileElement* dst, TileElement* src);

    std::optional<uint16_t> AllocateUserString(std::string_view value);
    void ExportUserStrings();
    void RebuildEntityLinks();
};
