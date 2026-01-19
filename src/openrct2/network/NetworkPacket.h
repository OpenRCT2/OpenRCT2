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
        uint16_t Size = 0;
        Command Id = Command::invalid;
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

        uint8_t* GetData() noexcept;
        const uint8_t* GetData() const noexcept;

        Command GetCommand() const noexcept;

        void Clear() noexcept;
        bool CommandRequiresAuth() const noexcept;

        const uint8_t* Read(size_t size);
        std::string_view ReadString();

        void Write(const void* bytes, size_t size);
        void WriteString(std::string_view s);

        template<typename T>
        Packet& operator>>(T& value)
        {
            if (BytesRead + sizeof(value) > Header.size)
            {
                value = T{};
            }
            else
            {
                T local;
                std::memcpy(&local, &GetData()[BytesRead], sizeof(local));
                value = ByteSwapBE(local);
                BytesRead += sizeof(value);
            }
            return *this;
        }

        template<typename T>
        Packet& operator<<(T value)
        {
            T swapped = ByteSwapBE(value);
            Write(&swapped, sizeof(T));
            return *this;
        }

        Packet& operator<<(DataSerialiser& data)
        {
            Write(static_cast<const uint8_t*>(data.GetStream().GetData()), data.GetStream().GetLength());
            return *this;
        }

    public:
        PacketHeader Header{};
        sfl::small_vector<uint8_t, 512> Data;
        size_t BytesTransferred = 0;
        size_t BytesRead = 0;
    };
} // namespace OpenRCT2::Network
