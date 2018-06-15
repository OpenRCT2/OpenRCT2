/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
        catch (const std::exception &)
        {
            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return false;
        }
    }
} // namespace SawyerEncoding
