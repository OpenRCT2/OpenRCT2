/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Formatter.h"

thread_local uint8_t gCommonFormatArgs[80];

Formatter& Formatter::operator=(const Formatter& other)
{
    // If using global or not
    if (other.StartBuf == other.Buffer.data())
    {
        std::copy(std::begin(other.Buffer), std::end(other.Buffer), std::begin(Buffer));
        StartBuf = Buffer.data();
    }
    else
    {
        StartBuf = other.StartBuf;
    }
    CurrentBuf = StartBuf + other.NumBytes();
    return *this;
}

Formatter Formatter::Common()
{
    return Formatter{ gCommonFormatArgs };
}
