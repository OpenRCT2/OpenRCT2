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

struct SupportCall {
    sint32 height;
    sint16 slope;

    friend bool operator==(const SupportCall& lhs, const SupportCall& rhs) {
        if (lhs.height != rhs.height) return false;
        if (lhs.slope != rhs.slope) return false;

        return true;
    }

    bool operator!=(const SupportCall &other) const {
        return !(*this == other);
    }

    bool operator<(const SupportCall &other) const {
        if (height != other.height) {
            return height < other.height;
        }

        return slope < other.slope;
    }
};

namespace GeneralSupportHeightCall {
    bool CallsMatch(SupportCall tileSupportCalls[4]);

    bool FindMostCommonSupportCall(SupportCall calls[4], SupportCall *out);

    bool AssertEquals(const SupportCall *lhs, const SupportCall *rhs);
};
