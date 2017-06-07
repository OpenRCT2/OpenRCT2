#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <openrct2/common.h>

extern "C" {
#include <openrct2/paint/map_element/map_element.h>
}

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
