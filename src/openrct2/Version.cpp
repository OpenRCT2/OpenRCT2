/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#include <string>
#include "Version.h"

#ifdef OPENRCT2_BUILD_INFO_HEADER
#include OPENRCT2_BUILD_INFO_HEADER
#endif

const char gVersionInfoFull[] =
    OPENRCT2_NAME ", v" OPENRCT2_VERSION
#ifdef OPENRCT2_BRANCH
    "-" OPENRCT2_BRANCH
#endif
#ifdef OPENRCT2_COMMIT_SHA1_SHORT
    " build " OPENRCT2_COMMIT_SHA1_SHORT
#endif
#ifdef OPENRCT2_BUILD_SERVER
    " provided by " OPENRCT2_BUILD_SERVER
#endif
#ifndef NDEBUG
    " (DEBUG)"
#endif
    ;
