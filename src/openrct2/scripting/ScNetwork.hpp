/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../network/network.h"
#include "Duktape.hpp"

namespace OpenRCT2::Scripting
{
    class ScPlayer
    {
    private:
        int32_t _id;

    public:
        ScPlayer(int32_t id)
            : _id(id)
        {
        }

        int32_t id_get()
        {
            return _id;
        }

        std::string name_get()
        {
            auto index = network_get_player_index(_id);
            if (index == -1)
                return {};
            return network_get_player_name(index);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScPlayer::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScPlayer::name_get, nullptr, "name");
        }
    };

    class ScNetwork
    {
    private:
        duk_context* _context;

    public:
        ScNetwork(duk_context* ctx)
            : _context(ctx)
        {
        }

        int32_t players_get()
        {
            return network_get_num_players();
        }

        std::shared_ptr<ScPlayer> getPlayer(int32_t index)
        {
            auto numPlayers = network_get_num_players();
            if (index < numPlayers)
            {
                auto playerId = network_get_player_id(index);
                return std::make_shared<ScPlayer>(playerId);
            }
            return nullptr;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScNetwork::players_get, nullptr, "players");
            dukglue_register_method(ctx, &ScNetwork::getPlayer, "getPlayer");
        }
    };
} // namespace OpenRCT2::Scripting
