/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FileClassifier.h"

#include "Diagnostic.h"
#include "core/Console.hpp"
#include "core/FileStream.h"
#include "core/Memory.hpp"
#include "core/Path.hpp"
#include "core/SawyerCoding.h"
#include "core/String.hpp"
#include "park/ParkFile.h"
#include "rct12/SawyerChunkReader.h"
#include "rct2/RCT2.h"
#include "scenario/Scenario.h"

using namespace OpenRCT2;

static bool TryClassifyAsPark(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);
static bool TryClassifyAsS6(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);
static bool TryClassifyAsS4(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);
static bool TryClassifyAsTD4_TD6(OpenRCT2::IStream* stream, ClassifiedFileInfo* result);

bool TryClassifyFile(const std::string& path, ClassifiedFileInfo* result)
{
    try
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);
        return TryClassifyFile(&fs, result);
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool TryClassifyFile(OpenRCT2::IStream* stream, ClassifiedFileInfo* result)
{
    // TODO Currently track designs get classified as SC4s because they use the
    //      same checksum algorithm. The only way after to tell the difference
    //      between them is to decode it. Decoding however is currently not protected
    //      against invalid compression data for that decoding algorithm and will crash.

    // Park detection
    if (TryClassifyAsPark(stream, result))
    {
        return true;
    }

    // S6 detection
    if (TryClassifyAsS6(stream, result))
    {
        return true;
    }

    // S4 detection
    if (TryClassifyAsS4(stream, result))
    {
        return true;
    }

    // TD6 detection
    if (TryClassifyAsTD4_TD6(stream, result))
    {
        return true;
    }

    return false;
}

static bool TryClassifyAsPark(OpenRCT2::IStream* stream, ClassifiedFileInfo* result)
{
    bool success = false;
    uint64_t originalPosition = stream->GetPosition();
    try
    {
        auto magic = stream->ReadValue<uint32_t>();
        if (magic == OpenRCT2::kParkFileMagic)
        {
            result->Type = FILE_TYPE::PARK;
            result->Version = 0;
            success = true;
        }
    }
    catch (const std::exception& e)
    {
        success = false;
        LOG_VERBOSE(e.what());
    }
    stream->SetPosition(originalPosition);
    return success;
}

static bool TryClassifyAsS6(OpenRCT2::IStream* stream, ClassifiedFileInfo* result)
{
    bool success = false;
    uint64_t originalPosition = stream->GetPosition();
    try
    {
        auto chunkReader = SawyerChunkReader(stream);
        auto s6Header = chunkReader.ReadChunkAs<RCT2::S6Header>();
        if (s6Header.Type == S6_TYPE_SAVEDGAME)
        {
            result->Type = FILE_TYPE::SAVED_GAME;
        }
        else if (s6Header.Type == S6_TYPE_SCENARIO)
        {
            result->Type = FILE_TYPE::SCENARIO;
        }
        result->Version = s6Header.Version;
        success = true;
    }
    catch (const std::exception& e)
    {
        // Exceptions are likely to occur if file is not S6 format
        LOG_VERBOSE(e.what());
    }
    stream->SetPosition(originalPosition);
    return success;
}

static bool TryClassifyAsS4(OpenRCT2::IStream* stream, ClassifiedFileInfo* result)
{
    bool success = false;
    uint64_t originalPosition = stream->GetPosition();
    try
    {
        size_t dataLength = static_cast<size_t>(stream->GetLength());
        auto data = stream->ReadArray<uint8_t>(dataLength);
        stream->SetPosition(originalPosition);
        int32_t fileTypeVersion = SawyerCoding::DetectFileType(data.get(), dataLength);

        int32_t type = fileTypeVersion & FILE_TYPE_MASK;
        int32_t version = fileTypeVersion & FILE_VERSION_MASK;

        if (type == FILE_TYPE_SV4)
        {
            result->Type = FILE_TYPE::SAVED_GAME;
            result->Version = version;
            success = true;
        }
        else if (type == FILE_TYPE_SC4)
        {
            result->Type = FILE_TYPE::SCENARIO;
            result->Version = version;
            success = true;
        }
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine(e.what());
    }

    stream->SetPosition(originalPosition);
    return success;
}

static bool TryClassifyAsTD4_TD6(OpenRCT2::IStream* stream, ClassifiedFileInfo* result)
{
    bool success = false;
    uint64_t originalPosition = stream->GetPosition();
    try
    {
        size_t dataLength = static_cast<size_t>(stream->GetLength());

        auto data = stream->ReadArray<uint8_t>(dataLength);
        stream->SetPosition(originalPosition);

        if (SawyerCoding::ValidateTrackChecksum(data.get(), dataLength))
        {
            std::unique_ptr<uint8_t, decltype(&Memory::Free<uint8_t>)> td6data(
                Memory::Allocate<uint8_t>(0x10000), &Memory::Free<uint8_t>);
            size_t td6len = SawyerCoding::DecodeTD6(data.get(), td6data.get(), dataLength);
            if (td6data != nullptr && td6len >= 8)
            {
                uint8_t version = (td6data.get()[7] >> 2) & 3;
                if (version <= 2)
                {
                    result->Type = FILE_TYPE::TRACK_DESIGN;
                    result->Version = version;
                    success = true;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine(e.what());
    }

    return success;
}

FileExtension GetFileExtensionType(u8string_view path)
{
    auto extension = Path::GetExtension(path);
    if (String::iequals(extension, ".dat") || String::iequals(extension, ".pob"))
        return FileExtension::DAT;
    if (String::iequals(extension, ".sc4"))
        return FileExtension::SC4;
    if (String::iequals(extension, ".sv4"))
        return FileExtension::SV4;
    if (String::iequals(extension, ".td4"))
        return FileExtension::TD4;
    if (String::iequals(extension, ".sc6"))
        return FileExtension::SC6;
    if (String::iequals(extension, ".sea"))
        return FileExtension::SC6;
    if (String::iequals(extension, ".sv6"))
        return FileExtension::SV6;
    if (String::iequals(extension, ".sv7"))
        return FileExtension::SV6;
    if (String::iequals(extension, ".td6"))
        return FileExtension::TD6;
    if (String::iequals(extension, ".park"))
        return FileExtension::PARK;
    return FileExtension::Unknown;
}
