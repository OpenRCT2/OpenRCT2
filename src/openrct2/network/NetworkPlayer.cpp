/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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

void NetworkPlayer::SetName(std::string_view name)
{
    // 36 == 31 + strlen(" #255");
    Name = name.substr(0, 36);
}

void NetworkPlayer::Read(NetworkPacket& packet)
{
    auto name = packet.ReadString();
    SetName(name);
    packet >> Id >> Flags >> Group >> LastAction >> LastActionCoord.x >> LastActionCoord.y >> LastActionCoord.z >> MoneySpent
        >> CommandsRan;
}

void NetworkPlayer::Write(NetworkPacket& packet)
{
    packet.WriteString(Name);
    packet << Id << Flags << Group << LastAction << LastActionCoord.x << LastActionCoord.y << LastActionCoord.z << MoneySpent
           << CommandsRan;
}

void NetworkPlayer::IncrementNumCommands()
{
    CommandsRan++;
    auto* windowMgr = OpenRCT2::Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Player, Id);
}

void NetworkPlayer::AddMoneySpent(money64 cost)
{
    MoneySpent += cost;
    auto* windowMgr = OpenRCT2::Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Player, Id);
}

#endif
