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

#include "SideTunnelCall.hpp"

sint16 SideTunnelCall::GetTunnelOffset(uint32 baseHeight, tunnel_entry calls[3]) {
    for (sint16 offset = -56; offset <= 56; offset += 8) {
        if (calls[0].height != (baseHeight - 8 + offset) / 16) continue;
        if (calls[1].height != (baseHeight + 0 + offset) / 16) continue;
        if (calls[2].height != (baseHeight + 8 + offset) / 16) continue;

        return offset;
    }

    log_error("Unknown tunnel height. (%d, %d, %d)", calls[0].height, calls[1].height, calls[2].height);
    return 0;
}


TunnelCall SideTunnelCall::ExtractTunnelCalls(tunnel_entry *calls, uint8 count, uint16 baseHeight, bool *error) {
    TunnelCall tunnelCall = {0};

    if (count == 0) {
        tunnelCall.call = TUNNELCALL_NONE;
        return tunnelCall;
    }

    if (count == 3) {
        tunnelCall.call = TUNNELCALL_CALL;
        tunnelCall.offset = GetTunnelOffset(baseHeight, calls);
        tunnelCall.type = calls[0].type;
        return tunnelCall;
    }

    *error = true;
    return tunnelCall;
}


bool SideTunnelCall::TunnelCallsLineUp(TunnelCall tunnelCalls[4][4])
{
    for (int side = 0; side < 4; ++side) {
        TunnelCall * referenceCall = nullptr;
        for (int direction = 0; direction < 4; ++direction) {
            if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED) {
                continue;
            }

            if (referenceCall == nullptr) {
                referenceCall = &tunnelCalls[direction][side];
                continue;
            }

            if (referenceCall->call != tunnelCalls[direction][side].call) return false;

            if (referenceCall->call == TUNNELCALL_CALL) {
                if (referenceCall->type != tunnelCalls[direction][side].type) return false;
                if (referenceCall->offset != tunnelCalls[direction][side].offset) return false;
            }
        }
    }

    return true;
}

void SideTunnelCall::GetTunnelCallReferencePattern(TunnelCall tunnelCalls[4][4], TunnelCall (*out)[4])
{
    for (int side = 0; side < 4; ++side) {
        for (int direction = 0; direction < 4; ++direction) {
            if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED) {
                continue;
            }

            (*out)[side].call = tunnelCalls[direction][side].call;
            (*out)[side].type = tunnelCalls[direction][side].type;
            (*out)[side].offset = tunnelCalls[direction][side].offset;
        }
    }
}

bool SideTunnelCall::TunnelPatternsMatch(TunnelCall expected[4], TunnelCall actual[4])
{
    for (int side = 0; side < 4; side++) {
        if (expected[side].call != actual[side].call) return false;

        if (expected[side].call == TUNNELCALL_CALL) {
            if (expected[side].type != actual[side].type) return false;
            if (expected[side].offset != actual[side].offset) return false;
        }
    }

    return true;
}
