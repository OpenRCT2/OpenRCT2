/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../common.h"
#include "../core/DataSerialiser.h"
#include "../core/Identifier.hpp"
#include "../localisation/StringIds.h"
#include "GameActionResult.h"

#include <array>
#include <functional>
#include <memory>
#include <utility>

namespace GameActions
{
    namespace Flags
    {
        constexpr uint16_t AllowWhilePaused = 1 << 0;
        constexpr uint16_t ClientOnly = 1 << 1;
        constexpr uint16_t EditorOnly = 1 << 2;
    } // namespace Flags

} // namespace GameActions

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

/**
 *
 */
class GameActionParameterVisitor
{
public:
    virtual ~GameActionParameterVisitor() = default;

    virtual void Visit(std::string_view name, bool& param)
    {
    }

    virtual void Visit(std::string_view name, int32_t& param)
    {
    }

    virtual void Visit(std::string_view name, std::string& param)
    {
    }

    void Visit(CoordsXY& param)
    {
        Visit("x", param.x);
        Visit("y", param.y);
    }

    void Visit(CoordsXYZ& param)
    {
        Visit("x", param.x);
        Visit("y", param.y);
        Visit("z", param.z);
    }

    void Visit(CoordsXYZD& param)
    {
        Visit("x", param.x);
        Visit("y", param.y);
        Visit("z", param.z);
        Visit("direction", param.direction);
    }

    void Visit(MapRange& param)
    {
        Visit("x1", param.Point1.x);
        Visit("y1", param.Point1.y);
        Visit("x2", param.Point2.x);
        Visit("y2", param.Point2.y);
    }

    template<typename T> void Visit(std::string_view name, T& param)
    {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "Not an arithmetic type");
        auto value = static_cast<int32_t>(param);
        Visit(name, value);
        param = static_cast<T>(value);
    }

    template<typename T, T TNull, typename TTag> void Visit(std::string_view name, TIdentifier<T, TNull, TTag>& param)
    {
        auto value = param.ToUnderlying();
        Visit(name, value);
        param = TIdentifier<T, TNull, TTag>::FromUnderlying(value);
    }

    template<typename T, size_t _TypeID> void Visit(std::string_view name, NetworkObjectId_t<T, _TypeID>& param)
    {
        Visit(name, param.id);
    }
};

class GameAction
{
public:
    using Ptr = std::unique_ptr<GameAction>;
    using Callback_t = std::function<void(const class GameAction*, const GameActions::Result*)>;

private:
    GameCommand const _type;

    NetworkPlayerId_t _playerId = { -1 }; // Callee
    uint32_t _flags = 0;                  // GAME_COMMAND_FLAGS
    uint32_t _networkId = 0;
    Callback_t _callback;

public:
    GameAction(GameCommand type)
        : _type(type)
    {
    }

    virtual ~GameAction() = default;

    const char* GetName() const;

    virtual void AcceptParameters(GameActionParameterVisitor&)
    {
    }

    void AcceptFlags(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("flags", _flags);
    }

    NetworkPlayerId_t GetPlayer() const
    {
        return _playerId;
    }

    void SetPlayer(NetworkPlayerId_t playerId)
    {
        _playerId = playerId;
    }

    /**
     * Gets the GameActions::Flags flags that are enabled for this game action.
     */
    virtual uint16_t GetActionFlags() const
    {
        // Make sure we execute some things only on the client.
        uint16_t flags = 0;

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) != 0 || (GetFlags() & GAME_COMMAND_FLAG_NO_SPEND) != 0)
        {
            flags |= GameActions::Flags::ClientOnly;
        }

        if (GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)
        {
            flags |= GameActions::Flags::AllowWhilePaused;
        }

        return flags;
    }

    /**
     * Currently used for GAME_COMMAND_FLAGS, needs refactoring once everything is replaced.
     */
    uint32_t GetFlags() const
    {
        return _flags;
    }

    uint32_t SetFlags(uint32_t flags)
    {
        return _flags = flags;
    }

    GameCommand GetType() const
    {
        return _type;
    }

    void SetCallback(Callback_t cb)
    {
        _callback = cb;
    }

    const Callback_t& GetCallback() const
    {
        return _callback;
    }

    void SetNetworkId(uint32_t id)
    {
        _networkId = id;
    }

    uint32_t GetNetworkId() const
    {
        return _networkId;
    }

    virtual void Serialise(DataSerialiser& stream)
    {
        stream << DS_TAG(_networkId) << DS_TAG(_flags) << DS_TAG(_playerId);
    }

    // Helper function, allows const Objects to still serialize into DataSerialiser while being const.
    void Serialise(DataSerialiser& stream) const
    {
        return const_cast<GameAction&>(*this).Serialise(stream);
    }

    /**
     * Override this to specify the wait time in milliseconds the player is required to wait before
     * being able to execute it again.
     */
    virtual uint32_t GetCooldownTime() const
    {
        return 0;
    }

    /**
     * Query the result of the game action without changing the game state.
     */
    virtual GameActions::Result Query() const abstract;

    /**
     * Apply the game action and change the game state.
     */
    virtual GameActions::Result Execute() const abstract;

    bool LocationValid(const CoordsXY& coords) const;
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

template<GameCommand TId> struct GameActionNameQuery
{
};

template<GameCommand TType> struct GameActionBase : GameAction
{
public:
    static constexpr GameCommand TYPE = TType;

    GameActionBase()
        : GameAction(TYPE)
    {
    }
};

namespace GameActions
{
    using GameActionFactory = GameAction* (*)();

    bool IsValidId(uint32_t id);
    const char* GetName(GameCommand id);

    // Halts the queue processing until ResumeQueue is called, any calls to ProcessQueue
    // will have no effect during suspension. It has no effect of actions that will not
    // cross the network.
    void SuspendQueue();

    // Resumes queue processing.
    void ResumeQueue();

    void Enqueue(const GameAction* ga, uint32_t tick);
    void Enqueue(GameAction::Ptr&& ga, uint32_t tick);
    void ProcessQueue();
    void ClearQueue();

    GameAction::Ptr Create(GameCommand id);
    GameAction::Ptr Clone(const GameAction* action);

    // This should be used if a round trip is to be expected.
    GameActions::Result Query(const GameAction* action);
    GameActions::Result Execute(const GameAction* action);

    // This should be used from within game actions.
    GameActions::Result QueryNested(const GameAction* action);
    GameActions::Result ExecuteNested(const GameAction* action);

} // namespace GameActions
