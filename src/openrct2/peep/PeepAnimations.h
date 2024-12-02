/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumMap.hpp"
#include "../core/EnumUtils.hpp"
#include "../drawing/ImageIndexType.h"
#include "../entity/Peep.h"

#include <vector>

namespace OpenRCT2
{
    enum class AnimationPeepType : uint8_t
    {
        Guest,
        Handyman,
        Mechanic,
        Security,
        Entertainer,
    };

    const EnumMap<PeepAnimationType>& getAnimationsByPeepType(AnimationPeepType peepType);

    struct SpriteBounds
    {
        uint8_t sprite_width;           // 0x00
        uint8_t sprite_height_negative; // 0x01
        uint8_t sprite_height_positive; // 0x02
    };

    struct PeepAnimation
    {
        uint32_t base_image;
        ImageIndex imageTableOffset{};
        std::vector<uint8_t> frame_offsets;
        SpriteBounds bounds{};

        constexpr PeepAnimation() = default;

        PeepAnimation(uint32_t baseImage, std::vector<uint8_t> frameOffsets)
            : base_image(baseImage)
            , frame_offsets(frameOffsets)
        {
        }
    };

    struct PeepAnimations
    {
    public:
        // TODO: move type to RCT12?
        PeepAnimationGroup legacyPosition = PeepAnimationGroup::Invalid;

        constexpr PeepAnimation& operator[](PeepAnimationType n)
        {
            return animations[EnumValue(n)];
        }
        constexpr const PeepAnimation& operator[](PeepAnimationType n) const
        {
            return animations[EnumValue(n)];
        }

    private:
        PeepAnimation animations[37]{};
    };

    SpriteBounds inferMaxAnimationDimensions(const PeepAnimation& anim);
} // namespace OpenRCT2
