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

constexpr ObjectEntryIndex BANNER_NULL = kObjectEntryIndexNull;
constexpr size_t MAX_BANNERS = 8192;

constexpr uint8_t SCROLLING_MODE_NONE = 255;

struct Banner
{
    BannerIndex id = BannerIndex::GetNull();
    ObjectEntryIndex type = BANNER_NULL;
    uint8_t flags{};
    std::string text;
    mutable std::string formattedTextBuffer;
    uint8_t colour{};
    RideId ride_index{};
    uint8_t text_colour{};
    TileCoordsXY position;

    bool IsNull() const
    {
        return type == BANNER_NULL;
    }

    std::string GetText() const;
    void FormatTextTo(Formatter&, bool addColour) const;
    void FormatTextTo(Formatter&) const;
};

enum BANNER_FLAGS
{
    BANNER_FLAG_NO_ENTRY = (1 << 0),
    BANNER_FLAG_IS_LARGE_SCENERY = (1 << 1),
    BANNER_FLAG_LINKED_TO_RIDE = (1 << 2),
    BANNER_FLAG_IS_WALL = (1 << 3)
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
