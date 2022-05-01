/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

struct encoding_convert_entry
{
    uint16_t code;
    uint32_t unicode;
};

extern const encoding_convert_entry RCT2ToUnicodeTable[];

wchar_t encoding_convert_rct2_to_unicode(wchar_t rct2str);
