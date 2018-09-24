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

struct SupportCall
{
    int32_t height;
    int16_t slope;

    friend bool operator==(const SupportCall& lhs, const SupportCall& rhs)
    {
        if (lhs.height != rhs.height)
            return false;
        if (lhs.slope != rhs.slope)
            return false;

        return true;
    }

    bool operator!=(const SupportCall& other) const
    {
        return !(*this == other);
    }

    bool operator<(const SupportCall& other) const
    {
        if (height != other.height)
        {
            return height < other.height;
        }

        return slope < other.slope;
    }
};

namespace GeneralSupportHeightCall
{
    bool CallsMatch(SupportCall tileSupportCalls[4]);

    bool FindMostCommonSupportCall(SupportCall calls[4], SupportCall* out);

    bool AssertEquals(const SupportCall* lhs, const SupportCall* rhs);
}; // namespace GeneralSupportHeightCall
