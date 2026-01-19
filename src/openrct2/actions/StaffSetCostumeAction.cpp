/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StaffSetCostumeAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../entity/EntityRegistry.h"
#include "../localisation/StringIds.h"
#include "../object/ObjectManager.h"
#include "../object/PeepAnimationsObject.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void StaffSetCostumeAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_spriteIndex) << DS_TAG(_costume);
    }

    Result StaffSetCostumeAction::Query(GameState_t& gameState) const
    {
        if (_spriteIndex.ToUnderlying() >= kMaxEntities || _spriteIndex.IsNull())
        {
            LOG_ERROR("Invalid sprite index %u", _spriteIndex);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto* staff = gameState.entities.TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(_costume);

        auto animPeepType = AnimationPeepType(static_cast<uint8_t>(staff->AssignedStaffType) + 1);
        if (animObj->GetPeepType() != animPeepType)
        {
            LOG_ERROR("Invalid entertainer costume %u", _costume);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        return Result();
    }

    Result StaffSetCostumeAction::Execute(GameState_t& gameState) const
    {
        auto* staff = gameState.entities.TryGetEntity<Staff>(_spriteIndex);
        if (staff == nullptr)
        {
            LOG_ERROR("Staff entity not found for spriteIndex %u", _spriteIndex);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_STAFF_NOT_FOUND);
        }

        staff->AnimationObjectIndex = _costume;
        staff->AnimationGroup = PeepAnimationGroup::normal;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(_costume);

        staff->PeepFlags &= ~PEEP_FLAGS_SLOW_WALK;
        if (animObj->IsSlowWalking(PeepAnimationGroup::normal))
            staff->PeepFlags |= PEEP_FLAGS_SLOW_WALK;

        staff->AnimationFrameNum = 0;
        staff->UpdateCurrentAnimationType();
        staff->Invalidate();

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::peep, _spriteIndex);

        auto intent = Intent(INTENT_ACTION_REFRESH_STAFF_LIST);
        ContextBroadcastIntent(&intent);

        auto res = Result();
        res.position = staff->GetLocation();

        return res;
    }
} // namespace OpenRCT2::GameActions
