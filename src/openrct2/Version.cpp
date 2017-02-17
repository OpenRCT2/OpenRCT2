#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "core/String.hpp"
#include "Version.h"

#ifdef OPENRCT2_BUILD_INFO_HEADER
#include OPENRCT2_BUILD_INFO_HEADER
#endif

#ifdef OPENRCT2_BUILD_NUMBER
    const char *gBuildNumber = OPENRCT2_BUILD_NUMBER;
#else
    const char *gBuildNumber = "";
#endif

#ifdef OPENRCT2_BUILD_SERVER
    const char *gBuildServer = OPENRCT2_BUILD_SERVER;
#else
    const char *gBuildServer = "";
#endif

#ifdef OPENRCT2_BRANCH
    const char *gGitBranch = OPENRCT2_BRANCH;
#else
    const char *gGitBranch = "";
#endif

#ifdef OPENRCT2_COMMIT_SHA1
    const char *gCommitSha1 = OPENRCT2_COMMIT_SHA1;
#else
    const char *gCommitSha1 = "";
#endif

#ifdef OPENRCT2_COMMIT_SHA1_SHORT
    const char *gCommitSha1Short = OPENRCT2_COMMIT_SHA1_SHORT;
#else
    const char *gCommitSha1Short = "";
#endif

namespace Version
{
    static std::string _info;

    std::string GetInfo()
    {
        if (_info.empty())
        {
            utf8 buffer[256];
            size_t bufferSize = sizeof(buffer);
            String::Set(buffer, bufferSize, OPENRCT2_NAME ", v" OPENRCT2_VERSION);
            if (!String::IsNullOrEmpty(gGitBranch))
            {
                String::AppendFormat(buffer, bufferSize, "-%s", gGitBranch);
            }
            if (!String::IsNullOrEmpty(gCommitSha1Short))
            {
                String::AppendFormat(buffer, bufferSize, " build %s", gCommitSha1Short);
            }
            if (!String::IsNullOrEmpty(gBuildServer))
            {
                String::AppendFormat(buffer, bufferSize, " provided by %s", gBuildServer);
            }
#ifdef DEBUG
            String::AppendFormat(buffer, bufferSize, " (DEBUG)", gBuildServer);
#endif
            _info = std::string(buffer);
        }
        return _info;
    }
}
