/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Peep.h"

#include "../../Context.h"
#include "../../entity/Peep.h"
#include "../../object/ObjectManager.h"
#include "../../object/PeepAnimationsObject.h"

using namespace OpenRCT2;

BaseImageAndOffset PaintPeepGetBaseImageAndOffset(const Peep& peep, uint8_t imageDirection)
{
    PeepAnimationType actionAnimationGroup = peep.animationType;
    uint8_t imageOffset = peep.animationImageIdOffset;

    if (peep.action == PeepActionType::idle)
    {
        actionAnimationGroup = peep.nextAnimationType;
        imageOffset = 0;
    }

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep.animationObjectIndex);

    const auto& anim = animObj->GetPeepAnimation(peep.animationGroup, actionAnimationGroup);
    ImageIndex baseImageId = anim.baseImage;

    // Offset frame onto the base image, using rotation except for the 'picked up' state
    if (actionAnimationGroup == PeepAnimationType::hanging)
        baseImageId += imageOffset;
    else if (anim.baseImage32 != 0)
        baseImageId = anim.baseImage32 + imageDirection + imageOffset * 32;
    else
        baseImageId += (imageDirection >> 3) + imageOffset * 4;

    return {
        .baseImageId = baseImageId,
        .offset = imageOffset,
        .actionAnimationGroup = actionAnimationGroup,
    };
}
