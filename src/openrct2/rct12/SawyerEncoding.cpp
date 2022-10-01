/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerEncoding.h"

#include "../core/IStream.hpp"
#include "../core/Numerics.hpp"
#include "RCT12.h"

#include <algorithm>

namespace SawyerEncoding
{
    bool ValidateChecksum(OpenRCT2::IStream* stream)
    {
        uint64_t initialPosition = stream->GetPosition();
        uint64_t dataSize = stream->GetLength() - initialPosition;
        if (dataSize < 8)
        {
            return false;
        }
        dataSize -= 4;

        try
        {
            // Calculate checksum
            uint32_t checksum = 0;
            do
            {
                uint8_t buffer[4096];
                uint64_t bufferSize = std::min<uint64_t>(dataSize, sizeof(buffer));
                stream->Read(buffer, bufferSize);

                for (uint64_t i = 0; i < bufferSize; i++)
                {
                    checksum += buffer[i];
                }

                dataSize -= bufferSize;
            } while (dataSize != 0);

            // Read file checksum
            uint32_t fileChecksum = stream->ReadValue<uint32_t>();

            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return checksum == fileChecksum;
        }
        catch (const std::exception&)
        {
            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return false;
        }
    }

    // Returns version number
    RCT12TrackDesignVersion ValidateTrackChecksum(OpenRCT2::IStream* stream)
    {
        uint64_t initialPosition = stream->GetPosition();
        uint64_t dataSize = stream->GetLength() - initialPosition;

        if (dataSize < 4)
        {
            return RCT12TrackDesignVersion::unknown;
        }
        dataSize -= 4;

        try
        {
            const auto buffer = stream->ReadArray<uint8_t>(dataSize);
            const auto* data = buffer.get();
            uint32_t checksum = 0;
            for (size_t i = 0; i < dataSize; i++, ++data)
            {
                uint8_t newByte = ((checksum & 0xFF) + *data) & 0xFF;
                checksum = (checksum & 0xFFFFFF00) + newByte;
                checksum = Numerics::rol32(checksum, 3);
            }

            uint32_t fileChecksum = stream->ReadValue<uint32_t>();
            // Rewind back to original position
            stream->SetPosition(initialPosition);

            if (checksum - 0x1D4C1 == fileChecksum)
                return RCT12TrackDesignVersion::TD6;
            if (checksum - 0x1A67C == fileChecksum)
                return RCT12TrackDesignVersion::TD4;
            if (checksum - 0x1A650 == fileChecksum)
                return RCT12TrackDesignVersion::TD4;

            return RCT12TrackDesignVersion::unknown;
        }
        catch (const std::exception&)
        {
            // Rewind back to original position
            stream->SetPosition(initialPosition);
            return RCT12TrackDesignVersion::unknown;
        }
    }
} // namespace SawyerEncoding
