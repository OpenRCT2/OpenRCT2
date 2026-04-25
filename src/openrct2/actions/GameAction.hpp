/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/DataSerialiser.h"
#include "../core/Identifier.hpp"
#include "GameActionParameterVisitor.h"
#include "GameActionResult.h"
#include "GameCommand.h"

#include <functional>
#include <memory>

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::Park
{
    struct ParkData;
}

namespace OpenRCT2::GameActions
{
    namespace Flags
    {
        constexpr uint16_t AllowWhilePaused = 1 << 0;
        constexpr uint16_t ClientOnly = 1 << 1;
        constexpr uint16_t EditorOnly = 1 << 2;
        constexpr uint16_t IgnoreForReplays = 1 << 3;
    } // namespace Flags

    // GCC 15.1 is overzealous with its 'final' warnings
#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

    class GameAction
    {
    public:
        using Ptr = std::unique_ptr<GameAction>;
        using Callback_t = std::function<void(const class GameAction*, const Result*)>;

    private:
        GameCommand const _type;

        Network::PlayerId_t _playerId = { -1 }; // Callee
        CommandFlags _flags = {};
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
            visitor.Visit("flags", _flags.holder);
        }

        Network::PlayerId_t GetPlayer() const
        {
            return _playerId;
        }

        void SetPlayer(Network::PlayerId_t playerId)
        {
            _playerId = playerId;
        }

        /**
         * Gets the OpenRCT2::GameActions::Flags flags that are enabled for this game action.
         */
        virtual uint16_t GetActionFlags() const
        {
            // Make sure we execute some things only on the client.
            uint16_t flags = 0;

            if (GetFlags().hasAny(CommandFlag::ghost, CommandFlag::noSpend))
            {
                flags |= Flags::ClientOnly;
            }

            if (GetFlags().has(CommandFlag::allowDuringPaused))
            {
                flags |= Flags::AllowWhilePaused;
            }

            return flags;
        }

        CommandFlags GetFlags() const
        {
            return _flags;
        }

        CommandFlags SetFlags(CommandFlags flags)
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
            stream << DS_TAG(_networkId) << DS_TAG(_flags.holder) << DS_TAG(_playerId);
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
        virtual Result Query(GameState_t& gameState, Park::ParkData& park) const = 0;

        /**
         * Apply the game action and change the game state.
         */
        virtual Result Execute(GameState_t& gameState, Park::ParkData& park) const = 0;

        bool LocationValid(const CoordsXY& coords) const;
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
    #pragma GCC diagnostic pop
#endif

    template<GameCommand TType>
    struct GameActionBase : GameAction
    {
    public:
        static constexpr GameCommand kType = TType;

        GameActionBase()
            : GameAction(kType)
        {
        }
    };

    GameAction::Ptr Create(GameCommand id);
    GameAction::Ptr Clone(const GameAction* action);

} // namespace OpenRCT2::GameActions
