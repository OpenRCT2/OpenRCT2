/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_HTTP

#    include "../common.h"

#    include <functional>
#    include <map>
#    include <string>
#    include <thread>

namespace Http
{
    enum class Status
    {
        Invalid = 0,
        Ok = 200,
        NotFound = 404
    };

    enum class Method
    {
        GET,
        POST,
        PUT
    };

    struct Response
    {
        Status status{};
        std::string content_type;
        std::string body;
        std::map<std::string, std::string> header = {};
        std::string error;
    };

    struct Request
    {
        std::string url;
        std::map<std::string, std::string> header;
        Method method = Method::GET;
        std::string body;
        bool forceIPv4{};
    };

    Response Do(const Request& req);

    inline void DoAsync(const Request& req, std::function<void(Response& res)> fn)
    {
        auto thread = std::thread([=]() {
            Response res{};
            try
            {
                res = Do(req);
            }
            catch (std::exception& e)
            {
                res.error = e.what();
                return;
            }
            fn(res);
        });
        thread.detach();
    }
} // namespace Http

#endif // DISABLE_HTTP
