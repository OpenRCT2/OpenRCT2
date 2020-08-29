/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include "../core/IStream.hpp"
#include "../localisation/StringIds.h"
#include "../world/Map.h"

#include <array>
#include <functional>
#include <memory>
#include <utility>

/**
 * Common error codes for game actions.
 */
enum class GA_ERROR : uint16_t
{
    OK,
    INVALID_PARAMETERS,
    DISALLOWED,
    GAME_PAUSED,
    INSUFFICIENT_FUNDS,
    NOT_IN_EDITOR_MODE,

    NOT_OWNED,
    TOO_LOW,
    TOO_HIGH,
    NO_CLEARANCE,
    ITEM_ALREADY_PLACED,

    NOT_CLOSED,
    BROKEN,

    NO_FREE_ELEMENTS,

    UNKNOWN = UINT16_MAX,
};

namespace GA_FLAGS
{
    constexpr uint16_t ALLOW_WHILE_PAUSED = 1 << 0;
    constexpr uint16_t CLIENT_ONLY = 1 << 1;
    constexpr uint16_t EDITOR_ONLY = 1 << 2;
} // namespace GA_FLAGS

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

class StringVariant
{
private:
    rct_string_id StringId = STR_NONE;
    std::string String;

public:
    StringVariant() = default;

    StringVariant(rct_string_id stringId)
        : StringId(stringId)
    {
    }

    StringVariant(const std::string& s)
        : String(s)
    {
    }

    StringVariant(std::string&& s)
        : String(s)
    {
    }

    StringVariant(const char* s)
        : String(s)
    {
    }

    const std::string* AsString() const
    {
        if (!String.empty())
        {
            return &String;
        }
        return {};
    }

    const rct_string_id* AsStringId() const
    {
        if (String.empty())
        {
            return &StringId;
        }
        return {};
    }

    rct_string_id GetStringId() const
    {
        return String.empty() ? StringId : STR_NONE;
    }
};

/**
 * Represents the result of a game action query or execution.
 */
class GameActionResult
{
public:
    using Ptr = std::unique_ptr<GameActionResult>;

    GA_ERROR Error = GA_ERROR::OK;
    StringVariant ErrorTitle;
    StringVariant ErrorMessage;
    std::array<uint8_t, 32> ErrorMessageArgs;
    CoordsXYZ Position = { LOCATION_NULL, LOCATION_NULL, LOCATION_NULL };
    money32 Cost = 0;
    ExpenditureType Expenditure = ExpenditureType::Count;

    GameActionResult() = default;
    GameActionResult(GA_ERROR error, rct_string_id message);
    GameActionResult(GA_ERROR error, rct_string_id title, rct_string_id message);
    GameActionResult(GA_ERROR error, rct_string_id title, rct_string_id message, uint8_t* args);
    GameActionResult(const GameActionResult&) = delete;
    virtual ~GameActionResult(){};

    std::string GetErrorTitle() const;
    std::string GetErrorMessage() const;
};

class ConstructClearResult final : public GameActionResult
{
public:
    uint8_t GroundFlags{ 0 };
};

/**
 *
 */
class GameActionParameterVisitor
{
public:
    virtual ~GameActionParameterVisitor() = default;

    virtual void Visit(const std::string_view& name, bool& param)
    {
    }

    virtual void Visit(const std::string_view& name, int32_t& param)
    {
    }

    virtual void Visit(const std::string_view& name, std::string& param)
    {
    }

    void Visit(CoordsXY& param)
    {
        Visit("x", param.x);
        Visit("y", param.y);
    }

    void Visit(CoordsXYZD& param)
    {
        Visit("x", param.x);
        Visit("y", param.y);
        Visit("z", param.z);
        Visit("direction", param.direction);
    }

