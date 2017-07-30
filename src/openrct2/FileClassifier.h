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

#include "common.h"

enum
{
    FILE_EXTENSION_UNKNOWN,
    FILE_EXTENSION_DAT,
    FILE_EXTENSION_SC4,
    FILE_EXTENSION_SV4,
    FILE_EXTENSION_TD4,
    FILE_EXTENSION_SC6,
    FILE_EXTENSION_SV6,
    FILE_EXTENSION_TD6,
};

#ifdef __cplusplus

#include <string>

interface IStream;

enum class FILE_TYPE
{
    UNDEFINED,
    OBJECT,
    SAVED_GAME,
    SCENARIO,
    TRACK_DESIGN,
};

struct ClassifiedFileInfo
{
    FILE_TYPE Type;
    uint32    Version;
};

#define FILE_TYPE_S4_CUTOFF 2
bool TryClassifyFile(const std::string &path, ClassifiedFileInfo * result);
bool TryClassifyFile(IStream * stream, ClassifiedFileInfo * result);

#endif // __cplusplus


#ifdef __cplusplus
extern "C"
{
#endif
    uint32 get_file_extension_type(const utf8 * path);
#ifdef __cplusplus
}
#endif
