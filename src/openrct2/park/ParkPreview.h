/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct DrawPixelInfo;

namespace OpenRCT2
{
    constexpr auto kMaxPreviewImageSize = 250;

    enum class PreviewImageType : uint8_t
    {
        miniMap,
        screenshot,
        logo,
    };

    struct PreviewImage
    {
        PreviewImageType type;
        uint8_t width;
        uint8_t height;
        uint8_t pixels[kMaxPreviewImageSize * kMaxPreviewImageSize]{};
    };

    enum class PreviewInfoKind : uint8_t
    {
        month,
        day,
        cash,
        numGuests,
        numRides,
    };

    using PreviewInfo = std::pair<PreviewInfoKind, int64_t>;

    struct ParkPreview
    {
        std::string parkName;
        std::vector<PreviewImage> images;
        std::vector<PreviewInfo> info;

        void clear();
    };

    struct GameState_t;

    ParkPreview generatePreviewFromGameState(const GameState_t& gameState);
    void drawPreviewImage(const PreviewImage& image, DrawPixelInfo& dpi, ScreenCoordsXY screenPos);
} // namespace OpenRCT2
