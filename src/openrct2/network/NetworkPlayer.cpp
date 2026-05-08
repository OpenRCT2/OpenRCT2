/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkPlayer.h"

    #include "../core/Money.hpp"
    #include "../ui/WindowManager.h"
    #include "NetworkPacket.h"

namespace OpenRCT2::Network
{
    void Player::setName(std::string_view newName)
    {
        // 36 == 31 + strlen(" #255");
        name = newName.substr(0, 36);
    }

    void Player::read(Packet& packet)
    {
        auto newName = packet.ReadString();
        setName(newName);
        packet >> id >> flags >> group >> lastAction >> lastActionCoord.x >> lastActionCoord.y >> lastActionCoord.z
            >> moneySpent >> commandsRan;
    }

    void Player::write(Packet& packet)
    {
        packet.WriteString(name);
        packet << id << flags << group << lastAction << lastActionCoord.x << lastActionCoord.y << lastActionCoord.z
               << moneySpent << commandsRan;
    }

    void Player::incrementNumCommands()
    {
        commandsRan++;
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::player, id);
    }

    void Player::addMoneySpent(money64 cost)
    {
        moneySpent += cost;
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::player, id);
    }
} // namespace OpenRCT2::Network

#endif
