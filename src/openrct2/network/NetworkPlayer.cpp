/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

#include "NetworkPacket.h"
#include "NetworkPlayer.h"

#include "../interface/Window.h"
#include "../localisation/Localisation.h"

void NetworkPlayer::SetName(const std::string &name)
{
    // 36 == 31 + strlen(" #255");
    Name = name.substr(0, 36);
    utf8_remove_format_codes((utf8 *)Name.data(), false);
}

void NetworkPlayer::Read(NetworkPacket &packet)
{
    const utf8 * name = packet.ReadString();
    SetName(name);
    packet >> Id >> Flags >> Group;
}

void NetworkPlayer::Write(NetworkPacket &packet)
{
    packet.WriteString((const char*)Name.c_str());
    packet << Id << Flags << Group;
}

void NetworkPlayer::AddMoneySpent(money32 cost)
{
    MoneySpent += cost;
    CommandsRan++;
    window_invalidate_by_number(WC_PLAYER, Id);
}

#endif
