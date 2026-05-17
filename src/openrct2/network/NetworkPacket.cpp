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
        : header{ PacketHeader::kMagic, PacketHeader::kVersion, 0, id }
    {
    }

    uint8_t* Packet::getData() noexcept
    {
        return data.data();
    }

    const uint8_t* Packet::getData() const noexcept
    {
        return data.data();
    }

    Command Packet::getCommand() const noexcept
    {
        return header.id;
    }

    void Packet::clear() noexcept
    {
        bytesTransferred = 0;
        bytesRead = 0;
        data.clear();
    }

    bool Packet::commandRequiresAuth() const noexcept
    {
        switch (getCommand())
        {
            case Command::ping:
            case Command::auth:
            case Command::token:
            case Command::gameInfo:
            case Command::objectsList:
            case Command::scriptsHeader:
            case Command::scriptsData:
            case Command::heartbeat:
                return false;
            default:
                return true;
        }
    }

    void Packet::write(const void* bytes, size_t size)
    {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(bytes);
        data.insert(data.end(), src, src + size);
    }

    void Packet::writeString(std::string_view s)
    {
        write(s.data(), s.size());
        data.push_back(0);
    }

    const uint8_t* Packet::read(size_t size)
    {
        if (bytesRead + size > data.size())
        {
            return nullptr;
        }

        const uint8_t* result = data.data() + bytesRead;
        bytesRead += size;
        return result;
    }

    std::string_view Packet::readString()
    {
        if (bytesRead >= data.size())
            return {};

        const char* str = reinterpret_cast<const char*>(data.data() + bytesRead);

        size_t stringLen = 0;
        while (bytesRead < data.size() && str[stringLen] != '\0')
        {
            bytesRead++;
            stringLen++;
        }

        if (str[stringLen] != '\0')
            return {};

        // Skip null terminator.
        bytesRead++;

        return std::string_view(str, stringLen);
    }
} // namespace OpenRCT2::Network

#endif
