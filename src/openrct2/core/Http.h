/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifndef DISABLE_HTTP

    #include <chrono>
    #include <functional>
    #include <future>
    #include <map>
    #include <string>

namespace OpenRCT2::Http
{
    enum class Status
    {
        invalid = 0,
        error = 1,
        ok = 200,
        notFound = 404
    };

    enum class Method
    {
        get,
        post,
        put
    };

    constexpr auto kDefaultTimeout = std::chrono::seconds(10);

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
        Method method = Method::get;
        std::string body;
        bool forceIPv4{};
        std::chrono::seconds timeout = kDefaultTimeout;
    };

    Response Do(const Request& req);

    [[nodiscard]] inline auto DoAsync(const Request& req, std::function<void(Response& res)> fn)
    {
        return std::async(std::launch::async, [=]() {
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
    }
} // namespace OpenRCT2::Http

#endif // DISABLE_HTTP
