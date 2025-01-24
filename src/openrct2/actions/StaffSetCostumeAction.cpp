/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetCostumeAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../entity/EntityRegistry.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectManager.h"
#include "../object/PeepAnimationsObject.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

using namespace OpenRCT2;

StaffSetCostumeAction::StaffSetCostumeAction(EntityId spriteIndex, ObjectEntryIndex costume)
    : _spriteIndex(spriteIndex)
    , _costume(costume)
{
}

void StaffSetCostumeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("id", _spriteIndex);
    visitor.Visit("costume", _costume);
}

uint16_t StaffSetCostumeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void StaffSetCostumeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_spriteIndex) << DS_TAG(_costume);
}

GameActions::Result StaffSetCostumeAction::Query() const
{
    if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
    {
        LOG_ERROR("Invalid sprite index %u", _spriteIndex);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
    }

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(_costume);

    auto animPeepType = AnimationPeepType(static_cast<uint8_t>(staff->AssignedStaffType) + 1);
    if (animObj->GetPeepType() != animPeepType)
    {
        LOG_ERROR("Invalid entertainer costume %u", _costume);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }
    return GameActions::Result();
}

GameActions::Result StaffSetCostumeAction::Execute() const
{
    auto* staff = TryGetEntity<Staff>(_spriteIndex);
    if (staff == nullptr)
    {
        LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
    }

    staff->AnimationObjectIndex = _costume;
    staff->AnimationGroup = PeepAnimationGroup::Normal;

    auto& objManager = GetContext()->GetObjectManager();
    auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(_costume);

    staff->PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
    if (animObj->IsSlowWalking(PeepAnimationGroup::Normal))
        staff->PeepFlags |= PEEP_FLAGS_SLOW_WALK;

    staff->AnimationFrameNum = 0;
    staff->UpdateCurrentAnimationType();
    staff->Invalidate();

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Peep, _spriteIndex);

    auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
    ContextBroadcastIntent(&intent);

    auto res = GameActions::Result();
    res.Position = staff->GetLocation();

    return res;
}
