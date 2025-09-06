/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "PeepPickupAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../Input.h"
#include "../entity/EntityRegistry.h"
#include "../entity/Peep.h"
#include "../network/Network.h"

namespace OpenRCT2::GameActions
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void PeepPickupAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_type) << DS_TAG(_entityId) << DS_TAG(_loc) << DS_TAG(_owner);
    }

    Result PeepPickupAction::Query() const
    {
        if (_entityId.ToUnderlying() >= kMaxEntities || _entityId.IsNull())
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        if (!_loc.IsNull() && !LocationValid(_loc))
        {
            return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto* const peep = getGameState().entities.TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto res = Result();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res.Position = peep->GetLocation();
                if (!peep->CanBePickedUp())
                {
                    return Result(Status::Disallowed, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
                }
                Peep* existing = NetworkGetPickupPeep(_owner);
                if (existing != nullptr)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->Id, { NetworkGetPickupPeepOldX(_owner), 0, 0 }, _owner
                    };
                    auto result = QueryNested(&existingPickupAction);

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
                    return Result(Status::Unknown, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
                }

                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), false); res2.Error != Status::Ok)
                {
                    return res2;
                }
                break;
            default:
                LOG_ERROR("Invalid peep pickup type %u", _type);
                return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }
        return res;
    }

    Result PeepPickupAction::Execute() const
    {
        Peep* const peep = getGameState().entities.TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto res = Result();

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
                        PeepPickupType::Cancel, existing->Id, { NetworkGetPickupPeepOldX(_owner), 0, 0 }, _owner
                    };
                    auto result = ExecuteNested(&existingPickupAction);

                    if (existing == peep)
                    {
                        return result;
                    }
                    if (_owner == NetworkGetCurrentPlayerId())
                    {
                        // prevent tool_cancel()
                        gInputFlags.unset(InputFlag::toolActive);
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
                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), true); res2.Error != Status::Ok)
                {
                    return res2;
                }
                CancelConcurrentPickups(peep);
                break;
            default:
                LOG_ERROR("Invalid peep pickup type %u", _type);
                return Result(Status::InvalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
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
} // namespace OpenRCT2::GameActions
