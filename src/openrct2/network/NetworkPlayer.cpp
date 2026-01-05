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
    void Player::SetName(std::string_view name)
    {
        // 36 == 31 + strlen(" #255");
        Name = name.substr(0, 36);
    }

    void Player::Read(Packet& packet)
    {
        auto name = packet.ReadString();
        SetName(name);
        packet >> Id >> Flags >> Group >> LastAction >> LastActionCoord.x >> LastActionCoord.y >> LastActionCoord.z
            >> MoneySpent >> CommandsRan;
    }

    void Player::Write(Packet& packet)
    {
        packet.WriteString(Name);
        packet << Id << Flags << Group << LastAction << LastActionCoord.x << LastActionCoord.y << LastActionCoord.z
               << MoneySpent << CommandsRan;
    }

    void Player::IncrementNumCommands()
    {
        CommandsRan++;
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::player, Id);
    }

    void Player::AddMoneySpent(money64 cost)
    {
        MoneySpent += cost;
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::player, Id);
    }
} // namespace OpenRCT2::Network

#endif
