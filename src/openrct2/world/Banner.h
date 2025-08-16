/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/FlagHolder.hpp"
#include "../drawing/TextColour.h"
#include "../ride/RideTypes.h"
#include "Location.hpp"

#include <string>

class Formatter;
struct TileElement;
struct WallElement;

namespace OpenRCT2
{
    struct GameState_t;
}

constexpr OpenRCT2::ObjectEntryIndex kBannerNull = OpenRCT2::kObjectEntryIndexNull;
constexpr size_t kMaxBanners = 8192;

constexpr uint8_t kScrollingModeNone = 255;

enum class BannerFlag : uint8_t
{
    noEntry = 0,
    isLargeScenery = 1,
    linkedToRide = 2,
    isWall = 3,
};
using BannerFlags = FlagHolder<uint8_t, BannerFlag>;

struct Banner
{
    BannerIndex id = BannerIndex::GetNull();
    OpenRCT2::ObjectEntryIndex type = kBannerNull;
    BannerFlags flags{};
    std::string text;
    mutable std::string formattedTextBuffer;
    uint8_t colour{};
    RideId rideIndex{};
    OpenRCT2::TextColour textColour{};
    TileCoordsXY position;

    bool isNull() const
    {
        return type == kBannerNull;
    }

    std::string getText() const;
    void formatTextWithColourTo(Formatter&) const;
    void formatTextTo(Formatter&) const;
};

void BannerInit(OpenRCT2::GameState_t& gameState);
TileElement* BannerGetTileElement(BannerIndex bannerIndex);
WallElement* BannerGetScrollingWallTileElement(BannerIndex bannerIndex);
RideId BannerGetClosestRideIndex(const CoordsXYZ& mapPos);
void BannerApplyFixes();
void UnlinkAllRideBanners();
void UnlinkAllBannersForRide(RideId rideId);
Banner* GetBanner(BannerIndex id);
Banner* GetOrCreateBanner(BannerIndex id);
Banner* CreateBanner();
void DeleteBanner(BannerIndex id);
void TrimBanners();
size_t GetNumBanners();
bool HasReachedBannerLimit();
