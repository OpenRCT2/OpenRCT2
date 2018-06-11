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

#include <string>
#include "Version.h"

#ifdef OPENRCT2_BUILD_INFO_HEADER
#include OPENRCT2_BUILD_INFO_HEADER
#endif

const char gVersionInfoFull[] =
    OPENRCT2_NAME ", "
#ifdef OPENRCT2_VERSION_TAG
    OPENRCT2_VERSION_TAG
#else
    "v" OPENRCT2_VERSION
#endif
#if defined(OPENRCT2_BRANCH) || defined(OPENRCT2_COMMIT_SHA1_SHORT) || !defined(NDEBUG)
    " ("
#if defined(OPENRCT2_BRANCH) && defined(OPENRCT2_COMMIT_SHA1_SHORT)
    OPENRCT2_COMMIT_SHA1_SHORT " on " OPENRCT2_BRANCH
#elif defined(OPENRCT2_COMMIT_SHA1_SHORT)
    OPENRCT2_COMMIT_SHA1_SHORT
#elif defined(OPENRCT2_BRANCH)
    OPENRCT2_BRANCH
#endif
#ifndef NDEBUG
    ", DEBUG"
#endif
    ")"
#endif
#ifdef OPENRCT2_BUILD_SERVER
    " provided by " OPENRCT2_BUILD_SERVER
#endif
    ;
