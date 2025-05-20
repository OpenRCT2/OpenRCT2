/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Banner.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../Game.h"
#include "../GameState.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"
#include "../management/Finance.h"
#include "../network/Network.h"
#include "../object/BannerSceneryEntry.h"
#include "../object/ObjectEntryManager.h"
#include "../object/WallSceneryEntry.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/Track.h"
#include "../windows/Intent.h"
#include "MapAnimation.h"
#include "Park.h"
#include "Scenery.h"
#include "TileElementsView.h"
#include "tile_element/BannerElement.h"
#include "tile_element/TileElement.h"
#include "tile_element/TrackElement.h"
#include "tile_element/WallElement.h"

#include <cstring>
#include <iterator>
#include <limits>

using namespace OpenRCT2;

std::string Banner::GetText() const
{
    Formatter ft;
    FormatTextTo(ft);
    return FormatStringIDLegacy(STR_STRINGID, ft.Data());
}

void Banner::FormatTextTo(Formatter& ft, bool addColour) const
{
    if (addColour)
    {
        auto formatToken = FormatTokenFromTextColour(textColour);
        formattedTextBuffer = FormatTokenToStringWithBraces(formatToken);
        ft.Add<StringId>(STR_STRING_STRINGID);
        ft.Add<const char*>(formattedTextBuffer.data());
    }

    FormatTextTo(ft);
}

void Banner::FormatTextTo(Formatter& ft) const
{
    if (flags.has(BannerFlag::noEntry))
    {
        ft.Add<StringId>(STR_NO_ENTRY);
    }
    else if (flags.has(BannerFlag::linkedToRide))
    {
        auto ride = GetRide(ride_index);
        if (ride != nullptr)
        {
            ride->formatNameTo(ft);
        }
        else
        {
            ft.Add<StringId>(STR_DEFAULT_SIGN);
        }
    }
    else if (text.empty())
    {
        ft.Add<StringId>(STR_DEFAULT_SIGN);
    }
    else
    {
        ft.Add<StringId>(STR_STRING).Add<const char*>(text.c_str());
    }
}

/**
 *
 *  rct2: 0x006B7EAB
 */
