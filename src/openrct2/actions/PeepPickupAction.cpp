/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    PeepPickupAction::PeepPickupAction(PeepPickupType type, EntityId entityId, const CoordsXYZ& loc, Network::PlayerId_t owner)
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

    Result PeepPickupAction::Query(GameState_t& gameState) const
    {
        if (_entityId.ToUnderlying() >= kMaxEntities || _entityId.IsNull())
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        if (!_loc.IsNull() && !LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto* const peep = gameState.entities.TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto res = Result();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res.position = peep->GetLocation();
                if (!peep->CanBePickedUp())
                {
                    return Result(Status::disallowed, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
                }
                Peep* existing = Network::GetPickupPeep(_owner);
                if (existing != nullptr)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->Id, { Network::GetPickupPeepOldX(_owner), 0, 0 }, _owner
                    };
                    auto result = QueryNested(&existingPickupAction, gameState);

                    if (existing == peep)
                    {
                        return result;
                    }
                }
            }
            break;
            case PeepPickupType::Cancel:
                res.position = peep->GetLocation();
                break;
            case PeepPickupType::Place:
                res.position = _loc;
                if (Network::GetPickupPeep(_owner) != peep)
                {
                    return Result(Status::unknown, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
                }

                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), false); res2.error != Status::ok)
                {
                    return res2;
                }
                break;
            default:
                LOG_ERROR("Invalid peep pickup type %u", _type);
                return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }
        return res;
    }

    Result PeepPickupAction::Execute(GameState_t& gameState) const
    {
        Peep* const peep = gameState.entities.TryGetEntity<Peep>(_entityId);
        if (peep == nullptr)
        {
            LOG_ERROR("Failed to pick up peep for sprite %d", _entityId);
            return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }

        auto res = Result();

        switch (_type)
        {
            case PeepPickupType::Pickup:
            {
                res.position = peep->GetLocation();

                Peep* existing = Network::GetPickupPeep(_owner);
                if (existing != nullptr)
                {
                    // already picking up a peep
                    PeepPickupAction existingPickupAction{
                        PeepPickupType::Cancel, existing->Id, { Network::GetPickupPeepOldX(_owner), 0, 0 }, _owner
                    };
                    auto result = ExecuteNested(&existingPickupAction, gameState);

                    if (existing == peep)
                    {
                        return result;
                    }
                    if (_owner == Network::GetCurrentPlayerId())
                    {
                        // prevent tool_cancel()
                        gInputFlags.unset(InputFlag::toolActive);
                    }
                }

                Network::SetPickupPeep(_owner, peep);
                Network::SetPickupPeepOldX(_owner, peep->x);
                peep->Pickup();
            }
            break;
            case PeepPickupType::Cancel:
            {
                res.position = peep->GetLocation();

                Peep* const pickedUpPeep = Network::GetPickupPeep(_owner);
                if (pickedUpPeep != nullptr)
                {
                    pickedUpPeep->PickupAbort(_loc.x);
                }
                Network::SetPickupPeep(_owner, nullptr);
            }
            break;
            case PeepPickupType::Place:
                res.position = _loc;
                if (auto res2 = peep->Place(TileCoordsXYZ(_loc), true); res2.error != Status::ok)
                {
                    return res2;
                }
                CancelConcurrentPickups(peep);
                break;
            default:
                LOG_ERROR("Invalid peep pickup type %u", _type);
                return Result(Status::invalidParameters, STR_ERR_CANT_PLACE_PERSON_HERE, kStringIdNone);
        }
        return res;
    }

    void PeepPickupAction::CancelConcurrentPickups(Peep* pickedPeep) const
    {
        // This part is only relevant in multiplayer games.
        if (Network::GetMode() == Network::Mode::none)
            return;

        // Not relevant for owner, owner gets to place it normally.
        Network::PlayerId_t currentPlayerId = Network::GetCurrentPlayerId();
        if (currentPlayerId == _owner)
            return;

        Peep* peep = Network::GetPickupPeep(Network::GetCurrentPlayerId());
        if (peep != pickedPeep)
            return;

        // By assigning the peep to null before calling tool_cancel we can avoid
        // resetting the peep to the initial position.
        Network::SetPickupPeep(currentPlayerId, nullptr);
        ToolCancel();
    }
} // namespace OpenRCT2::GameActions
