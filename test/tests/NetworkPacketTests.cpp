/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <gtest/gtest.h>
#include <openrct2/network/NetworkPacket.h>
#ifndef DISABLE_NETWORK

using namespace OpenRCT2::Network;

TEST(NetworkPacket, read_returns_bytes_when_available)
{
    Packet packet(Command::ping);
    constexpr std::array<uint8_t, 3> payload = { 0x01, 0x02, 0x03 };

    packet.Write(payload.data(), payload.size());

    const auto* data = packet.Read(payload.size());
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data[0], payload[0]);
    EXPECT_EQ(data[1], payload[1]);
    EXPECT_EQ(data[2], payload[2]);
    EXPECT_EQ(packet.BytesRead, payload.size());
}

TEST(NetworkPacket, read_returns_nullptr_when_packet_is_truncated)
{
    Packet packet(Command::ping);
    constexpr std::array<uint8_t, 2> payload = { 0x01, 0x02 };

    packet.Write(payload.data(), payload.size());

    EXPECT_EQ(packet.Read(payload.size() + 1), nullptr);
    EXPECT_EQ(packet.BytesRead, 0u);
}

TEST(NetworkPacket, read_returns_nullptr_when_read_position_is_out_of_bounds)
{
    Packet packet(Command::ping);
    packet.Data.push_back(0x01);
    packet.BytesRead = packet.Data.size() + 1;

    EXPECT_EQ(packet.Read(1), nullptr);
}

#endif
