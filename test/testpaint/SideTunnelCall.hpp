/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/common.h>
#include <openrct2/paint/Paint.h>

enum
{
    TUNNELCALL_SKIPPED,
    TUNNELCALL_NONE,
    TUNNELCALL_CALL,
};

struct TunnelCall
{
    uint8_t call;
    int16_t offset;
    uint8_t type;
};

namespace SideTunnelCall
{
    int16_t GetTunnelOffset(uint32_t baseHeight, tunnel_entry calls[3]);
    TunnelCall ExtractTunnelCalls(tunnel_entry* list, uint8_t count, uint16_t baseHeight, bool* error);

    bool TunnelPatternsMatch(TunnelCall expected[4], TunnelCall actual[4]);
    void GetTunnelCallReferencePattern(TunnelCall tunnelCalls[4][4], TunnelCall (*out)[4]);
    bool TunnelCallsLineUp(TunnelCall tunnelCalls[4][4]);
}; // namespace SideTunnelCall
