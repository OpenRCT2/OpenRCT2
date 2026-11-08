/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_HTTP

    #include "helpers/LoopbackServers.hpp"

    #include <chrono>
    #include <gtest/gtest.h>
    #include <openrct2/core/Http.h>

using namespace OpenRCT2;

TEST(HttpTests, DoFailsWithinTimeoutWhenServerNeverResponds)
{
    constexpr auto kTimeout = std::chrono::seconds(1);
    // Generous upper bound so the assertion holds on slow CI machines.
    constexpr auto kMaxFailureTime = kTimeout + std::chrono::seconds(5);
    UnresponsiveTcpServer server;

    Http::Request request;
    request.url = server.getUrl();
    request.timeout = kTimeout;

    auto start = std::chrono::steady_clock::now();
    auto response = Http::Do(request);
    auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_EQ(response.status, Http::Status::error);
    EXPECT_FALSE(response.error.empty());
    // The request must actually have waited for the server rather than failed to connect.
    EXPECT_GE(elapsed, kTimeout);
    EXPECT_LT(elapsed, kMaxFailureTime);
}

TEST(HttpTests, DoSucceedsWhenProgressingTransferOutlastsTimeout)
{
    const std::string body = "slow";
    constexpr auto kInterval = std::chrono::milliseconds(400);
    const auto transferTime = kInterval * static_cast<int64_t>(body.size());
    TrickleHttpServer server(body, kInterval);

    Http::Request request;
    request.url = server.getUrl();
    request.timeout = std::chrono::seconds(1);
    ASSERT_GT(transferTime, request.timeout);

    auto start = std::chrono::steady_clock::now();
    auto response = Http::Do(request);
    auto elapsed = std::chrono::steady_clock::now() - start;

    // Only a stall may trigger the timeout; a transfer that keeps progressing must outlast it.
    EXPECT_EQ(response.status, Http::Status::ok);
    EXPECT_EQ(response.body, body);
    EXPECT_GE(elapsed, transferTime);
}

#endif // DISABLE_HTTP
