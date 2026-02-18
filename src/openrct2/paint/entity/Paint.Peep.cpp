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

BaseImageAndOffset PaintPeepGetBaseImageAndOffset(const Peep& peep, Direction direction)
{
    PeepAnimationType actionAnimationGroup = peep.AnimationType;
    uint8_t imageOffset = peep.AnimationImageIdOffset;

    if (peep.Action == PeepActionType::idle)
    {
        actionAnimationGroup = peep.NextAnimationType;
        imageOffset = 0;
    }

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep.AnimationObjectIndex);

    ImageIndex baseImageId = animObj->GetPeepAnimation(peep.AnimationGroup, actionAnimationGroup).baseImage;

    // Offset frame onto the base image, using rotation except for the 'picked up' state
    if (actionAnimationGroup != PeepAnimationType::hanging)
        baseImageId += direction + imageOffset * 4;
    else
        baseImageId += imageOffset;

    return {
        .baseImageId = baseImageId,
        .offset = imageOffset,
        .actionAnimationGroup = actionAnimationGroup,
    };
}
