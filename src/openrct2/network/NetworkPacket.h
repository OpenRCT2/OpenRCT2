/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include "NetworkTypes.h"
#include "../core/DataSerialiser.h"
#include "../common.h"

class NetworkPacket final
{
public:
    uint16_t                              Size = 0;
    std::shared_ptr<std::vector<uint8_t>> Data = std::make_shared<std::vector<uint8_t>>();
    size_t                              BytesTransferred = 0;
    size_t                              BytesRead = 0;

    static std::unique_ptr<NetworkPacket> Allocate();
    static std::unique_ptr<NetworkPacket> Duplicate(NetworkPacket& packet);

    uint8_t * GetData();
    int32_t  GetCommand();

    void Clear();
    bool CommandRequiresAuth();

    const uint8_t * Read(size_t size);
    const utf8 *  ReadString();

    void Write(const uint8_t * bytes, size_t size);
    void WriteString(const utf8 * string);

    template <typename T>
    NetworkPacket & operator >>(T &value)
    {
        if (BytesRead + sizeof(value) > Size)
        {
            value = 0;
        }
        else
        {
            value = ByteSwapBE(*((T *)&GetData()[BytesRead]));
            BytesRead += sizeof(value);
        }
        return *this;
    }

    template <typename T>
    NetworkPacket & operator <<(T value) {
        T swapped = ByteSwapBE(value);
        uint8_t * bytes = (uint8_t *)&swapped;
        Data->insert(Data->end(), bytes, bytes + sizeof(value));
        return *this;
    }

    NetworkPacket& operator<<(DataSerialiser& data)
    {
        Write((const uint8_t*)data.GetStream().GetData(), data.GetStream().GetLength());
        return *this;
    }
};
