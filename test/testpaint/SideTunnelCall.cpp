/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SideTunnelCall.hpp"

int16_t SideTunnelCall::GetTunnelOffset(uint32_t baseHeight, tunnel_entry calls[3])
{
    for (int16_t offset = -56; offset <= 56; offset += 8)
    {
        if (calls[0].height != (baseHeight - 8 + offset) / 16)
            continue;
        if (calls[1].height != (baseHeight + 0 + offset) / 16)
            continue;
        if (calls[2].height != (baseHeight + 8 + offset) / 16)
            continue;

        return offset;
    }

    log_error("Unknown tunnel height. (%d, %d, %d)", calls[0].height, calls[1].height, calls[2].height);
    return 0;
}

TunnelCall SideTunnelCall::ExtractTunnelCalls(tunnel_entry* calls, uint8_t count, uint16_t baseHeight, bool* error)
{
    TunnelCall tunnelCall = {};

    if (count == 0)
    {
        tunnelCall.call = TUNNELCALL_NONE;
        return tunnelCall;
    }

    if (count == 3)
    {
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
    for (int side = 0; side < 4; ++side)
    {
        TunnelCall* referenceCall = nullptr;
        for (int direction = 0; direction < 4; ++direction)
        {
            if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED)
            {
                continue;
            }

            if (referenceCall == nullptr)
            {
                referenceCall = &tunnelCalls[direction][side];
                continue;
            }

            if (referenceCall->call != tunnelCalls[direction][side].call)
                return false;

            if (referenceCall->call == TUNNELCALL_CALL)
            {
                if (referenceCall->type != tunnelCalls[direction][side].type)
                    return false;
                if (referenceCall->offset != tunnelCalls[direction][side].offset)
                    return false;
            }
        }
    }

    return true;
}

void SideTunnelCall::GetTunnelCallReferencePattern(TunnelCall tunnelCalls[4][4], TunnelCall (*out)[4])
{
    for (int side = 0; side < 4; ++side)
    {
        for (int direction = 0; direction < 4; ++direction)
        {
            if (tunnelCalls[direction][side].call == TUNNELCALL_SKIPPED)
            {
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
    for (int side = 0; side < 4; side++)
    {
        if (expected[side].call != actual[side].call)
            return false;

        if (expected[side].call == TUNNELCALL_CALL)
        {
            if (expected[side].type != actual[side].type)
                return false;
            if (expected[side].offset != actual[side].offset)
                return false;
        }
    }

    return true;
}
