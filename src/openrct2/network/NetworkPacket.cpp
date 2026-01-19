/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef DISABLE_NETWORK

    #include "NetworkPacket.h"

    #include "NetworkTypes.h"

    #include <memory>

namespace OpenRCT2::Network
{
    Packet::Packet(Command id) noexcept
        : Header{ PacketHeader::kMagic, PacketHeader::kVersion, 0, id }
    {
    }

    uint8_t* Packet::GetData() noexcept
    {
        return Data.data();
    }

    const uint8_t* Packet::GetData() const noexcept
    {
        return Data.data();
    }

    Command Packet::GetCommand() const noexcept
    {
        return Header.id;
    }

    void Packet::Clear() noexcept
    {
        BytesTransferred = 0;
        BytesRead = 0;
        Data.clear();
    }

    bool Packet::CommandRequiresAuth() const noexcept
    {
        switch (GetCommand())
        {
            case Command::ping:
            case Command::auth:
            case Command::token:
            case Command::gameInfo:
            case Command::objectsList:
            case Command::scriptsHeader:
            case Command::scriptsData:
            case Command::mapRequest:
            case Command::heartbeat:
                return false;
            default:
                return true;
        }
    }

    void Packet::Write(const void* bytes, size_t size)
    {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(bytes);
        Data.insert(Data.end(), src, src + size);
    }

    void Packet::WriteString(std::string_view s)
    {
        Write(s.data(), s.size());
        Data.push_back(0);
    }

    const uint8_t* Packet::Read(size_t size)
    {
        if (BytesRead + size > Data.size())
        {
            return nullptr;
        }

        const uint8_t* data = Data.data() + BytesRead;
        BytesRead += size;
        return data;
    }

    std::string_view Packet::ReadString()
    {
        if (BytesRead >= Data.size())
            return {};

        const char* str = reinterpret_cast<const char*>(Data.data() + BytesRead);

        size_t stringLen = 0;
        while (BytesRead < Data.size() && str[stringLen] != '\0')
        {
            BytesRead++;
            stringLen++;
        }

        if (str[stringLen] != '\0')
            return {};

        // Skip null terminator.
        BytesRead++;

        return std::string_view(str, stringLen);
    }
} // namespace OpenRCT2::Network

#endif
