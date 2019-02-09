/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

// clang-format off

#ifndef _RCT_STRINGS_H_
#define _RCT_STRINGS_H_

#include "../common.h"

#include <unordered_map>

#define STR_NONE ((rct_string_id)-1)
#define STR_VIEWPORT ((rct_string_id)-2)

#define RCT_STRING_BEGIN()                                                                                                     \
enum                                                                                                                           \
{
#define RCT_STRING(name, val) name = val
#define RCT_STRING_END()                                                                                                       \
};


#include "StringIds.h"

#undef RCT_STRING_BEGIN
#undef RCT_STRING
#undef RCT_STRING_END
#define RCT_STRING_BEGIN() const std::unordered_map<std::string, rct_string_id> stringIdMap = {
#define RCT_STRING(name, val) { #name, name }
#define RCT_STRING_END()                                                                                                       \
};

#include "StringIds.h"

#endif

// clang-format on
