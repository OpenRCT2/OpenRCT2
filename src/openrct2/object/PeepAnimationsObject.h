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

#include <string_view>
#include <vector>

enum class RCT12PeepAnimationGroup : uint8_t;

class PeepAnimationsObject final : public Object
{
private:
    ImageIndex _imageOffsetId;
    std::vector<OpenRCT2::PeepAnimations> _animationGroups;
    OpenRCT2::AnimationPeepType _peepType;
    bool _noRandomPlacement;

public:
    static constexpr ObjectType kObjectType = ObjectType::PeepAnimations;

    void ReadJson(IReadObjectContext* context, json_t& root) override;
    OpenRCT2::PeepAnimations ReadAnimations(const EnumMap<PeepAnimationType>& requiredAnimationMap, json_t& animations);
    void ReadProperties(json_t& properties);
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
    RCT12PeepAnimationGroup GetLegacyPosition(PeepAnimationGroup animGroup) const;
    std::string_view GetScriptName(PeepAnimationGroup animGroup) const;

    bool IsSlowWalking(PeepAnimationGroup animGroup) const;

    bool ShouldExcludeFromRandomPlacement() const
    {
        return _noRandomPlacement;
    }

    void DrawPreview(DrawPixelInfo& dpi, int32_t width, int32_t height) const override;
    void SetRepositoryItem(ObjectRepositoryItem* item) const override;
};
