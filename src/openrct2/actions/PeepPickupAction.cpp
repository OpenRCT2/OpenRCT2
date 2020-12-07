/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Input.h"
#include "../network/network.h"
#include "../util/Util.h"
#include "../world/Sprite.h"
#include "GameAction.h"

enum class PeepPickupType : uint8_t
{
    Pickup,
    Cancel,
    Place,
    Count
};

DEFINE_GAME_ACTION(PeepPickupAction, GAME_COMMAND_PICKUP_GUEST, GameActions::Result)
{
private:
    PeepPickupType _type{ PeepPickupType::Count };
    uint32_t _spriteId{ SPRITE_INDEX_NULL };
    CoordsXYZ _loc;
    NetworkPlayerId_t _owner{ -1 };

public:
    PeepPickupAction() = default;
    PeepPickupAction(PeepPickupType type, uint32_t spriteId, const CoordsXYZ& loc, NetworkPlayerId_t owner)
        : _type(type)
        , _spriteId(spriteId)
        , _loc(loc)
        , _owner(owner)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_spriteId) << DS_TAG(_loc) << DS_TAG(_owner);
    }

    GameActions::Result::Ptr Query() const override
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
                Peep* existing = network_get_pickup_peep(_owner);
                if (existing)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->sprite_index, { network_get_pickup_peep_old_x(_owner), 0, 0 }, _owner
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
                if (network_get_pickup_peep(_owner) != peep)
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
                break;
        }
        return res;
    }

    GameActions::Result::Ptr Execute() const override
    {
        Peep* const peep = TryGetEntity<Peep>(_spriteId);
        if (!peep || peep->sprite_identifier != SpriteIdentifier::Peep)
        {
            log_error("Failed to pick up peep for sprite %d", _spriteId);
            return MakeResult(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE);
        }

        auto res = MakeResult();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res->Position = { peep->x, peep->y, peep->z };

                Peep* existing = network_get_pickup_peep(_owner);
                if (existing)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->sprite_index, { network_get_pickup_peep_old_x(_owner), 0, 0 }, _owner
                    };
                    auto result = GameActions::ExecuteNested(&existingPickupAction);

                    if (existing == peep)
                    {
                        return result;
                    }
                    if (_owner == network_get_current_player_id())
                    {
                        // prevent tool_cancel()
                        input_set_flag(INPUT_FLAG_TOOL_ACTIVE, false);
                    }
                }

                network_set_pickup_peep(_owner, peep);
                network_set_pickup_peep_old_x(_owner, peep->x);
                peep->Pickup();
            }
            break;
            case PeepPickupType::Cancel:
            {
                res->Position = { peep->x, peep->y, peep->z };

                Peep* const pickedUpPeep = network_get_pickup_peep(_owner);
                if (pickedUpPeep)
                {
                    pickedUpPeep->PickupAbort(_loc.x);
                }

                network_set_pickup_peep(_owner, nullptr);
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
                break;
        }
        return res;
    }

private:
    void CancelConcurrentPickups(Peep * pickedPeep) const
    {
        // This part is only relevant in multiplayer games.
        if (network_get_mode() == NETWORK_MODE_NONE)
            return;

        // Not relevant for owner, owner gets to place it normally.
        NetworkPlayerId_t currentPlayerId = network_get_current_player_id();
        if (currentPlayerId == _owner)
            return;

        Peep* peep = network_get_pickup_peep(network_get_current_player_id());
        if (peep != pickedPeep)
            return;

        // By assigning the peep to null before calling tool_cancel we can avoid
        // resetting the peep to the initial position.
        network_set_pickup_peep(currentPlayerId, nullptr);
        tool_cancel();
    }
};
