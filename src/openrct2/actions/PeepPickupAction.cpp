/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepPickupAction.h"

#include "../Input.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Peep.h"
#include "../network/network.h"
#include "../util/Util.h"
namespace OpenRCT2
{
    PeepPickupAction::PeepPickupAction(PeepPickupType type, EntityId entityId, const CoordsXYZ& loc, NetworkPlayerId_t owner)
        : _type(type)
        , _entityId(entityId)
        , _loc(loc)
        , _owner(owner)
    {
    }

    void PeepPickupAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("type", _type);
        visitor.Visit("id", _entityId);
        visitor.Visit(_loc);
        visitor.Visit("playerId", _owner);
    }

    uint16_t PeepPickupAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void PeepPickupAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_entityId) << DS_TAG(_loc) << DS_TAG(_owner);
    }

    GameActions::Result PeepPickupAction::Query() const
    {
        if (_entityId.ToUnderlying() >= MAX_ENTITIES || _entityId.IsNull())
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }

        if (!_loc.IsNull() && !LocationValid(_loc))
        {
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }

        auto* const peep = TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }

        auto res = GameActions::Result();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res.Position = peep->GetLocation();
                if (!peep->CanBePickedUp())
                {
                    return GameActions::Result(GameActions::Status::Disallowed, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
                }
                Peep* existing = NetworkGetPickupPeep(_owner);
                if (existing != nullptr)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->sprite_index, { NetworkGetPickupPeepOldX(_owner), 0, 0 }, _owner
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
                res.Position = peep->GetLocation();
                break;
            case PeepPickupType::Place:
                res.Position = _loc;
                if (NetworkGetPickupPeep(_owner) != peep)
                {
                    return GameActions::Result(GameActions::Status::Unknown, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
                }

                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), false); res2.Error != GameActions::Status::Ok)
                {
                    return res2;
                }
                break;
            default:
                LOG_ERROR("Invalid pickup type: %u", _type);
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }
        return res;
    }

    GameActions::Result PeepPickupAction::Execute() const
    {
        Peep* const peep = TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }

        auto res = GameActions::Result();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res.Position = peep->GetLocation();

                Peep* existing = NetworkGetPickupPeep(_owner);
                if (existing != nullptr)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->sprite_index, { NetworkGetPickupPeepOldX(_owner), 0, 0 }, _owner
                    };
                    auto result = GameActions::ExecuteNested(&existingPickupAction);

                    if (existing == peep)
                    {
                        return result;
                    }
                    if (_owner == NetworkGetCurrentPlayerId())
                    {
                        // prevent tool_cancel()
                        InputSetFlag(INPUT_FLAG_TOOL_ACTIVE, false);
                    }
                }

                NetworkSetPickupPeep(_owner, peep);
                NetworkSetPickupPeepOldX(_owner, peep->x);
                peep->Pickup();
            }
            break;
            case PeepPickupType::Cancel:
            {
                res.Position = peep->GetLocation();

                Peep* const pickedUpPeep = NetworkGetPickupPeep(_owner);
                if (pickedUpPeep != nullptr)
                {
                    pickedUpPeep->PickupAbort(_loc.x);
                }

                NetworkSetPickupPeep(_owner, nullptr);
            }
            break;
            case PeepPickupType::Place:
                res.Position = _loc;
                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), true); res2.Error != GameActions::Status::Ok)
                {
                    return res2;
                }
                CancelConcurrentPickups(peep);
                break;
            default:
                LOG_ERROR("Invalid pickup type: %u", _type);
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, STR_NONE);
        }
        return res;
    }

    void PeepPickupAction::CancelConcurrentPickups(Peep* pickedPeep) const
    {
        // This part is only relevant in multiplayer games.
        if (NetworkGetMode() == NETWORK_MODE_NONE)
            return;

        // Not relevant for owner, owner gets to place it normally.
        NetworkPlayerId_t currentPlayerId = NetworkGetCurrentPlayerId();
        if (currentPlayerId == _owner)
            return;

        Peep* peep = NetworkGetPickupPeep(NetworkGetCurrentPlayerId());
        if (peep != pickedPeep)
            return;

        // By assigning the peep to null before calling tool_cancel we can avoid
        // resetting the peep to the initial position.
        NetworkSetPickupPeep(currentPlayerId, nullptr);
        ToolCancel();
    }
} // namespace OpenRCT2
