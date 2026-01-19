/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScPlayer.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../actions/PlayerSetGroupAction.h"
    #include "../../../network/Network.h"
    #include "../../../network/NetworkAction.h"

namespace OpenRCT2::Scripting
{
    ScPlayer::ScPlayer(int32_t id)
        : _id(id)
    {
    }

    int32_t ScPlayer::id_get() const
    {
        return _id;
    }

    std::string ScPlayer::name_get() const
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(_id);
        if (index == -1)
            return {};
        return Network::GetPlayerName(index);
    #else
        return {};
    #endif
    }

    int32_t ScPlayer::group_get() const
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(_id);
        if (index == -1)
            return {};
        return Network::GetPlayerGroup(index);
    #else
        return 0;
    #endif
    }
    void ScPlayer::group_set(int32_t value)
    {
    #ifndef DISABLE_NETWORK
        auto playerSetGroupAction = GameActions::PlayerSetGroupAction(_id, value);
        GameActions::Execute(&playerSetGroupAction, getGameState());
    #endif
    }

    int32_t ScPlayer::ping_get() const
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(_id);
        if (index == -1)
            return {};
        return Network::GetPlayerPing(index);
    #else
        return 0;
    #endif
    }

    int32_t ScPlayer::commandsRan_get() const
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(_id);
        if (index == -1)
            return {};
        return Network::GetPlayerCommandsRan(index);
    #else
        return 0;
    #endif
    }

    int32_t ScPlayer::moneySpent_get() const
    {
    #ifndef DISABLE_NETWORK
        auto index = Network::GetPlayerIndex(_id);
        if (index == -1)
            return {};
        return Network::GetPlayerMoneySpent(index);
    #else
        return 0;
    #endif
    }

    std::string ScPlayer::ipAddress_get() const
    {
        return Network::GetPlayerIPAddress(_id);
    }

    std::string ScPlayer::publicKeyHash_get() const
    {
        return Network::GetPlayerPublicKeyHash(_id);
    }

    void ScPlayer::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScPlayer::id_get, nullptr, "id");
        dukglue_register_property(ctx, &ScPlayer::name_get, nullptr, "name");
        dukglue_register_property(ctx, &ScPlayer::group_get, &ScPlayer::group_set, "group");
        dukglue_register_property(ctx, &ScPlayer::ping_get, nullptr, "ping");
        dukglue_register_property(ctx, &ScPlayer::commandsRan_get, nullptr, "commandsRan");
        dukglue_register_property(ctx, &ScPlayer::moneySpent_get, nullptr, "moneySpent");
        dukglue_register_property(ctx, &ScPlayer::ipAddress_get, nullptr, "ipAddress");
        dukglue_register_property(ctx, &ScPlayer::publicKeyHash_get, nullptr, "publicKeyHash");
    }

} // namespace OpenRCT2::Scripting

#endif
