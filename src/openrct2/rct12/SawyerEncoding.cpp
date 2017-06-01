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

#include "../core/IStream.hpp"
#include "../core/Math.hpp"
#include "SawyerEncoding.h"

namespace SawyerEncoding
{
    bool ValidateChecksum(IStream * stream)
    {
        uint64 initialPosition = stream->GetPosition();
        uint64 dataSize = stream->GetLength() - initialPosition;
        if (dataSize < 8)
        {
            return false;
        }
        dataSize -= 4;

        try
        {
            // Calculate checksum
            uint32 checksum = 0;
            do
            {
                uint8 buffer[4096];
                uint64 bufferSize = Math::Min<uint64>(dataSize, sizeof(buffer));
                stream->Read(buffer, bufferSize);

                for (uint64 i = 0; i < bufferSize; i++)
                {
                    checksum += buffer[i];
                }

                dataSize -= bufferSize;
            }
            while (dataSize != 0);

            // Read file checksum
            uint32 fileChecksum = stream->ReadValue<uint32>();

            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return checksum == fileChecksum;
        }
        catch (Exception)
        {
            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return false;
        }
    }
}
