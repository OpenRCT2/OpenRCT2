/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepPickupAction.h"

#include "../Input.h"
#include "../network/network.h"
#include "../util/Util.h"

PeepPickupAction::PeepPickupAction(PeepPickupType type, uint32_t spriteId, const CoordsXYZ& loc, NetworkPlayerId_t owner)
    : _type(type)
    , _spriteId(spriteId)
    , _loc(loc)
    , _owner(owner)
{
}

uint16_t PeepPickupAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void PeepPickupAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_type) << DS_TAG(_spriteId) << DS_TAG(_loc) << DS_TAG(_owner);
}

GameActions::Result::Ptr PeepPickupAction::Query() const
{
    if (_spriteId >= MAX_SPRITES || _spriteId == SPRITE_INDEX_NULL)
    {
        log_error("Failed to pick up peep for sprite %d", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }

    if (!_loc.isNull() && !LocationValid(_loc))
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }

    auto* const peep = TryGetEntity<Peep>(_spriteId);
    if (!peep || peep->sprite_identifier != SpriteIdentifier::Peep)
    {
        log_error("Failed to pick up peep for sprite %d", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }

    auto* network = OpenRCT2::GetContext()->GetNetwork();

    auto res = MakeResult();
    switch (_type)
    {
        case PeepPickupType::Pickup:
        {
            res->Position = { peep->x, peep->y, peep->z };
            if (!peep_can_be_picked_up(peep))
            {
                return MakeResult(GameActions::Status::Disallowed, STR_ERR_CANT_PLACE_PERSON_HERE);
            }
            Peep* existing = network->GetPickupPeep(_owner);
            if (existing)
            {
                // already picking up a peep
                PeepPickupAction existingPickupAction{
                    PeepPickupType::Cancel, existing->sprite_index, { network->GetPickupPeepOldX(_owner), 0, 0 }, _owner
                };
                auto result = GameActions::QueryNested(&existingPickupAction);

                if (existing == peep)
                {
                    return result;
                }
            }
        }
        break;
        case PeepPickupType::Cancel:
            res->Position = { peep->x, peep->y, peep->z };
            break;
        case PeepPickupType::Place:
            res->Position = _loc;
            if (network->GetPickupPeep(_owner) != peep)
            {
                return MakeResult(GameActions::Status::Unknown, STR_ERR_CANT_PLACE_PERSON_HERE);
            }

            if (auto res2 = peep->Place(TileCoordsXYZ(_loc), false); res2->Error != GameActions::Status::Ok)
            {
                return res2;
            }
            break;
        default:
            log_error("Invalid pickup type: %u", _type);
            return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }
    return res;
}

GameActions::Result::Ptr PeepPickupAction::Execute() const
{
    Peep* const peep = TryGetEntity<Peep>(_spriteId);
    if (!peep || peep->sprite_identifier != SpriteIdentifier::Peep)
    {
        log_error("Failed to pick up peep for sprite %d", _spriteId);
        return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }

    auto* network = OpenRCT2::GetContext()->GetNetwork();
    auto res = MakeResult();

    switch (_type)
    {
        case PeepPickupType::Pickup:
        {
            res->Position = { peep->x, peep->y, peep->z };

            Peep* existing = network->GetPickupPeep(_owner);
            if (existing)
            {
                // already picking up a peep
                PeepPickupAction existingPickupAction{
                    PeepPickupType::Cancel, existing->sprite_index, { network->GetPickupPeepOldX(_owner), 0, 0 }, _owner
                };
                auto result = GameActions::ExecuteNested(&existingPickupAction);

                if (existing == peep)
                {
                    return result;
                }
                if (_owner == network->GetCurrentPlayerId())
                {
                    // prevent tool_cancel()
                    input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);
                }
            }

            network->SetPickupPeep(_owner, peep);
            network->SetPickupPeepOldX(_owner, peep->x);
            peep->Pickup();
        }
        break;
        case PeepPickupType::Cancel:
        {
            res->Position = { peep->x, peep->y, peep->z };

            Peep* const pickedUpPeep = network->GetPickupPeep(_owner);
            if (pickedUpPeep)
            {
                pickedUpPeep->PickupAbort(_loc.x);
            }

            network->SetPickupPeep(_owner, nullptr);
        }
        break;
        case PeepPickupType::Place:
            res->Position = _loc;
            if (auto res2 = peep->Place(TileCoordsXYZ(_loc), true); res2->Error != GameActions::Status::Ok)
            {
                return res2;
            }
            CancelConcurrentPickups(peep);
            break;
        default:
            log_error("Invalid pickup type: %u", _type);
            return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
    }
    return res;
}

void PeepPickupAction::CancelConcurrentPickups(Peep* pickedPeep) const
{
    auto* network = OpenRCT2::GetContext()->GetNetwork();

    // This part is only relevant in multiplayer games.
    if (network->GetMode() == NETWORK_MODE_NONE)
        return;

    // Not relevant for owner, owner gets to place it normally.
    NetworkPlayerId_t currentPlayerId = network->GetCurrentPlayerId();
    if (currentPlayerId == _owner)
        return;

    Peep* peep = network->GetPickupPeep(network->GetCurrentPlayerId());
    if (peep != pickedPeep)
        return;

    // By assigning the peep to null before calling tool_cancel we can avoid
    // resetting the peep to the initial position.
    network->SetPickupPeep(currentPlayerId, nullptr);
    tool_cancel();
}
