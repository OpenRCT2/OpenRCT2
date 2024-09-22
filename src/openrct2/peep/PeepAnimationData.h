#pragma once

#include "../entity/Peep.h"

#include <span>

namespace OpenRCT2
{
    struct SpriteBounds
    {
        uint8_t sprite_width;           // 0x00
        uint8_t sprite_height_negative; // 0x01
        uint8_t sprite_height_positive; // 0x02
    };

    struct PeepAnimation
    {
        uint32_t base_image;
        SpriteBounds bounds;
        std::span<const uint8_t> frame_offsets;
    };

    struct PeepAnimations
    {
    public:
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

    const PeepAnimation& GetPeepAnimation(
        PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup = PeepAnimationType::None);
    const SpriteBounds& GetSpriteBounds(
        PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup = PeepAnimationType::None);
} // namespace OpenRCT2
