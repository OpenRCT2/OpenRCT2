/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Version.h"

#include "config/Config.h"
#include "core/Console.hpp"
#include "core/Http.h"
#include "core/Json.hpp"

#include <chrono>

using namespace OpenRCT2;

#ifdef OPENRCT2_BUILD_INFO_HEADER
    #include OPENRCT2_BUILD_INFO_HEADER
#endif

const char gVersionInfoTag[] =
#ifdef OPENRCT2_VERSION_TAG
    OPENRCT2_VERSION_TAG
#else
    "v" kOpenRCT2Version
#endif
    ;

const char gVersionInfoFull[] = OPENRCT2_NAME ", "
#ifdef OPENRCT2_VERSION_TAG
    OPENRCT2_VERSION_TAG
#else
                                              "v" kOpenRCT2Version
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

#ifdef __EMSCRIPTEN__
// This must be wrapped in extern "C", according to the emscripten docs, "to prevent C++ name mangling"
extern "C" {
const char* GetVersion()
{
    return gVersionInfoFull;
}
}
#endif

NewVersionInfo GetLatestVersion()
{
    // If the check doesn't succeed, provide current version so we don't bother user
    // with invalid data.
    std::string tag = gVersionInfoTag;
    NewVersionInfo verinfo{ tag, "", "", "" };
#ifndef DISABLE_HTTP
    auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto then = Config::Get().general.LastVersionCheckTime;
    using namespace std::chrono_literals;

    if (then < now - std::chrono::seconds(24h).count())
    {
        Http::Request request;
        request.url = "https://api.github.com/repos/OpenRCT2/OpenRCT2/releases/latest";
        request.method = Http::Method::GET;

        Http::Response res;
        try
        {
            res = Do(request);
            if (res.status != Http::Status::Ok)
                throw std::runtime_error("bad http status");
        }
        catch (std::exception& e)
        {
            Console::Error::WriteLine("Failed to download '%s', cause %s", request.url.c_str(), e.what());
            return {};
        }

        json_t root = Json::FromString(res.body);

        verinfo.tag = Json::GetString(root["tag_name"]);
        verinfo.name = Json::GetString(root["name"]);
        verinfo.changelog = Json::GetString(root["body"]);
        verinfo.url = Json::GetString(root["html_url"]);

        Config::Get().general.LastVersionCheckTime = now;
        Config::Save();
    }
#endif
    return verinfo;
}
