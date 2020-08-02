/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/DataSerialiser.h"
#include "NetworkTypes.h"

#include <memory>
#include <vector>

class NetworkPacket final
{
public:
    uint16_t Size = 0;
    std::vector<uint8_t> Data;
    size_t BytesTransferred = 0;
    size_t BytesRead = 0;

    uint8_t* GetData();
    const uint8_t* GetData() const;

    NetworkCommand GetCommand() const;

    void Clear();
    bool CommandRequiresAuth();

    const uint8_t* Read(size_t size);
    const utf8* ReadString();

    void Write(const void* bytes, size_t size);
    void WriteString(const utf8* string);

    template<typename T> NetworkPacket& operator>>(T& value)
    {
        if (BytesRead + sizeof(value) > Size)
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

    template<typename T> NetworkPacket& operator<<(T value)
    {
        T swapped = ByteSwapBE(value);
        Write(&swapped, sizeof(T));
        return *this;
    }

    NetworkPacket& operator<<(DataSerialiser& data)
    {
        Write(static_cast<const uint8_t*>(data.GetStream().GetData()), data.GetStream().GetLength());
        return *this;
    }
};
