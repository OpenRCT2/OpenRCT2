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

#include <string>
#include "../common.h"

namespace Path
{
    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src);
    std::string Combine(const std::string &a, const std::string &b);

    template<typename... Args>
    std::string Combine(const std::string &a, const std::string &b, Args... args)
    {
        return Combine(a, Combine(b, args...));
    }

    std::string GetDirectory(const std::string &path);
    utf8 * GetDirectory(const utf8 * path);
    utf8 * GetDirectory(utf8 * buffer, size_t bufferSize, const utf8 * path);
    std::string GetFileName(const std::string &path);
    const utf8 * GetFileName(const utf8 * path);
    std::string GetFileNameWithoutExtension(const std::string &path);
    utf8 * GetFileNameWithoutExtension(const utf8 * path);
    utf8 * GetFileNameWithoutExtension(utf8 * buffer, size_t bufferSize, const utf8 * path);
    const std::string GetExtension(const std::string &path);
    const utf8 * GetExtension(const utf8 * path);
    utf8 * GetAbsolute(utf8 * buffer, size_t bufferSize, const utf8 * relativePath);
    bool Equals(const std::string &a, const std::string &b);
    bool Equals(const utf8 * a, const utf8 * b);
}
