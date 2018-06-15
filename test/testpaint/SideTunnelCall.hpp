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

enum {
    TUNNELCALL_SKIPPED,
    TUNNELCALL_NONE,
    TUNNELCALL_CALL,
};

struct TunnelCall {
    uint8 call;
    sint16 offset;
    uint8 type;
};

namespace SideTunnelCall {
    sint16 GetTunnelOffset(uint32 baseHeight, tunnel_entry calls[3]);
    TunnelCall ExtractTunnelCalls(tunnel_entry * list, uint8 count, uint16 baseHeight, bool * error);

    bool TunnelPatternsMatch(TunnelCall expected[4], TunnelCall actual[4]);
    void GetTunnelCallReferencePattern(TunnelCall tunnelCalls[4][4], TunnelCall (*out)[4]);
    bool TunnelCallsLineUp(TunnelCall tunnelCalls[4][4]);
};
