/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../Duktape.hpp"
#    include "ScPlayer.hpp"
#    include "ScPlayerGroup.hpp"
#    include "ScSocket.hpp"

#    include <memory>

namespace OpenRCT2::Scripting
{
    class ScNetwork
    {
    private:
#    ifdef __clang__
        [[maybe_unused]]
#    endif
        duk_context* _context;

    public:
        ScNetwork(duk_context* ctx);

        std::string mode_get() const;
        int32_t numPlayers_get() const;
        int32_t numGroups_get() const;
        int32_t defaultGroup_get() const;
        void defaultGroup_set(int32_t value);

        std::vector<std::shared_ptr<ScPlayerGroup>> groups_get() const;

        std::vector<std::shared_ptr<ScPlayer>> players_get() const;

        std::shared_ptr<ScPlayer> currentPlayer_get() const;

        std::shared_ptr<ScPlayer> getPlayer(int32_t index) const;

        DukValue stats_get() const;

        std::shared_ptr<ScPlayerGroup> getGroup(int32_t index) const;

        void addGroup();

        void removeGroup(int32_t index);

        void kickPlayer(int32_t index);

        void sendMessage(std::string message, DukValue players);

#    ifndef DISABLE_NETWORK
        std::shared_ptr<ScListener> createListener();
#    else
        void createListener();
#    endif

#    ifndef DISABLE_NETWORK
        std::shared_ptr<ScSocket> createSocket();
#    else
        void createSocket();
#    endif

        static void Register(duk_context* ctx);
    };
} // namespace OpenRCT2::Scripting

#endif