    template<typename T> void Visit(const std::string_view& name, T& param)
    {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "Not an arithmetic type");
        auto value = static_cast<int32_t>(param);
        Visit(name, value);
        param = static_cast<T>(value);
    }

    template<typename T, size_t _TypeID> void Visit(const std::string_view& name, NetworkObjectId_t<T, _TypeID>& param)
    {
        Visit(name, param.id);
    }
};

struct GameAction
{
public:
    using Ptr = std::unique_ptr<GameAction>;
    using Callback_t = std::function<void(const struct GameAction*, const GameActionResult*)>;

private:
    uint32_t const _type;

    NetworkPlayerId_t _playerId = { -1 }; // Callee
    uint32_t _flags = 0;                  // GAME_COMMAND_FLAGS
    uint32_t _networkId = 0;
    Callback_t _callback;

public:
    GameAction(uint32_t type)
        : _type(type)
    {
    }

    virtual ~GameAction() = default;

    virtual const char* GetName() const = 0;

    virtual void AcceptParameters(GameActionParameterVisitor&)
    {
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
     * Gets the GA_FLAGS flags that are enabled for this game action.
     */
    virtual uint16_t GetActionFlags() const
    {
        // Make sure we execute some things only on the client.
        uint16_t flags = 0;

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) != 0 || (GetFlags() & GAME_COMMAND_FLAG_NO_SPEND) != 0)
        {
            flags |= GA_FLAGS::CLIENT_ONLY;
        }

        if (GetFlags() & GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED)
        {
            flags |= GA_FLAGS::ALLOW_WHILE_PAUSED;
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

    uint32_t GetType() const
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
    virtual GameActionResult::Ptr Query() const abstract;

    /**
     * Apply the game action and change the game state.
     */
    virtual GameActionResult::Ptr Execute() const abstract;

    bool LocationValid(const CoordsXY& coords) const;
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

template<uint32_t TId> struct GameActionNameQuery
{
};

template<uint32_t TType, typename TResultType> struct GameActionBase : GameAction
{
public:
    using Result = TResultType;

    static constexpr uint32_t TYPE = TType;

    GameActionBase()
        : GameAction(TYPE)
    {
    }

    virtual const char* GetName() const override
    {
        return GameActionNameQuery<TType>::Name();
    }

    void SetCallback(std::function<void(const struct GameAction*, const TResultType*)> typedCallback)
    {
        GameAction::SetCallback([typedCallback](const GameAction* ga, const GameActionResult* result) {
            typedCallback(ga, static_cast<const TResultType*>(result));
        });
    }

protected:
    template<class... TTypes> static constexpr std::unique_ptr<TResultType> MakeResult(TTypes&&... args)
    {
        return std::make_unique<TResultType>(std::forward<TTypes>(args)...);
    }
};

using GameActionFactory = GameAction* (*)();

namespace GameActions
{
    void Initialize();
    void Register();
    bool IsValidId(uint32_t id);

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

    GameAction::Ptr Create(uint32_t id);
    GameAction::Ptr Clone(const GameAction* action);

    // This should be used if a round trip is to be expected.
    GameActionResult::Ptr Query(const GameAction* action);
    GameActionResult::Ptr Execute(const GameAction* action);

    // This should be used from within game actions.
    GameActionResult::Ptr QueryNested(const GameAction* action);
    GameActionResult::Ptr ExecuteNested(const GameAction* action);

    GameActionFactory Register(uint32_t id, GameActionFactory action);

    template<typename T> static GameActionFactory Register()
    {
        GameActionFactory factory = []() -> GameAction* { return new T(); };
        Register(T::TYPE, factory);
        return factory;
    }

    // clang-format off
#define DEFINE_GAME_ACTION(cls, id, res)                                         \
    template<> struct GameActionNameQuery<id>                                    \
    {                                                                            \
        static const char* Name()                                                \
        {                                                                        \
            return #cls;                                                         \
        }                                                                        \
    };                                                                           \
    struct cls final : public GameActionBase<id, res>
    // clang-format on

} // namespace GameActions
