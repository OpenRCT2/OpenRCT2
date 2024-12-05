/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/IStream.hpp"
#include "../peep/PeepAnimations.h"
#include "Object.h"

#include <vector>

class PeepAnimationsObject final : public Object
{
private:
    ImageIndex _imageOffsetId;
    OpenRCT2::AnimationPeepType _peepType;
    bool _slowWalking;
    std::vector<OpenRCT2::PeepAnimations> _animationGroups;

public:
    static constexpr ObjectType kObjectType = ObjectType::PeepAnimations;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    void Load() override;
    void Unload() override;

    std::string GetCostumeName() const;
    ImageIndex GetInlineImageId() const;

    const OpenRCT2::PeepAnimation& GetPeepAnimation(
        PeepAnimationGroup animGroup, PeepAnimationType animType = PeepAnimationType::Walking) const;
    const OpenRCT2::SpriteBounds& GetSpriteBounds(
        PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup = PeepAnimationType::Walking) const;

    OpenRCT2::AnimationPeepType GetPeepType() const;
    size_t GetNumAnimationGroups() const;
    PeepAnimationGroup GetLegacyPosition(PeepAnimationGroup animGroup) const;

    bool IsSlowWalking()
    {
        return _slowWalking;
    };

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;
};
