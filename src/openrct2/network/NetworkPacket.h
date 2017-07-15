#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <memory>
#include <vector>
#include "NetworkTypes.h"
#include "../core/DataSerialiser.h"
#include "../common.h"

class NetworkPacket final
{
public:
    uint16                              Size = 0;
    std::shared_ptr<std::vector<uint8>> Data = std::make_shared<std::vector<uint8>>();
    size_t                              BytesTransferred = 0;
    size_t                              BytesRead = 0;

    static std::unique_ptr<NetworkPacket> Allocate();
    static std::unique_ptr<NetworkPacket> Duplicate(NetworkPacket& packet);

    uint8 * GetData();
    uint32  GetCommand();

    void Clear();
    bool CommandRequiresAuth();

    const uint8 * Read(size_t size);
    const utf8 *  ReadString();

    void Write(const uint8 * bytes, size_t size);
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
        uint8 * bytes = (uint8 *)&swapped;
        Data->insert(Data->end(), bytes, bytes + sizeof(value));
        return *this;
    }

    NetworkPacket& operator<<(DataSerialiser& data)
    {
        Write((const uint8_t*)data.GetStream().GetData(), data.GetStream().GetLength());
        return *this;
    }
};