static RideId BannerGetRideIndexAt(const CoordsXYZ& bannerCoords)
{
    TileElement* tileElement = MapGetFirstElementAt(bannerCoords);
    RideId resultRideIndex = RideId::GetNull();
    if (tileElement == nullptr)
        return resultRideIndex;
    do
    {
        if (tileElement->GetType() != TileElementType::Track)
            continue;

        RideId rideIndex = tileElement->AsTrack()->GetRideIndex();
        auto ride = GetRide(rideIndex);
        if (ride == nullptr || ride->getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
            continue;

        if ((tileElement->GetClearanceZ()) + (4 * kCoordsZStep) <= bannerCoords.z)
            continue;

        resultRideIndex = rideIndex;
    } while (!(tileElement++)->IsLastForTile());

    return resultRideIndex;
}

static BannerIndex BannerGetNewIndex()
{
    auto& gameState = getGameState();
    for (BannerIndex::UnderlyingType bannerIndex = 0; bannerIndex < kMaxBanners; bannerIndex++)
    {
        if (bannerIndex < gameState.banners.size())
        {
            if (gameState.banners[bannerIndex].IsNull())
            {
                return BannerIndex::FromUnderlying(bannerIndex);
            }
        }
        else
        {
            gameState.banners.emplace_back();
            return BannerIndex::FromUnderlying(bannerIndex);
        }
    }
    return BannerIndex::GetNull();
}

/**
 *
 *  rct2: 0x006B9CB0
 */
void BannerInit(GameState_t& gameState)
{
    gameState.banners.clear();
}

TileElement* BannerGetTileElement(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner != nullptr)
    {
        auto tileElement = MapGetFirstElementAt(banner->position);
        if (tileElement != nullptr)
        {
            do
            {
                if (tileElement->GetBannerIndex() == bannerIndex)
                {
                    return tileElement;
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
    return nullptr;
}

WallElement* BannerGetScrollingWallTileElement(BannerIndex bannerIndex)
{
    auto banner = GetBanner(bannerIndex);
    if (banner == nullptr)
        return nullptr;

    auto tileElement = MapGetFirstElementAt(banner->position);
    if (tileElement == nullptr)
        return nullptr;

    do
    {
        auto wallElement = tileElement->AsWall();

        if (wallElement == nullptr)
            continue;

        auto* wallEntry = wallElement->GetEntry();
        if (wallEntry->scrolling_mode == kScrollingModeNone)
            continue;
        if (wallElement->GetBannerIndex() != bannerIndex)
            continue;
        return wallElement;
    } while (!(tileElement++)->IsLastForTile());

    return nullptr;
}

/**
 *
 *  rct2: 0x006B7D86
 */
RideId BannerGetClosestRideIndex(const CoordsXYZ& mapPos)
{
    static constexpr std::array NeighbourCheckOrder = {
        CoordsXY{ kCoordsXYStep, 0 },
        CoordsXY{ -kCoordsXYStep, 0 },
        CoordsXY{ 0, kCoordsXYStep },
        CoordsXY{ 0, -kCoordsXYStep },
        CoordsXY{ -kCoordsXYStep, +kCoordsXYStep },
        CoordsXY{ +kCoordsXYStep, -kCoordsXYStep },
        CoordsXY{ +kCoordsXYStep, +kCoordsXYStep },
        CoordsXY{ -kCoordsXYStep, +kCoordsXYStep },
        CoordsXY{ 0, 0 },
    };

    for (const auto& neighhbourCoords : NeighbourCheckOrder)
    {
        RideId rideIndex = BannerGetRideIndexAt({ CoordsXY{ mapPos } + neighhbourCoords, mapPos.z });
        if (!rideIndex.IsNull())
        {
            return rideIndex;
        }
    }

    auto rideIndex = RideId::GetNull();
    auto resultDistance = std::numeric_limits<int32_t>::max();
    for (auto& ride : GetRideManager())
    {
        if (ride.getRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
            continue;

        auto rideCoords = ride.overallView;
        if (rideCoords.IsNull())
            continue;

        int32_t distance = abs(mapPos.x - rideCoords.x) + abs(mapPos.y - rideCoords.y);
        if (distance < resultDistance)
        {
            resultDistance = distance;
            rideIndex = ride.id;
        }
    }
    return rideIndex;
}

struct BannerElementWithPos
{
    BannerElement* Element;
    TileCoordsXY Pos;
};

// Returns a list of BannerElement's with the tile position.
static std::vector<BannerElementWithPos> GetAllBannerElementsOnMap()
{
    auto& gameState = getGameState();
    std::vector<BannerElementWithPos> banners;
    for (int y = 0; y < gameState.mapSize.y; y++)
    {
        for (int x = 0; x < gameState.mapSize.x; x++)
        {
            const auto tilePos = TileCoordsXY{ x, y };
            for (auto* bannerElement : OpenRCT2::TileElementsView<BannerElement>(tilePos.ToCoordsXY()))
            {
                auto bannerIndex = bannerElement->GetIndex();
                if (bannerIndex == BannerIndex::GetNull())
                    continue;

                banners.push_back({ bannerElement, tilePos });
            }
        }
    }
    return banners;
}

// Iterates all banners and checks if the tile specified by the position actually
// has a tile with the banner index, if no tile is found then the banner element will be released.
static void BannerDeallocateUnlinked()
{
    auto& gameState = getGameState();
    for (BannerIndex::UnderlyingType index = 0; index < gameState.banners.size(); index++)
    {
        const auto bannerId = BannerIndex::FromUnderlying(index);
        auto* tileElement = BannerGetTileElement(bannerId);
        if (tileElement == nullptr)
        {
            auto* banner = GetBanner(bannerId);
            if (banner != nullptr)
            {
                banner->type = kBannerNull;
            }
        }
    }
}

// BannerElement tiles should not share a banner entry, this iterates
// over all banner elements that shares the index and creates a new entry also
// copying the data from the current assigned banner entry.
static void BannerFixDuplicates(std::vector<BannerElementWithPos>& bannerElements)
{
    // Sort the banners by index
    std::sort(bannerElements.begin(), bannerElements.end(), [](const BannerElementWithPos& a, const BannerElementWithPos& b) {
        return a.Element->GetIndex() < b.Element->GetIndex();
    });

    // Create a list of all banners with duplicate indices.
    std::vector<BannerElementWithPos> duplicates;
    for (size_t i = 1; i < bannerElements.size(); i++)
    {
        if (bannerElements[i - 1].Element->GetIndex() == bannerElements[i].Element->GetIndex())
        {
            duplicates.push_back(bannerElements[i]);
        }
    }

    // For each duplicate, create a new banner and copy the old data
    for (const auto& duplicate : duplicates)
    {
        const auto oldIndex = duplicate.Element->GetIndex();
        const auto* oldBanner = GetBanner(oldIndex);
        if (oldBanner == nullptr)
        {
            LOG_ERROR("Unable to get old banner for index %u.", oldIndex.ToUnderlying());
            continue;
        }

        auto* newBanner = CreateBanner();
        if (newBanner == nullptr)
        {
            LOG_ERROR("Failed to create new banner.");
            continue;
        }

        const auto newBannerId = newBanner->id;

        // Copy the old data to the new banner.
        *newBanner = *oldBanner;
        newBanner->id = newBannerId;

        // Assign the new banner index to the tile element.
        duplicate.Element->SetIndex(newBannerId);
    }
}

// Ensures that all banner entries have the correct position based on the element
// that references the banner entry.
static void BannerFixPositions(std::vector<BannerElementWithPos>& bannerElements)
{
    for (const auto& entry : bannerElements)
    {
        const auto index = entry.Element->GetIndex();
        auto* banner = GetBanner(index);
        if (banner == nullptr)
        {
            LOG_ERROR("Unable to get banner for index %u.", index.ToUnderlying());
            continue;
        }
        banner->position = entry.Pos;
    }
}

void BannerApplyFixes()
{
    auto bannerElements = GetAllBannerElementsOnMap();

    BannerFixDuplicates(bannerElements);

    BannerFixPositions(bannerElements);

    BannerDeallocateUnlinked();
}

void UnlinkAllRideBanners()
{
    auto& gameState = getGameState();
    for (auto& banner : gameState.banners)
    {
        if (!banner.IsNull())
        {
            banner.flags.unset(BannerFlag::linkedToRide);
            banner.ride_index = RideId::GetNull();
        }
    }
}

void UnlinkAllBannersForRide(RideId rideId)
{
    auto& gameState = getGameState();
    for (auto& banner : gameState.banners)
    {
        if (!banner.IsNull() && (banner.flags.has(BannerFlag::linkedToRide)) && banner.ride_index == rideId)
        {
            banner.flags.unset(BannerFlag::linkedToRide);
            banner.ride_index = RideId::GetNull();
            banner.text = {};
        }
    }
}

Banner* GetBanner(BannerIndex id)
{
    auto& gameState = getGameState();
    const auto index = id.ToUnderlying();
    if (index < gameState.banners.size())
    {
        auto banner = &gameState.banners[index];
        if (banner != nullptr && !banner->IsNull())
        {
            return banner;
        }
    }
    return nullptr;
}

Banner* GetOrCreateBanner(BannerIndex id)
{
    auto& gameState = getGameState();
    const auto index = id.ToUnderlying();
    if (index < kMaxBanners)
    {
        if (index >= gameState.banners.size())
        {
            gameState.banners.resize(index + 1);
        }
        // Create the banner
        auto& banner = gameState.banners[index];
        banner.id = id;
        return &banner;
    }
    return nullptr;
}

Banner* CreateBanner()
{
    auto bannerIndex = BannerGetNewIndex();
    auto banner = GetOrCreateBanner(bannerIndex);
    if (banner != nullptr)
    {
        banner->id = bannerIndex;
        banner->flags = {};
        banner->type = 0;
        banner->text = {};
        banner->colour = COLOUR_WHITE;
        banner->textColour = TextColour::white;
    }
    return banner;
}

void DeleteBanner(BannerIndex id)
{
    auto* const banner = GetBanner(id);
    if (banner != nullptr)
    {
        *banner = {};
    }
}

void TrimBanners()
{
    auto& gameState = getGameState();
    if (gameState.banners.size() > 0)
    {
        auto lastBannerId = gameState.banners.size() - 1;
        while (lastBannerId != std::numeric_limits<size_t>::max() && gameState.banners[lastBannerId].IsNull())
        {
            lastBannerId--;
        }
        gameState.banners.resize(lastBannerId + 1);
        gameState.banners.shrink_to_fit();
    }
}

size_t GetNumBanners()
{
    auto& gameState = getGameState();
    size_t count = 0;
    for (const auto& banner : gameState.banners)
    {
        if (!banner.IsNull())
        {
            count++;
        }
    }
    return count;
}

bool HasReachedBannerLimit()
{
    auto numBanners = GetNumBanners();
    return numBanners >= kMaxBanners;
}
