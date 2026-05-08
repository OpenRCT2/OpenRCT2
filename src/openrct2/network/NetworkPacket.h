/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/DataSerialiser.h"
#include "NetworkTypes.h"

#include <memory>
#include <sfl/small_vector.hpp>
#include <vector>

namespace OpenRCT2::Network
{
#pragma pack(push, 1)
    struct PacketLegacyHeader
    {
        uint16_t size = 0;
        Command id = Command::invalid;
    };
    static_assert(sizeof(PacketLegacyHeader) == 6);

    struct PacketHeader
    {
        static constexpr uint32_t kMagic = 0x3254524F; // 'ORT2'
        static constexpr uint16_t kVersion = 2;

        uint32_t magic{};
        uint16_t version{};
        uint32_t size{};
        Command id{};
    };
#pragma pack(pop)

    struct Packet final
    {
        Packet() noexcept = default;
        Packet(Command id) noexcept;

        uint8_t* getData() noexcept;
        const uint8_t* getData() const noexcept;

        Command getCommand() const noexcept;

        void clear() noexcept;
        bool commandRequiresAuth() const noexcept;

        const uint8_t* read(size_t size);
        std::string_view readString();

        void write(const void* bytes, size_t size);
        void writeString(std::string_view s);

        template<typename T>
        Packet& operator>>(T& value)
        {
            if (bytesRead + sizeof(value) > header.size)
            {
                value = T{};
            }
            else
            {
                T local;
                std::memcpy(&local, &getData()[bytesRead], sizeof(local));
                value = ByteSwapBE(local);
                bytesRead += sizeof(value);
            }
            return *this;
        }

        template<typename T>
        Packet& operator<<(T value)
        {
            T swapped = ByteSwapBE(value);
            write(&swapped, sizeof(T));
            return *this;
        }

        Packet& operator<<(DataSerialiser& serialiser)
        {
            write(
                static_cast<const uint8_t*>(serialiser.GetStream().GetData()),
                serialiser.GetStream().GetLength());
            return *this;
        }

    public:
        PacketHeader header{};
        sfl::small_vector<uint8_t, 512> data;
        size_t bytesTransferred = 0;
        size_t bytesRead = 0;
    };
} // namespace OpenRCT2::Network
