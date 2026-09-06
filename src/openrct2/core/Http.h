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
    #include <mutex>
    #include <string>
    #include <vector>

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

    // Takes ownership of an in-flight async request so no foreground thread ever blocks in a
    // future destructor during teardown. A std::async future (even after .share()) blocks in its
    // destructor while it holds the last reference to a not-yet-ready shared state; the backing
    // store here is intentionally never destroyed (function-local static pointer, deliberately
    // leaked) so process exit does not block either - the OS reclaims any worker still running.
    // Combined with the connect/total timeouts set in the Http back-ends a detached request
    // self-completes within ~30 s and is pruned on the next call.
    inline void Detach(std::shared_future<void> request)
    {
        if (!request.valid())
            return;

        struct Reaper
        {
            std::mutex mutex;
            std::vector<std::shared_future<void>> pending;
        };
        static Reaper* reaper = new Reaper(); // NOLINT(cppcoreguidelines-owning-memory) - deliberate lifetime leak

        std::scoped_lock lock(reaper->mutex);
        std::erase_if(reaper->pending, [](const std::shared_future<void>& f) {
            return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });
        reaper->pending.push_back(std::move(request));
    }
} // namespace OpenRCT2::Http

#endif // DISABLE_HTTP
